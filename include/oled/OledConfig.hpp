/**
 * @file OledConfig.hpp
 * @brief Единая конфигурация библиотеки OLED SSD1315
 * 
 * Все файлы библиотеки должны включать этот заголовок для получения
 * флага OLED_ENABLED и базовых настроек.
 */

#ifndef OLED_CONFIG_HPP
#define OLED_CONFIG_HPP

// === Флаг включения библиотеки ===
// Определяется в build_flags: -DOLED_SSD1315_ENABLE=1
#if defined(OLED_SSD1315_ENABLE) && OLED_SSD1315_ENABLE
    #define OLED_ENABLED 1
#else
    #define OLED_ENABLED 0
#endif

// === Размеры буферов ===
#ifndef OLED_MAX_WIDTH
    #define OLED_MAX_WIDTH 128
#endif

#ifndef OLED_MAX_HEIGHT
    #define OLED_MAX_HEIGHT 64
#endif

// Максимальный размер framebuffer (128x64 / 8 = 1024 байт)
#define OLED_MAX_BUFFER_SIZE ((OLED_MAX_WIDTH * OLED_MAX_HEIGHT) / 8)

// Размер буфера для printf
#define OLED_PRINTF_BUFFER_SIZE 128

// === Wire buffer size ===
#ifndef OLED_WIRE_BUFFER_SIZE
    #define OLED_WIRE_BUFFER_SIZE 32
#endif

#endif // OLED_CONFIG_HPP
