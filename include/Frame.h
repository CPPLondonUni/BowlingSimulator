#ifndef BOWLINGSIMULATOR_FRAME_H
#define BOWLINGSIMULATOR_FRAME_H

#include "interface/IFrame.h"

#include <memory>

class Frame : public IFrame {
    enum class TurnState {
        NONE,
        ONE,
        TWO,
    };

    std::unique_ptr<IPinSet> pins;
    TurnState turnState = TurnState::NONE;
    uint_fast8_t first = 0;
    uint_fast8_t second = 0;

public:

    Frame(std::unique_ptr<IPinSet>&& pins);

    void Bowled(const IPinSet& newPins) override;

    bool TurnEnded() const override;

    Score_t Score() const override;
};

#endif //BOWLINGSIMULATOR_FRAME_H
