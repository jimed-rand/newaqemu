# newaqemu — thin wrapper around CMake
#
# Default: minimal build (no embedded VNC / LibVNCServer).
# Full build: make full

BUILD_DIR ?= build
PREFIX    ?= /usr
JOBS      ?= $(shell nproc 2>/dev/null || echo 4)
CMAKE     ?= cmake
CMAKE_ARGS ?=

CMAKE_COMMON  := -DCMAKE_INSTALL_PREFIX=$(PREFIX)
CMAKE_MINIMAL := -DWITHOUT_EMBEDDED_DISPLAY=on

ifneq ($(shell command -v ninja 2>/dev/null),)
CMAKE_GENERATOR := -G Ninja
else
CMAKE_GENERATOR :=
endif

.PHONY: all full configure build install run clean distclean reconfigure debug help

all:
ifdef FULL
	@$(MAKE) configure PROFILE_FLAGS=
else
	@$(MAKE) configure PROFILE_FLAGS="$(CMAKE_MINIMAL)"
endif
	@$(MAKE) build

full:
	@$(MAKE) configure PROFILE_FLAGS=
	@$(MAKE) build

debug:
	@$(MAKE) configure PROFILE_FLAGS="$(CMAKE_MINIMAL) -DDEBUG=on"
	@$(MAKE) build

configure:
	$(CMAKE) -B $(BUILD_DIR) $(CMAKE_GENERATOR) $(CMAKE_COMMON) $(PROFILE_FLAGS) $(CMAKE_ARGS)

build:
	$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS)

install: build
	$(CMAKE) --install $(BUILD_DIR)

run: build
	$(BUILD_DIR)/newaqemu

clean:
	@if [ -f "$(BUILD_DIR)/CMakeCache.txt" ]; then \
		$(CMAKE) --build $(BUILD_DIR) --target clean; \
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
	@echo "  CMAKE_ARGS     Extra flags passed to cmake -B (e.g. -DWITH_LIBVIRT=off)"
	@echo "  FULL=1         Same as 'make full' when used with all (e.g. make FULL=1)"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make full"
	@echo "  sudo make install"
	@echo "  make PREFIX=$$HOME/.local install"
	@echo ""
	@echo "Switching minimal <-> full reconfigures CMake; use 'make distclean' if stuck."
	@echo "Advanced / Windows: see README.md (raw cmake commands)."
