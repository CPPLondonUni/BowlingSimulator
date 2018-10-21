#include "catch.hpp"

#include "FrameSet.h"

#include "mock/MockPinSet.h"
#include "mock/MockFrame.h"

#include <array>
#include <utility>
#include <vector>

using FrameMocks = std::pair<std::array<std::unique_ptr<IFrame>, 10>,
        std::vector<std::reference_wrapper<MockFrame>>>;

FrameMocks GenerateMockFrames() {
    std::array<std::unique_ptr<IFrame>, 10> ptrs;
    std::vector<std::reference_wrapper<MockFrame>> refs;
    refs.reserve(10);
    for (auto&& i : ptrs) {
        auto obj = std::make_unique<MockFrame>();
        refs.emplace_back(*obj);
        i = std::move(obj);
    }
    return {std::move(ptrs), std::move(refs)};
}

SCENARIO("A FrameSet where we bowl nothing but empty frames has a score of 0") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl total misses continuously") {
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto&& i : mockFrames.second) {
                trompeloeil::sequence s;
                REQUIRE_CALL(i.get(), Bowled(ANY(const IPinSet&)))
                    .TIMES(2);
                REQUIRE_CALL(i.get(), TurnEnded())
                    .RETURN(false)
                    .IN_SEQUENCE(s);
                REQUIRE_CALL(i.get(), TurnEnded())
                    .RETURN(true)
                    .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(i.get(), Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            const auto& frameSet = frameSetMutable;
            THEN("Our total score should be zero and the set has ended") {
                CHECK(frameSet.Ended());
                CHECK(frameSet.Score() == 0);
            }
        }
    }
}

SCENARIO("A FrameSet where we score 5 only once gives a score of 5") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl all games and one mock is set to report a score of 5") {
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto&& i : mockFrames.second) {
                trompeloeil::sequence s;
                REQUIRE_CALL(i.get(), Bowled(ANY(const IPinSet&)))
                .TIMES(2);
                REQUIRE_CALL(i.get(), TurnEnded())
                        .RETURN(false)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(i.get(), TurnEnded())
                        .RETURN(true)
                        .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(i.get(), Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            const auto& frameSet = frameSetMutable;
            REQUIRE_CALL(mockFrames.second[4].get(), Score()).RETURN(IFrame::Open{5});
            THEN("Our total score should be 5 and the set has ended") {
                CHECK(frameSet.Ended());
                CHECK(frameSet.Score() == 5);
            }
        }
    }
}

SCENARIO("A FrameSet where we score all strikes results in a score of 300") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl all games and all mocks are set to report a strike") {
            auto& frames = mockFrames.second;
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto i = 0; i < 9; ++i) {
                REQUIRE_CALL(frames[i].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(true);
                expects.emplace_back(
                        NAMED_ALLOW_CALL(frames[i].get(), Score())
                            .RETURN(IFrame::Strike{})
                );
                frameSetMutable.Bowled(MockPinSet{});
                REQUIRE_FALSE(frameSetMutable.Ended());
            }
            REQUIRE_CALL(frames[9].get(), Bowled(ANY(const IPinSet&))).TIMES(3);
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[9].get(), TurnEnded())
                .RETURN(false)
                .TIMES(2)
                .IN_SEQUENCE(s);
            REQUIRE_CALL(frames[9].get(), TurnEnded())
                .RETURN(true)
                .TIMES(1)
                .IN_SEQUENCE(s);
            REQUIRE_CALL(frames[9].get(), Score()).RETURN(IFrame::ThreeStrikes{});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            const auto& frameSet = frameSetMutable;
            THEN("Our total score should be 300 and the set has ended") {
                CHECK(frameSet.Ended());
                CHECK(frameSet.Score() == 300);
            }
        }
    }
}

SCENARIO("Scoring a strike on the 9th frame and 3 strikes on the 10th gives a score of 60") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl nothing on the first 8 frames") {
            auto& frames = mockFrames.second;
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto i = 0; i < 8; ++i) {
                trompeloeil::sequence s;
                auto& frame = frames[i].get();
                REQUIRE_CALL(frame, Bowled(ANY(const IPinSet&)))
                .TIMES(2);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(false)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(true)
                        .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(frame, Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            AND_WHEN("We bowl a strike on the 9th frame") {
                REQUIRE_CALL(frames[8].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[8].get(), TurnEnded()).RETURN(true);
                REQUIRE_CALL(frames[8].get(), Score()).RETURN(IFrame::Strike{});
                frameSetMutable.Bowled(MockPinSet{});
                AND_WHEN("We bowl 3 strikes on the 10th (final) frame") {
                    REQUIRE_CALL(frames[9].get(), Bowled(ANY(const IPinSet&))).TIMES(3);
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(false)
                            .TIMES(2)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(true)
                            .TIMES(1)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), Score()).RETURN(IFrame::ThreeStrikes{});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    const auto& frameSet = frameSetMutable;
                    THEN("We should have a score of 60 and the game has ended") {
                        CHECK(frameSet.Ended());
                        CHECK(frameSet.Score() == 60);
                    }
                }
            }
        }
    }
}

SCENARIO("Getting a strike on the first frame and 7 next gives a score of 24") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto& frames = mockFrames.second;
        WHEN("We bowl a strike and then just 7") {
            REQUIRE_CALL(frames[0].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(true);
            REQUIRE_CALL(frames[0].get(), Score()).RETURN(IFrame::Strike{});
            frameSetMutable.Bowled(MockPinSet{});
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[1].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), Score()).RETURN(IFrame::Open{7, 7, 0});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We bowl nothing for the rest") {
                std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
                for (auto i = 2; i < 10; i++) {
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                            const IPinSet&)))
                    .TIMES(2);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(false)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(true)
                            .IN_SEQUENCE(s);
                    expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                }
                const auto& frameSet = frameSetMutable;
                THEN("We should have a score of 24 and the game has ended") {
                    CHECK(frameSet.Ended());
                    CHECK(frameSet.Score() == 24);
                }
            }
        }
    }
}

SCENARIO("Getting a spare and then an open frame of 5 + 0, then nothing gives a score of 20") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto &frames = mockFrames.second;
        WHEN("We bowl a spare") {
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[0].get(), Bowled(ANY(
                    const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[0].get(), Score()).RETURN(IFrame::Spare{6});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We then bowl an open frame of 5") {
                REQUIRE_CALL(frames[1].get(), Bowled(ANY(
                        const IPinSet&))).TIMES(2);
                REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
                REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
                REQUIRE_CALL(frames[1].get(), Score()).RETURN(IFrame::Open{5, 5, 0});
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
                AND_WHEN("We bowl nothing on all subsequent frames") {
                    std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
                    for (auto i = 2; i < 10; i++) {
                        trompeloeil::sequence s;
                        REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                                const IPinSet&)))
                        .TIMES(2);
                        REQUIRE_CALL(frames[i].get(), TurnEnded())
                                .RETURN(false)
                                .IN_SEQUENCE(s);
                        REQUIRE_CALL(frames[i].get(), TurnEnded())
                                .RETURN(true)
                                .IN_SEQUENCE(s);
                        expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                        frameSetMutable.Bowled(MockPinSet{});
                        frameSetMutable.Bowled(MockPinSet{});
                    }
                    const auto& frameSet = frameSetMutable;
                    THEN("We should have a score of 20") {
                        CHECK(frameSet.Ended());
                        CHECK(frameSet.Score() == 20);
                    }
                }
            }
        }
    }
}

SCENARIO("Getting a strike on the first frame and a spare next gives a score of 30") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto& frames = mockFrames.second;
        WHEN("We bowl a strike and then a spare") {
            REQUIRE_CALL(frames[0].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(true);
            REQUIRE_CALL(frames[0].get(), Score()).RETURN(IFrame::Strike{});
            frameSetMutable.Bowled(MockPinSet{});
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[1].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), Score()).RETURN(IFrame::Spare{7});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We bowl nothing for the rest") {
                std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
                for (auto i = 2; i < 10; i++) {
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                            const IPinSet&)))
                    .TIMES(2);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(false)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(true)
                            .IN_SEQUENCE(s);
                    expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                }
                const auto& frameSet = frameSetMutable;
                THEN("We should have a score of 30 and the game has ended") {
                    CHECK(frameSet.Ended());
                    CHECK(frameSet.Score() == 30);
                }
            }
        }
    }
}

SCENARIO("Getting a spare on the first frame and a (9+1) spare next gives a score of 29") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto& frames = mockFrames.second;
        WHEN("We bowl a spare twice (7 + 5) then (5 + 5)") {
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[0].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[0].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[0].get(), Score()).RETURN(IFrame::Spare{7});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            REQUIRE_CALL(frames[1].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[1].get(), Score()).RETURN(IFrame::Spare{9});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We bowl nothing for the rest") {
                std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
                for (auto i = 2; i < 10; i++) {
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                            const IPinSet&)))
                    .TIMES(2);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(false)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(true)
                            .IN_SEQUENCE(s);
                    expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                }
                const auto& frameSet = frameSetMutable;
                THEN("We should have a score of 29 and the game has ended") {
                    CHECK(frameSet.Ended());
                    CHECK(frameSet.Score() == 29);
                }
            }
        }
    }
}

SCENARIO("Scoring a strike on the 9th frame and a spare + bonus (7 + 3 + 8) on the 10th gives a score of 38") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl nothing on the first 8 frames") {
            auto& frames = mockFrames.second;
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto i = 0; i < 8; ++i) {
                trompeloeil::sequence s;
                auto& frame = frames[i].get();
                REQUIRE_CALL(frame, Bowled(ANY(const IPinSet&)))
                .TIMES(2);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(false)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(true)
                        .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(frame, Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            AND_WHEN("We bowl a strike on the 9th frame") {
                REQUIRE_CALL(frames[8].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[8].get(), TurnEnded()).RETURN(true);
                REQUIRE_CALL(frames[8].get(), Score()).RETURN(IFrame::Strike{});
                frameSetMutable.Bowled(MockPinSet{});
                AND_WHEN("We bowl a spare + bonus on the 10th (final) frame") {
                    REQUIRE_CALL(frames[9].get(), Bowled(ANY(const IPinSet&))).TIMES(3);
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(false)
                            .TIMES(2)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(true)
                            .TIMES(1)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), Score()).RETURN(IFrame::SpareWithBonus{7, 8});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    const auto& frameSet = frameSetMutable;
                    THEN("We should have a score of 38 and the game has ended") {
                        CHECK(frameSet.Ended());
                        CHECK(frameSet.Score() == 38);
                    }
                }
            }
        }
    }
}

SCENARIO("Scoring a strike on the 8th + 9th frame and a spare + bonus (4 + 6 + 5) on the 10th gives a score of 38") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl nothing on the first 7 frames") {
            auto& frames = mockFrames.second;
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto i = 0; i < 7; ++i) {
                trompeloeil::sequence s;
                auto& frame = frames[i].get();
                REQUIRE_CALL(frame, Bowled(ANY(const IPinSet&)))
                .TIMES(2);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(false)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(true)
                        .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(frame, Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            AND_WHEN("We bowl a strike on the 8th + 9th frame") {
                REQUIRE_CALL(frames[7].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[7].get(), TurnEnded()).RETURN(true);
                REQUIRE_CALL(frames[7].get(), Score()).RETURN(IFrame::Strike{});
                frameSetMutable.Bowled(MockPinSet{});
                REQUIRE_CALL(frames[8].get(), Bowled(ANY(const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[8].get(), TurnEnded()).RETURN(true);
                REQUIRE_CALL(frames[8].get(), Score()).RETURN(IFrame::Strike{});
                frameSetMutable.Bowled(MockPinSet{});
                AND_WHEN("We bowl a spare + bonus on the 10th (final) frame") {
                    REQUIRE_CALL(frames[9].get(), Bowled(ANY(const IPinSet&))).TIMES(3);
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(false)
                            .TIMES(2)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(true)
                            .TIMES(1)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), Score()).RETURN(IFrame::SpareWithBonus{4, 5});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    const auto& frameSet = frameSetMutable;
                    THEN("We should have a score of 59 and the game has ended") {
                        CHECK(frameSet.Ended());
                        CHECK(frameSet.Score() == 59);
                    }
                }
            }
        }
    }
}

SCENARIO("Scoring a spare on the 9th frame and a spare + bonus (2 + 8 + 6) on the 10th gives a score of 38") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        WHEN("We bowl nothing on the first 8 frames") {
            auto& frames = mockFrames.second;
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            for (auto i = 0; i < 8; ++i) {
                trompeloeil::sequence s;
                auto& frame = frames[i].get();
                REQUIRE_CALL(frame, Bowled(ANY(const IPinSet&)))
                .TIMES(2);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(false)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(frame, TurnEnded())
                        .RETURN(true)
                        .IN_SEQUENCE(s);
                expects.emplace_back(NAMED_ALLOW_CALL(frame, Score()).RETURN(IFrame::Open{0}));
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
            }
            AND_WHEN("We bowl a spare on the 9th frame") {
                trompeloeil::sequence s;
                REQUIRE_CALL(frames[8].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
                REQUIRE_CALL(frames[8].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
                REQUIRE_CALL(frames[8].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
                REQUIRE_CALL(frames[8].get(), Score()).RETURN(IFrame::Spare{6});
                frameSetMutable.Bowled(MockPinSet{});
                frameSetMutable.Bowled(MockPinSet{});
                AND_WHEN("We bowl a spare + bonus on the 10th (final) frame") {
                    REQUIRE_CALL(frames[9].get(), Bowled(ANY(const IPinSet&))).TIMES(3);
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(false)
                            .TIMES(2)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), TurnEnded())
                            .RETURN(true)
                            .TIMES(1)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[9].get(), Score()).RETURN(IFrame::SpareWithBonus{2, 6});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                    const auto& frameSet = frameSetMutable;
                    THEN("We should have a score of 28 and the game has ended") {
                        CHECK(frameSet.Ended());
                        CHECK(frameSet.Score() == 28);
                    }
                }
            }
        }
    }
}

SCENARIO("Getting a strike on the first 2 frames and Open (4 + 5) next gives a score of 52") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto& frames = mockFrames.second;
        WHEN("We bowl a strike twice and then just 7") {
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            auto i = 0;
            for (; i < 2; ++i) {
                REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                        const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(true);
                expects.emplace_back(NAMED_REQUIRE_CALL(frames[i].get(), Score()).RETURN(IFrame::Strike{}));
                frameSetMutable.Bowled(MockPinSet{});
            }
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[i].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{9, 4, 5});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We bowl nothing for the rest") {
                for (i = 3; i < 10; i++) {
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                            const IPinSet&)))
                    .TIMES(2);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(false)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(true)
                            .IN_SEQUENCE(s);
                    expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                }
                const auto& frameSet = frameSetMutable;
                THEN("We should have a score of 52 and the game has ended") {
                    CHECK(frameSet.Ended());
                    CHECK(frameSet.Score() == 52);
                }
            }
        }
    }
}

SCENARIO("Getting a strike on the first 2 frames and a Spare (6 + 4) next gives a score of 56") {
    GIVEN("A FrameSet with 10 mock frames") {
        auto mockFrames = GenerateMockFrames();
        FrameSet frameSetMutable{std::move(mockFrames.first)};
        auto& frames = mockFrames.second;
        WHEN("We bowl a strike twice and then just 7") {
            std::vector<std::unique_ptr<trompeloeil::expectation>> expects;
            auto i = 0;
            for (; i < 2; ++i) {
                REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                        const IPinSet&))).TIMES(1);
                REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(true);
                expects.emplace_back(NAMED_REQUIRE_CALL(frames[i].get(), Score()).RETURN(IFrame::Strike{}));
                frameSetMutable.Bowled(MockPinSet{});
            }
            trompeloeil::sequence s;
            REQUIRE_CALL(frames[i].get(), Bowled(ANY(const IPinSet&))).TIMES(2);
            REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(false).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[i].get(), TurnEnded()).RETURN(true).IN_SEQUENCE(s);
            REQUIRE_CALL(frames[i].get(), Score()).RETURN(IFrame::Spare{6});
            frameSetMutable.Bowled(MockPinSet{});
            frameSetMutable.Bowled(MockPinSet{});
            AND_WHEN("We bowl nothing for the rest") {
                for (i = 3; i < 10; i++) {
                    trompeloeil::sequence s;
                    REQUIRE_CALL(frames[i].get(), Bowled(ANY(
                            const IPinSet&)))
                    .TIMES(2);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(false)
                            .IN_SEQUENCE(s);
                    REQUIRE_CALL(frames[i].get(), TurnEnded())
                            .RETURN(true)
                            .IN_SEQUENCE(s);
                    expects.emplace_back(NAMED_ALLOW_CALL(frames[i].get(), Score()).RETURN(IFrame::Open{0}));
                    frameSetMutable.Bowled(MockPinSet{});
                    frameSetMutable.Bowled(MockPinSet{});
                }
                const auto& frameSet = frameSetMutable;
                THEN("We should have a score of 56 and the game has ended") {
                    CHECK(frameSet.Ended());
                    CHECK(frameSet.Score() == 56);
                }
            }
        }
    }
}
