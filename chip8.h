#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include "logger.h"
#include <string>
#include <vector>
#include <utility>
#include "chip8gfx.h"

class Chip8GFX; // Forward declaration of Chip8GFX class

class Chip8
{
public:
    // Constructor
    Chip8();

    // Initialize the system, clear the memory, registers, and screen
    void initialize();

    // Load the game into the memory
    bool loadGame(const char *filename);

    // Emulate one cycle of the system
    void emulateCycle();

    // Set the state of the keypad
    void setKey(int key, int value);


    //clear all keys
    void clearKeys();

    // Update internal timers (sound/etc)
    void tickTimers();

    bool drawFlag = false;

    void handleEvents(bool &running, bool &restart);

    void enableLogging();

    void setGFX(Chip8GFX* gfxPtr); // Add this setter

    unsigned char* getDisplayBuffer() { return gfx; }




    // --- Getters for private members ---
    unsigned char* getV() { return V; }
    unsigned short getI() { return I; }
    unsigned short getPC() { return pc; }
    unsigned char getDelayTimer() { return delay_timer; }
    unsigned char getSoundTimer() { return sound_timer; }
    unsigned char* getMemory() { return memory; }
    unsigned long getBufferSize() { return bufferSize; }
    unsigned short getSP() { return sp; }



private:
    // -- system state variables --
    unsigned short opcode; // current opcode, two bytes long

    unsigned char gfx[64 * 32]; // 64x32 pixel monochrome display, each pixel is either on(1) or off(0)

    unsigned char memory[4096]; // 4KB memory

    unsigned char V[16]; // 16 8-bit registers
    // ogranised from V0 to VF
    // VF is used as a carry flag for some instructions
    // each register is 8 bits long

    unsigned short I;  // 16-bit register used to store memory addresses (index register)
    unsigned short pc; // 16-bit register used to store the currently executing address (program counter)

    /*
    System memory map:
    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM
    */

    unsigned char delay_timer; // timer that counts at 60Hz, when set above 0, it will count down to 0
    unsigned char sound_timer; // timer that counts at 60Hz, when set above 0, it will count down to 0 and beep

    unsigned short stack[16]; // 16 levels of stack to store return addresses when subroutines are called
    unsigned short sp;        // stack pointer

    unsigned char key[16]; // hex keypad with 16 keys, each key is either pressed or not pressed (1 or 0)

    long bufferSize = 0;


    // -- constants and fontset --

    // Fontset for the Chip-8 system, 80 bytes long
    unsigned char chip8_fontset[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };



    // -- logging --

    bool loggingEnabled = false;
    Logger logger = Logger("log.jsonl");


    // -- graphics --

    // Pointer to Chip8GFX for graphics operations
    Chip8GFX* gfxPtr = nullptr;
    
};

#endif // CHIP8_H
