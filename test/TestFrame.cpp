#include "catch.hpp"
#include "trompeloeil.hpp"

#include "Frame.h"
#include "mock/MockPinSet.h"

SCENARIO("A Frame correctly reports what we bowled on that frame when we get 7 points") {
    GIVEN("A Frame constructed with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        REQUIRE_CALL(*mockPinsPtr, OperatorAndEquals(ANY(const IPinSet&))).TIMES(2);
        ALLOW_CALL(*mockPinsPtr, PinsDown()).RETURN(7);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl 4 on the first attempt") {
            MockPinSet firstBowl;
            frameMutable.Bowled(firstBowl);
            REQUIRE_FALSE(frameMutable.TurnEnded());
            AND_WHEN("We bowl down another 3 on the second attempt") {
                MockPinSet secondBowl;
                frameMutable.Bowled(secondBowl);
                const auto& frame = frameMutable;
                THEN("The Frame should report the turn ended and the score was 7") {
                    REQUIRE(frame.TurnEnded());
                    REQUIRE(std::get<Frame::Open>(frame.Score()).total == 7);
                }
            }
        }
    }
}

SCENARIO("A Frame ends when the player gets a strike on the first bowl") {
    GIVEN("A Frame constructed with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)));
        ALLOW_CALL(mockPins, PinsDown())
        .RETURN(0);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl") {
            MockPinSet firstBowl;
            frameMutable.Bowled(firstBowl);
            ALLOW_CALL(mockPins, PinsDown())
            .RETURN(10);
            const auto& frame = frameMutable;
            THEN("The turn should have ended and we should have gotten a strike") {
                REQUIRE(frame.TurnEnded());
                REQUIRE(std::holds_alternative<Frame::Strike>(frame.Score()));
            }
        }
    }
}

SCENARIO("A Frame correctly reports what we bowled on that frame when we get a spare") {
    GIVEN("A Frame constructed with a mock PinSet and a score sequence") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(2);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl 4 on the first attempt") {
            ALLOW_CALL(mockPins, PinsDown())
                    .RETURN(4);
            frameMutable.Bowled(MockPinSet{});
            REQUIRE_FALSE(frameMutable.TurnEnded());
            AND_WHEN("We bowl down another 6 on the second attempt") {
                frameMutable.Bowled(MockPinSet{});
                ALLOW_CALL(mockPins, PinsDown())
                    .RETURN(4 + 6);
                const auto& frame = frameMutable;
                THEN("The Frame should report the turn ended and the score was 7") {
                    REQUIRE(frame.TurnEnded());
                    REQUIRE(std::holds_alternative<Frame::Spare>(frame.Score()));
                }
            }
        }
    }
}

SCENARIO("Frame throws an exception if attempting to bowl after a strike") {
    GIVEN("A Frame constructed with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        ALLOW_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)));
        ALLOW_CALL(mockPins, PinsDown())
        .RETURN(0);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl") {
            MockPinSet firstBowl;
            frameMutable.Bowled(firstBowl);
            ALLOW_CALL(mockPins, PinsDown())
            .RETURN(10);
            const auto& frame = frameMutable;
            THEN("The turn should have ended and we should have gotten a strike") {
                REQUIRE(frame.TurnEnded());
                REQUIRE(std::holds_alternative<Frame::Strike>(frame.Score()));
                REQUIRE_THROWS_AS(frameMutable.Bowled(MockPinSet{}), FrameEndedException);
            }
        }
    }
}

SCENARIO("A Frame correctly reports an open frame of 4 + 5") {
    GIVEN("A Frame constructed with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(2);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl 4 on the first attempt") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(4);
            frameMutable.Bowled(MockPinSet{});
            REQUIRE_FALSE(frameMutable.TurnEnded());
            AND_WHEN("We bowl down another 5 on the second attempt") {
                ALLOW_CALL(mockPins, PinsDown()).RETURN(4 + 5);
                frameMutable.Bowled(MockPinSet{});
                const auto& frame = frameMutable;
                THEN("The Frame should report the turn ended and the score was 9") {
                    REQUIRE(frame.TurnEnded());
                    const auto score = std::get<Frame::Open>(frame.Score());
                    REQUIRE(score.total == 9);
                    REQUIRE(score.first == 4);
                    REQUIRE(score.second == 5);
                }
            }
        }
    }
}

SCENARIO("A Frame correctly reports the first bowl score on a Spare") {
    GIVEN("A Frame constructed with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(2);
        Frame frameMutable{std::move(mockPinsPtr)};
        WHEN("We bowl 7 on the first attempt") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(7);
            frameMutable.Bowled(MockPinSet{});
            REQUIRE_FALSE(frameMutable.TurnEnded());
            AND_WHEN("We bowl down another 3 on the second attempt (Spare)") {
                trompeloeil::sequence s;
                REQUIRE_CALL(mockPins, PinsDown()).RETURN(7).IN_SEQUENCE(s);
                REQUIRE_CALL(mockPins, PinsDown())
                    .RETURN(7 + 3)
                    .IN_SEQUENCE(s)
                    .TIMES(AT_LEAST(1));
                frameMutable.Bowled(MockPinSet{});
                const auto& frame = frameMutable;
                THEN("The Frame should report the turn ended and the score was a Spare of 7") {
                    REQUIRE(frame.TurnEnded());
                    const auto score = std::get<Frame::Spare>(frame.Score());
                    REQUIRE(score.first == 7);
                }
            }
        }
    }
}

SCENARIO("A Frame in its initial state does not consider the turn ended") {
    GIVEN("A Frame instance") {
        Frame frame{std::make_unique<MockPinSet>()};
        WHEN("We get the return value of TurnEnded()") {
            auto turnEnded = frame.TurnEnded();
            THEN("The result should be false") {
                REQUIRE_FALSE(turnEnded);
            }
        }
    }
}