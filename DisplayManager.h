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
    void drawColoredRect(int x, int y, int width, int height, int r, int g, int b);
    void drawFilledBar(int x, int y, int width, int height, int fillPercent);
    void drawErrorPopup(const std::string& message, int x, int y, int width, int height, bool flash);
    void endFrame();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};