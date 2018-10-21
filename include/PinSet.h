#ifndef BOWLINGSIMULATOR_PINSET_H
#define BOWLINGSIMULATOR_PINSET_H

#include <bitset>

#include "interface/IPinSet.h"

class PinSet : public IPinSet{
    std::bitset<10> pins{0b11'11'11'11'11};
public:

    bool AllPinsUp() const override;

    bool AllPinsDown() const override;

    void KnockDownPin(Pin p) override;

    bool IsDown(Pin p) const override;

    bool IsUp(Pin p) const override;

    uint_fast8_t PinsUp() const override;

    uint_fast8_t PinsDown() const override;

    void Reset() override;

    IPinSet& operator&=(const IPinSet&) override;
};
#endif //BOWLINGSIMULATOR_PINSET_H
