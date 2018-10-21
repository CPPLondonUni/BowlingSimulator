#include "FrameSet.h"

class FrameScoreVisitor {
    uint_fast16_t total = 0;
    uint_fast8_t pendingBonuses = 0;
public:
    void operator()(const IFrame::Open& score);
    void operator()(const IFrame::Strike& score);
    void operator()(const IFrame::Spare& score);
    void operator()(const IFrame::SpareWithBonus& score);
    void operator()(const IFrame::ThreeStrikes& score);
    operator uint_fast16_t() const;
};

FrameSet::FrameSet(decltype(frames)&& frames) : frames{std::move(frames)} {
}

void FrameSet::Bowled(const IPinSet& pinSet) {
    frames[currentFrame]->Bowled(pinSet);
    auto turnEnded = frames[currentFrame]->TurnEnded();
    if (turnEnded)
        ++currentFrame;
}

bool FrameSet::Ended() const {
    return currentFrame == frames.size();
}

uint_fast16_t FrameSet::Score() const {
    FrameScoreVisitor fsVisitor{};
    for (const auto& i : frames)
        std::visit(fsVisitor, i->Score());
    return fsVisitor;
}

void FrameScoreVisitor::operator()(const IFrame::Open& score) {
    total += score.total;
    if (pendingBonuses >= 2) {
        total += score.first + score.second;
        pendingBonuses -= 2;
    } if (pendingBonuses == 1) {
        total += score.first;
        --pendingBonuses;
    }
}

void FrameScoreVisitor::operator()(const IFrame::Strike& score) {
    total += 10;
    if (pendingBonuses) {
        total += 10;
        --pendingBonuses;
    }
    pendingBonuses += 2;
}

void FrameScoreVisitor::operator()(const IFrame::Spare& score) {
    total += 10;
    if (pendingBonuses >= 2) {
        total += 10;
        pendingBonuses -= 2;
    }
    if (pendingBonuses == 1) {
        total += score.first;
        --pendingBonuses;
    }
    ++pendingBonuses;
}

void FrameScoreVisitor::operator()(const IFrame::SpareWithBonus& score) {
    total += 10 + score.bonus;
    switch (pendingBonuses) {
        case 3:
            total += score.first + 10;
            break;
        case 2:
            total += 10;
            break;
        case 1:
            total += score.first;
            break;
    }
}

void FrameScoreVisitor::operator()(const IFrame::ThreeStrikes& score) {
    total += 30;
    while (pendingBonuses--)
        total += 10;
}

FrameScoreVisitor::operator uint_fast16_t() const {
    return total;
}
