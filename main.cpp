#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "chip8gfx.h"

Chip8    chip8;
Chip8GFX gfx(&chip8);

void runAtFrequency(unsigned int hz, bool &running, bool &restart)
{
    using namespace std::chrono;
    static const duration<double> interval{1.0 / hz};
    static auto next = steady_clock::now() + interval;

    // 1) Poll and handle SDL events
    chip8.handleEvents(running, restart);
    if (!running) return;

    // 2) Emulate one CPU cycle
    chip8.emulateCycle();

    // 3) Draw to screen if requested
    if (chip8.drawFlag)
    {
        gfx.drawGraphics();
        chip8.drawFlag = false;
    }

    // 4) Wait until next tick
    auto now = steady_clock::now();
    if (now > next)
    {
        std::cerr << "Warning: Overrun detected!\n";
        next = now + interval;
    }
    else
    {
        std::this_thread::sleep_until(next);
        next += interval;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./chip8 <gamePath>\n";
        return 0;
    }

    chip8.setGFX(&gfx);
    constexpr unsigned int FPS = 500;

    while (true)
    {
        chip8.initialize();

        if (!chip8.loadGame(argv[1]))
        {
            std::cerr << "Failed to load game!\n";
            return 1;
        }

        bool running = true;
        bool restart = false;

        while (running)
        {
            runAtFrequency(FPS, running, restart);
        }

        gfx.cleanUp();

        if (!restart)
            break;
    }

    return 0;
}
