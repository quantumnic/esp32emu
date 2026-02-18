CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
INCLUDES  = -Iinclude

SRCDIR = src
SRCS   = $(SRCDIR)/globals.cpp $(SRCDIR)/webserver.cpp $(SRCDIR)/wificlient.cpp $(SRCDIR)/httpclient.cpp

# Library build (archive)
OBJDIR = build
OBJS   = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
LIB    = build/libesp32emu.a

.PHONY: all clean test lib examples

all: lib

lib: $(LIB)

$(LIB): $(OBJS)
	@mkdir -p $(dir $@)
	ar rcs $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ── Tests ────────────────────────────────────────────────────────────
TEST_SRCS = $(wildcard test/*.cpp)
TEST_BINS = $(patsubst test/%.cpp,build/test/%,$(TEST_SRCS))

test: $(TEST_BINS)
	@echo "══════════════════════════════════════"
	@echo "  Running esp32emu tests..."
	@echo "══════════════════════════════════════"
	@pass=0; fail=0; \
	for t in $(TEST_BINS); do \
		name=$$(basename $$t); \
		if $$t; then \
			echo "  ✅ $$name"; \
			pass=$$((pass+1)); \
		else \
			echo "  ❌ $$name"; \
			fail=$$((fail+1)); \
		fi; \
	done; \
	echo "══════════════════════════════════════"; \
	echo "  $$pass passed, $$fail failed"; \
	echo "══════════════════════════════════════"; \
	[ $$fail -eq 0 ]

build/test/%: test/%.cpp $(LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -Lbuild -lesp32emu -o $@

# ── Examples (single-file .cpp) ──────────────────────────────────────
build/examples/%: examples/%.cpp $(LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(SRCDIR)/esp32emu_main.cpp -Lbuild -lesp32emu -o $@

# ── Examples (directory-based .ino) ──────────────────────────────────
EXAMPLE_DIRS = $(wildcard examples/*/.)
EXAMPLE_INOS = $(foreach d,$(EXAMPLE_DIRS),$(wildcard $(d:/.=)/*.ino))
EXAMPLE_CPPS = $(foreach d,$(EXAMPLE_DIRS),$(wildcard $(d:/.=)/*.cpp))
EXAMPLE_BINS_INO = $(foreach ino,$(EXAMPLE_INOS),build/examples/$(notdir $(patsubst %/,%,$(dir $(ino)))))
EXAMPLE_BINS_CPP = $(foreach cpp,$(EXAMPLE_CPPS),build/examples/$(notdir $(patsubst %/,%,$(dir $(cpp)))))

build/examples/%: examples/%/*.ino $(LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -include Arduino.h -x c++ $< $(SRCDIR)/esp32emu_main.cpp -Lbuild -lesp32emu -o $@

build/examples/%: examples/%/*.cpp $(LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(SRCDIR)/esp32emu_main.cpp -Lbuild -lesp32emu -o $@

examples: build/examples/blink build/examples/webserver_demo $(EXAMPLE_BINS_INO) $(EXAMPLE_BINS_CPP)

# ── Clean ────────────────────────────────────────────────────────────
clean:
	rm -rf build
