#ifndef BOWLINGSIMULATOR_FRAMESET_H
#define BOWLINGSIMULATOR_FRAMESET_H

#include "interface/IFrame.h"

#include <array>
#include <memory>

class FrameSet {
    std::array<std::unique_ptr<IFrame>, 10> frames;
    uint_fast8_t currentFrame = 0;

public:
    FrameSet(decltype(frames)&& frames);
    void Bowled(const IPinSet& pinSet);
    bool Ended() const;
    uint_fast16_t Score() const;
};
#endif //BOWLINGSIMULATOR_FRAMESET_H
