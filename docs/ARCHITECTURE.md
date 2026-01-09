# Архитектура библиотеки OLED SSD1315

## Обзор

Библиотека построена по принципу **слоёной архитектуры** с чётким разделением ответственности.

```text
┌─────────────────────────────────────────────────┐
│                 OledSsd1315                     │  ← Facade (публичный API)
│              (Фасад / Главный класс)            │
├─────────────────────────────────────────────────┤
│                     Gfx                         │  ← Graphics Layer
│         (Графика, примитивы, текст)             │
├─────────────────────────────────────────────────┤
│                Ssd1315Driver                    │  ← Driver Layer
│      (Инициализация, команды, протокол)         │
├─────────────────────────────────────────────────┤
│              WireI2cAdapter                     │  ← Transport Layer
│                  (I2C)                          │
├─────────────────────────────────────────────────┤
│                  II2c                           │  ← Abstract Interface
│           (Абстракция транспорта)               │
└─────────────────────────────────────────────────┘
```

---

## Слои архитектуры

### 1. Facade — `OledSsd1315`

**Файл:** `include/oled/OledSsd1315.hpp`

Единая точка входа для пользователя. Скрывает внутреннюю сложность.

**Ответственность:**
- Публичный API библиотеки
- Координация между слоями
- Управление жизненным циклом

```cpp
class OledSsd1315 {
private:
    WireI2cAdapter adapter_;  // Статическое поле
    Ssd1315Driver driver_;    // Статическое поле
    Gfx gfx_;                 // Статическое поле
    uint8_t buffer_[1024];    // Framebuffer
};
```

---

### 2. Graphics Layer — `Gfx`

**Файл:** `include/oled/internal/Gfx.hpp`

Работа с framebuffer и отрисовка.

**Ответственность:**
- Управление буфером (clear, fill)
- Графические примитивы (pixel, line, rect)
- Текстовый вывод (print, printf)
- UTF-8 декодирование
- Шрифты (ASCII + кириллица)

**Организация буфера:**

```text
Буфер 128×64 = 1024 байта
Организация: 8 страниц × 128 колонок

Страница 0: строки 0-7
Страница 1: строки 8-15
...
Страница 7: строки 56-63

Каждый байт = 8 вертикальных пикселей
LSB = верхний пиксель страницы
```

---

### 3. Driver Layer — `Ssd1315Driver`

**Файл:** `include/oled/internal/Ssd1315Driver.hpp`

Низкоуровневое управление контроллером.

**Ответственность:**
- Последовательность инициализации SSD1315
- Отправка команд (Control Byte = 0x00)
- Отправка данных (Control Byte = 0x40)
- Управление питанием и контрастом
- Horizontal Addressing Mode

**Формат I2C пакета:**

```text
┌──────────┬─────────────┬──────────────────┐
│ I2C Addr │ Control Byte│ Команда/Данные   │
│ (7 bit)  │ 0x00 / 0x40 │ 1+ байт          │
└──────────┴─────────────┴──────────────────┘

Control Byte:
  0x00 = Команда (D/C# = 0)
  0x40 = Данные  (D/C# = 1)
```

---

### 4. Transport Layer — `WireI2cAdapter`

**Файл:** `include/oled/internal/WireI2cAdapter.hpp`

Абстракция I2C транспорта.

**Ответственность:**
- Отправка данных через Arduino Wire
- Разбиение на пакеты (Wire buffer limit = 32 байта)
- Обработка ошибок I2C

---

### 5. Abstract Interface — `II2c`

**Файл:** `include/oled/internal/II2c.hpp`

Интерфейс для подмены транспорта.

```cpp
struct II2c {
    virtual bool write(uint8_t addr7, const uint8_t* data, size_t len) = 0;
    virtual ~II2c() = default;
};
```

**Применение:** Позволяет использовать библиотеку с любым I2C драйвером (HAL, ESP-IDF и т.д.).

---

## Структура файлов

```text
lib/oled_ssd1315/
├── include/oled/
│   ├── OledConfig.hpp         # Конфигурация, флаги компиляции
│   ├── OledTypes.hpp          # Типы: OledResult, OledConfig, VccMode
│   ├── OledSsd1315.hpp        # Главный заголовок (Facade)
│   └── internal/              # Внутренние компоненты
│       ├── II2c.hpp           # Абстракция транспорта
│       ├── WireI2cAdapter.hpp # Arduino Wire адаптер
│       ├── Ssd1315Driver.hpp  # Драйвер контроллера
│       ├── Ssd1315Commands.hpp# Константы команд SSD1315
│       └── Gfx.hpp            # Графический слой
├── src/
│   ├── OledSsd1315.cpp        # Реализация Facade
│   ├── driver/
│   │   └── Ssd1315Driver.cpp  # Реализация драйвера
│   ├── gfx/
│   │   ├── Font5x7.hpp        # ASCII шрифт
│   │   ├── FontCyrillic5x7.hpp# Кириллический шрифт
│   │   └── Gfx.cpp            # Реализация графики
│   └── transport/
│       └── WireI2cAdapter.cpp # Реализация I2C адаптера
└── docs/                      # Документация
```

---

## Поток данных

### Инициализация

```text
User                    OledSsd1315              Driver              Transport
  │                          │                      │                     │
  │ begin(cfg)               │                      │                     │
  │─────────────────────────>│                      │                     │
  │                          │ adapter_.init(wire)  │                     │
  │                          │─────────────────────────────────────────────>
  │                          │                      │                     │
  │                          │ driver_.init(adapter_, cfg)                │
  │                          │─────────────────────>│                     │
  │                          │                      │ writeCommand(0xAE)  │
  │                          │                      │────────────────────>│
  │                          │                      │ ... init sequence   │
  │                          │                      │────────────────────>│
  │                          │                      │ writeCommand(0xAF)  │
  │                          │                      │────────────────────>│
  │                          │                      │                     │
  │                          │ gfx_.init(buffer_, w, h)                   │
  │                          │─────────────────────────────────────────────>
  │                          │                      │                     │
  │<─────────────────────────│                      │                     │
  │ OledResult::Ok           │                      │                     │
```

### Отрисовка

```text
User                    OledSsd1315              Gfx              Driver
  │                          │                    │                  │
  │ clear()                  │                    │                  │
  │─────────────────────────>│ gfx_.clear()       │                  │
  │                          │───────────────────>│ memset(0)        │
  │                          │                    │                  │
  │ print("Привет")          │                    │                  │
  │─────────────────────────>│ gfx_.print()       │                  │
  │                          │───────────────────>│ UTF-8 decode     │
  │                          │                    │ drawGlyph()      │
  │                          │                    │ pixel()...       │
  │                          │                    │                  │
  │ flush()                  │                    │                  │
  │─────────────────────────>│                    │                  │
  │                          │ driver_.writeBuffer(buffer_, 1024)    │
  │                          │─────────────────────────────────────>│
  │                          │                    │     I2C packets  │
  │                          │                    │                  │
```

---

## Условная компиляция

### Механизм

```cpp
// OledConfig.hpp
#if defined(OLED_SSD1315_ENABLE) && OLED_SSD1315_ENABLE
    #define OLED_ENABLED 1
#else
    #define OLED_ENABLED 0
#endif
```

### Поведение

| Флаг | OLED_ENABLED | Поведение |
|------|--------------|-----------|
| `-DOLED_SSD1315_ENABLE=1` | 1 | Полная функциональность |
| Не задан | 0 | Заглушки, всё возвращает `Disabled` |

**Преимущества:**
- Нулевой размер кода при выключении
- Нет ошибок компиляции
- Легко интегрировать в условную сборку

---

## Принципы проектирования

### 1. Без динамической аллокации

Все объекты — статические поля класса. Буфер выделяется на стеке.

```cpp
class OledSsd1315 {
    WireI2cAdapter adapter_;  // НЕ указатель
    Ssd1315Driver driver_;    // НЕ указатель
    Gfx gfx_;                 // НЕ указатель
    uint8_t buffer_[1024];    // Статический массив
};
```

### 2. Отложенная инициализация

Конструкторы тривиальны. Вся логика — в методах `init()`.

```cpp
WireI2cAdapter() : wire_(nullptr) {}  // Конструктор пустой
void init(TwoWire& wire);             // Инициализация отдельно
```

### 3. Dependency Injection

Driver получает Transport через интерфейс `II2c`.

```cpp
OledResult Ssd1315Driver::init(II2c& i2c, const OledConfig& cfg);
```

### 4. RAII не применяется

Для embedded-систем предпочтительнее явное управление ресурсами через `begin()` / `resetState()`.

---

## Расширение библиотеки

### Добавление нового транспорта

```cpp
class MyI2cAdapter : public II2c {
public:
    bool write(uint8_t addr7, const uint8_t* data, size_t len) override {
        // Ваша реализация I2C
    }
};
```

### Добавление нового шрифта

1. Создайте файл `FontMyFont.hpp` в `src/gfx/`
2. Определите массив глифов в формате 5×7
3. Добавьте обработку в `Gfx::drawGlyph()`

---

## См. также

- [API Reference](API.md)
- [Команды SSD1315](SSD1315_COMMANDS.md)
