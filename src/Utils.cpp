#include "Utils.h"

#include <random>
#include <sstream>
#include <iomanip>

std::string Utils::GenerateGuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 15);
    std::uniform_int_distribution<uint32_t> dist2(8, 11);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (int i = 0; i < 8; i++) {
        ss << dist(gen);
    }

    ss << '-';
    for (int i = 0; i < 4; i++) {
        ss << dist(gen);
    }

    ss << '-';
    ss << 4; // The 13th digit is always '4'
    for (int i = 0; i < 3; i++) {
        ss << dist(gen);
    }

    ss << '-';
    ss << dist2(gen); // The 17th digit is in the range [8, B]
    for (int i = 0; i < 3; i++) {
        ss << dist(gen);
    }

    ss << '-';
    for (int i = 0; i < 12; i++) {
        ss << dist(gen);
    }

    return ss.str();
}