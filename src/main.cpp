#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "CHIP8.h"
#include <iostream>
#include <fstream>

using namespace std;

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

void key_down(CHIP8 &chip8, SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_1:
            chip8.set_key(0x0, true);
            break;
        case SDLK_2:
            chip8.set_key(0x1, true);
            break;
        case SDLK_3:
            chip8.set_key(0x2, true);
            break;
        case SDLK_4:
            chip8.set_key(0x3, true);
            break;
        case SDLK_q:
            chip8.set_key(0x4, true);
            break;
        case SDLK_w:
            chip8.set_key(0x5, true);
            break;
        case SDLK_e:
            chip8.set_key(0x6, true);
            break;
        case SDLK_r:
            chip8.set_key(0x7, true);
            break;
        case SDLK_a:
            chip8.set_key(0x8, true);
            break;
        case SDLK_s:
            chip8.set_key(0x9, true);
            break;
        case SDLK_d:
            chip8.set_key(0xA, true);
            break;
        case SDLK_f:
            chip8.set_key(0xB, true);
            break;
        case SDLK_z:
            chip8.set_key(0xC, true);
            break;
        case SDLK_x:
            chip8.set_key(0xD, true);
            break;
        case SDLK_c:
            chip8.set_key(0xE, true);
            break;
        case SDLK_v:
            chip8.set_key(0xF, true);
            break;
        default:
            break;
    }
}

void key_up(CHIP8 &chip8, SDL_Event &event) {
    switch (event.key.keysym.sym) {
        case SDLK_1:
            chip8.set_key(0x0, false);
            break;
        case SDLK_2:
            chip8.set_key(0x1, false);
            break;
        case SDLK_3:
            chip8.set_key(0x2, false);
            break;
        case SDLK_4:
            chip8.set_key(0x3, false);
            break;
        case SDLK_q:
            chip8.set_key(0x4, false);
            break;
        case SDLK_w:
            chip8.set_key(0x5, false);
            break;
        case SDLK_e:
            chip8.set_key(0x6, false);
            break;
        case SDLK_r:
            chip8.set_key(0x7, false);
            break;
        case SDLK_a:
            chip8.set_key(0x8, false);
            break;
        case SDLK_s:
            chip8.set_key(0x9, false);
            break;
        case SDLK_d:
            chip8.set_key(0xA, false);
            break;
        case SDLK_f:
            chip8.set_key(0xB, false);
            break;
        case SDLK_z:
            chip8.set_key(0xC, false);
            break;
        case SDLK_x:
            chip8.set_key(0xD, false);
            break;
        case SDLK_c:
            chip8.set_key(0xE, false);
            break;
        case SDLK_v:
            chip8.set_key(0xF, false);
            break;
        default:
            break;
    }
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
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init Error: " << SDL_GetError() << endl;
        return -1;
    }

    window = SDL_CreateWindow("CHIP-8_Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, chip8.getWidth() * SCALE, chip8.getHeight() * SCALE, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        SDL_Quit();
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
    inBuffer.close();

    while (!quit) {
        while(SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                key_down(chip8, event);
            }
            else if (event.type == SDL_KEYUP) {
                key_up(chip8, event);
            }
        }

        chip8.process_instruction();

        chip8.decrement_timers();

        /* TODO: Buzzer */

        /* TODO: Fix drawing because it doesn't work */
        draw_screen(chip8, *renderer, SCALE, chip8.getWidth(), chip8.getHeight());

        /* Means the loop runs at about 60 Hz. */
        SDL_Delay(16);
    }

    return 0;
}