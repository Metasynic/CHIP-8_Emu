#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "CHIP8.h"
#include <iostream>
#include <fstream>

using namespace std;

/* This function creates all the SDL objects, and throws an error if something goes wrong. */
bool sdl_init(SDL_Window* window, SDL_Renderer* renderer, const int scale, const int width, const int height) {
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

/* Draw the screen. This should happen 60 times a second. */
void draw_screen(CHIP8 &chip8, SDL_Renderer &renderer, const int scale, const int width, const int height) {
    SDL_SetRenderDrawColor(&renderer, 0, 0, 0, 255);
    SDL_RenderClear(&renderer);

    SDL_SetRenderDrawColor(&renderer, 255, 255, 255, 255);
    SDL_Rect rect;
    rect.w = scale;
    rect.h = scale;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (chip8.get_pixel(x, y)) {
                rect.x = x * scale;
                rect.y = y * scale;
                SDL_RenderFillRect(&renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(&renderer);
}

int main(int argc, char *argv[]) {
    /* Define the SDL objects and set other variables. Create the CHIP-8. */
    SDL_Window* window;
    SDL_Renderer* renderer;
    const int SCALE = 16;
    bool quit = false;
    SDL_Event event;

    CHIP8 chip8;

    /* Create the SDL objects and throw an error if this fails. */
    if (!sdl_init(window, renderer, SCALE, chip8.getWidth(), chip8.getHeight())) {
        return -1;
    }

    /* Now we sanitize the argument to make sure there's only one and it ends with ".ch8". */
    if (argc != 2) {
        cout << "Argument error. Supply the filename of the program as a single argument." << endl;
        return -1;
    }
    string pr_name = argv[1];
    if (pr_name.length() > 4 && (pr_name.substr(pr_name.length() - 4, 4) != ".ch8")) {
        cout << "Argument error. The filename needs to end with \".ch8\"." << endl;
        return -1;
    }

    /* CHIP-8 programs are loaded into memory between address 0x200 and 0xFFF. */
    chip8.memory_init();
    ifstream inBuffer(pr_name, ios::in | ios::binary);
    chip8.load_program(inBuffer);

    while (!quit) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }

        chip8.process_instruction();

        chip8.decrement_timers();

        /* TODO: Buzzer */

        /* TODO: Process key input */

        draw_screen(chip8, *renderer, SCALE, chip8.getWidth(), chip8.getHeight());

        /* Means the loop runs at about 60 Hz. */
        SDL_Delay(16);
    }

    return 0;
}