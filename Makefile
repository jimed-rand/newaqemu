# newaqemu — thin wrapper around CMake
#
# First build on a fresh system:  make deps && make
# Default: minimal build (no embedded VNC / LibVNCServer).
# Full build: make full

BUILD_DIR ?= build
PREFIX    ?= /usr
JOBS      ?= $(shell nproc 2>/dev/null || echo 4)
CMAKE_ARGS ?=
DEPS_SCRIPT := $(abspath scripts/install-build-deps.sh)

# Shell snippet: print cmake path (resolved each recipe so auto-deps works in one "make" run)
CMAKE_BIN = command -v cmake 2>/dev/null || command -v cmake3 2>/dev/null

CMAKE_COMMON  := -DCMAKE_INSTALL_PREFIX=$(PREFIX)
CMAKE_MINIMAL := -DWITHOUT_EMBEDDED_DISPLAY=on

ifneq ($(shell command -v ninja 2>/dev/null),)
CMAKE_GENERATOR := -G Ninja
else
CMAKE_GENERATOR :=
endif

.PHONY: all full configure build install run clean distclean reconfigure debug help
.PHONY: deps deps-full deps-runtime ensure-tools ensure-tools-minimal ensure-tools-full

all: ensure-tools-minimal
ifdef FULL
	@$(MAKE) configure PROFILE_FLAGS=
else
	@$(MAKE) configure PROFILE_FLAGS="$(CMAKE_MINIMAL)"
endif
	@$(MAKE) build

full: ensure-tools-full
	@$(MAKE) configure PROFILE_FLAGS=
	@$(MAKE) build

debug: ensure-tools-minimal
	@$(MAKE) configure PROFILE_FLAGS="$(CMAKE_MINIMAL) -DDEBUG=on"
	@$(MAKE) build

deps:
	$(DEPS_SCRIPT)

deps-full:
	$(DEPS_SCRIPT) --full

deps-runtime:
	$(DEPS_SCRIPT) --runtime

deps-full-runtime:
	$(DEPS_SCRIPT) --full --runtime

ensure-tools-minimal:
	@$(MAKE) ensure-tools DEPS_SCRIPT_ARGS=

ensure-tools-full:
	@$(MAKE) ensure-tools DEPS_SCRIPT_ARGS=--full

ensure-tools:
	@test -n "$$($(CMAKE_BIN))" || { \
		echo "==> cmake not found; installing build dependencies (sudo may be required)..."; \
		$(DEPS_SCRIPT) $(DEPS_SCRIPT_ARGS); \
	}
	@test -n "$$($(CMAKE_BIN))" || { \
		echo "ERROR: cmake not found after 'make deps'. Install CMake and Qt6 dev packages, then retry."; \
		exit 1; \
	}

configure:
	@CMAKE="$$($(CMAKE_BIN))"; \
	"$$CMAKE" -B $(BUILD_DIR) $(CMAKE_GENERATOR) $(CMAKE_COMMON) $(PROFILE_FLAGS) $(CMAKE_ARGS)

build:
	@CMAKE="$$($(CMAKE_BIN))"; \
	"$$CMAKE" --build $(BUILD_DIR) --parallel $(JOBS)

install: build
	@CMAKE="$$($(CMAKE_BIN))"; \
	"$$CMAKE" --install $(BUILD_DIR)

run: build
	$(BUILD_DIR)/newaqemu

clean:
	@if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		CMAKE="$$($(CMAKE_BIN))"; \
		"$$CMAKE" --build $(BUILD_DIR) --target clean; \
	fi

distclean:
	rm -rf $(BUILD_DIR)

reconfigure: distclean
	@$(MAKE) configure PROFILE_FLAGS="$(CMAKE_MINIMAL)"

help:
	@echo "newaqemu build wrapper (CMake backend)"
	@echo ""
	@echo "Targets:"
	@echo "  all (default)  Minimal build (no embedded VNC; no LibVNCServer)"
	@echo "  full           Embedded VNC + SPICE (requires LibVNCServer dev package)"
	@echo "  deps           Install build dependencies (apt/dnf/zypper/pacman)"
	@echo "  deps-full      Install deps for 'make full' (includes LibVNCServer)"
	@echo "  deps-runtime   Install QEMU for testing VMs after build"
	@echo "  configure      Run cmake -B $(BUILD_DIR) only"
	@echo "  build          Compile (after configure)"
	@echo "  install        Install to PREFIX (usually: sudo make install)"
	@echo "  run            Run $(BUILD_DIR)/newaqemu"
	@echo "  debug          Minimal build with -DDEBUG=on"
	@echo "  clean          cmake --build clean"
	@echo "  distclean      Remove $(BUILD_DIR)/"
	@echo "  reconfigure    distclean + minimal configure"
	@echo "  help           Show this message"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_DIR=$(BUILD_DIR)"
	@echo "  PREFIX=$(PREFIX)"
	@echo "  JOBS=$(JOBS)"
	@echo "  CMAKE          cmake or cmake3 (auto-detected)"
	@echo "  CMAKE_ARGS     Extra flags passed to cmake -B (e.g. -DWITH_LIBVIRT=off)"
	@echo "  FULL=1         Same as 'make full' when used with all (e.g. make FULL=1)"
	@echo ""
	@echo "First-time build:"
	@echo "  make deps && make"
	@echo "  make deps-runtime   # optional: install QEMU to test VMs"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make full"
	@echo "  sudo make install"
	@echo "  make PREFIX=$$HOME/.local install"
	@echo ""
	@echo "If cmake is missing, 'make' runs 'make deps' automatically (may prompt for sudo)."
	@echo "Switching minimal <-> full reconfigures CMake; use 'make distclean' if stuck."
	@echo "Advanced / Windows: see README.md (raw cmake commands)."
