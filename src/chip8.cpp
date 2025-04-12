#include "chip8.h"
#include "log.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>


Chip8::Chip8(chip8_display_t display_type, uint16_t ram_size)
{
    if (ram_size < max_ram_size) {
        std::cout << Log::WARNING << "RAM size of " + std::to_string(ram_size) + 
            " bytes is smaller than the maximum of " + std::to_string(max_ram_size) + 
            ". Program might not run successfully." << std::endl;
    }

    // allocate 0-initialized RAM buffer;
    RAM = std::make_unique<std::byte[]>(ram_size);

    if (display_type == SUPER) {
        display_width = 128;
        display_height = 64;
    }

    // allocate 0-initialized pixel buffer;
    pixels = std::make_unique<std::byte[]>(display_width * display_height);

    // create the window and display
    window.reset(SDL_CreateWindow("CHIP-8", display_width * scaling_factor, display_height * scaling_factor, 0));
    display.reset(SDL_CreateRenderer(window.get(), NULL));

    if (!window || !display) {
        std::cout << Log::ERROR << "Failed to create window or renderer: " << SDL_GetError() << std::endl;
        return;
    }

    std::cout << Log::INFO << "Initialized CHIP-8 emulator." << std::endl;
}

Chip8::~Chip8() {
    std::cout << Log::INFO << "Destroyed CHIP-8 emulator." << std::endl;
}

bool Chip8::execute_program() {
    if (!program_loaded) {
        std::cout << Log::ERROR << "No program loaded. Cannot execute." << std::endl;
        return false;
    }

    // SDL event loop
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                std::cout << Log::INFO << "Exiting program." << std::endl;
                return true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                // Handle key press
                std::cout << Log::INFO << "Key pressed: " << event.key.key << std::endl;
            }
        }

        // Execute a cycle
        execute_cycle();


        // Update the display from the pixel buffer

        // We emulate at 60Hz
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << Log::INFO << "Executing " + program_name << std::endl;

    return true;
}

bool Chip8::load_program(std::string fname) {
    if (!std::filesystem::exists(fname)) {
        std::cout << Log::ERROR << "Program file " + fname + " does not exist." << std::endl;
        return false;
    }

    program_loaded = true;

    // open file
    std::ifstream file(fname, std::ios::binary);
    if (!file) {
        std::cout << Log::ERROR << "Failed to open program file " + fname + "." << std::endl;
        return false;
    }

    // Copy program into RAM buffer
    file.seekg(0, std::ios::end);

    // Get the size of the program
    std::streamsize size = file.tellg();

    if (size > max_ram_size) {
        std::cout << Log::ERROR << "Cannot load program of size " + std::to_string(size) + 
            " bytes. Maximum size is " + std::to_string(max_ram_size) + " bytes." << std::endl;
        return false;
    }

    // Return to the beginning, prepare to load program into RAM
    file.seekg(0, std::ios::beg);

    // Load program into RAM
    file.read(reinterpret_cast<char*>(RAM.get()), size);

    // Check if load was successful
    if (file.gcount() != size) {
        std::cout << Log::ERROR << "Failed to load program " + fname + "." << std::endl;
        return false;
    }

    std::cout << Log::INFO << "Loaded program " + fname + " into RAM." << std::endl;

    return true;
}

void Chip8::execute_cycle() {
    if (PC >= max_ram_size) {
        std::cout << Log::ERROR << "PC out of bounds. Exiting." << std::endl;
        return;
    }

    // fetch next instruction
    uint16_t opcode = (uint16_t) RAM.get()[PC];

    // print opcode as hex
    std::cout << Log::DEBUG << "Fetched opcode: " << std::hex << opcode << std::endl;

    // only look at the first part of opcode for now
    switch ((opcode & 0xF000) >> 12) {
    case 0x0:
        switch (opcode & 0xF) {
            case 0x0: // OPCODE = 0x00E0 -- clear screen
                clear_screen();
            break;
            case 0xE: // OPCODE = 0x00EE -- return
                PC = stack[SP];
                SP = (SP - 1);
            break;
        }
    break;
    case 0x1: // OPCODE = 0x1nnn -- jump
        PC = opcode & 0x0FFF;
    break;
    case 0x2: // OPCODE = 0x2nnn -- call
        SP++;
        if (SP >= 16) {
            std::cout << Log::ERROR << "Stack overflow" << std::endl;
            dump_registers();
        }
        stack[SP] = PC;
        PC = opcode & 0x0FFF;
    break;
    case 0x6: // OPCODE = 0x6xkk -- LD Vx, byte
        V[opcode & 0x0F00] = opcode & 0x00FF;
    break;
    case 0xA: // OPCODE = 0xAnnn -- LD I, addr
        I = opcode & 0x0FFF;
    break;
    case 0xD: // OPCODE = Dxyn -- DRW Vx, Vy, nibble
        draw_sprite(opcode & 0x0F00, opcode & 0x00F0, opcode & 0x000F);
    break;
    }

    PC += 2;
}

void Chip8::clear_screen() {
    // Clear the screen
    SDL_SetRenderDrawColor(display.get(), 0, 0, 0, 255);
    SDL_RenderClear(display.get());

    // Update the display
    SDL_RenderPresent(display.get());

    std::cout << Log::DEBUG << "Cleared screen." << std::endl;
}

void Chip8::dump_registers() {
    std::cout << Log::DEBUG << "I:  " << std::hex << I << std::endl;
    std::cout << Log::DEBUG << "PC: " << std::hex << PC << std::endl;
    std::cout << Log::DEBUG << "SP: " << std::hex << SP << std::endl;
    std::cout << Log::DEBUG << "DT: " << std::hex << DT << std::endl;
    std::cout << Log::DEBUG << "ST: " << std::hex << ST << std::endl;

    for (int i = 0; i < 16; i++) {
        std::cout << Log::DEBUG << "V" << std::hex << i << ": " << std::hex << (int)V[i] << std::endl;
    }

    for (int i = 0; i < 16; i++) {
        std::cout << Log::DEBUG << "Stack[" << std::hex << i << "]: " << std::hex << stack[i] << std::endl;
    }
}

void Chip8::draw_sprite(uint8_t x, uint8_t y, uint8_t n) {
    auto start_x = V[x];
    auto start_y = V[y];
    auto sprite = RAM.get() + I;

    auto sprite_display_start = pixels.get() + (start_x * start_y);

    // Don't report a collision until we find one
    V[0xF] = 0;

    // xor sprite onto the pixel buffer
    for (int i = 0; i < n; i++) {
        auto temp = pixels[x*y];
        pixels[x * y] ^= *(sprite + I);
    }
}

void Chip8::display_pixel_buffer() {
    for (int y = 0; y < display_height; y++) {
        for (int x = 0; x < display_width; x++) {
            if (pixels[y * display_width + x] == 1) {
                SDL_SetRenderDrawColor(display.get(), 255, 255, 255, 255);
                SDL_RenderDrawPoint(display.get(), x * scaling_factor, y * scaling_factor);
            }
        }
    }

    SDL_RenderPresent(display.get());
}
