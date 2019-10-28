EXE = minesweeper
BUILD ?= debug
BUILD_DIR ?= build

################################################################################
# Compilation flags
################################################################################
CXX ?= clang++
CXXFLAGS += -std=c++17 -pthread -fPIC

### Configuration flags
TRAP_FP_FAULTS ?= false
ifeq ($(TRAP_FP_FAULTS),true)
    CXXFLAGS += -DTRAP_FP_FAULTS
endif

### Build flags
CXXFLAGS.debug := -DDEBUG -g -O0
CXXFLAGS.release := -DNDEBUG -O2
CXXFLAGS += $(CXXFLAGS.$(BUILD))

### Warning flags
WARN_FLAGS = -Wall -Werror -Wextra -Weffc++ -Wfloat-equal -Werror-implicit-function-declaration -Wundef -Wpointer-arith -Wcast-align -Wstrict-overflow=4 -Wwrite-strings -Wno-unused-result
# Potentially noisy and not worth it
WARN_FLAGS += -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code

## Flags that could not
# CI using gcc5 which does not have this flag, clang does not have this flag
#CXXFLAGS += -Wshadow-compatible-local
# -ftrapv on 32bit arch calls __mulodi4, leading to an undefined reference
# See bug: https://bugs.llvm.org/show_bug.cgi?id=14469
#CXXFLAGS += -ftrapv
# asan not available on windows...
#CXXFLAGS += -fsanitize=address

## Warnings to ignore for now
WARN_FLAGS += -Wno-unused-parameter -Wno-float-equal -Wno-sign-conversion

CXXFLAGS += $(WARN_FLAGS)

### Arch flags
ARCH ?= x64
ifeq ($(findstring $(ARCH),x64),x64)
CXXFLAGS += -m64
else
ifeq ($(findstring $(ARCH),x86),x86)
CXXFLAGS += -m32
else
$(error Unknown arch type. Use one of x86,x64)
endif
endif

### Linker flags
LDFLAGS := $(CXXFLAGS)
# Remove -pthread from clang to avoid unused linker flag
ifeq ($(findstring clang,$(CXX)),clang)
# TODO: Works locally with clang++5. Fails TravisCI with:
#     /usr/bin/ld: build/clang++/debug/src/utility/thread_pool.o: undefined reference to symbol 'pthread_setspecific@@GLIBC_2.2.5'
# WAR by ignoring unused command line argument warning.
#LDFLAGS := $(filter-out -pthread,$(CXXFLAGS))
LDFLAGS += -Wno-unused-command-line-argument
endif
################################################################################

# Whitelist build types
ifeq ($(BUILD),$(filter $(BUILD),debug release))
# Select build directory based on compilation settings
BUILD_PATH := $(BUILD_DIR)/$(CXX)/$(BUILD)/linux/$(ARCH)
else
$(error Unknown build type. Use one of debug,release)
endif

SOURCE_DIR = .
INCLUDE_DIRS := -I$(SOURCE_DIR)
CPP_FILES := $(shell find $(SOURCE_DIR) -name '*.cpp')
OBJ_FILES = $(addprefix $(BUILD_PATH)/,$(CPP_FILES:%.cpp=%.o))
DEP_FILES = $(OBJ_FILES:%.o=%.d)
EXE_LINK := $(EXE).link.$(CXX).$(ARCH).$(BUILD)

.PHONY: all header clean clean_all

all: header $(BUILD_PATH)/$(EXE) $(EXE_LINK)

# Include .d dependencies
-include $(DEP_FILES)

$(BUILD_PATH)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo $@
	@$(CXX) -c -o $@ $< -MMD $(INCLUDE_DIRS) $(CXXFLAGS)

$(BUILD_PATH)/$(EXE): $(OBJ_FILES)
	@echo "== LINK"
	@echo $@
	@$(CXX) -o $@ $^ $(INCLUDE_DIRS) $(LDFLAGS)

$(EXE_LINK): $(BUILD_PATH)/$(EXE)
	ln -s $(BUILD_PATH)/$(EXE) $(EXE_LINK)

header:
	@echo "==== START BUILD $(CXX) $(ARCH) $(BUILD)"
	@echo "== VERSION ($(CXX)):"
	@$(CXX) -v
	@echo "== COMPILATION FLAGS:"
	@echo $(CXXFLAGS)
	@echo "== LINKER FLAGS:"
	@echo $(LDFLAGS)
	@echo "== BUILD"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(EXE).link.*
