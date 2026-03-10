#pragma once
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>

class DisplayManager {
public:
    DisplayManager(int width, int height);
    ~DisplayManager();

    void beginFrame();
    void drawText(const std::string& text, int x, int y, TTF_Font* font);
    void drawRect(int x, int y, int width, int height);
    void endFrame();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};