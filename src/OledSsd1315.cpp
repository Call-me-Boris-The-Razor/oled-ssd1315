/**
 * @file OledSsd1315.cpp
 * @brief Реализация главного класса OledSsd1315
 */

#include "../include/oled/OledSsd1315.hpp"

#if OLED_ENABLED

#include <cstdio>
#include <cstdarg>

namespace oled {

OledSsd1315::OledSsd1315(TwoWire& wire) : wire_(&wire) {
    // Объекты инициализируются в begin()
}

OledSsd1315::~OledSsd1315() {
    // Статические поля - деструктор тривиален
}

OledResult OledSsd1315::begin(const OledConfig& cfg) {
    // Сброс состояния
    resetState();
    
    // Проверка Wire
    if (!wire_) {
        return OledResult::InvalidArg;
    }
    
    // Проверка размера буфера
    size_t bufSize = static_cast<size_t>(cfg.width) * cfg.height / 8;
    if (bufSize > OLED_MAX_BUFFER_SIZE) {
        return OledResult::InvalidArg;
    }
    
    // Инициализируем адаптер I2C
    adapter_.init(*wire_);
    
    // Инициализируем драйвер
    OledResult res = driver_.init(adapter_, cfg);
    if (res != OledResult::Ok) {
        return res;
    }
    
    // Инициализируем графический контекст
    gfx_.init(buffer_, cfg.width, cfg.height);
    
    // Очищаем буфер
    gfx_.clear();
    
    initialized_ = true;
    return OledResult::Ok;
}

bool OledSsd1315::isReady() const {
    return initialized_ && driver_.isReady();
}

void OledSsd1315::resetState() {
    initialized_ = false;
    // Статические объекты не требуют освобождения
}

OledResult OledSsd1315::setPower(bool on) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setPower(on);
}

OledResult OledSsd1315::setContrast(uint8_t value) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setContrast(value);
}

OledResult OledSsd1315::invert(bool on) {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.setInvert(on);
}

void OledSsd1315::clear() {
    if (gfx_.isInitialized()) {
        gfx_.clear();
    }
}

void OledSsd1315::fill(bool color) {
    if (gfx_.isInitialized()) {
        gfx_.fill(color);
    }
}

OledResult OledSsd1315::flush() {
    if (!isReady()) {
        return OledResult::NotInitialized;
    }
    return driver_.writeBuffer(gfx_.buffer(), gfx_.bufferSize());
}

void OledSsd1315::pixel(int x, int y, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.pixel(x, y, color);
    }
}

void OledSsd1315::line(int x0, int y0, int x1, int y1, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.line(x0, y0, x1, y1, color);
    }
}

void OledSsd1315::rect(int x, int y, int w, int h, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.rect(x, y, w, h, color);
    }
}

void OledSsd1315::rectFill(int x, int y, int w, int h, bool color) {
    if (gfx_.isInitialized()) {
        gfx_.rectFill(x, y, w, h, color);
    }
}

void OledSsd1315::setCursor(int x, int y) {
    if (gfx_.isInitialized()) {
        gfx_.setCursor(x, y);
    }
}

void OledSsd1315::setTextSize(uint8_t scale) {
    if (gfx_.isInitialized()) {
        gfx_.setTextSize(scale);
    }
}

void OledSsd1315::setTextColor(bool color) {
    if (gfx_.isInitialized()) {
        gfx_.setTextColor(color);
    }
}

void OledSsd1315::print(const char* str) {
    if (gfx_.isInitialized()) {
        gfx_.print(str);
    }
}

void OledSsd1315::printf(const char* fmt, ...) {
    if (!gfx_.isInitialized()) return;
    
    char buf[OLED_PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    
    gfx_.print(buf);
}

} // namespace oled

#else // OLED_ENABLED == 0

// === Заглушки когда библиотека отключена ===

namespace oled {

OledSsd1315::~OledSsd1315() {}

OledResult OledSsd1315::begin(const OledConfig&) {
    return OledResult::Disabled;
}

bool OledSsd1315::isReady() const {
    return false;
}

void OledSsd1315::resetState() {}

OledResult OledSsd1315::setPower(bool) {
    return OledResult::Disabled;
}

OledResult OledSsd1315::setContrast(uint8_t) {
    return OledResult::Disabled;
}

OledResult OledSsd1315::invert(bool) {
    return OledResult::Disabled;
}

void OledSsd1315::clear() {}

void OledSsd1315::fill(bool) {}

OledResult OledSsd1315::flush() {
    return OledResult::Disabled;
}

void OledSsd1315::pixel(int, int, bool) {}

void OledSsd1315::line(int, int, int, int, bool) {}

void OledSsd1315::rect(int, int, int, int, bool) {}

void OledSsd1315::rectFill(int, int, int, int, bool) {}

void OledSsd1315::setCursor(int, int) {}

void OledSsd1315::setTextSize(uint8_t) {}

void OledSsd1315::setTextColor(bool) {}

void OledSsd1315::print(const char*) {}

void OledSsd1315::printf(const char*, ...) {}

} // namespace oled

#endif // OLED_ENABLED
