#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

bool init(SDL_Window* window, int scale) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init Error:" << SDL_GetError() << endl;
        return false;
    }
    window = SDL_CreateWindow("CHIP-8_Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * scale, 32 * scale, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cerr << "SDL_CreateWindow Error:" << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    return true;
}

int main() {
    SDL_Window* window;
    const int SCALE = 8;

    if (!init(window, SCALE)) {
        return -1;
    }



    return 0;
}