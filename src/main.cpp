#include <SDL2/SDL.h>
#include "CHIP8.h"
#include <iostream>

using namespace std;

bool sdl_init(SDL_Window* window, SDL_Renderer* renderer, int scale, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("CHIP-8_Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * scale, height * scale, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    SDL_Window* window;
    SDL_Renderer* renderer;
    const int SCALE = 16;
    bool quit = false;
    SDL_Event event;

    CHIP8* chip8 = new CHIP8();

    if (!sdl_init(window, renderer, SCALE, chip8->getWidth(), chip8->getHeight())) {
        return -1;
    }

    chip8->memory_init();

    while (!quit) {
        chip8->process_instruction();

        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    return 0;
}