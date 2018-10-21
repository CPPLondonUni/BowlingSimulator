#ifndef BOWLINGSIMULATOR_MOCKFRAME_H
#define BOWLINGSIMULATOR_MOCKFRAME_H

#include "interface/IFrame.h"
#include "trompeloeil.hpp"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;
class MockFrame : public IFrame {
public:
    MAKE_MOCK1(Bowled, void(const IPinSet&), override);
    MAKE_CONST_MOCK0(TurnEnded, bool(), override);
    MAKE_CONST_MOCK0(Score, Score_t(), override);
};

#endif //BOWLINGSIMULATOR_MOCKFRAME_H
