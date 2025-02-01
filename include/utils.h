#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

// Define color codes as macros
#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLD        "\033[1m"
#define UNDERLINE   "\033[4m"
#define PINK        "\033[95m"

// Bright Colors
#define BRIGHT_BLACK    "\033[90m"
#define BRIGHT_RED      "\033[91m"
#define BRIGHT_GREEN    "\033[92m"
#define BRIGHT_YELLOW   "\033[93m"
#define BRIGHT_BLUE     "\033[94m"
#define BRIGHT_MAGENTA  "\033[95m"
#define BRIGHT_CYAN     "\033[96m"
#define BRIGHT_WHITE    "\033[97m"

// 256-Color Mode
#define ORANGE          "\033[38;5;214m"
#define PURPLE          "\033[38;5;93m"
#define HIGHLIGHT       "\033[38;5;205m"
#define LIGHT_BLUE      "\033[38;5;45m"
#define LIGHT_GREEN     "\033[38;5;120m"
#define GOLD            "\033[38;5;220m"
#define SKY_BLUE        "\033[38;5;117m"
#define LIME            "\033[38;5;154m"
#define DARK_RED        "\033[38;5;88m"
#define DARK_GREEN      "\033[38;5;28m"
#define TEAL            "\033[38;5;30m"
#define VIOLET          "\033[38;5;129m"
#define BEIGE           "\033[38;5;230m"
#define LIGHT_GRAY      "\033[38;5;250m"
#define DARK_GRAY       "\033[38;5;238m"
#define BROWN           "\033[38;5;130m"

// Background Colors
#define BG_BLACK        "\033[40m"
#define BG_RED          "\033[41m"
#define BG_GREEN        "\033[42m"
#define BG_YELLOW       "\033[43m"
#define BG_BLUE         "\033[44m"
#define BG_MAGENTA      "\033[45m"
#define BG_CYAN         "\033[46m"
#define BG_WHITE        "\033[47m"
#define BG_ORANGE       "\033[48;5;214m"
#define BG_PURPLE       "\033[48;5;93m"
#define BG_PINK         "\033[48;5;205m"
#define BG_LIGHT_BLUE   "\033[48;5;45m"
#define BG_LIGHT_GREEN  "\033[48;5;120m"
#define BG_DARK_GRAY    "\033[48;5;238m"
#define BG_BROWN        "\033[48;5;130m"

#define BIG_SEP "========================================================="
#define SML_SEP "---------------------------------------------------------"

// Utility macros for easy colored output
#define PRINT_BIG_SEPARATION     std::cout << PINK << BIG_SEP << RESET << "\x0A"
#define PRINT_SML_SEPARATION     std::cout << PINK << SML_SEP << RESET << "\x0A"

#define PRINT_SUCCESS(text)      std::cout << GREEN     << text << RESET << "\x0A"
#define PRINT_HIGHLIGHT(text)    std::cout << HIGHLIGHT << text << RESET << "\x0A"
#define PRINT_COLOR(color, text) std::cout << color     << text << RESET << "\x0A"

#define PRINT_WARNING(text)      std::cout << YELLOW    << "WARNING: " << text << RESET << "\x0A"
#define PRINT_ERROR(text)        std::cout << RED       << "ERROR: "   << text << RESET << "\x0A"

#define PRINT_SUCCESS_NO_SPACE(text)      std::cout << GREEN     << text << RESET
#define PRINT_WARNING_NO_SPACE(text)      std::cout << YELLOW    << text << RESET
#define PRINT_ERROR_NO_SPACE(text)        std::cout << RED       << text << RESET
#define PRINT_HIGHLIGHT_NO_SPACE(text)    std::cout << HIGHLIGHT << text << RESET
#define PRINT_COLOR_NO_SPACE(color, text) std::cout << color     << text << RESET

#define PRINT_BLOB(text)    PRINT_COLOR(BLUE, text)
#define PRINT_TREE(text)    PRINT_COLOR(PINK, text)
#define PRINT_COMMIT(text)  PRINT_COLOR(ORANGE, text)
#define PRINT_DELTA(text)   PRINT_COLOR(LIGHT_BLUE, text)

