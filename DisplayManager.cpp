#include "DisplayManager.h"
#include <iostream>

DisplayManager::DisplayManager(int width, int height) {
    // REMOVED: SDL_setenv("SDL_VIDEODRIVER", "KMSDRM", 1);
    // REASON: sdl_test.cpp works without this, so let SDL auto-detect the driver
    
    // check sdl init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL initialization failed");
    }
    
    // check font lib init
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        throw std::runtime_error("TTF initialization failed");
    }

    // CHANGED: Use same window creation as sdl_test.cpp
    window = SDL_CreateWindow(
        "Race Dash",
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,  
        width, height,
        SDL_WINDOW_SHOWN  
    );

    // check window creation
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("Window creation failed");
    }

    // prevent tearing
    renderer = SDL_CreateRenderer(
        window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    // check renderer creation
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("Renderer creation failed");
    }
    
    // Hide the mouse cursor
    SDL_ShowCursor(SDL_DISABLE);
    
    std::cout << "DisplayManager initialized successfully" << std::endl;
}

DisplayManager::~DisplayManager() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void DisplayManager::beginFrame() {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    SDL_RenderClear(renderer);
}

void DisplayManager::drawText(const std::string& text, int x, int y, TTF_Font* font) {
    // check font
    if (!font) {
        std::cerr << "Font is null, cannot render text: " << text << std::endl;
        return;
    }
    
    SDL_Color black = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), black);
    
    // check if text surface was created
    if (!surface) {
        std::cerr << "Failed to render text surface: " << TTF_GetError() << std::endl;
        return;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void DisplayManager::drawRect(int x, int y, int width, int height) {
    SDL_Rect section = {x, y, width, height};

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderFillRect(renderer, &section);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &section);
}

void DisplayManager::endFrame() {
    SDL_RenderPresent(renderer);
}