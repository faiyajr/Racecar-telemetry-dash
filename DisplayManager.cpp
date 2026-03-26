#include "DisplayManager.h"
#include <iostream>

DisplayManager::DisplayManager(int width, int height) {
    
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

    // jse same window creation as sdl_test.cpp
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
    
    // hide the mouse cursor
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

void DisplayManager::drawColoredRect(int x, int y, int width, int height, int r, int g, int b) {
    SDL_Rect section = {x, y, width, height};

    // fill with specified color
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &section);

    // black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &section);
}

void DisplayManager::drawErrorPopup(const std::string& message, int x, int y, int width, int height, bool flash) {
    if (flash) {
        // get current time
        Uint32 currentTime = SDL_GetTicks();
        
        // flash on/off every 600ms (slower than brightness pulse)
        bool showPopup = ((currentTime / 600) % 2) == 0;
        
        // iff flash is OFF, don't draw anything (popup disappears completely)
        if (!showPopup) {
            return;
        }
        
        // when popup IS visible, also pulse between bright and dark red
        bool brightRed = ((currentTime / 300) % 2) == 0;
        
        SDL_Rect popup = {x, y, width, height};
        if (brightRed) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // bright red
        } else {
            SDL_SetRenderDrawColor(renderer, 180, 0, 0, 255);  // dark red
        }
        SDL_RenderFillRect(renderer, &popup);
    } else {
        // solid red when not flashing
        SDL_Rect popup = {x, y, width, height};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // bright red
        SDL_RenderFillRect(renderer, &popup);
    }
    
    // thick black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect popup = {x, y, width, height};
    SDL_RenderDrawRect(renderer, &popup);
    
    // inner border for emphasis
    SDL_Rect innerBorder = {x + 2, y + 2, width - 4, height - 4};
    SDL_RenderDrawRect(renderer, &innerBorder);
}

void DisplayManager::drawFilledBar(int x, int y, int width, int height, int fillPercent) {
    if (fillPercent < 0) {
    fillPercent = 0;
    }

    if (fillPercent > 100) {
        fillPercent = 100;
    }
    
    SDL_Rect outline = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &outline);
    
    int fillWidth = (width - 4) * fillPercent / 100;
    
    // filled portion(green)
    SDL_Rect fill = {x + 2, y + 2, fillWidth, height - 4};
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(renderer, &fill);
}

void DisplayManager::endFrame() {
    SDL_RenderPresent(renderer);
}