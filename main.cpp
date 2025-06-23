#include <SDL2/SDL.h>
#include <iostream>
#include "chip8.h"
#include "chip8gfx.h"
#include <chrono>
#include <thread>

Chip8 chip8;
Chip8GFX gfx(&chip8);



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
        gfx.drawGraphics();
        chip8.drawFlag = false;
    }

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

    chip8.setGFX(&gfx);


    unsigned int frequency = 60; // 60 Hz is default, may change for testing

    while (true)
    {
        chip8.initialize();
        if (!chip8.loadGame(argv[1]))
        {
            std::cerr << "Failed to load game!" << std::endl;
            return 1;
        }

        bool running = true;
        bool restart = false;

        while (running)
        {
            chip8.clearKeys();
            chip8.handleEvents(running, restart);
            runAtFrequency(frequency);
        }

        gfx.cleanUp();

        if (!restart)
        {
            break; // Exit the loop if not restarting
        }
    }

    return 0;
}
