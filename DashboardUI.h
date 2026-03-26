#pragma once
#include "DisplayManager.h"
#include "CanBus.hpp"
#include <SDL_ttf.h>

class DashboardUI {
public:
    DashboardUI();
    ~DashboardUI();

    void drawPageOne(DisplayManager& display, const CanBus& bus);
    void drawPageTwo(DisplayManager& display, const CanBus& bus);

private:
    TTF_Font* fontSpeed;
    TTF_Font* fontData;
    TTF_Font* fontLabel;
};