#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <vector>

std::stringstream print_bytes(const std::vector<uint8_t>& data);
std::stringstream print_bytes(std::vector<uint8_t>::iterator b, std::vector<uint8_t>::iterator e);

std::vector<std::string> text_to_line_vector(std::stringstream& ss);

#endif
