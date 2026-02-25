# ============================================================================
# Cross-platform Makefile for RaySim (CMake)
# Author: @DMsuDev
# ============================================================================
#
# Works on Linux, macOS, and Windows (MSYS/MinGW or PowerShell).
# Configurable via CLI:  make build BUILD_TYPE=Release CMAKE_GEN=Ninja
#
# Usage:
#   make                    Build (Debug, auto-configures)
#   make release            Build in Release mode
#   make run                Build and run
#   make tidy               Static analysis with clang-tidy
#   make help               Show all targets
#
# ============================================================================

.DEFAULT_GOAL := all
.SUFFIXES:

# ─── Project ─────────────────────────────────────────────────────────────

PROJECT_NAME  := RaySim
PROJECT_VER   := 0.2.1
BUILD_DIR     := build
BUILD_TYPE    ?= Debug
LIBRARY_TYPE  := Static Library

# ─── Sources ─────────────────────────────────────────────────────────────
# Add directories here; cppcheck and clang-tidy pick them up automatically.

SRC_DIRS      := include src examples
SRC_FILES     := $(foreach d,$(SRC_DIRS),$(wildcard $(d)/**/*.cpp $(d)/**/*.hpp $(d)/*.cpp $(d)/*.hpp))
CPP_FILES     := $(filter %.cpp,$(SRC_FILES))
INCLUDE_FLAGS := $(addprefix -I ,$(SRC_DIRS))

# ─── CMake ────────────────────────────────────────────────────────────────

CONFIGURE_STAMP := $(BUILD_DIR)/.configured
CMAKE_LISTS     := $(wildcard CMakeLists.txt vendor/CMakeLists.txt)
CMAKE_FLAGS     = -DCMAKE_BUILD_TYPE:STRING=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE

# ─── Platform ────────────────────────────────────────────────────────────

ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    PYTHON      := python -m
    EXE_EXT     := .exe
    FIXPATH      = $(subst /,\,$1)
    CMAKE_GEN   ?= Visual Studio 18 2026
    _find_tool   = $(shell pwsh -NoProfile -Command "(Get-Command $(1) -ErrorAction SilentlyContinue).Source")
else
    DETECTED_OS := $(shell uname -s)
    PYTHON      := python3 -m
    EXE_EXT     :=
    FIXPATH      = $1
    CMAKE_GEN   ?= Unix Makefiles
    _find_tool   = $(shell command -v $(1) 2>/dev/null)
endif

# ─── Verbose (0 = silent · 1 = colored detail) ───────────────────────────
# Automatically silenced when make is invoked with -s or -j flags.

VERBOSE ?= 1

ifneq ($(findstring s,$(MAKEFLAGS)),)
    VERBOSE := 0
endif

# ─── Colors ───────────────────────────────────────────────────────────────

ifeq ($(VERBOSE),1)
    RST   := \033[0m
    BLD   := \033[1m
    DIM   := \033[2m
    GRN   := \033[0;32m
    YLW   := \033[0;33m
    BLU   := \033[0;34m
    CYN   := \033[0;36m
    # Semantic
    LINE  := \033[1;30m
    TITLE := \033[1;35m
    INFO  := \033[0;34m
    OK    := \033[0;32m
    WARN  := \033[0;33m
    ERR   := \033[0;31m
    HL    := \033[1;36m
else
    RST   :=
    BLD   :=
    DIM   :=
    GRN   :=
    YLW   :=
    BLU   :=
    CYN   :=
    LINE  :=
    TITLE :=
    INFO  :=
    OK    :=
    WARN  :=
    ERR   :=
    HL    :=
endif

# ─── Display helpers ─────────────────────────────────────────────────────
#
#   $(call _banner,SECTION)   project-info header (build / configure / tidy)
#   $(call _section,TITLE)    lightweight section divider
#   $(call _ok,MSG)           success   ✓
#   $(call _fail,MSG)         error     ✗  (exits with code 1)
#   $(call _warn,MSG)         warning   ⚠
#   $(call _done)             closing rule
#   $(call _require,VAR,name,install-hint)

define _banner
@printf "\n$(LINE)─── $(TITLE)$(1): $(RST)$(BLD)$(PROJECT_NAME)$(RST) $(DIM)v$(PROJECT_VER)$(RST)\n"
@printf "  $(INFO)Build Type:$(RST)   $(BLD)$(BUILD_TYPE)$(RST)\n"
@printf "  $(BLD)> Generator:$(RST)  %s\n" "$(CMAKE_GEN)"
@printf "  $(BLD)> Platform:$(RST)   $(DETECTED_OS)\n"
@printf "$(LINE)────────────────────────────────────────────$(RST)\n"
endef

_section = @printf "\n$(LINE)───────$(RST) $(TITLE)$(1)$(RST)\n"
_ok      = @printf "  $(OK)✓$(RST) %s\n" "$(1)"
_fail    = @printf "  $(ERR)✗ Error: $(1)$(RST)\n" && exit 1
_warn    = @printf "  $(WARN)⚠ $(1)$(RST)\n"
_done    = @printf "$(LINE)────────────────────────────────────────────$(RST)\n"

_require = $(if $($(1)),,$(call _fail,$(2) is not installed. Install: $(3)))

# ─── Tool detection (evaluated once) ─────────────────────────────────────

HAS_CLANG_TIDY := $(call _find_tool,clang-tidy)
HAS_CPPCHECK   := $(call _find_tool,cppcheck)
HAS_ACT        := $(call _find_tool,act)

# ─── Generator layout ────────────────────────────────────────────────────

IS_MULTICONFIG := $(or $(findstring Visual Studio,"$(CMAKE_GEN)"),$(findstring Xcode,"$(CMAKE_GEN)"))

ifeq ($(IS_MULTICONFIG),)
    EXAMPLE_PATH ?= examples/build
    LIB_PATH     ?= $(BUILD_DIR)/lib
else
    EXAMPLE_PATH ?= examples/build
    LIB_PATH     ?= $(BUILD_DIR)/lib/$(BUILD_TYPE)
endif

# ─── Tidy (dedicated Ninja build for compile_commands.json) ──────────────

TIDY_DIR   := build-tidy
TIDY_STAMP := $(TIDY_DIR)/.configured

# ─── Phony ────────────────────────────────────────────────────────────────

.PHONY: all configure build rebuild release relwithdebinfo \
        example-bouncing example-lissajous example-mouse \
        example-run clean purge \
        tidy cppcheck \
        pre-commit pre-commit-install pre-commit-update \
        tag act-ci act-quality info help

# ═════════════════════════════════════════════════════════════════════════
#  BUILD
# ═════════════════════════════════════════════════════════════════════════

ifeq ($(words $(SRC_FILES)),0)
all:
	$(call _fail,No source files found in: $(SRC_DIRS))
else
all: build
endif

$(CONFIGURE_STAMP): $(CMAKE_LISTS)
	$(call _banner,Configure)
	@cmake -B $(BUILD_DIR) -G "$(CMAKE_GEN)" $(CMAKE_FLAGS)
	@cmake -E touch $@

configure:
	$(call _banner,Configure)
	@cmake -B $(BUILD_DIR) -G "$(CMAKE_GEN)" $(CMAKE_FLAGS)
	@cmake -E touch $(CONFIGURE_STAMP)

build: $(CONFIGURE_STAMP)
	$(call _banner,Build)
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) --parallel
	$(call _ok,Build succeeded)
	$(call _done)

rebuild: clean build

release:
	@$(MAKE) --no-print-directory build BUILD_TYPE=Release
	$(call _ok,Release build complete (optimized))

relwithdebinfo:
	@$(MAKE) --no-print-directory build BUILD_TYPE=RelWithDebInfo
	$(call _ok,RelWithDebInfo build complete (optimized + symbols))

# ═════════════════════════════════════════════════════════════════════════
#  EXAMPLES
# ═════════════════════════════════════════════════════════════════════════

example-bouncing: build
	$(call _section,Example: Bouncing Balls)
	@$(call FIXPATH,$(EXAMPLE_PATH)/$(BUILD_TYPE)/Bouncing_Balls$(EXE_EXT))
	$(call _done)

example-lissajous: build
	$(call _section,Example: Lissajous Curves)
	@$(call FIXPATH,$(EXAMPLE_PATH)/$(BUILD_TYPE)/Lissajous_Curves$(EXE_EXT))
	$(call _done)

example-mouse: build
	$(call _section,Example: Mouse Detection 2D)
	@$(call FIXPATH,$(EXAMPLE_PATH)/$(BUILD_TYPE)/Mouse_detection_2D$(EXE_EXT))
	$(call _done)

example-run: example-bouncing

# ═════════════════════════════════════════════════════════════════════════
#  CLEAN
# ═════════════════════════════════════════════════════════════════════════

clean:
	$(call _section,Clean)
	@printf "  %-14s : %s\n" "Removing" "build artifacts"
	$(if $(wildcard $(CONFIGURE_STAMP)),-@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) > /dev/null)
	@cmake -E rm -f cppcheck.log
	@cmake -E rm -rf examples/build
	$(call _ok,Clean done)
	$(call _done)

purge:
	$(call _section,Full Clean)
	@printf "  %-14s : %s\n" "Removing" "$(BUILD_DIR)/ $(TIDY_DIR)/ examples/build/"
	@cmake -E rm -rf $(BUILD_DIR) $(TIDY_DIR) examples/build
	$(call _ok,Purge done)
	$(call _done)

# ═════════════════════════════════════════════════════════════════════════
#  CODE QUALITY
# ═════════════════════════════════════════════════════════════════════════

$(TIDY_STAMP): $(CMAKE_LISTS)
	$(call _section,Configure Tidy)
	@printf "  $(DIM)Setting up Ninja build for compile_commands.json...$(RST)\n"
	@cmake -B $(TIDY_DIR) -G Ninja $(CMAKE_FLAGS)
	@cmake -E touch $@

tidy: $(TIDY_STAMP)
	$(call _require,HAS_CLANG_TIDY,clang-tidy,apt install clang-tidy | choco install llvm)
	$(call _section,Tidy)
	@printf "  %-14s : %d file(s)\n" "Scanning" $(words $(CPP_FILES))
	@printf "  %-14s : %s\n" "Build DB" "$(TIDY_DIR)/"
	@$(foreach f,$(CPP_FILES),clang-tidy -p $(TIDY_DIR) --extra-arg=-Iinclude $(f) ;)
	$(call _ok,Tidy done)
	$(call _done)

cppcheck:
	$(call _require,HAS_CPPCHECK,cppcheck,apt install cppcheck | choco install cppcheck)
	$(call _section,Cppcheck)
	@printf "  %-14s : %d file(s)\n" "Scanning" $(words $(CPP_FILES))
	@cppcheck --enable=warning,style,performance,portability \
	    --std=c++17 \
	    --suppress=missingIncludeSystem \
	    --suppress=unmatchedSuppression \
	    --inline-suppr \
	    --template="{file}:{line}: {severity} ({id}): {message}" \
	    $(INCLUDE_FLAGS) $(SRC_DIRS) 2>&1 \
	    $(if $(filter $(DETECTED_OS),Linux Darwin),| tee cppcheck.log,> cppcheck.log)
	$(call _ok,Results saved to cppcheck.log)
	$(call _done)

# ═════════════════════════════════════════════════════════════════════════
#  PRE-COMMIT
# ═════════════════════════════════════════════════════════════════════════

pre-commit-install:
	$(call _section,Pre-commit Install)
	@$(PYTHON) pre_commit install
	@$(PYTHON) pre_commit install --hook-type commit-msg
	$(call _ok,Hooks installed)
	$(call _done)

pre-commit:
	$(call _section,Pre-commit)
	@$(PYTHON) pre_commit run --all-files
	$(call _done)

pre-commit-update:
	$(call _section,Pre-commit Update)
	@$(PYTHON) pre_commit autoupdate
	$(call _ok,Hooks updated)
	$(call _done)

# ═════════════════════════════════════════════════════════════════════════
#  CI / RELEASE
# ═════════════════════════════════════════════════════════════════════════

tag:
ifndef VERSION
	$(call _fail,Usage: make tag VERSION=v0.1.0)
else
	$(call _section,Tag)
	@printf "  %-14s : %s\n" "Version" "$(VERSION)"
	@git tag -a $(VERSION) -m "Release $(VERSION)"
	@git push origin $(VERSION)
	$(call _ok,Tag $(VERSION) pushed)
	$(call _done)
endif

act-ci:
	$(call _require,HAS_ACT,act,choco install act-cli | winget install nektos.act | brew install act)
	$(call _section,Act CI)
	@act push -W .github/workflows/ci.yml --container-architecture linux/amd64
	$(call _done)

act-quality:
	$(call _require,HAS_ACT,act,choco install act-cli | winget install nektos.act | brew install act)
	$(call _section,Act Quality)
	@act push -W .github/workflows/code-quality.yml --container-architecture linux/amd64
	$(call _done)

# ═════════════════════════════════════════════════════════════════════════
#  INFO
# ═════════════════════════════════════════════════════════════════════════

info:
	$(call _section,Configuration)
	@printf "  %-14s : %s\n" "Type" "C++ Library"
	@printf "  %-14s : %d file(s)\n" "Sources" $(words $(CPP_FILES))
	@printf "  %-14s : %s\n" "Build Type" "$(BUILD_TYPE)"
	@printf "  %-14s : %s\n" "Generator" "$(CMAKE_GEN)"
	@printf "  %-14s : %s\n" "Platform" "$(DETECTED_OS)"
	@printf "  %-14s : %s\n" "Library Path" "$(LIB_PATH)/"
	@printf "  %-14s : %s\n" "Examples Path" "$(EXAMPLE_PATH)/"
	@printf "\n$(LINE)───────$(RST) $(TITLE)Tools$(RST)\n"
	@printf "  %-14s : %s\n" "Tidy Dir" "$(TIDY_DIR)/"
	@printf "  %-14s : $(if $(HAS_CLANG_TIDY),$(OK)✓ found$(RST),$(ERR)✗ missing$(RST))\n" "clang-tidy"
	@printf "  %-14s : $(if $(HAS_CPPCHECK),$(OK)✓ found$(RST),$(ERR)✗ missing$(RST))\n" "cppcheck"
	@printf "  %-14s : $(if $(HAS_ACT),$(OK)✓ found$(RST),$(ERR)✗ missing$(RST))\n" "act"
	$(call _done)

help:
	@printf "\n$(LINE)───────$(RST) $(TITLE)$(PROJECT_NAME) library$(RST)\n"
	@printf "    $(GRN)make configure$(RST)          Force CMake reconfiguration\n"
	@printf "    $(GRN)make release$(RST)            Release build\n"
	@printf "    $(GRN)make relwithdebinfo$(RST)     Release + debug symbols\n"
	@printf "    $(GRN)make rebuild$(RST)            Clean + build\n"
	@printf "\n"
	@printf "  $(BLD)Examples:$(RST)\n"
	@printf "    $(GRN)make example-run$(RST)        Run Bouncing Balls example\n"
	@printf "    $(GRN)make example-bouncing$(RST)   Run Bouncing Balls\n"
	@printf "    $(GRN)make example-lissajous$(RST)  Run Lissajous Curves\n"
	@printf "    $(GRN)make example-mouse$(RST)      Run Mouse Detection 2D\n"
	@printf "\n"
	@printf "  $(BLD)Clean:$(RST)\n"
	@printf "    $(GRN)make clean$(RST)              Remove build artifacts\n"
	@printf "    $(GRN)make purge$(RST)              Delete all build directories\n"
	@printf "\n"
	@printf "  $(BLD)Code Quality:$(RST)\n"
	@printf "    $(GRN)make tidy$(RST)               clang-tidy analysis\n"
	@printf "    $(GRN)make cppcheck$(RST)           cppcheck analysis\n"
	@printf "\n"
	@printf "  $(BLD)Pre-commit:$(RST)\n"
	@printf "    $(GRN)make pre-commit-install$(RST) Install git hooks\n"
	@printf "    $(GRN)make pre-commit$(RST)         Run on all files\n"
	@printf "    $(GRN)make pre-commit-update$(RST)  Update hook versions\n"
	@printf "\n"
	@printf "  $(BLD)CI / Release:$(RST)\n"
	@printf "    $(GRN)make tag VERSION=v0.2.1$(RST) Create & push release tag\n"
	@printf "    $(GRN)make act-ci$(RST)             Run CI workflow locally\n"
	@printf "    $(GRN)make act-quality$(RST)        Run quality workflow locally\n"
	@printf "\n"
	@printf "  $(BLD)Utility:$(RST)\n"
	@printf "    $(GRN)make info$(RST)               Project configuration & tools\n"
	@printf "    $(GRN)make help$(RST)               This help\n"
	@printf "\n"
	@printf "  $(BLD)Examples:$(RST)\n"
	@printf "    make BUILD_TYPE=Release             Release build\n"
	@printf "    make example-bouncing               Test example\n"
	@printf "    make tidy                           Static analysis\n"
	@printf "    make purge build                    Fresh build from scratch\n"
	@printf "    make VERBOSE=0 build                Silent build\n"
	$(call _done)
