#ifndef BOWLINGSIMULATOR_MOCKPINSET_H
#define BOWLINGSIMULATOR_MOCKPINSET_H

#include "trompeloeil.hpp"
#include "interface/IPinSet.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;
struct MockPinSet : public IPinSet {
    MAKE_CONST_MOCK0(AllPinsUp, bool(), override);
    MAKE_CONST_MOCK0(AllPinsDown, bool(), override);
    MAKE_MOCK1(KnockDownPin, void(Pin), override);
    MAKE_CONST_MOCK1(IsDown, bool(Pin), override);
    MAKE_CONST_MOCK1(IsUp, bool(Pin), override);
    MAKE_CONST_MOCK0(PinsUp, uint_fast8_t(), override);
    MAKE_CONST_MOCK0(PinsDown, uint_fast8_t(), override);
    MAKE_MOCK0(Reset, void(), override);
    MAKE_MOCK1(OperatorAndEquals, void(const IPinSet&));
    IPinSet& operator&=(const IPinSet& rhs) override {
        OperatorAndEquals(rhs);
        return *this;
    }
};

#endif //BOWLINGSIMULATOR_MOCKPINSET_H
