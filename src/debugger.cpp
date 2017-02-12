/*
 * debugger.cpp
 * Copyright (C) 2017 Emiliano Firmino <emiliano.firmino@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "debugger.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>
#include <vector>

Debugger::Debugger(GBCPU& cpu, GBGPU& gpu, GBJoypad& joypad) :
    cpu(cpu), gpu(gpu), mmu(cpu.mmu), joypad(joypad), window(nullptr), renderer(nullptr), font(nullptr) {

}

void Debugger::show() {
    if (!window) {
        window = SDL_CreateWindow("GBDebugger",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            500,
            500,
            SDL_WINDOW_SHOWN);

        if (window) {
            renderer = SDL_CreateRenderer(window, -1, 0);
            if (!renderer) {
                std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
            }

            font = TTF_OpenFont("res/font/DroidSansMono.ttf", 12);
            if (font) {
                TTF_SetFontHinting(font, TTF_HINTING_MONO);
            } else {
                std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << "\n";
            }

        } else {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        }
    }

}

void Debugger::hide() {
    if (!font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

}

void Debugger::draw() {
    if (!window || !renderer || !font) {
        return;
    }

    SDL_Color black = {0, 0, 0, 0};

    auto lines = dump_registers();

    std::vector<std::pair<SDL_Surface*, SDL_Texture*>> render_list;
    for (auto &line : lines) {
        auto surface = TTF_RenderText_Shaded(font, line.c_str(), {0, 0, 0, 0}, {255, 255, 255, 0});
        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        render_list.push_back(std::make_pair(surface, texture));
    }

    int textureWidth = 0, textureHeight = 0;

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer);

    int y = 0;
    for (auto &render_pair : render_list) {
        SDL_Rect rect = {0, y, 0, 0};
        SDL_QueryTexture(render_pair.second, NULL, NULL, &rect.w, &rect.h);
        SDL_RenderCopy(renderer, render_pair.second, NULL, &rect);
        y += rect.h;
    }

    SDL_RenderPresent(renderer);

    for (auto &render_pair : render_list) {
        SDL_DestroyTexture(render_pair.second);
        SDL_FreeSurface(render_pair.first);
    }
}

std::vector<std::string> Debugger::dump_registers() {
    std::stringstream cpu_register_dump;

    cpu_register_dump << "Registers:\n";
    cpu_register_dump << std::hex;

    cpu_register_dump << "a:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.a) << " ";
    cpu_register_dump << "f:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.f) << " ";

    cpu_register_dump << "b:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.b) << " ";
    cpu_register_dump << "c:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.c) << " ";
    cpu_register_dump << "d:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.d) << " ";
    cpu_register_dump << "e:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.e) << " ";
    cpu_register_dump << "h:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.h) << " ";
    cpu_register_dump << "l:" << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(cpu.reg.l) << " ";

    cpu_register_dump << ((cpu.reg.f & 0x80) ? "z" : "-");
    cpu_register_dump << ((cpu.reg.f & 0x40) ? "n" : "-");
    cpu_register_dump << ((cpu.reg.f & 0x20) ? "h" : "-");
    cpu_register_dump << ((cpu.reg.f & 0x10) ? "c" : "-") << "\n";

    cpu_register_dump << "bc:" << std::setw(4) << std::setfill('0') << cpu.reg.bc << " ";
    cpu_register_dump << "de:" << std::setw(4) << std::setfill('0') << cpu.reg.de << " ";
    cpu_register_dump << "hl:" << std::setw(4) << std::setfill('0') << cpu.reg.hl << " ";
    cpu_register_dump << "sp:" << std::setw(4) << std::setfill('0') << cpu.reg.sp << " ";
    cpu_register_dump << "pc:" << std::setw(4) << std::setfill('0') << cpu.reg.pc;
    cpu_register_dump << std::dec;

    std::vector<std::string> lines;
    while (cpu_register_dump.good()) {
        std::string line;
        std::getline(cpu_register_dump, line);
        lines.push_back(line);
    }

    return lines;
}
