#ifndef BOWLINGSIMULATOR_IFRAME_H
#define BOWLINGSIMULATOR_IFRAME_H

#include "interface/IPinSet.h"

#include <cstdint>
#include <stdexcept>
#include <variant>

class FrameEndedException : public std::logic_error {
public:
    using std::logic_error::logic_error;
};

class IFrame {
public:
    struct ThreeStrikes{};
    struct Strike{};
    struct Spare{uint_fast8_t first = 0;};
    struct SpareWithBonus{uint_fast8_t first = 0; uint_fast8_t bonus = 0; };
    struct Open{uint_fast8_t total = 0; uint_fast8_t first = 0; uint_fast8_t second = 0;};
    using Score_t = std::variant<Open, Strike, Spare, SpareWithBonus, ThreeStrikes>;

    virtual void Bowled(const IPinSet& newPins) = 0;

    virtual bool TurnEnded() const = 0;

    virtual Score_t Score() const = 0;

    virtual ~IFrame() = default;
};
#endif //BOWLINGSIMULATOR_IFRAME_H
