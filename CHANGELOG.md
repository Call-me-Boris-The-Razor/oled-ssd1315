# Changelog

Все значимые изменения в библиотеке OLED SSD1315.

Формат основан на [Keep a Changelog](https://keepachangelog.com/ru/1.0.0/).

## [2.1.0] - 2025-01-09

### Добавлено (STM32 Plug-and-Play)

- **Диагностика ошибок**
  - `getLastResult()` — получить код последней ошибки
  - `getLastError()` — текстовое описание ошибки
- **I2C сканер** — `scanAddress()` для автоматического поиска адреса
- **DMA поддержка (STM32)** — non-blocking передача буфера
  - `flushDMA()` — начать DMA передачу
  - `isDMAComplete()` — проверить завершение
- **I2C Bus Recovery (STM32)** — `i2cBusRecovery()` для восстановления зависшей шины
- **Увеличенный CHUNK_SIZE** — 128 байт для STM32 HAL (было 16)
- **Расширенная документация CubeMX** — пошаговая настройка для H7 серии

### Изменено

- **`OledResult`** — добавлены `Busy` и `Timeout`
- **`OLED_I2C_CHUNK_SIZE`** — платформо-зависимый размер чанка

### Новые методы API

```cpp
// Диагностика
OledResult getLastResult() const;
const char* getLastError() const;
uint8_t scanAddress(uint8_t startAddr = 0x3C, uint8_t endAddr = 0x3D);

// STM32 HAL только
OledResult flushDMA();
bool isDMAComplete() const;
static bool i2cBusRecovery(void* gpioPort, uint16_t sclPin, uint16_t sdaPin);
```

---

## [2.0.0] - 2025-01-09

### Добавлено (Multi-Platform Support)

- **Поддержка STM32 HAL I2C** — работает с `stm32cube` framework
- **Platform-agnostic архитектура** — единый API для Arduino и STM32
- **Автоопределение платформы** — через `ARDUINO` или явные флаги
- **Новый адаптер `Stm32HalI2cAdapter`** — для `I2C_HandleTypeDef*`
- **`PlatformDelay.hpp`** — унифицированные задержки для всех платформ
- **`resetCallback`** — platform-agnostic аппаратный reset вместо GPIO номера
- **Скрипт `platformio_build.py`** — автоматический выбор адаптера
- **Примеры для STM32 HAL** — `examples/stm32hal_basic/`
- **Документация `STM32_HAL.md`** — полное руководство

### Изменено

- **`OledConfig.resetGpio`** заменён на **`OledConfig.resetCallback`**
- **`library.json`** — добавлен `stm32cube` framework
- **Убрана зависимость от Arduino.h** в драйвере
- **Условная компиляция конструкторов** — `TwoWire&` или `I2C_HandleTypeDef*`

### Флаги компиляции

| Платформа | Флаги |
|-----------|-------|
| Arduino | `-DOLED_SSD1315_ENABLE=1` (автодетекция) |
| STM32 HAL | `-DOLED_SSD1315_ENABLE=1 -DOLED_PLATFORM_STM32HAL=1` |

### Новая структура

```text
include/oled/internal/
├── PlatformDelay.hpp      (NEW)
├── Stm32HalI2cAdapter.hpp (NEW)
├── WireI2cAdapter.hpp
├── ...

scripts/
└── platformio_build.py    (NEW)

examples/
├── basic/                 (Arduino)
└── stm32hal_basic/        (NEW: STM32 HAL)
```

---

## [1.1.0] - 2025-01-09

### Изменено (Рефакторинг по аудиту)

- **Убрано динамическое выделение памяти** — `new`/`delete` заменены на статические поля
- **Добавлены `init()` методы** — отложенная инициализация для WireI2cAdapter, Ssd1315Driver, Gfx
- **Перенос internal headers** — внутренние заголовки в `include/oled/internal/`
- **Единый `OledConfig.hpp`** — централизованный флаг `OLED_ENABLED` и константы
- **Унификация условной компиляции** — все файлы используют `#if OLED_ENABLED`
- **Уменьшена задержка инициализации** — `delay(50)` → `delay(20)` без reset GPIO
- **Исправлены signed/unsigned warnings** — добавлены explicit casts

### Структура файлов

```text
include/oled/
├── OledConfig.hpp      (NEW) Единая конфигурация
├── OledTypes.hpp
├── OledSsd1315.hpp
└── internal/           (NEW) Внутренние компоненты
    ├── II2c.hpp
    ├── WireI2cAdapter.hpp
    ├── Ssd1315Driver.hpp
    ├── Ssd1315Commands.hpp
    └── Gfx.hpp
```

## [1.0.0] - 2024-01-09

### Добавлено

- Первый релиз библиотеки
- Поддержка контроллера SSD1315 (совместим с SSD1306)
- Размеры дисплея: 128×64, 128×32
- I2C транспорт через Arduino Wire
- Абстракция II2c для кастомных драйверов
- Драйвер SSD1315 с полной инициализацией
- Horizontal Addressing Mode для эффективной заливки буфера
- Charge Pump управление (internal/external VCC)
- Framebuffer с операциями:
  - `clear()` / `fill()`
  - `pixel()` — установка пикселя
  - `line()` — линия (алгоритм Брезенхэма)
  - `rect()` / `rectFill()` — прямоугольники
- Текстовый вывод:
  - Встроенный шрифт 5×7 (ASCII 32-126)
  - **Кириллица** А-Я, а-я, Ё, ё (тот же размер 5×7)
  - Полная поддержка UTF-8 строк
  - Масштабирование текста
  - `print()` и `printf()`
- Управление дисплеем:
  - `setPower()` — вкл/выкл с корректным управлением pump
  - `setContrast()` — контраст 0-255
  - `invert()` — инверсия цветов
- Условное включение через флаг `OLED_SSD1315_ENABLE`
- Заглушки при выключенной библиотеке
- Пример использования в `examples/basic/`
- Документация README.md

### Технические детали

- C++17 (минимум C++14)
- Без динамического выделения в рантайме (буфер статический)
- Все команды SSD1315 в constexpr
- Модульная архитектура: Transport → Driver → GFX → Facade
