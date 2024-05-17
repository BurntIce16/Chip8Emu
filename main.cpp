#include <SDL2/SDL.h>
#include <iostream>
#include "chip8.h"
#include <chrono>
#include <thread>

Chip8 chip8;

void runAtFrequency(unsigned int hz)
{
    using namespace std::chrono;
    auto interval = duration<double>(1.0 / hz);
    auto next = steady_clock::now() + interval;

    while (true)
    {
        // Emulator Logic
        chip8.emulateCycle();


        //if draw flag is set update the screen:
        //if(chip8.getGfx()){
        //    drawGraphics();
        //}

        //set the keys
        //1,2,3,4
        //Q,W,E,R
        //A,S,D,F
        //Z,X,C,V 
        //chip8.setKey();




        //Clock logic
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
    runAtFrequency(frequency);

    /*
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
            return 1;
        }

        SDL_Window *win = SDL_CreateWindow("Hello SDL", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
        if (win == NULL) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (ren == NULL) {
            SDL_DestroyWindow(win);
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return 1;
        }

        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderClear(ren);
        SDL_RenderPresent(ren);

        SDL_Delay(2000);

        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();

        */

    return 0;
}
