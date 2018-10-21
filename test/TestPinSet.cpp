#include "catch.hpp"

#include "PinSet.h"

SCENARIO("Default-constructed PinSet has all pins standing") {
    GIVEN("A default constructed PinSet class") {
        const PinSet pins;
        WHEN("We check if all the pins are set") {
            auto allPinsSet = pins.AllPinsUp();
            THEN("The result should be true") {
                REQUIRE(allPinsSet);
            }
        }
    }
}

SCENARIO("A PinSet with one pin knocked down does not return true when we check all pins are standing") {
    GIVEN("A default constructed PinSet class") {
        PinSet pins;
        WHEN("We mark a pin as knocked down") {
            pins.KnockDownPin(Pin::ONE);
            THEN("AllPinsSet should return false") {
                REQUIRE_FALSE(pins.AllPinsUp());
            }
        }
    }
}

SCENARIO("A PinSet with a pin knocked down can confirm that the pin is not set, but all others are") {
    GIVEN("A default constructed PinSet class") {
        PinSet pinsMutable;
        WHEN("We mark a pin as knocked down") {
            pinsMutable.KnockDownPin(Pin::FOUR);
            const auto& pins = pinsMutable;
            THEN("The pin should be considered knocked down") {
                REQUIRE(pins.IsDown(Pin::FOUR));
                AND_THEN("All the other pins should be up") {
                    REQUIRE_FALSE(pins.IsDown(Pin::ONE));
                    REQUIRE_FALSE(pins.IsDown(Pin::TWO));
                    REQUIRE_FALSE(pins.IsDown(Pin::THREE));
                    REQUIRE_FALSE(pins.IsDown(Pin::FIVE));
                    REQUIRE_FALSE(pins.IsDown(Pin::SIX));
                    REQUIRE_FALSE(pins.IsDown(Pin::SEVEN));
                    REQUIRE_FALSE(pins.IsDown(Pin::EIGHT));
                    REQUIRE_FALSE(pins.IsDown(Pin::NINE));
                    REQUIRE_FALSE(pins.IsDown(Pin::TEN));
                }
            }
        }
    }
}

SCENARIO("IsUp returns the opposite of IsDown for any given pin") {
    GIVEN("A default-constructed PinSet") {
        PinSet pinsMutable;
        WHEN("We knock a couple of pins down") {
            pinsMutable.KnockDownPin(Pin::SEVEN);
            pinsMutable.KnockDownPin(Pin::NINE);
            const auto& pins = pinsMutable;
            THEN("IsUp should be the opposite of IsDown") {
                REQUIRE(pins.IsDown(Pin::SEVEN));
                REQUIRE(pins.IsDown(Pin::NINE));
                REQUIRE(pins.IsDown(Pin::SEVEN) != pins.IsUp(Pin::SEVEN));
                REQUIRE(pins.IsDown(Pin::NINE) != pins.IsUp(Pin::NINE));
                REQUIRE(pins.IsDown(Pin::FOUR) != pins.IsUp(Pin::FOUR));
            }
        }
    }
}

SCENARIO("AllPinsDown returns false when we leave all the pins alone") {
    GIVEN("A default constructed PinSet") {
        PinSet pinsMutable;
        WHEN("We do nothing") {
            const auto& pins = pinsMutable;
            THEN("All the pins should be up") {
                REQUIRE_FALSE(pins.AllPinsDown());
                REQUIRE(pins.AllPinsUp());
            }
        }
    }
}

SCENARIO("AllPinsDown returns true when we knock down all the pins") {
    GIVEN("A default constructed PinSet") {
        PinSet pinsMutable;
        WHEN("We knock down all the pins") {
            for (auto i = static_cast<uint8_t>(Pin::ONE); i <= static_cast<uint8_t>(Pin::TEN); ++i)
                pinsMutable.KnockDownPin(static_cast<Pin>(i));
            const auto& pins = pinsMutable;
            THEN("All the pins should be down") {
                REQUIRE(pins.AllPinsDown());
                REQUIRE_FALSE(pins.AllPinsUp());
            }
        }
    }
}

SCENARIO("AllPinsDown returns false when only one pin is down") {
    GIVEN("A default constructed PinSet") {
        PinSet pinsMutable;
        WHEN("We knock down a pin") {
                pinsMutable.KnockDownPin(Pin::FIVE);
            const auto& pins = pinsMutable;
            THEN("All the pins should not be considered down") {
                REQUIRE_FALSE(pins.AllPinsDown());
                REQUIRE_FALSE(pins.AllPinsUp());
            }
        }
    }
}

SCENARIO("PinsUp returns 8 when we knock down 2 pins") {
    GIVEN("A default constructed PinSet") {
        PinSet pinsMutable;
        WHEN("We knock down 2 pins") {
            pinsMutable.KnockDownPin(Pin::FIVE);
            pinsMutable.KnockDownPin(Pin::EIGHT);
            const auto& pins = pinsMutable;
            THEN("PinsUp should return 8 and PinsDown should return 2") {
                REQUIRE(pins.PinsUp() == 8);
                REQUIRE(pins.PinsDown() == 2);
            }
        }
    }
}

SCENARIO("PinsUp returns 5 when we & it with another PinSet with 5 pins down") {
    GIVEN("A default constructed PinSet"){
        PinSet pinsMutable;
        WHEN("We & it with a PinSet with 5 pins down") {
            PinSet fivePinsDownMutable;
            fivePinsDownMutable.KnockDownPin(Pin::TWO);
            fivePinsDownMutable.KnockDownPin(Pin::FOUR);
            fivePinsDownMutable.KnockDownPin(Pin::FIVE);
            fivePinsDownMutable.KnockDownPin(Pin::SIX);
            fivePinsDownMutable.KnockDownPin(Pin::EIGHT);
            const auto& fivePinsDown = fivePinsDownMutable;
            pinsMutable &= fivePinsDown;
            const auto& pins = pinsMutable;
            THEN("It should report 5 pins up and 5 pins down") {
                REQUIRE(pins.PinsDown() == 5);
                REQUIRE(pins.PinsUp() == 5);
            }
        }
    }
}

SCENARIO("A PinSet correctly resets its state") {
    GIVEN("A default-constructed PinSet") {
        PinSet pinSetMutable;
        WHEN("We knock down some pins") {
            pinSetMutable.KnockDownPin(Pin::TWO);
            pinSetMutable.KnockDownPin(Pin::FIVE);
            pinSetMutable.KnockDownPin(Pin::SEVEN);
            REQUIRE_FALSE(pinSetMutable.AllPinsUp());
            AND_WHEN("We reset its state") {
                pinSetMutable.Reset();
                const auto& pinSet = pinSetMutable;
                THEN("All the pins should now be up") {
                    REQUIRE(pinSet.AllPinsUp());
                    REQUIRE(pinSet.IsUp(Pin::TWO));
                    REQUIRE(pinSet.IsUp(Pin::FIVE));
                    REQUIRE(pinSet.IsUp(Pin::SEVEN));
                    REQUIRE(pinSet.IsUp(Pin::NINE));
                }
            }
        }
    }
}
