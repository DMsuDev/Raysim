# ============================================================================
# Cross-platform Makefile for Raysim (CMake)
# Author: @DMsuDev
# ============================================================================
#
# Works on Linux, macOS, and Windows (MSYS/MinGW or PowerShell).
# Configurable via CLI:  make build BUILD_TYPE=Release CMAKE_GEN=Ninja
# ============================================================================


.DEFAULT_GOAL := all
.SUFFIXES:

# =========================================================================
# Project
# =========================================================================

PROJECT_NAME  := Raysim
PROJECT_VER   := 0.3.1
BUILD_DIR     := build
BUILD_TYPE    ?= Release

# =========================================================================
# Sources
# =========================================================================
# Add directories here; cppcheck and clang-tidy pick them up automatically.

SRC_DIRS      := include src examples
ifeq ($(OS),Windows_NT)
SRC_FILES := $(shell pwsh -NoProfile -Command "Get-ChildItem -Recurse -Include *.cpp,*.hpp -Path $(SRC_DIRS) | ForEach-Object { $$_.FullName }")
else
SRC_FILES := $(shell find $(SRC_DIRS) -type f \( -name "*.cpp" -o -name "*.hpp" \))
endif
CPP_FILES     := $(filter %.cpp,$(SRC_FILES))
INCLUDE_FLAGS := $(addprefix -I ,$(SRC_DIRS))

# =========================================================================
# CMake
# =========================================================================

CMAKE_LISTS     := $(wildcard CMakeLists.txt vendor/CMakeLists.txt)
VCPKG_TOOLCHAIN := $(CURDIR)/vcpkg/scripts/buildsystems/vcpkg.cmake
CMAKE_FLAGS     = -DCMAKE_BUILD_TYPE:STRING=$(BUILD_TYPE) \
                  -DCMAKE_TOOLCHAIN_FILE=$(VCPKG_TOOLCHAIN) \
                  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
                  -DRS_BUILD_EXAMPLES=ON

# Sanitise generator name for use in file paths (replace spaces with underscores)
empty :=
space := $(empty) $(empty)
CONFIGURE_STAMP := $(BUILD_DIR)/.configured_$(BUILD_TYPE)_$(subst $(space),_,$(CMAKE_GEN))

# =========================================================================
# Platform
# =========================================================================

ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    PYTHON      := python -m
    EXE_EXT     := .exe
    FIXPATH      = $(subst /,\,$1)
    _find_tool   = $(shell pwsh -NoProfile -Command "(Get-Command $(1) -ErrorAction SilentlyContinue).Source")
else
    DETECTED_OS := $(shell uname -s)
    PYTHON      := python3 -m
    EXE_EXT     :=
    FIXPATH      = $1
    _find_tool   = $(shell command -v $(1) 2>/dev/null)
endif

ifeq ($(DETECTED_OS),Windows)
	ENV_SCRIPT   = scripts/setup_env.ps1
	# Activate backend venv and run command in one line (PowerShell)
	run_in_venv = pwsh -NoProfile -Command ".\.venv\Scripts\Activate.ps1; $(1)"
else
	ENV_SCRIPT   = scripts/setup_env.sh
	# Activate backend venv and run command in one line (Bash)
	run_in_venv = .venv/bin/activate && $(1)
endif

# =========================================================================
# Core Numbers
# =========================================================================

ifeq ($(OS),Windows_NT)
	CORES := $(shell pwsh -NoProfile -Command 'Write-Output $$env:NUMBER_OF_PROCESSORS')
else
	CORES := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)
endif

# =========================================================================
# Platform-Specific Generator Selection
# =========================================================================

HAS_NINJA := $(call _find_tool,ninja)
HAS_MAKE  := $(call _find_tool,make)

ifeq ($(OS),Windows_NT)
    # Windows default: Visual Studio
    CMAKE_GEN ?= Visual Studio 18 2026
else ifneq ($(HAS_NINJA),)
    # Prefer Ninja if available (fastest, recommended)
    CMAKE_GEN ?= Ninja
else ifneq ($(HAS_MAKE),)
    # Fallback to Unix Makefiles
    CMAKE_GEN ?= Unix Makefiles
else
    $(error No supported CMake generator found. Install Ninja or Make.)
endif

# =========================================================================
# Verbose (0 = silent . 1 = colored detail)
# =========================================================================
# Automatically silenced when make is invoked with -s or -j flags.

VERBOSE ?= 1

# Optional: if the user passes -s (--silent), force VERBOSE=0
ifneq ($(findstring -s,$(MAKEFLAGS)),)
    VERBOSE := 0
else ifneq ($(filter -j%,$(MAKEFLAGS)),)
	VERBOSE := 0
endif

# =========================================================================
# Colors
# =========================================================================

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

# =========================================================================
# Display helpers
# =========================================================================
#
#   $(call _banner,SECTION)   project-info header (build / configure / tidy)
#   $(call _section,TITLE)    lightweight section divider
#   $(call _ok,MSG)           success   [OK]
#   $(call _fail,MSG)         error     [ERR]  (exits with code 1)
#   $(call _warn,MSG)         warning   [!]
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
_ok      = @printf "  $(OK)[OK]$(RST) %s\n" "$(1)"
_fail    = @printf "  $(ERR)[ERR] Error: $(1)$(RST)\n" && exit 1
_warn    = @printf "  $(WARN)[!] $(1)$(RST)\n"
_done    = @printf "$(LINE)────────────────────────────────────────────$(RST)\n"

_require = $(if $($(1)),,$(call _fail,$(2) is not installed. Install: $(3)))

# =========================================================================
# Tool detection (evaluated once)
# =========================================================================

HAS_CLANG_TIDY := $(call _find_tool,clang-tidy)
HAS_CPPCHECK   := $(call _find_tool,cppcheck)
HAS_ACT        := $(call _find_tool,act)
HAS_DOXYGEN    := $(call _find_tool,doxygen)

# =========================================================================
# Generator layout
# =========================================================================

IS_MULTICONFIG := $(or $(findstring Visual Studio,"$(CMAKE_GEN)"),$(findstring Xcode,"$(CMAKE_GEN)"))

ifeq ($(IS_MULTICONFIG),)
    EXAMPLE_PATH ?= $(BUILD_DIR)/bin
    define EXAMPLE_EXEC
$(EXAMPLE_PATH)/$(1)$(EXE_EXT)
endef
else
    EXAMPLE_PATH ?= $(BUILD_DIR)/bin/$(BUILD_TYPE)
    define EXAMPLE_EXEC
$(EXAMPLE_PATH)/$(1)$(EXE_EXT)
endef
endif

# =========================================================================
# Generator argument (optional)
# =========================================================================

GEN_ARG :=
ifneq ($(CMAKE_GEN),)
GEN_ARG := -G "$(CMAKE_GEN)"
endif

# =========================================================================
# Library path
# =========================================================================

ifeq ($(IS_MULTICONFIG),)
    LIB_PATH     ?= $(BUILD_DIR)/lib
else
    LIB_PATH     ?= $(BUILD_DIR)/lib/$(BUILD_TYPE)
endif

# =========================================================================
# Tidy (dedicated Ninja build for compile_commands.json)
# =========================================================================

TIDY_DIR   := build-tidy
TIDY_STAMP := $(TIDY_DIR)/.configured

# =========================================================================
# Documentation
# =========================================================================

DOC_BUILD      := ./docs
DOXYGEN_CONFIG ?= Doxyfile

ifeq ($(DETECTED_OS),Darwin)
    SED_I := sed -i ''
else
    SED_I := sed -i
endif

# =========================================================================
# Phony
# =========================================================================

.PHONY: all configure build rebuild release relwithdebinfo \
        examples example-bouncing example-lissajous example-mouse example-noise \
        install test \
        clean purge \
        tidy cppcheck \
        docs clean-docs \
        venv-install pre-commit pre-commit-install pre-commit-update \
        act-ci act-quality info help

# =========================================================================
# BUILD
# =========================================================================

ifeq ($(words $(SRC_FILES)),0)
all:
	$(call _fail,No source files found in: $(SRC_DIRS))
else
all: build
endif

$(CONFIGURE_STAMP): $(CMAKE_LISTS)
	$(call _banner,Configure)
	@cmake -E make_directory $(BUILD_DIR)
	@cmake -B $(BUILD_DIR) $(GEN_ARG) $(CMAKE_FLAGS)
	@cmake -E touch $@

configure:
	$(call _banner,Configure)
	@cmake -E make_directory $(BUILD_DIR)
	@cmake -B $(BUILD_DIR) $(GEN_ARG) $(CMAKE_FLAGS)
	@cmake -E touch $(CONFIGURE_STAMP)

build: $(CONFIGURE_STAMP)
	$(call _banner,Build)
	@cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) --parallel $(CORES)
	$(call _ok,Build succeeded)
	$(call _done)

rebuild: clean build

release:
	@$(MAKE) --no-print-directory build BUILD_TYPE=Release
	$(call _ok,Release build complete (optimized))

relwithdebinfo:
	@$(MAKE) --no-print-directory build BUILD_TYPE=RelWithDebInfo
	$(call _ok,RelWithDebInfo build complete (optimized + symbols))

# =========================================================================
# EXAMPLES
# =========================================================================

example-bouncing: build
	$(call _section,Example: Bouncing Balls)
	@$(call FIXPATH,$(call EXAMPLE_EXEC,BouncingBalls))
	$(call _done)

example-lissajous: build
	$(call _section,Example: Lissajous Curves)
	@$(call FIXPATH,$(call EXAMPLE_EXEC,LissajousCurves))
	$(call _done)

example-mouse: build
	$(call _section,Example: Mouse Detection 2D)
	@$(call FIXPATH,$(call EXAMPLE_EXEC,Mouse2D))
	$(call _done)

example-noise: build
	$(call _section,Example: Noise Landscape)
	@$(call FIXPATH,$(call EXAMPLE_EXEC,NoiseLandscape))
	$(call _done)

examples: build
	$(call _section,Available Examples)
	@printf "  $(GRN)make example-bouncing$(RST)   Run Bouncing Balls\n"
	@printf "  $(GRN)make example-lissajous$(RST)  Run Lissajous Curves\n"
	@printf "  $(GRN)make example-mouse$(RST)      Run Mouse Detection 2D\n"
	@printf "  $(GRN)make example-noise$(RST)      Run Noise Landscape\n"
	$(call _done)

# =========================================================================
# INSTALL & TEST
# =========================================================================

install: build
	$(call _section,Install)
	@cmake --install $(BUILD_DIR) --config $(BUILD_TYPE)
	$(call _ok,Install complete)
	$(call _done)

test: build
	$(call _section,Tests)
	@ctest --test-dir $(BUILD_DIR) --config $(BUILD_TYPE)
	$(call _done)

# =========================================================================
# CLEAN
# =========================================================================

clean:
	$(call _section,Clean)
	-@cmake --build $(BUILD_DIR) --target clean --config $(BUILD_TYPE) > /dev/null 2>&1
	@cmake -E rm -rf $(BUILD_DIR)/bin
	@cmake -E rm -f cppcheck.log
	$(call _ok,Clean done)
	$(call _done)

purge:
	$(call _section,Full Clean)
	@printf "  %-14s : %s\n" "Removing" "$(BUILD_DIR)/ $(TIDY_DIR)/"
	@cmake -E rm -rf $(BUILD_DIR) $(TIDY_DIR)
	$(call _ok,Purge done)
	$(call _done)

# =========================================================================
# DOCUMENTATION
# =========================================================================

$(DOXYGEN_CONFIG):
	$(call _section,Creating Doxyfile)
	@doxygen -g $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^PROJECT_NAME .*|PROJECT_NAME           = "$(PROJECT_NAME)"|'   $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^PROJECT_NUMBER .*|PROJECT_NUMBER         = $(PROJECT_VER)|'    $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^OUTPUT_DIRECTORY .*|OUTPUT_DIRECTORY       = $(DOC_BUILD)|'    $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^INPUT .*|INPUT      = include src README.md docs/images|'      $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^IMAGE_PATH .*|IMAGE_PATH = docs/images|'                       $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^RECURSIVE .*|RECURSIVE              = YES|'                    $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^GENERATE_LATEX .*|GENERATE_LATEX         = NO|'                $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^EXTRACT_ALL .*|EXTRACT_ALL            = YES|'                  $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^JAVADOC_AUTOBRIEF .*|JAVADOC_AUTOBRIEF      = YES|'            $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^USE_MDFILE_AS_MAINPAGE .*|USE_MDFILE_AS_MAINPAGE = README.md|' $(DOXYGEN_CONFIG)
	@$(SED_I) 's|^MARKDOWN_SUPPORT .*|MARKDOWN_SUPPORT            = YES|'        $(DOXYGEN_CONFIG)
	$(call _ok,$(DOXYGEN_CONFIG) created - edit it to customise further)
	$(call _done)

docs: $(DOXYGEN_CONFIG)
	$(call _require,HAS_DOXYGEN,doxygen,choco install doxygen.install | apt install doxygen | brew install doxygen)
	$(call _section,Documentation)
	@printf "  %-14s : %s\n" "Config" "$(DOXYGEN_CONFIG)"
	@printf "  %-14s : %s\n" "Output" "$(DOC_BUILD)/"
	@cmake -E make_directory $(DOC_BUILD)
	@doxygen $(DOXYGEN_CONFIG)
	$(call _ok,Docs generated -> $(DOC_BUILD)/html/index.html)
	$(call _done)

clean-docs:
	$(call _section,Clean Docs)
	@cmake -E rm -rf $(DOC_BUILD)
	$(call _ok,Docs cleaned)
	$(call _done)

# =========================================================================
# CODE QUALITY
# =========================================================================

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
ifneq ($(DETECTED_OS),Windows)
	@printf "%s\n" $(CPP_FILES) | xargs -P $(CORES) -I{} clang-tidy -p $(TIDY_DIR) --extra-arg=-Iinclude {}
else
	@$(foreach f,$(CPP_FILES),clang-tidy -p $(TIDY_DIR) --extra-arg=-Iinclude $(f) ;)
endif
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
	    $(INCLUDE_FLAGS) $(SRC_DIRS) \
	    $(if $(filter $(DETECTED_OS),Linux Darwin),2>&1 | tee cppcheck.log,> cppcheck.log 2>&1)
	$(call _ok,Results saved to cppcheck.log)
	$(call _done)

# =========================================================================
# PRE-COMMIT
# =========================================================================

venv-install:
	$(call _section,Setup Virtual Environment)
ifeq ($(DETECTED_OS),Windows)
	@pwsh -NoProfile -ExecutionPolicy Bypass -Command "& $(ENV_SCRIPT)"
else
	@bash $(ENV_SCRIPT)
endif
	$(call _ok,Virtual environment created and pre-commit installed)
	$(call _done)

pre-commit-install: venv-install
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

# =========================================================================
# CI / RELEASE
# =========================================================================

act-ci:
	$(call _require,HAS_ACT,act,choco install act-cli | winget install nektos.act | brew install act)
	$(call _section,Act CI)
	@act push -W .github/workflows/ci.yml
	$(call _done)

act-quality:
	$(call _require,HAS_ACT,act,choco install act-cli | winget install nektos.act | brew install act)
	$(call _section,Act Quality)
	@act pull_request -W .github/workflows/quality.yml --container-architecture linux/amd64
	$(call _done)

# =========================================================================
# INFO
# =========================================================================

info:
	$(call _section,Configuration)
	@printf "  %-14s : %s\n" "Type" "C++ Library"
	@printf "  %-14s : %d file(s)\n" "Sources" $(words $(CPP_FILES))
	@printf "  %-14s : %s\n" "Build Type" "$(BUILD_TYPE)"
	@printf "  %-14s : %s\n" "Generator" "$(CMAKE_GEN)"
	@printf "  %-14s : %s\n" "Platform" "$(DETECTED_OS)"
	@printf "  %-14s : %s\n" "Parallel Jobs" "$(CORES)"
	@printf "  %-14s : %s\n" "Library Path" "$(LIB_PATH)/"
	@printf "  %-14s : %s\n" "Examples Path" "$(EXAMPLE_PATH)/"
	@printf "\n$(LINE)───────$(RST) $(TITLE)Tools$(RST)\n"
	@printf "  %-14s : %s\n" "Tidy Dir" "$(TIDY_DIR)/"
	@printf "  %-14s : $(if $(HAS_CLANG_TIDY),$(OK)[OK] found$(RST),$(ERR)[ERR] missing$(RST))\n" "clang-tidy"
	@printf "  %-14s : $(if $(HAS_CPPCHECK),$(OK)[OK] found$(RST),$(ERR)[ERR] missing$(RST))\n" "cppcheck"
	@printf "  %-14s : $(if $(HAS_ACT),$(OK)[OK] found$(RST),$(ERR)[ERR] missing$(RST))\n" "act"
	@printf "  %-14s : $(if $(HAS_DOXYGEN),$(OK)[OK] found$(RST),$(ERR)[ERR] missing$(RST))\n" "doxygen"
	$(call _done)

help:
	@printf "\n$(LINE)───────$(RST) $(TITLE)$(PROJECT_NAME) library$(RST)\n"
	@printf "    $(GRN)make configure$(RST)          Force CMake reconfiguration\n"
	@printf "    $(GRN)make build$(RST)              Build (auto-configures if needed)\n"
	@printf "    $(GRN)make rebuild$(RST)            Clean + build\n"
	@printf "\n"
	@printf "  $(BLD)Build Types:$(RST)\n"
	@printf "    $(GRN)make release$(RST)            Release build\n"
	@printf "    $(GRN)make relwithdebinfo$(RST)     Release + debug symbols\n"
	@printf "\n"
	@printf "  $(BLD)Examples:$(RST)\n"
	@printf "    $(GRN)make examples$(RST)            Build all & list available demos\n"
	@printf "    $(GRN)make example-bouncing$(RST)   Run Bouncing Balls\n"
	@printf "    $(GRN)make example-lissajous$(RST)  Run Lissajous Curves\n"
	@printf "    $(GRN)make example-mouse$(RST)      Run Mouse Detection 2D\n"
	@printf "    $(GRN)make example-noise$(RST)      Run Noise Landscape\n"
	@printf "\n"
	@printf "  $(BLD)Install & Test:$(RST)\n"
	@printf "    $(GRN)make install$(RST)            Install project\n"
	@printf "    $(GRN)make test$(RST)               Run tests\n"
	@printf "\n"
	@printf "  $(BLD)Clean:$(RST)\n"
	@printf "    $(GRN)make clean$(RST)              Remove build artifacts\n"
	@printf "    $(GRN)make purge$(RST)              Delete all build directories\n"
	@printf "\n"
	@printf "  $(BLD)Code Quality:$(RST)\n"
	@printf "    $(GRN)make tidy$(RST)               clang-tidy analysis\n"
	@printf "    $(GRN)make cppcheck$(RST)           cppcheck analysis\n"
	@printf "\n"
	@printf "  $(BLD)Documentation:$(RST)\n"
	@printf "    $(GRN)make docs$(RST)               Generate HTML docs (creates Doxyfile if absent)\n"
	@printf "    $(GRN)make clean-docs$(RST)         Remove generated docs\n"
	@printf "\n"
	@printf "  $(BLD)Pre-commit:$(RST)\n"
	@printf "    $(GRN)make venv-install$(RST)       Create virtual environment and install dependencies\n"
	@printf "    $(GRN)make pre-commit-install$(RST) Install git hooks\n"
	@printf "    $(GRN)make pre-commit$(RST)         Run on all files\n"
	@printf "    $(GRN)make pre-commit-update$(RST)  Update hook versions\n"
	@printf "\n"
	@printf "  $(BLD)CI / Release:$(RST)\n"
	@printf "    $(GRN)make act-ci$(RST)             Run CI workflow locally\n"
	@printf "    $(GRN)make act-quality$(RST)        Run quality workflow locally\n"
	@printf "\n"
	@printf "  $(BLD)Utility:$(RST)\n"
	@printf "    $(GRN)make info$(RST)               Project configuration & tools\n"
	@printf "    $(GRN)make help$(RST)               This help\n"
	@printf "\n"
	@printf "  $(BLD)CLI Examples:$(RST)\n"
	@printf "    make BUILD_TYPE=Release             Release build\n"
	@printf "    make example-bouncing               Test example\n"
	@printf "    make tidy                           Static analysis\n"
	@printf "    make purge build                    Fresh build from scratch\n"
	@printf "    make VERBOSE=0 build                Silent build\n"
	$(call _done)
