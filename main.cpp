#include "dash.hpp"
#include "main.hpp"
#include "candriver.hpp"
#include "CanBus.hpp"
#include "DisplayManager.h"
#include "DashboardUI.h"

#include <SDL.h>

#include <cstdio>
#include <iostream>
#include <thread>
#include <chrono>

volatile bool running = true;

void handle_signal(int) {
    running = false;
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    
    DisplayManager display(800, 480);
    DashboardUI ui;
    CAN_Handler handle;
    CanBus bus;
    
    if (!handle.init("can0")) {
        std::cerr << "Failed to initialize CAN interface 'can0'" << std::endl;
        std::cerr << "Ensure CAN interface is up: sudo ip link set can0 up type can bitrate 500000" << std::endl;
        return 1;
    }

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    SDL_Event event;
    int page = 1;

    std::cout << "Dashboard is running. Press Q/ESC to exist and the Spacebar to change pages." << std::endl;

    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                
                if (key == SDLK_ESCAPE || key == SDLK_q) {
                    running = false;
                }
                
                if (key == SDLK_SPACE) {
                    page++;
                    std::cout << "Page changed to " << (page % 2 == 1 ? "1" : "2") << std::endl;
                }
            }
        }

        handle.read_and_decode_once();
        const auto& bus = handle.bus();

        if (bus.wheel.btn_page){
            ++page;
        }

        display.beginFrame();

        if (page % 2 == 1){
            ui.drawPageOne(display, bus);
        }
        
        display.endFrame();

        auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - frameStart
        ).count();

        if (frameDelay > frameTime) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(frameDelay - frameTime)
            );
        }
    }
    
    return 0;
}