#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "chip8.h"
#include "chip8gfx.h"
#include "chip8audio.h"

Chip8    chip8;
Chip8GFX gfx(&chip8);


//Frequencies to run subsystems at
static constexpr double CPU_HZ    = 500.0;  // ~500–1000 typical
static constexpr double TIMER_HZ  = 60.0;
static constexpr double FRAME_HZ  = 120.0;

//Time steps
static constexpr double CPU_DT    = 1.0 / CPU_HZ;
static constexpr double TIMER_DT  = 1.0 / TIMER_HZ;
static constexpr double FRAME_DT  = 1.0 / FRAME_HZ;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./chip8 <gamePath>\n";
        return 0;
    }

    chip8.setGFX(&gfx);

    while (true)
    {
        chip8.initialize();
        beep_init();

        if (!chip8.loadGame(argv[1]))
        {
            std::cerr << "Failed to load game!\n";
            return 1;
        }

        bool running = true;
        bool restart = false;


        using clock = std::chrono::steady_clock;
        auto last = clock::now();
        double cpuAcc   = 0.0;
        double timerAcc = 0.0;
        double frameAcc = 0.0;

        while (running)
        {
            //Get time delta
            auto now = clock::now();
            double dt = std::chrono::duration<double>(now - last).count();
            last = now;

            //Accumulate time delta
            cpuAcc   += dt;
            timerAcc += dt;
            frameAcc += dt;

            // check events (updates keypad & may clear Fx0A wait)
            chip8.handleEvents(running, restart);
            if (!running) break;

            // --- run CPU at fixed rate; emulateCycle should early-return if Fx0A waiting
            while (cpuAcc >= CPU_DT) {
                chip8.emulateCycle();
                cpuAcc -= CPU_DT;
            }

           // timers driven by wall clock
            while (timerAcc >= TIMER_DT) {
                beep_set_on(false); //reset beeper
                chip8.tickTimers(); // decrement delay/sound timers here
                timerAcc -= TIMER_DT;
            }

            //render ~60 FPS
            if (frameAcc >= FRAME_DT) {
                if (chip8.drawFlag) {
                    gfx.drawGraphics();
                    chip8.drawFlag = false;
                }
                frameAcc -= FRAME_DT;
            }

            //tiny yield
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        gfx.cleanUp();

        if (!restart)
            break;
    }

    return 0;
}
