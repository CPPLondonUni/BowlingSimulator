#ifndef BOWLINGSIMULATOR_FINALFRAME_H
#define BOWLINGSIMULATOR_FINALFRAME_H

#include "interface/IFrame.h"

#include <memory>
#include <variant>

class FinalFrame : public IFrame {
    enum class TurnState {
        NONE,
        ONE,
        TWO,
        THREE
    };

    std::unique_ptr<IPinSet> pins;
    TurnState turnState = TurnState::NONE;
    uint_fast8_t first = 0;
    uint_fast8_t second = 0;
    uint_fast8_t bonus = 0;

public:
    FinalFrame(std::unique_ptr<IPinSet>&& pins);

    void Bowled(const IPinSet& newPinState) override;

    Score_t Score() const override;

    bool TurnEnded() const override;
};

#endif //BOWLINGSIMULATOR_FINALFRAME_H
