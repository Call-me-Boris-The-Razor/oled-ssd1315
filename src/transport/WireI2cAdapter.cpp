/**
 * @file WireI2cAdapter.cpp
 * @brief Реализация адаптера Arduino Wire
 */

#include "../../include/oled/internal/WireI2cAdapter.hpp"

#if OLED_ENABLED

namespace oled {

bool WireI2cAdapter::write(uint8_t addr7, const uint8_t* data, size_t len) {
    if (!wire_ || data == nullptr || len == 0) {
        return false;
    }
    
    // Отправляем данные пакетами по WIRE_BUFFER_SIZE - 1 байт
    // (1 байт резервируется под адрес)
    constexpr size_t maxChunk = WIRE_BUFFER_SIZE - 1;
    
    size_t offset = 0;
    while (offset < len) {
        size_t chunkSize = (len - offset > maxChunk) ? maxChunk : (len - offset);
        
        wire_->beginTransmission(addr7);
        wire_->write(data + offset, chunkSize);
        
        if (wire_->endTransmission() != 0) {
            return false;
        }
        
        offset += chunkSize;
    }
    
    return true;
}

} // namespace oled

#endif // OLED_ENABLED
