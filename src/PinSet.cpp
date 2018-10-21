#include "PinSet.h"

bool PinSet::AllPinsUp() const {
    return pins.all();
}

bool PinSet::AllPinsDown() const {
    return pins.none();
}

void PinSet::KnockDownPin(Pin p) {
    pins[static_cast<uint8_t>(p)] = false;
}

bool PinSet::IsDown(Pin p) const {
    return !pins.test(static_cast<uint8_t>(p));
}

bool PinSet::IsUp(Pin p) const {
    return !IsDown(p);
}

uint_fast8_t PinSet::PinsUp() const {
    return pins.count();
}

uint_fast8_t PinSet::PinsDown() const {
    return pins.size() - pins.count();
}

IPinSet& PinSet::operator&=(const IPinSet& rhs) {
    for (auto i = 0; i < 10; ++i)
        if (rhs.IsDown(static_cast<Pin>(i)))
            pins[i] = false;
    return *this;
}

void PinSet::Reset() {
    pins.set();
}
