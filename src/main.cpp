#include "chip8.h"
#include "log.h"

int main() {
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        std::cout << Log::ERROR << "Failed to initialize SDL3: " << SDL_GetError() << std::endl;
        return 1;
    } else {
        std::cout << Log::INFO << "SDL3 initialized." << std::endl;
    }

    Chip8 chip(DEFAULT, Chip8::max_ram_size);

    if (chip.load_program("../programs/1-chip8-logo.ch8")) {
        chip.execute_program();
    }

    SDL_Quit();

    return 0;
}
