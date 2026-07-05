CXX     = g++
AR      = ar

# Default version — override with: make VERSION=20
VERSION ?= 23

# Compiler flag for the chosen version
CXXSTD   = -std=c++$(VERSION)
CXXFLAGS = $(CXXSTD) -Wall -Werror -Wpedantic

# -------------------------------------------------------
# Directories
# -------------------------------------------------------

# Where the source files live e.g. c++23/
SRC_DIR = c++$(VERSION)

# Where .o files go during compilation
BUILD_DIR = build/c++$(VERSION)

# Where the finished library and headers go
DIST_DIR   = dist/c++$(VERSION)
LIB_DIR    = $(DIST_DIR)/lib
HEADER_DIR = $(DIST_DIR)/include/mok

# -------------------------------------------------------
# Files
# -------------------------------------------------------

# Recursive wildcard — finds files in all subdirs
# Usage: $(call rwildcard, directory, pattern)
rwildcard = $(foreach d, $(wildcard $(1:=/*)), $(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Find all .cpp and .hpp files recursively under c++23/
SRCS = $(call rwildcard, $(SRC_DIR), *.cpp)
HDRS = $(call rwildcard, $(SRC_DIR), *.hpp)

# Turn each .cpp path into a .o path under build/
# e.g. c++23/network/socket.cpp -> build/c++23/network/socket.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# The finished static library
LIB = $(LIB_DIR)/moklib.a

# -------------------------------------------------------
# Targets
# -------------------------------------------------------

.PHONY: all clean copy-headers versions

# Default — build the library then copy headers
all: $(LIB) copy-headers
	@echo ""
	@echo "Done."
	@echo "  Library: $(LIB)"
	@echo "  Headers: $(HEADER_DIR)"
	@echo ""
	@echo "Consumer usage:"
	@echo "  Compile flag: -I$(DIST_DIR)/include"
	@echo "  Include:      #include <mok/network.hpp>"

# Archive all .o files into moklib.a
$(LIB): $(OBJS)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Compile each .cpp into a .o
# Preserves subdir structure under build/
# $< = the .cpp file
# $@ = the .o file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

# Copy headers into dist/c++VERSION/include/mok/
# Preserves subdir structure so #include <mok/network/socket.hpp> works
copy-headers:
	@mkdir -p $(HEADER_DIR)
	@$(foreach hdr, $(HDRS), \
		mkdir -p $(HEADER_DIR)/$(dir $(patsubst $(SRC_DIR)/%,%,$(hdr))); \
		cp $(hdr) $(HEADER_DIR)/$(patsubst $(SRC_DIR)/%,%,$(hdr)); \
	)
	@echo "Installed headers into $(HEADER_DIR)"

# Remove all generated files
clean:
	rm -rf build/ dist/

# Print usage
versions:
	@echo "Usage:   make VERSION=23  (default)"
	