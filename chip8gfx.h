#ifndef CHIP8GFX_H
#define CHIP8GFX_H

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <utility>


class Chip8; // Forward declaration of Chip8 class

class Chip8GFX
{
public:
    // Constructor
    Chip8GFX(Chip8* chip8); // Constructor takes a pointer to Chip8

    // Initialize the system, clear the memory, registers, and screen
    void initialize();

    void drawGraphics();

    void cleanUp();

    // Debugging functions
    void initializeDebugWindow();
    void renderDebugInfo();
    void renderText(SDL_Renderer *renderer, int x, int y, const char *text, SDL_Color color);


    void clearDisplay();

private:
    Chip8* chip8; // Store pointer to Chip8 for access

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture* gfxTexture = nullptr;

    SDL_Window *debugWindow;
    SDL_Renderer *debugRenderer;
    TTF_Font *font; // Font for rendering text


    // Textures for registers
    std::string lastRegisterText[16];
    SDL_Texture* registerTextures[16] = {nullptr};

    std::vector<std::string> lastMemText;
    std::vector<SDL_Texture*> memTextures;

    unsigned char* display;

};

#endif // CHIP8GFX_H
