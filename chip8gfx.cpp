#include "chip8gfx.h"
#include "chip8.h"
#include <iostream>

Chip8GFX::Chip8GFX(Chip8* chip8Ptr) : chip8(chip8Ptr) {

    // Initialize display buffer
    display = chip8->getDisplayBuffer();
    if (display == nullptr) {
        std::cerr << "Error: Display buffer is null!" << std::endl;
        exit(1);
    }
    
    // clear display
    memset(display, 0, 64 * 32 * sizeof(unsigned char));

    // --- DEBUG WINDOW SETUP ---

    // Initialize SDL first
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Then initialize TTF
    if (TTF_Init() != 0)
    {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Load a font
    font = TTF_OpenFont("KodeMono-VariableFont_wght.ttf", 16); // Adjust path and size as needed
    if (font == nullptr)
    {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Now create both windows after SDL is initialized
    initializeDebugWindow();

    // Create main window for graphics
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // Create a texture for graphics on the game window
    gfxTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (gfxTexture == nullptr)
    {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

}

void Chip8GFX::initializeDebugWindow()
{
    // Create a window for debugging
    debugWindow = SDL_CreateWindow("CHIP-8 Debugger", SDL_WINDOWPOS_CENTERED + 320, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (debugWindow == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Create a renderer for the debug window
    debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
    if (debugRenderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(debugWindow);
        SDL_Quit();
        exit(1);
    }
}

void Chip8GFX::drawGraphics()
{
    // Prepare a pixel buffer (RGBA)
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; ++i) {
        pixels[i] = display[i] ? 0xFFFFFFFF : 0x000000FF; // White or Black (RGBA)
    }

    // Update the texture with the pixel buffer
    SDL_UpdateTexture(gfxTexture, nullptr, pixels, 64 * sizeof(uint32_t));

    // Clear the renderer
    SDL_RenderClear(renderer);

    // Set destination rect for scaling
    SDL_Rect destRect = {0, 0, 640, 320}; // Scale 64x32 to 640x320

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, gfxTexture, nullptr, &destRect);

    // Present the renderer
    SDL_RenderPresent(renderer);

    // update debug window
    renderDebugInfo();
}

void Chip8GFX::cleanUp()
{
    // Cleanup and exit
    SDL_DestroyTexture(gfxTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(debugRenderer);
    SDL_DestroyWindow(debugWindow);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}


void Chip8GFX::renderDebugInfo() {
    // Access values via chip8 pointer, e.g. chip8->V, chip8->I, etc.
    uint8_t* V = chip8->getV();
    uint16_t I = chip8->getI();
    uint16_t pc = chip8->getPC();
    uint8_t sp = chip8->getSP();
    uint8_t delay_timer = chip8->getDelayTimer();
    uint8_t sound_timer = chip8->getSoundTimer();
    uint8_t memory[4096];
    memcpy(memory, chip8->getMemory(), 4096);

    // Clear the debug window
    SDL_SetRenderDrawColor(debugRenderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(debugRenderer);

    SDL_Color white = {255, 255, 255, 255}; // White color for normal text

    // --- Register caching ---
    for (int i = 0; i < 16; ++i)
    {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "V[%X]: %02X", i, V[i]);
        std::string text(buffer);
        if (!registerTextures[i] || lastRegisterText[i] != text) {
            if (registerTextures[i]) SDL_DestroyTexture(registerTextures[i]);
            SDL_Surface *surface = TTF_RenderText_Solid(font, buffer, white);
            registerTextures[i] = SDL_CreateTextureFromSurface(debugRenderer, surface);
            SDL_FreeSurface(surface);
            lastRegisterText[i] = text;
        }
        // Render cached texture
        int text_width = 0, text_height = 0;
        TTF_SizeText(font, buffer, &text_width, &text_height);
        SDL_Rect destRect = {10, 20 * i, text_width, text_height};
        SDL_RenderCopy(debugRenderer, registerTextures[i], nullptr, &destRect);
    }

    // --- Other registers (no caching for simplicity, but can be added similarly) ---
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "I: %04X", I);
    renderText(debugRenderer, 10, 20 * 16, buffer, white);

    snprintf(buffer, sizeof(buffer), "PC: %04X", pc);
    renderText(debugRenderer, 10, 20 * 17, buffer, white);

    snprintf(buffer, sizeof(buffer), "SP: %02X", sp);
    renderText(debugRenderer, 10, 20 * 18, buffer, white);

    snprintf(buffer, sizeof(buffer), "Delay Timer: %02X", delay_timer);
    renderText(debugRenderer, 10, 20 * 19, buffer, white);

    snprintf(buffer, sizeof(buffer), "Sound Timer: %02X", sound_timer);
    renderText(debugRenderer, 10, 20 * 20, buffer, white);

    // --- Memory instruction caching ---
    int memCount = (chip8->getBufferSize() + 1) / 2;
    if ((int)memTextures.size() != memCount) {
        // Resize and clear if ROM size changes
        for (auto tex : memTextures) if (tex) SDL_DestroyTexture(tex);
        memTextures.resize(memCount, nullptr);
        lastMemText.resize(memCount, "");
    }

    int x = 200;
    int y = 10;
    const int padding = 10;
    const int windowWidth = 800;

    for (int i = 0, addr = 0x200; static_cast<size_t>(addr) < (0x200 + chip8->getBufferSize()); i++, addr += 2)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%04X: %02X%02X", addr, memory[addr], memory[addr + 1]);
        std::string text(buffer);

        if (!memTextures[i] || lastMemText[i] != text) {
            if (memTextures[i]) SDL_DestroyTexture(memTextures[i]);
            SDL_Surface *surface = TTF_RenderText_Solid(font, buffer, white);
            memTextures[i] = SDL_CreateTextureFromSurface(debugRenderer, surface);
            SDL_FreeSurface(surface);
            lastMemText[i] = text;
        }

        int textWidth = 0, textHeight = 0;
        TTF_SizeText(font, buffer, &textWidth, &textHeight);
        SDL_Rect destRect = {x, y, textWidth, textHeight};
        SDL_RenderCopy(debugRenderer, memTextures[i], nullptr, &destRect);

        x += textWidth + padding;
        if (x + textWidth > windowWidth - padding)
        {
            x = 200;
            y += 20;
        }
    }

    // --- Highlight the current instruction ---
    x = 200;
    y = 10;
    for (int i = 0, addr = 0x200; static_cast<size_t>(addr) < (0x200 + chip8->getBufferSize()); i++, addr += 2)
    {
        if (addr == pc)
        {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%04X: %02X%02X", addr, memory[addr], memory[addr + 1]);
            SDL_Color highlight = {255, 0, 0, 255};
            renderText(debugRenderer, x, y, buffer, highlight);
            break;
        }
        int textWidth = 0;
        TTF_SizeText(font, lastMemText[i].c_str(), &textWidth, nullptr);
        x += textWidth + padding;
        if (x + textWidth > windowWidth - padding)
        {
            x = 200;
            y += 20;
        }
    }

    SDL_RenderPresent(debugRenderer);
}

void Chip8GFX::renderText(SDL_Renderer *renderer, int x, int y, const char *text, SDL_Color color)
{
    // Create surface from text
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text, color);
    if (surfaceMessage == nullptr)
    {
        std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    if (message == nullptr)
    {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surfaceMessage);
        return;
    }

    // Get width and height of the texture
    int text_width = surfaceMessage->w;
    int text_height = surfaceMessage->h;

    SDL_FreeSurface(surfaceMessage);

    // Define destination rectangle
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = text_width;
    destRect.h = text_height;

    // Render the text
    SDL_RenderCopy(renderer, message, nullptr, &destRect);

    // Clean up
    SDL_DestroyTexture(message);
}

void Chip8GFX::clearDisplay() {
    // Clear the graphics buffer
    memset(display, 0, 64 * 32 * sizeof(unsigned char));
    
    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer); // Present the cleared renderer
}