cmake_minimum_required(VERSION 3.12)
project(BowlingSimulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(BowlingSimulator src/main.cpp include/PinSet.h src/PinSet.cpp include/interface/IPinSet.h test/mock/MockPinSet.h include/Frame.h src/Frame.cpp include/interface/IFrame.h include/FinalFrame.h src/FinalFrame.cpp test/mock/MockFrame.h include/FrameSet.h src/FrameSet.cpp)
add_executable(TestBowlingSimulator test/testmain.cpp test/TestPinSet.cpp include/PinSet.h src/PinSet.cpp test/TestFrame.cpp src/Frame.cpp test/TestFinalFrame.cpp include/interface/IFrame.h src/FinalFrame.cpp test/TestFrameSet.cpp include/FrameSet.h src/FrameSet.cpp)
target_include_directories(TestBowlingSimulator PRIVATE include/ test/)
target_include_directories(BowlingSimulator PRIVATE include/)
target_link_libraries(TestBowlingSimulator INTERFACE "-fuse-ld=gold")
