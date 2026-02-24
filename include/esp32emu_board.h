#pragma once
#include <cstdint>
#include <cstdio>
#include <string>

namespace esp32emu {

enum class BoardType {
    ESP32,
    ESP32_S3,
    ESP32_C3,
    ESP32_S2,
    ESP32_C6,
    ESP32_H2,
    ARDUINO_UNO,
    ARDUINO_MEGA,
    ARDUINO_NANO,
    RP2040,
    TEENSY40,
    STM32_BLUEPILL,
    ESP32_C2,
    ESP32_P4,
    ESP32_C5,
    XIAO_ESP32S3,
    FEATHER_ESP32S3,
    M5STACK_CORE2,
    LILYGO_TDISPLAY_S3,
    QTPY_ESP32S2,
    THINGPLUS_ESP32S3,
    MATRIXPORTAL_S3,
    XIAO_ESP32C3,
    FEATHER_ESP32S2,
    OLIMEX_ESP32_POE,
    WAVESHARE_ESP32S3_ZERO,
    FEATHER_ESP32C6,
    M5ATOM_LITE
};

struct BoardConfig {
    BoardType type;
    const char* name;
    const char* chip;
    int digital_pins;
    int analog_pins;
    int ram_bytes;
    int flash_bytes;
    int cpu_freq_mhz;
    bool has_wifi;
    bool has_bluetooth;
    int builtin_led;
};

inline const BoardConfig& getBoardConfig(BoardType t) {
    static const BoardConfig configs[] = {
        {BoardType::ESP32,        "ESP32",        "ESP32",       40, 8,  327680,  4194304, 240, true,  true,  2},
        {BoardType::ESP32_S3,     "ESP32-S3",     "ESP32-S3",    48, 10, 524288,  8388608, 240, true,  true,  2},
        {BoardType::ESP32_C3,     "ESP32-C3",     "ESP32-C3",    22, 6,  409600,  4194304, 160, true,  false, 8},
        {BoardType::ESP32_S2,     "ESP32-S2",     "ESP32-S2",    46, 10, 327680,  4194304, 240, true,  false, 2},
        {BoardType::ESP32_C6,     "ESP32-C6",     "ESP32-C6",    31, 7,  524288,  4194304, 160, true,  true,  8},
        {BoardType::ESP32_H2,     "ESP32-H2",     "ESP32-H2",    22, 5,  327680,  4194304, 96,  false, true,  8},
        {BoardType::ARDUINO_UNO,  "Arduino Uno",  "ATmega328P",  14, 6,  2048,    32768,   16,  false, false, 13},
        {BoardType::ARDUINO_MEGA, "Arduino Mega", "ATmega2560",  54, 16, 8192,    262144,  16,  false, false, 13},
        {BoardType::ARDUINO_NANO, "Arduino Nano", "ATmega328P",  14, 8,  2048,    32768,   16,  false, false, 13},
        {BoardType::RP2040,       "RP2040 Pico",  "RP2040",      30, 4,  264000,  2097152, 133, false, false, 25},
        {BoardType::TEENSY40,     "Teensy 4.0",   "IMXRT1062",   40, 14, 1048576, 2097152, 600, false, false, 13},
        {BoardType::STM32_BLUEPILL,"STM32 Blue Pill","STM32F103C8",37, 10, 20480,  65536,   72,  false, false, 13},
        {BoardType::ESP32_C2,     "ESP32-C2",     "ESP32-C2",    20, 5,  272384,  4194304, 120, true,  true,  8},
        {BoardType::ESP32_P4,     "ESP32-P4",     "ESP32-P4",    55, 12, 786432, 16777216, 400, true,  true,  2},
        {BoardType::ESP32_C5,     "ESP32-C5",     "ESP32-C5",    29, 6,  524288,  4194304, 240, true,  true,  8},
        {BoardType::XIAO_ESP32S3, "XIAO ESP32-S3","ESP32-S3",   11, 9,  524288,  8388608, 240, true,  true,  21},
        {BoardType::FEATHER_ESP32S3, "Feather ESP32-S3","ESP32-S3", 21, 6, 524288, 8388608, 240, true, true, 13},
        {BoardType::M5STACK_CORE2,  "M5Stack Core2", "ESP32-D0WDQ6", 40, 8, 524288, 16777216, 240, true, true, -1},
        {BoardType::LILYGO_TDISPLAY_S3, "LilyGO T-Display S3","ESP32-S3", 46, 10, 524288, 16777216, 240, true, true, -1},
        {BoardType::QTPY_ESP32S2,     "QT Py ESP32-S2",  "ESP32-S2",    19, 4,  327680, 4194304,  240, true,  false, 39},
        {BoardType::THINGPLUS_ESP32S3,"Thing Plus ESP32-S3","ESP32-S3",  21, 6,  524288, 16777216, 240, true,  true,  13},
        {BoardType::MATRIXPORTAL_S3,  "MatrixPortal S3",   "ESP32-S3",  21, 6,  524288,  8388608, 240, true,  true,  13},
        {BoardType::XIAO_ESP32C3,    "XIAO ESP32-C3",    "ESP32-C3",  11, 4,  409600,  4194304, 160, true,  false, 10},
        {BoardType::FEATHER_ESP32S2, "Feather ESP32-S2", "ESP32-S2",  21, 6,  327680,  4194304, 240, true,  false, 13},
        {BoardType::OLIMEX_ESP32_POE,"Olimex ESP32-PoE","ESP32-D0WDQ6",40, 8, 524288, 4194304, 240, true,  true,  -1},
        {BoardType::WAVESHARE_ESP32S3_ZERO,"Waveshare ESP32-S3-Zero","ESP32-S3",18, 7, 524288, 16777216,240,true, true,  -1},
        {BoardType::FEATHER_ESP32C6,"Feather ESP32-C6","ESP32-C6",  22, 6,  524288,  8388608, 160, true,  true,  13},
        {BoardType::M5ATOM_LITE,    "M5Atom Lite",    "ESP32-PICO-D4",26, 8, 524288, 4194304, 240, true,  true,  -1},
    };
    return configs[static_cast<int>(t)];
}

inline BoardType parseBoardName(const std::string& name) {
    if (name == "uno")    return BoardType::ARDUINO_UNO;
    if (name == "mega")   return BoardType::ARDUINO_MEGA;
    if (name == "nano")   return BoardType::ARDUINO_NANO;
    if (name == "esp32s3" || name == "esp32-s3") return BoardType::ESP32_S3;
    if (name == "esp32c3" || name == "esp32-c3") return BoardType::ESP32_C3;
    if (name == "esp32s2" || name == "esp32-s2") return BoardType::ESP32_S2;
    if (name == "esp32c6" || name == "esp32-c6") return BoardType::ESP32_C6;
    if (name == "esp32h2" || name == "esp32-h2") return BoardType::ESP32_H2;
    if (name == "pico" || name == "rp2040")      return BoardType::RP2040;
    if (name == "teensy" || name == "teensy40" || name == "teensy4.0") return BoardType::TEENSY40;
    if (name == "bluepill" || name == "stm32" || name == "stm32f103") return BoardType::STM32_BLUEPILL;
    if (name == "esp32c2" || name == "esp32-c2") return BoardType::ESP32_C2;
    if (name == "esp32p4" || name == "esp32-p4") return BoardType::ESP32_P4;
    if (name == "esp32c5" || name == "esp32-c5") return BoardType::ESP32_C5;
    if (name == "xiao" || name == "xiao-s3" || name == "xiao-esp32s3") return BoardType::XIAO_ESP32S3;
    if (name == "m5core2" || name == "m5stack-core2" || name == "m5stack") return BoardType::M5STACK_CORE2;
    if (name == "tdisplay-s3" || name == "lilygo-tdisplay" || name == "t-display-s3") return BoardType::LILYGO_TDISPLAY_S3;
    if (name == "qtpy" || name == "qtpy-s2" || name == "qtpy-esp32s2") return BoardType::QTPY_ESP32S2;
    if (name == "thingplus" || name == "thingplus-s3" || name == "sparkfun-thingplus") return BoardType::THINGPLUS_ESP32S3;
    if (name == "matrixportal" || name == "matrixportal-s3") return BoardType::MATRIXPORTAL_S3;
    if (name == "xiao-c3" || name == "xiao-esp32c3") return BoardType::XIAO_ESP32C3;
    if (name == "feather-s2" || name == "feather-esp32s2") return BoardType::FEATHER_ESP32S2;
    if (name == "olimex-poe" || name == "esp32-poe" || name == "olimex-esp32-poe") return BoardType::OLIMEX_ESP32_POE;
    if (name == "waveshare-s3-zero" || name == "esp32-s3-zero" || name == "waveshare-esp32-s3-zero") return BoardType::WAVESHARE_ESP32S3_ZERO;
    if (name == "feather-c6" || name == "feather-esp32c6" || name == "feather-esp32-c6") return BoardType::FEATHER_ESP32C6;
        if (name == "m5atom" || name == "m5atom-lite" || name == "m5atomlite") return BoardType::M5ATOM_LITE;
    return BoardType::ESP32; // default
}

class Board {
public:
    static Board& instance() {
        static Board b;
        return b;
    }

    void setBoard(BoardType t) { config_ = &getBoardConfig(t); }
    const BoardConfig& config() const { return *config_; }

    bool validatePin(uint8_t pin) const {
        if (pin >= config_->digital_pins && pin < A0_base()) {
            fprintf(stderr, "\033[33m[esp32emu] WARNING: Pin %d exceeds %s digital pin count (%d)\033[0m\n",
                    pin, config_->name, config_->digital_pins);
            return false;
        }
        return true;
    }

    bool checkWiFi() const {
        if (!config_->has_wifi) {
            fprintf(stderr, "\033[33m[esp32emu] WARNING: %s does not have WiFi! Call ignored.\033[0m\n",
                    config_->name);
            return false;
        }
        return true;
    }

    int A0_base() const {
        switch (config_->type) {
            case BoardType::ARDUINO_UNO:  return 14; // A0=14..A5=19
            case BoardType::ARDUINO_NANO: return 14; // A0=14..A7=21
            case BoardType::ARDUINO_MEGA: return 54; // A0=54..A15=69
            case BoardType::RP2040:       return 26; // A0=26..A3=29
            case BoardType::TEENSY40:     return 14; // A0=14..A13=27
            case BoardType::STM32_BLUEPILL: return 15; // A0=15..A9=24
            default: return 36; // ESP32
        }
    }

    void printBoardInfo() const {
        fprintf(stderr, "[esp32emu] Board: %s | Chip: %s | RAM: %dB | Flash: %dB | CPU: %dMHz\n",
                config_->name, config_->chip, config_->ram_bytes, config_->flash_bytes, config_->cpu_freq_mhz);
        fprintf(stderr, "[esp32emu] Digital Pins: %d | Analog Pins: %d | WiFi: %s | LED_BUILTIN: %d\n",
                config_->digital_pins, config_->analog_pins,
                config_->has_wifi ? "Yes" : "No", config_->builtin_led);
    }

    void printASCII() const {
        switch (config_->type) {
            case BoardType::ARDUINO_UNO:  printUnoASCII(); break;
            case BoardType::ARDUINO_MEGA: printMegaASCII(); break;
            case BoardType::ARDUINO_NANO: printNanoASCII(); break;
            case BoardType::ESP32_S3:     printESP32S3ASCII(); break;
            case BoardType::ESP32_C3:     printESP32C3ASCII(); break;
            case BoardType::ESP32_S2:     printESP32S2ASCII(); break;
            case BoardType::ESP32_C6:     printESP32C6ASCII(); break;
            case BoardType::ESP32_H2:     printESP32H2ASCII(); break;
            case BoardType::RP2040:       printRP2040ASCII(); break;
            case BoardType::TEENSY40:     printTeensy40ASCII(); break;
            case BoardType::STM32_BLUEPILL: printBluePillASCII(); break;
            case BoardType::ESP32_C2:     printESP32C2ASCII(); break;
            case BoardType::ESP32_P4:     printESP32P4ASCII(); break;
            case BoardType::ESP32_C5:     printESP32C5ASCII(); break;
            case BoardType::XIAO_ESP32S3: printXIAOESP32S3ASCII(); break;
            case BoardType::XIAO_ESP32C3: printXIAOESP32C3ASCII(); break;
            case BoardType::OLIMEX_ESP32_POE: printOlimexESP32PoEASCII(); break;
            case BoardType::WAVESHARE_ESP32S3_ZERO: printWaveshareESP32S3ZeroASCII(); break;
            case BoardType::FEATHER_ESP32C6: printFeatherESP32C6ASCII(); break;
            default: printESP32ASCII(); break;
        }
    }

private:
    const BoardConfig* config_ = &getBoardConfig(BoardType::ESP32);

    void printUnoASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║         ARDUINO UNO              ║
    ║         ATmega328P               ║
    ║                                  ║
    ║  D0-D13    [■■■■■■■■■■■■■■]     ║
    ║  A0-A5     [■■■■■■]             ║
    ║  PWR  [●]  USB [═══]            ║
    ╚══════════════════════════════════╝
)");
    }

    void printMegaASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════════════════╗
    ║              ARDUINO MEGA 2560               ║
    ║              ATmega2560                       ║
    ║                                              ║
    ║  D0-D53    [■■■■■■■■■■■■■■■■■■■■■■■■■■■]   ║
    ║  A0-A15    [■■■■■■■■■■■■■■■■]               ║
    ║  PWR  [●]  USB [═══]                         ║
    ╚══════════════════════════════════════════════╝
)");
    }

    void printESP32ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║           ESP32-WROOM            ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi/BT    ║
    ║          └──────────┘            ║
    ║  GPIO 0-39  [■■■■■■■■■■■■]     ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32S3ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║          ESP32-S3-WROOM          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi/BT5   ║
    ║          └──────────┘            ║
    ║  GPIO 0-48  [■■■■■■■■■■■■■■]   ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32C3ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║          ESP32-C3 (RISC-V)       ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi       ║
    ║          └──────────┘            ║
    ║  GPIO 0-21  [■■■■■■■■■■]       ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32S2ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║          ESP32-S2-SAOLA          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi       ║
    ║          └──────────┘            ║
    ║  GPIO 0-46  [■■■■■■■■■■■■■]    ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printNanoASCII() const {
        fprintf(stderr, R"(
    ╔════════════════════════════╗
    ║       ARDUINO NANO         ║
    ║       ATmega328P           ║
    ║                            ║
    ║  D0-D13  [■■■■■■■■■■■■■■] ║
    ║  A0-A7   [■■■■■■■■]       ║
    ║  Mini-USB [═══]            ║
    ╚════════════════════════════╝
)");
    }

    void printESP32C6ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       ESP32-C6 (RISC-V)          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi6/BT5  ║
    ║          └──────────┘            ║
    ║  GPIO 0-30  [■■■■■■■■■■■]      ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printRP2040ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       RASPBERRY PI PICO          ║
    ║           RP2040                 ║
    ║       ┌──────────────┐           ║
    ║       │  ◉  RP2040   │           ║
    ║       └──────────────┘           ║
    ║  GP0-GP29  [■■■■■■■■■■■■■■]    ║
    ║  Micro-USB [═══]  BOOTSEL [●]   ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32H2ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       ESP32-H2 (RISC-V)          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ BT5/Zigbee ║
    ║          └──────────┘            ║
    ║  GPIO 0-21  [■■■■■■■■■■]       ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printTeensy40ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║          TEENSY 4.0              ║
    ║         IMXRT1062 @600MHz        ║
    ║                                  ║
    ║  D0-D39    [■■■■■■■■■■■■■■■■]  ║
    ║  A0-A13    [■■■■■■■■■■■■■■]    ║
    ║  Micro-USB [═══]  Program [●]   ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32C2ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       ESP32-C2 (RISC-V)          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi/BT5   ║
    ║          └──────────┘            ║
    ║  GPIO 0-19  [■■■■■■■■■]        ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32P4ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       ESP32-P4 (RISC-V)          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ 400MHz     ║
    ║          │ ████████ │ WiFi/BT5   ║
    ║          └──────────┘            ║
    ║  GPIO 0-54  [■■■■■■■■■■■■■]    ║
    ║  USB-OTG [═══]  JTAG [════]     ║
    ║  MIPI-DSI  MIPI-CSI  Ethernet   ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printESP32C5ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       ESP32-C5 (RISC-V)          ║
    ║          ┌──────────┐            ║
    ║          │ ████████ │ WiFi6/BT5  ║
    ║          │          │ Thread     ║
    ║          └──────────┘            ║
    ║  GPIO 0-28  [■■■■■■■■■■■]      ║
    ║  PWR  [●]  USB-C [═══]          ║
    ╚══════════════════════════════════╝
)");
    }

    void printXIAOESP32S3ASCII() const {
        fprintf(stderr, R"(
    ╔═══════════════════════════════╗
    ║     SEEED XIAO ESP32-S3      ║
    ║     ┌────────────────┐       ║
    ║     │  ◉  ESP32-S3   │       ║
    ║     │    WiFi/BT5    │       ║
    ║     └────────────────┘       ║
    ║  D0-D10 [■■■■■■■■■■■]       ║
    ║  A0-A8  [■■■■■■■■■]         ║
    ║  USB-C [═══]  BOOT [●]      ║
    ╚═══════════════════════════════╝
)");
    }

    void printXIAOESP32C3ASCII() const {
        fprintf(stderr, R"(
    ╔═══════════════════════════════╗
    ║     SEEED XIAO ESP32-C3      ║
    ║     ┌────────────────┐       ║
    ║     │  ◉  ESP32-C3   │       ║
    ║     │   WiFi (RISC-V)│       ║
    ║     └────────────────┘       ║
    ║  D0-D10 [■■■■■■■■■■■]       ║
    ║  A0-A3  [■■■■]              ║
    ║  USB-C [═══]  BOOT [●]      ║
    ╚═══════════════════════════════╝
)");
    }

    void printBluePillASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║       STM32 BLUE PILL            ║
    ║       STM32F103C8 @72MHz         ║
    ║                                  ║
    ║  PA0-PC15  [■■■■■■■■■■■■■■■]   ║
    ║  BOOT0 [●]  BOOT1 [●]          ║
    ║  Micro-USB [═══]  RESET [●]    ║
    ╚══════════════════════════════════╝
)");
    }

    void printOlimexESP32PoEASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════════╗
    ║        OLIMEX ESP32-PoE              ║
    ║        ESP32-D0WDQ6 @240MHz          ║
    ║                                      ║
    ║  GPIO0-GPIO39 [■■■■■■■■■■■■■■■■■]  ║
    ║  ETH [RJ45]  PoE [⚡]  USB [═══]    ║
    ║  RESET [●]  BOOT [●]  SD [▪▪]      ║
    ╚══════════════════════════════════════╝
)");
    }
    void printWaveshareESP32S3ZeroASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════╗
    ║     Waveshare ESP32-S3-Zero      ║
    ║     ESP32-S3 @240MHz (16MB)      ║
    ║                                  ║
    ║  GPIO0-GPIO18 [■■■■■■■■■■■■■]  ║
    ║  USB-C [═══]  BOOT [●]  RST [●] ║
    ║  RGB LED [★]  Stamp form factor  ║
    ╚══════════════════════════════════╝
)");
    }

    void printFeatherESP32C6ASCII() const {
        fprintf(stderr, R"(
    ╔══════════════════════════════════════╗
    ║     Adafruit Feather ESP32-C6        ║
    ║     ESP32-C6 @160MHz (RISC-V)        ║
    ║                                      ║
    ║  ┌────────────────────┐              ║
    ║  │  ◉  ESP32-C6      │  WiFi6/BT5   ║
    ║  │     Zigbee/Thread  │  8MB Flash   ║
    ║  └────────────────────┘              ║
    ║  GPIO0-GPIO22 [■■■■■■■■■■■■■■■■]   ║
    ║  LiPo [🔋]  STEMMA QT [■■]         ║
    ║  USB-C [═══]  BOOT [●]  RST [●]    ║
    ╚══════════════════════════════════════╝
)");
    }
};

} // namespace esp32emu
