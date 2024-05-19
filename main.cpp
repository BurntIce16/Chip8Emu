#include <SDL2/SDL.h>
#include <iostream>
#include "chip8.h"
#include <chrono>
#include <thread>

Chip8 chip8;

void runAtFrequency(unsigned int hz)
{
    using namespace std::chrono;
    static auto interval = duration<double>(1.0 / hz);
    static auto next = steady_clock::now() + interval;

    // Emulator Logic
    chip8.emulateCycle();

    // if draw flag is set update the screen:
    if (chip8.drawFlag)
    {
        chip8.drawGraphics();
        chip8.drawFlag = false;
    }

    // set the keys
    // 1,2,3,4
    // Q,W,E,R
    // A,S,D,F
    // Z,X,C,V
    // chip8.setKey();

    // Clock logic
    auto now = steady_clock::now();
    if (now > next)
    {
        std::cerr << "Warning: Overrun detected!" << std::endl;
        next = now + interval;
    }
    else
    {
        std::this_thread::sleep_until(next);
        next += interval;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Correct use is ./chip8 <gamePath>" << std::endl;
        return 0;
    }
    else
    {
        chip8.initialize();
        chip8.loadGame(argv[1]);
    }

    unsigned int frequency = 60; // 60 Hz
    bool running = true;

    while (running)
    {
        chip8.handleEvents(running);
        runAtFrequency(frequency);
    }

    chip8.cleanUp();

    return 0;
}
