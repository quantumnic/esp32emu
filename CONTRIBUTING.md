# Contributing to esp32emu

Thanks for your interest in contributing! 🎉

## Quick Start

```bash
git clone https://github.com/redbasecap-buiss/esp32emu.git
cd esp32emu
make test     # Run all tests
make examples # Build all examples
```

## How to Contribute

### Adding a New Library Mock

1. Create header in `include/` (e.g., `MyLibrary.h`)
2. Implement as header-only if possible; use `src/` for larger implementations
3. Add test in `test/test_mylib.cpp`
4. Add example in `examples/myexample/myexample.ino`
5. Update `CHANGELOG.md`

### Mock Design Guidelines

- **Header-only preferred** — keeps build simple, one `#include` and it works
- **Thread-safe** — use `std::mutex` for shared state (FreeRTOS tasks run as real threads)
- **Test helpers** — add methods like `simulateX()`, `injectData()`, `getState()` for testing
- **Namespace** — put implementation in `esp32emu::` namespace, expose Arduino-compatible free functions
- **Reset** — every controller needs a `reset()` method for test isolation

### Code Style

- C++17, no external dependencies beyond POSIX
- `snake_case` for internal APIs, Arduino-compatible naming for public APIs
- Keep mocks faithful to the real Arduino/ESP-IDF API signatures
- Use `fprintf(stderr, "[esp32emu] ...")` for emulator-level logging

### Testing

Every mock needs a test in `test/`. Tests should:
- Use `assert()` for verification
- Print a summary line: `printf("test_name: all assertions passed\n");`
- Return 0 on success
- Be self-contained (reset state at the beginning)

### Examples

Examples should:
- Live in `examples/<name>/<name>.ino`
- Have a descriptive comment header explaining what they do
- Be runnable: `./esp32emu run examples/<name>/<name>.ino`
- Demonstrate practical use cases

## Adding a New Board

1. Add board definition in `include/esp32emu_board.h`
2. Include ASCII art for the board
3. Set correct pin counts, RAM, flash, and feature flags
4. Add to `--list-boards` output
5. Add an example if the board has unique features

## Pull Requests

- Keep PRs focused — one feature or fix per PR
- Run `make test` before submitting
- Update CHANGELOG.md with your changes
- Add tests for new functionality

## Questions?

Open an issue! We're happy to help.
