/**
 * @file OledTypes.hpp
 * @brief Общие типы и перечисления для OLED SSD1315
 * 
 * Библиотека OLED SSD1315 для PlatformIO
 * Флаг включения: OLED_SSD1315_ENABLE=1
 */

#ifndef OLED_TYPES_HPP
#define OLED_TYPES_HPP

#include "OledConfig.hpp"
#include <cstdint>

namespace oled {

/**
 * @brief Результаты операций OLED
 */
enum class OledResult {
    Ok,             // Операция успешна
    Disabled,       // Библиотека отключена (флаг не задан)
    I2cError,       // Ошибка I2C
    NotInitialized, // Дисплей не инициализирован
    InvalidArg,     // Неверный аргумент
    Unsupported     // Операция не поддерживается
};

/**
 * @brief Режим питания дисплея
 * 
 * InternalChargePump - используется встроенная "помпа" (charge pump)
 * ExternalVcc - внешнее питание VCC
 */
enum class VccMode {
    InternalChargePump, // Внутренний charge pump (0x8D, 0x14)
    ExternalVcc         // Внешнее VCC (0x8D, 0x10)
};

/**
 * @brief Конфигурация OLED дисплея
 */
struct OledConfig {
    uint8_t  i2cAddr7 = 0x3C;      // 7-битный адрес (0x3C или 0x3D)
    uint16_t width    = 128;       // Ширина в пикселях
    uint16_t height   = 64;        // Высота в пикселях (64 или 32)
    uint32_t i2cFreq  = 400000;    // Частота I2C (опционально)
    VccMode  vccMode  = VccMode::InternalChargePump;
    bool     flip180  = false;     // Поворот на 180 градусов
    int      resetGpio = -1;       // GPIO для reset (-1 = отсутствует)
};

} // namespace oled

#endif // OLED_TYPES_HPP
