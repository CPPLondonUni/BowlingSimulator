#include "FinalFrame.h"

FinalFrame::FinalFrame(std::unique_ptr<IPinSet> &&pins) : pins{std::move(pins)} {
}

void FinalFrame::Bowled(const IPinSet &newPinState) {
    if (TurnEnded())
        throw FrameEndedException{"This frame has been completed"};
    *pins &= newPinState;
    switch (turnState) {
        case TurnState::NONE:
            turnState = TurnState::ONE;
            first = pins->PinsDown();
            break;
        case TurnState::ONE:
            turnState = TurnState::TWO;
            if (first == 10) {
                pins->Reset();
                *pins &= newPinState;
            }
            second = pins->PinsDown() - first;
            break;
        case TurnState::TWO:
            turnState = TurnState::THREE;
            pins->Reset();
            *pins &= newPinState;
            bonus = pins->PinsDown();
            break;
    }
}

IFrame::Score_t FinalFrame::Score() const {
    const auto pinsDown = pins->PinsDown();
    if (turnState == TurnState::TWO && pinsDown < 10)
        return {Open{pinsDown, first, second}};
    if (turnState == TurnState::THREE && first < 10 && (first + second) == 10)
        return {SpareWithBonus{first, bonus}};
    return {ThreeStrikes{}};
}

bool FinalFrame::TurnEnded() const {
    return turnState == TurnState::THREE ||
           (turnState == TurnState::TWO && pins->PinsDown() < 10);
}
