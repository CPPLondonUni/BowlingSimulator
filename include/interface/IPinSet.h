#ifndef BOWLINGSIMULATOR_IPINSET_H
#define BOWLINGSIMULATOR_IPINSET_H

#include <cstdint>

enum class Pin {
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN
};

class IPinSet {
public:

    virtual bool AllPinsUp() const = 0;

    virtual bool AllPinsDown() const = 0;

    virtual void KnockDownPin(Pin p) = 0;

    virtual bool IsDown(Pin p) const = 0;

    virtual bool IsUp(Pin p) const = 0;

    virtual uint_fast8_t PinsUp() const = 0;

    virtual uint_fast8_t PinsDown() const = 0;

    virtual IPinSet& operator&=(const IPinSet&) = 0;

    virtual void Reset() = 0;

    virtual ~IPinSet() = default;
};
#endif //BOWLINGSIMULATOR_IPINSET_H
