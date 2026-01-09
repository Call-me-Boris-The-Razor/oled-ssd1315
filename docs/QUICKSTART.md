# Быстрый старт — OLED SSD1315

Руководство по началу работы с библиотекой за 5 минут.

---

## Требования

- **Платформа:** PlatformIO
- **Framework:** Arduino
- **MCU:** ESP32, ESP8266, STM32, AVR и другие
- **Дисплей:** OLED 128×64 или 128×32 с контроллером SSD1315/SSD1306

---

## Шаг 1: Установка

Скопируйте папку `oled_ssd1315` в директорию `lib/` вашего проекта:

```text
my_project/
├── lib/
│   └── oled_ssd1315/   ← Сюда
├── src/
│   └── main.cpp
└── platformio.ini
```

---

## Шаг 2: Настройка platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

build_flags = 
    -DOLED_SSD1315_ENABLE=1

lib_deps =
    # Если библиотека в lib/, дополнительных зависимостей не нужно
```

**Важно:** Флаг `-DOLED_SSD1315_ENABLE=1` обязателен!

---

## Шаг 3: Подключение дисплея

### Стандартное подключение (ESP32)

| OLED | ESP32 | Описание |
|------|-------|----------|
| VCC | 3.3V | Питание |
| GND | GND | Земля |
| SCL | GPIO22 | I2C Clock |
| SDA | GPIO21 | I2C Data |

### Для других плат

| Плата | SCL | SDA |
|-------|-----|-----|
| ESP32 | GPIO22 | GPIO21 |
| ESP8266 | GPIO5 (D1) | GPIO4 (D2) |
| Arduino Uno | A5 | A4 |
| STM32 (Blue Pill) | PB6 | PB7 |

---

## Шаг 4: Минимальный код

```cpp
#include <Wire.h>
#include <oled/OledSsd1315.hpp>

OledSsd1315 oled(Wire);

void setup() {
    // Инициализация I2C
    Wire.begin();
    
    // Конфигурация дисплея
    OledConfig cfg;
    cfg.i2cAddr7 = 0x3C;  // Стандартный адрес
    cfg.width = 128;
    cfg.height = 64;
    
    // Инициализация дисплея
    if (oled.begin(cfg) != OledResult::Ok) {
        // Ошибка!
        return;
    }
    
    // Вывод текста
    oled.clear();
    oled.print("Hello World!");
    oled.flush();
}

void loop() {
    // Ваш код
}
```

---

## Шаг 5: Компиляция и загрузка

```bash
pio run -t upload
```

На дисплее должна появиться надпись "Hello World!".

---

## Частые проблемы

### Дисплей не работает

1. **Проверьте адрес I2C:**
   ```cpp
   cfg.i2cAddr7 = 0x3D;  // Попробуйте альтернативный
   ```

2. **Сканер I2C:**
   ```cpp
   void scanI2C() {
       for (uint8_t addr = 1; addr < 127; addr++) {
           Wire.beginTransmission(addr);
           if (Wire.endTransmission() == 0) {
               Serial.printf("Найдено: 0x%02X\n", addr);
           }
       }
   }
   ```

### Ошибка компиляции "OledSsd1315 not found"

Убедитесь, что добавлен флаг:
```ini
build_flags = -DOLED_SSD1315_ENABLE=1
```

### Изображение перевёрнуто

```cpp
cfg.flip180 = true;
```

---

## Следующие шаги

- [Полный API](API.md)
- [Примеры использования](EXAMPLES.md)
- [Работа с текстом и кириллицей](EXAMPLES.md#кириллица)
