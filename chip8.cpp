#include "chip8.h"
#include "chip8gfx.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ctime>
#include <sstream>



Chip8::Chip8()
{
    enableLogging();

    if (loggingEnabled)
    {
        Logger logger = Logger("log.jsonl");
    }
}

void Chip8::initialize()
{
    pc = 0x200; // PC starts at 0x200
    opcode = 0; // clear opcode
    I = 0;      // reset index
    sp = 0;     // reset stack pointer

    // clear stack
    memset(stack, 0, sizeof(stack));

    // clear registers V0-Vf
    memset(V, 0, sizeof(V));

    // clear memory
    memset(memory, 0, sizeof(memory));

    // Load fontset
    for (int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }

    // reset timers
    delay_timer = 0;
    sound_timer = 0;

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
    Chip8::bufferSize = bufferSize;
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
    opcode = memory[pc] << 8 | memory[pc + 1]; // value of first memory address, shifted 8 to the left and concatenated with the seccond value

    printf("Executing opcode: 0x%X at PC: %X\n", opcode, pc);

/*     if (loggingEnabled)
    {
        std::ostringstream logStream;
        logStream << "\"Opcode\": \"0x" << std::hex << std::uppercase << opcode 
                  << "\", \"PC\": \"0x" << std::hex << std::uppercase << pc << "\"";
        logger.writeLog(logStream.str().c_str());
    } */

    // Decode & Execute Opcode
    switch (opcode & 0xF000) // only need 12 bits so mask the rest
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0:
            // Clear the display
            std::cout << "Clear the display" << std::endl;
            gfxPtr->clearDisplay();
            pc += 2;
            break;
        case 0x00EE:
            // Return from subroutine
            sp--;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            // std::cout << "Call machine code routine (not needed on most machines)" << std::endl;
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
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x4000: // Skip next instruction if VX does not equal NN
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x5000: // Skip the next instruction if Vx equals Vy
        // 0x5XY0
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x6000: // set Vx to NN
        // 6XNN
        V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
        pc += 2;
        break;
    case 0x7000: // add NN to VX (dont set carry flag)
        // 7XNN
        V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
        pc += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F) // mask for just last few bits
        {
        case 0x0000: // Set Vx to Vy
            // 0x8XY0
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0001: // Set Vx to (Vx or Vy)
            // 0x8XY1
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0002: // Set Vx to (Vx and Vy)
            // 8XY2
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0003: // Set Vx to (Vx xor Vy)
            // 8XY3
            V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4]);
            pc += 2;
            break;
        case 0x0004: // 8XY4: Add VY to VX, set VF to 1 if there's a carry, otherwise 0
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;

            bool carry = false;

            if (V[y] > (0xFF - V[x]))
            {               // Check for overflow
                carry = true; // There is a carry
            }
            else
            {
                carry = false; // There is no carry
            }
            V[x] += V[y]; // Add VY to VX   
            V[0xF] = carry ? 1 : 0; // Set the carry flag
            pc += 2;
            break;
        }
        case 0x0005: // 8XY5: Vx = Vx - Vy, set VF to 0 if there's a borrow, 1 if there isn't
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;

            bool borrow = false;

            if (V[x] >= V[y])
            {
                borrow = true; // There is no borrow
            }
            else
            {
                borrow = false; // There is a borrow
            }
            V[x] -= V[y];
            V[0xF] = borrow ? 1 : 0; // Set the carry flag based on the borrow
            pc += 2;
            break;
        }
        case 0x0006: // 8XY6: Store the least significant bit of Vx in Vf and shift Vx to the right by 1
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            V[0xF] = V[x] & 0x1; // Store the least significant bit in Vf
            V[x] >>= 1;          // Shift Vx to the right by 1
            pc += 2;
            break;
        }
        case 0x0007: // 8XY7: Vx = Vy - Vx, set VF to 0 if there's a borrow, 1 if there isn't
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if (V[y] > V[x])
            {
                V[0xF] = 1; // There is no borrow
            }
            else
            {
                V[0xF] = 0; // There is a borrow
            }
            V[x] = V[y] - V[x];
            pc += 2;
            break;
        }
        case 0x000E: // 8XYE: Store the most significant bit of Vx in Vf and shift Vx to the left by 1
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            V[0xF] = (V[x] & 0x80) >> 7; // Store the most significant bit in Vf
            V[x] <<= 1;                  // Shift Vx to the left by 1
            pc += 2;
            break;
        }
        default:
            printf("Unknown opcode: 0x%X at PC: %d\n", opcode, pc);
            break;
        }
        break;
    case 0x9000: // 9XY0: Skip the next instruction if Vx does not equal Vy
    {
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;
        if (V[x] != V[y])
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    }
    case 0xA000: // ANNN: Set I to the address NNN
    {
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    }
    case 0xB000: // BNNN: Jump to the address NNN plus V0
    {
        pc = (opcode & 0x0FFF) + V[0];
        break;
    }
    case 0xC000: // CXNN: Set Vx to the result of a bitwise and operation on a random number (typically 0 to 255) and NN
    {
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t nn = opcode & 0x00FF;
        V[x] = (rand() % 256) & nn;
        pc += 2;
        break;
    }
    case 0xD000: //	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels
    {            // 0xDXYN
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short n = opcode & 0x000F;
        unsigned short pixel;

        V[0xF] = 0; // reset Vf register

        for (int ycount = 0; ycount < n; ycount++)
        {
            pixel = memory[I + ycount];
            for (int xcount = 0; xcount < 8; xcount++)
            {
                if ((pixel & (0x80 >> xcount)) != 0)
                {
                    if (gfx[(x + xcount + ((y + ycount) * 64))] == 1)
                    {
                        V[0xF] = 1; // set flag to true
                    }
                    gfx[x + xcount + ((y + ycount) * 64)] ^= 1;
                }
            }
        }
        drawFlag = true;
        pc += 2;
        break;
    }
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // EX9E: Skip the next instruction if the key stored in VX is pressed
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            if (key[V[x]] != 0)
            {
                pc += 4; // Skip the next instruction
            }
            else
            {
                pc += 2;
            }
            break;
        }
        case 0x00A1: // EXA1: Skip the next instruction if the key stored in VX is not pressed
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            if (key[V[x]] == 0)
            {
                pc += 4; // Skip the next instruction
            }
            else
            {
                pc += 2;
            }
            break;
        }
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // FX07: Set VX to the value of the delay timer
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            V[x] = delay_timer;
            pc += 2;
            break;
        }
        case 0x000A: // FX0A: A key press is awaited, and then stored in VX
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            bool keyPressDetected = false;

            //Get the first (numerical) key pressed
            for (uint8_t i = 0; i < 16; ++i)
            {
                if (key[i] != 0)
                {
                    V[x] = i;
                    keyPressDetected = true;
                    break;
                }
            }

            if (!keyPressDetected)
            {
                return; // Don't increment pc, wait for key press
            }

            pc += 2; // key press has been detected, increment program counter
            break;
        }
        case 0x0015: // FX15: Set the delay timer to VX
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            delay_timer = V[x];
            pc += 2;
            break;
        }
        case 0x0018: // FX18: Set the sound timer to VX
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            sound_timer = V[x];
            pc += 2;
            break;
        }
        case 0x001E: // FX1E: Adds VX to I
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            I += V[x];
            pc += 2;
            break;
        }
        case 0x0029: // FX29: Set I to the location of the sprite for the character in VX
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            I = V[x] * 5; // Each character is 5 bytes long
            pc += 2;
            break;
        }
        case 0x0033: // FX33: Store the binary-coded decimal representation of VX
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = (V[x] % 100) % 10;
            pc += 2;
            break;
        }
        case 0x0055: // FX55: Store registers V0 through VX in memory starting at location I
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            for (uint8_t i = 0; i <= x; ++i)
            {
                memory[I + i] = V[i];
            }
            // I += x + 1; // On the original interpreter, I is incremented by x + 1 after this operation.
            pc += 2;
            break;
        }
        case 0x0065: // FX65: Read registers V0 through VX from memory starting at location I
        {
            uint8_t x = (opcode & 0x0F00) >> 8;
            for (uint8_t i = 0; i <= x; ++i)
            {
                V[i] = memory[I + i];
            }
            // I += x + 1; // On the original interpreter, I is incremented by x + 1 after this operation.
            pc += 2;
            break;
        }
        }
        break;

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
            printf("Beep!\n");
        }
        --sound_timer;
    }

    // update debug window
    gfxPtr->renderDebugInfo();
}

// Set the state of the keypad
void Chip8::setKey(int key, int value)
{
    Chip8::key[key] = value;
}

void Chip8::clearKeys()
{
    memset(key, 0, sizeof(key));
}


void Chip8::handleEvents(bool &running, bool &restart)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
            std::cout << "Quit" << std::endl;
            if (loggingEnabled)
            {
                //logFile.close();
            }
        }
        else if (event.type == SDL_KEYDOWN)
        {
            // TODO This is beyond cursed and needs a better solution
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                restart = true;
                running = false;
                //clear any textures
                std::cout << "Restart" << std::endl;
            }

            // This is so fucked, im gonna implement a new system using an array of enums (SDLK_...) and itterate using a for loop
            // Why the *fuck* did I every write it like this
            


            else if (event.key.keysym.sym == SDLK_1)
            {
                setKey(0, 1);
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                setKey(1, 1);
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                setKey(2, 1);
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                setKey(3, 1);
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                setKey(4, 1);
            }
            else if (event.key.keysym.sym == SDLK_w)
            {
                setKey(5, 1);
            }
            else if (event.key.keysym.sym == SDLK_e)
            {
                setKey(6, 1);
            }
            else if (event.key.keysym.sym == SDLK_r)
            {
                setKey(7, 1);
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                setKey(8, 1);
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                setKey(9, 1);
            }
            else if (event.key.keysym.sym == SDLK_d)
            {
                setKey(10, 1);
            }
            else if (event.key.keysym.sym == SDLK_f)
            {
                setKey(11, 1);
            }
            else if (event.key.keysym.sym == SDLK_z)
            {
                setKey(12, 1);
            }
            else if (event.key.keysym.sym == SDLK_x)
            {
                setKey(13, 1);
            }
            else if (event.key.keysym.sym == SDLK_c)
            {
                setKey(14, 1);
            }
            else if (event.key.keysym.sym == SDLK_v)
            {
                setKey(15, 1);
            }
        }
        // Add more event handling here if needed, e.g., keypresses
    }
}

void Chip8::enableLogging()
{
    loggingEnabled = true;
}

void Chip8::setGFX(Chip8GFX* gfxPtr) {
    this->gfxPtr = gfxPtr;
}