#include "catch.hpp"
#include "trompeloeil.hpp"

#include "FinalFrame.h"

#include "mock/MockPinSet.h"

SCENARIO("FinalFrame allows 3 turns when we get 3 strikes") {
    GIVEN("A FinalFrame instance with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        FinalFrame fFrameMutable{std::move(mockPinsPtr)};
        WHEN("We score 2 strikes in a row") {
            trompeloeil::sequence s;
            ALLOW_CALL(mockPins, PinsDown()).RETURN(10);
            REQUIRE_FALSE(fFrameMutable.TurnEnded());
            REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)))
                .TIMES(1)
                .IN_SEQUENCE(s);
            fFrameMutable.Bowled(MockPinSet{});
            REQUIRE_FALSE(fFrameMutable.TurnEnded());
            REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)))
                .TIMES(1)
                .IN_SEQUENCE(s);
            REQUIRE_CALL(mockPins, Reset()).IN_SEQUENCE(s);
            REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)))
                    .TIMES(1)
                    .IN_SEQUENCE(s);
            fFrameMutable.Bowled(MockPinSet{});
            THEN("The pins should be reset and we are allowed to bowl a third strike") {
                REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)))
                        .TIMES(1)
                        .IN_SEQUENCE(s);
                REQUIRE_CALL(mockPins, Reset())
                    .IN_SEQUENCE(s);
                REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&)))
                    .IN_SEQUENCE(s);
                REQUIRE_FALSE(fFrameMutable.TurnEnded());
                fFrameMutable.Bowled(MockPinSet{});
                const auto& fFrame = fFrameMutable;
                AND_THEN("Our Score should consist of 3 strikes and our turn ended") {
                    REQUIRE(std::holds_alternative<IFrame::ThreeStrikes>(fFrame.Score()));
                    REQUIRE(fFrame.TurnEnded());
                }
            }
        }
    }
}

SCENARIO("An open FinalFrame results on only allowing two turns") {
    GIVEN("A FinalFrame instance with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(AT_LEAST(1));
        FinalFrame fFrameMutable{std::move(mockPinsPtr)};
        WHEN("We knock down 4 pins") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(4);
            REQUIRE_FALSE(fFrameMutable.TurnEnded());
            fFrameMutable.Bowled(MockPinSet{});
            AND_WHEN("We knock down another 2 pins") {
                ALLOW_CALL(mockPins, PinsDown()).RETURN(4 + 2);
                REQUIRE_FALSE(fFrameMutable.TurnEnded());
                fFrameMutable.Bowled(MockPinSet{});
                const auto& fFrame = fFrameMutable;
                THEN("Our turn should have ended and we scored an Open frame of 6") {
                    REQUIRE(fFrame.TurnEnded());
                    const auto score = std::get<IFrame::Open>(fFrame.Score());
                    REQUIRE(score.total == 6);
                    REQUIRE(score.first == 4);
                    REQUIRE(score.second == 2);
                }
            }
        }
    }
}

SCENARIO("Getting a spare allows a third go and returns the correct result") {
    GIVEN("A FinalFrame instance with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(AT_LEAST(1));
        FinalFrame fFrameMutable{std::move(mockPinsPtr)};
        WHEN("We knock down 4 pins") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(4);
            REQUIRE_FALSE(fFrameMutable.TurnEnded());
            fFrameMutable.Bowled(MockPinSet{});
            AND_WHEN("We knock down another 6 pins") {
                ALLOW_CALL(mockPins, PinsDown()).RETURN(4 + 6);
                REQUIRE_FALSE(fFrameMutable.TurnEnded());
                fFrameMutable.Bowled(MockPinSet{});
                THEN("We should be allowed to take another turn and we will score 7") {
                    trompeloeil::sequence s;
                    REQUIRE_FALSE(fFrameMutable.TurnEnded());
                    REQUIRE_CALL(mockPins, PinsDown()).RETURN(4 + 6).IN_SEQUENCE(s);
                    REQUIRE_CALL(mockPins, PinsDown()).RETURN(7).IN_SEQUENCE(s);
                    REQUIRE_CALL(mockPins, Reset());
                    fFrameMutable.Bowled(MockPinSet{});
                    const auto& fFrame = fFrameMutable;
                    AND_THEN("We should have a SpareWithBonus") {
                        REQUIRE(fFrame.TurnEnded());
                        const auto score = std::get<IFrame::SpareWithBonus>(fFrame.Score());
                        REQUIRE(score.first == 4);
                        REQUIRE(score.bonus == 7);
                    }
                }
            }
        }
    }
}

SCENARIO("Attempting to bowl a third time on an Open FinalFrame throws an exception") {
    GIVEN("A FinalFrame instance with a mock PinSet") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(AT_LEAST(1));
        FinalFrame fFrameMutable{std::move(mockPinsPtr)};
        WHEN("We knock down 4 pins") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(4);
            REQUIRE_FALSE(fFrameMutable.TurnEnded());
            fFrameMutable.Bowled(MockPinSet{});
            AND_WHEN("We knock down another 2 pins") {
                ALLOW_CALL(mockPins, PinsDown()).RETURN(4 + 2);
                REQUIRE_FALSE(fFrameMutable.TurnEnded());
                FORBID_CALL(mockPins, Reset());
                fFrameMutable.Bowled(MockPinSet{});
                const auto& fFrame = fFrameMutable;
                THEN("Our turn should have ended and throw an exception when we try to bowl") {
                    REQUIRE(fFrame.TurnEnded());
                    REQUIRE_THROWS_AS(fFrameMutable.Bowled(MockPinSet{}), FrameEndedException);
                }
            }
        }
    }
}

SCENARIO("A FinalFrame in its initial state does not consider the turn ended") {
    GIVEN("A FinalFrame instance") {
        FinalFrame fFrame{std::make_unique<MockPinSet>()};
        WHEN("We get the return value of TurnEnded()") {
            auto turnEnded = fFrame.TurnEnded();
            THEN("The result should be false") {
                REQUIRE_FALSE(turnEnded);
            }
        }
    }
}

SCENARIO("A FinalFrame after one turn does not consider the turn ended") {
    GIVEN("A FinalFrame instance") {
        auto mockPinsPtr = std::make_unique<MockPinSet>();
        auto& mockPins = *mockPinsPtr;
        REQUIRE_CALL(mockPins, OperatorAndEquals(ANY(const IPinSet&))).TIMES(AT_LEAST(1));
        FinalFrame fFrameMutable{std::move(mockPinsPtr)};
        WHEN("We knock down 0 pins") {
            ALLOW_CALL(mockPins, PinsDown()).RETURN(0);
            fFrameMutable.Bowled(MockPinSet{});
            THEN("The result should be false") {
                REQUIRE_FALSE(fFrameMutable.TurnEnded());
            }
        }
    }
}
