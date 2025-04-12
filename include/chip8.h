#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <array>
#include <SDL3/SDL.h>

typedef enum {
    DEFAULT,
    SUPER
} chip8_display_t;

class Chip8 {

private:
    uint8_t display_width = 64;
    uint8_t display_height = 32;
    uint8_t display_scaling_factor = 10;

    // 16 8-bit general purpose registers (0x0 - 0xF)
    // VF register reserved for flags
    std::array<uint8_t, 16> V = {0};

    // 1 16-bit register for storing addresses
    // max ram = 4KB = 12 bits
    uint16_t I = 0;

    // 16 bit PC
    uint16_t PC = 0;

    // Stack pointer
    uint8_t SP = 0;

    // stack = 16 16-bit values
    std::array<uint16_t, 16> stack = {0};

    // delay timer
    uint16_t DT = 0;

    // sound timer
    uint16_t ST = 0;

    // pointer to the memory block we are using as RAM
    std::unique_ptr<std::byte[]> RAM;

    // define pointers to the window and display
    std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window = 
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>(nullptr, SDL_DestroyWindow);
    std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> display = 
        std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>(nullptr, SDL_DestroyRenderer);

    std::unique_ptr<std::byte[]> pixels;

    // check if we have loaded a program yet
    bool program_loaded = false;

    // Store the program name for QOL
    std::string program_name;

    void handle_key_press(SDL_Keycode key);

    char translate_key(char in);

    void execute_cycle();

    void dump_registers();

    void clear_screen();

    void draw_sprite(uint8_t x, uint8_t y, uint8_t n);

    void display_pixel_buffer();
public:
    static const uint16_t max_ram_size = 4096;
    static const uint16_t scaling_factor = 10;

    Chip8(chip8_display_t display_type, uint16_t ram_size);
    ~Chip8();

    bool load_program(std::string fname);
    bool execute_program();
};

#endif
