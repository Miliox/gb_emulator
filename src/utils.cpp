/*
 * utils.cpp
 * Copyright (C) 2017 Emiliano Firmino <emiliano.firmino@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include "utils.hpp"

std::stringstream print_bytes(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::setfill('0');
    for(size_t i = 0; i < data.size(); ++i) {
        ss << std::hex << std::setw(2) << (int)data[i];
        ss << (((i + 1) % 32 == 0) ? "\n" : " ");
    }
    ss << "\n";
    return ss;
}

std::stringstream print_bytes(std::vector<uint8_t>::iterator b, std::vector<uint8_t>::iterator e) {
    std::stringstream ss;
    ss << std::setfill('0');
    int count = 0;
    while(b != e) {
        ss << std::hex << std::setw(2) << (int)*b;
        ss << (((count + 1) % 32 == 0) ? "\n" : " ");
        ++b;
        ++count;
    }
    ss << "\n";
    return ss;
}

std::vector<std::string> text_to_line_vector(std::stringstream& ss) {
    std::vector<std::string> lines;
    while (ss.good()) {
        std::string line;
        std::getline(ss, line);
        lines.push_back(line);
    }
    return lines;
}

