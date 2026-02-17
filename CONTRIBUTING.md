# Contributing to esp32emu

Thanks for your interest! Here's how to contribute.

## Quick Start

```bash
git clone https://github.com/redbasecap-buiss/esp32emu.git
cd esp32emu
make test
```

## Guidelines

- **Tests must pass.** Run `make test` before submitting a PR.
- **Keep headers Arduino-compatible.** The API should match the real ESP32/Arduino SDK.
- **Header-only when possible.** Minimize `.cpp` files — inline implementations are preferred.
- **No external dependencies.** The emulator uses only the C++ standard library and POSIX APIs.

## Adding a New Mock

1. Create `include/YourHeader.h` with the Arduino-compatible API
2. If needed, add implementation in `src/yourheader.cpp` and update the Makefile
3. Add a test in `test/test_yourheader.cpp`
4. Add an extern global in `src/globals.cpp` if the mock uses a singleton
5. Update the API coverage table in README.md

## Code Style

- C++17
- `snake_case` for internal names, Arduino `camelCase` for public API
- Braces on same line
- No tabs — 4 spaces

## Reporting Issues

Open a GitHub issue with:
- What you expected
- What happened
- Minimal reproduction code
- Your OS and compiler version
