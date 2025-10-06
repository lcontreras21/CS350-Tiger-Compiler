# https://makefiletutorial.com/#makefile-cookbook

### Directories and Filenames
BUILD_DIR := ./Debug
TARGET_EXEC := ${BUILD_DIR}/tiger 
SRC_DIRS := ./src

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

### Compiler and Flags
# Program for compiling C++ programs
CXX := g++ 
# Extra flags to give to the C++ compiler
CXXFLAGS := $(INC_FLAGS) -MMD -MP -std=c++1y -g -Wall -Wno-sign-compare -Wno-unused-function -Wno-unused-variable -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# Extra flags to give to compilers when they are supposed to invoke the linker
# LDFLAGS := -L/home/courses/lib
# Extra libraries to give to compilers when they are supposed to invoke the linker
# LDLIBS := -lcourses

### Flex/Bison Sources
BISON := bison
LEX := flex
LEX_SRC := tiger-lex.ll
BISON_SRC := tiger-grammar.yy
LEX_GEN := lex.yy.cc
BISON_GEN := tiger-grammar.tab.cc

### Sources and Objects 
SRC := $(filter-out lex.yy.cc tiger-grammar.tab.cc, $(wildcard *.cc))
OBJ := $(patsubst %.cc,$(BUILD_DIR)/%.o,$(SRC))

# ------ End definitions ------

.PHONY: all clean distclean

# Default target
all: $(TARGET_EXEC)

# Final binary
$(TARGET_EXEC): $(LEX_GEN) $(BISON_GEN) $(OBJ) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LEX_GEN) $(BISON_GEN) $(LDFLAGS) $(LDLIBS) -o $@

# Lexer
$(LEX_GEN): $(LEX_SRC)
	$(LEX) $(LEX_SRC)
	mv lex.yy.c $(LEX_GEN)

# Parser
$(BISON_GEN): $(BISON_SRC)
	$(BISON) -v $<

# Compile .cc to .o
$(BUILD_DIR)/%.o: %.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure output directory exists
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Clean up
clean:
	rm -f $(LEX_GEN) lex.yy.c tiger-grammar.tab.* *~ 2>/dev/null || true

distclean: clean
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d $(TARGET_EXEC) 2>/dev/null || true


# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(OBJ:.o=.d)
