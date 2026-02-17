// esp32emu — main() that calls user's setup() and loop()
#include "Arduino.h"
#include <csignal>
#include <cstdio>
#include <atomic>

// User must define these
extern void setup();
extern void loop();

static std::atomic<bool> running{true};

static void sighandler(int sig) {
    (void)sig;
    fprintf(stderr, "\n[esp32emu] Interrupted — shutting down.\n");
    running = false;
}

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);

    fprintf(stderr, "[esp32emu] Starting emulator...\n");
    fprintf(stderr, "[esp32emu] Chip: ESP32-EMU | Heap: 256KB | SDK: esp32emu-1.0.0\n");
    fprintf(stderr, "[esp32emu] Press Ctrl+C to stop.\n\n");

    setup();

    while (running) {
        loop();
    }

    fprintf(stderr, "[esp32emu] Stopped.\n");
    return 0;
}
