#include <iostream>
#include <random>

#include "FrameSet.h"
#include "Frame.h"
#include "FinalFrame.h"
#include "PinSet.h"

int main() {
    std::mt19937_64 rng{std::random_device{}()};

    std::array<std::unique_ptr<IFrame>, 10> frames;
    for (auto i = 0; i < 9; ++i)
        frames[i] = std::make_unique<Frame>(std::make_unique<PinSet>());
    frames.back() = std::make_unique<FinalFrame>(std::make_unique<PinSet>());
    FrameSet frameSet{std::move(frames)};

    auto turnsTaken = 0;
    while (!frameSet.Ended()) {
        auto pinsDown = rng() % 11;
        PinSet pins;
        for (auto i = 0; i < pinsDown; ++i) {
            pins.KnockDownPin(static_cast<Pin>(i));
        }
        std::cout << "Bowled: " << pinsDown << " on turn " << turnsTaken + 1 << "\n";
        frameSet.Bowled(pins);
        ++turnsTaken;
    }
    std::cout << "Final Score: " << frameSet.Score() << "\n";
    std::cout << "Turns Taken: " << turnsTaken << "\n";

    return 0;
}