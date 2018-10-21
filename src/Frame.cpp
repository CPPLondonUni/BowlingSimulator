#include "Frame.h"

Frame::Frame(std::unique_ptr<IPinSet>&& pins) : pins{std::move(pins)} {
}

void Frame::Bowled(const IPinSet& newPins) {
    if (TurnEnded())
        throw FrameEndedException{"This frame has ended"};
    *pins &= newPins;
    switch (turnState) {
        case TurnState::NONE:
            turnState = TurnState::ONE;
            first = pins->PinsDown();
            break;
        case TurnState::ONE:
            turnState = TurnState::TWO;
            second = pins->PinsDown() - first;
            break;
    }
}

bool Frame::TurnEnded() const {
    if (turnState == TurnState::NONE)
        return false;
    return turnState == TurnState::TWO || pins->PinsDown() == 10;
}

Frame::Score_t Frame::Score() const {
    const auto result = pins->PinsDown();
    if (result == 10)
        if (turnState == TurnState::TWO)
            return {Spare{first}};
        else
            return {Strike{}};
    return {Open{result, first, second}};
}
