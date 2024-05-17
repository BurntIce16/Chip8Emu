#include "chip8.h"
#include <iostream>
#include <cstdio>
#include <iostream>
#include <cstdlib>

Chip8::Chip8()
{
    // std::cout<<"test123"<<std::endl;
}

void Chip8::initialize()
{
    pc = 0x200; // PC starts at 0x200
    opcode = 0; // clear opcode
    I = 0;      // reset index
    sp = 0;     // reset stack pointer

    // clear display
    // clear stack
    for (int i : stack)
    {
        stack[i] = 0;
    }
    // clear registers V0-Vf
    // clear memory

    // Load fontset
    for (int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }

    // reset timers
}

// Load the game into the memory
bool Chip8::loadGame(const char *filename)
{
    // use fopen in binary mode to read the file

    FILE *file = fopen(filename, "rb");
    if (file == nullptr)
    {
        std::perror("Error opening file for reading");
        return false;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long bufferSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer to hold the file contents
    char *buffer = new char[bufferSize];
    if (buffer == nullptr)
    {
        std::cerr << "Memory allocation failed" << std::endl;
        fclose(file);
        return false;
    }

    size_t bytesRead = fread(buffer, 1, bufferSize, file);
    if (bytesRead != static_cast<size_t>(bufferSize))
    {
        std::cerr << "Error reading file" << std::endl;
        delete[] buffer;
        fclose(file);
        return false;
    }

    // Copy the buffer into the Chip8 memory starting at 0x200 (512)
    for (long i = 0; i < bufferSize; ++i)
    {
        memory[i + 512] = buffer[i];
    }

    // Clean up
    delete[] buffer;
    fclose(file);

    return true;
}

// Emulate one cycle of the system
void Chip8::emulateCycle()
{
    // Fetch Opcode

    // we need to fetch two bytes per opcode, so we preform the following opperation
    opcode = memory[pc] << 8 | memory[pc + 1]; // value of first memory address, shifted 8 to the left and concatinated with the seccond value

    // Decode & Execute Opcode
    switch (opcode & 0xF000) // only need 12 bits so mask the rest
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            // Clear the display
            std::cout << "Clear the display" << std::endl;
            break;
        case 0x00EE:
            // Return from subroutine
            std::cout << "Return from subroutine" << std::endl;
            break;
        default:
            std::cout << "Call machine code routine (not needed on most machines)" << std::endl;
            break;
        }
        break;
    case 0x1000:
        // Jump to address NNN
        pc = opcode & 0x0FFF; // set pc to NNN by masking off the first un needed bits
        break;
    case 0x2000:        // call subroutine at NNN
        stack[sp] = pc; // add current pc to stack
        sp++;
        pc = opcode & 0x0FFF; // set pc to NNN by masking off the first un needed bits
        break;
    case 0x3000: // Skip next instruction if VX equals NN
        // 0x3XNN
        break;
    case 0x4000:
        // Skip next instruction if VX does not equal NN
        break;
    case 0x5000:
        // 0x5XY0
        // Skip the next instruction if Vx equals Vy
        break;
    case 0x6000:
        // set Vx to NN
        break;
    case 0x7000:
        // add NN to VX (dont set carry flag)
        break;
    case 0x8000:
        switch (opcode & 0x000F) // mask for just last few bits
        {
        case 0x0000: // Set Vx to Vy
            // 0x8XY0
            break;
        case 0x0001: // Set Vx to (Vx or Vy)
            // 0x8XY1
            break;
        case 0x0002: // Set Vx to (Vx and Vy)
            // 8XY2
            break;
        case 0x0003: // Set Vx to (Vx xor Vy)
            // 8XY3
            break;
        case 0x0004: // Add Vy to Vx, if theres overflow set Vf to 1, or 0 if there isnt
            // 8XY4
            break;
        case 0x0005: // Vy is subtracted from Vx, set Vf to 0 if there is underflow, set Vf to 1 if there isnt
            // 0x8XY5
            break;
        case 0x0006: // Store the least significant bit of Vx in Vf and shift Vx to the right by 1
            break;
        case 0x0007: // Set Vx to (Vy minus Vx), set Vf to 0 if there is underflow, set Vf to 1 if there isnt
            break;
        case 0x000E: // store the most significant bit of Vx in Vf and shift Vx to the left by 1
            break;
        }
        break;
    case 0x9000: // Skip the next isntruction if Vx does not equal Vy
        break;
    case 0xA000: // Set I to the address NNN
        break;
    case 0xB000: // Jump to the address NNN plus V0
        break;
    case 0xC000: //Set Vx to the result of a bitwise and operation on a random number (typicially 0 to 255) and NN
        break;
    case 0xD000: //
    default:
        printf("Unknown opcode: 0x%X at PC: %d\n", opcode, pc);
    }

    // Update timers
    if (delay_timer > 0)
    {
        --delay_timer;
    }

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
        {
            printf("Beep!");
        }
        --sound_timer;
    }

    pc += 2; // increment program counter
}

/*
// Get the current state of the display
unsigned char *getGfx()
{

}
*/

// Set the state of the keypad
void Chip8::setKey(int key, int value)
{
    Chip8::key[key] = value;
}