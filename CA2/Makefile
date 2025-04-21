CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -fsanitize=address -fno-omit-frame-pointer -I.

# Binaries
TARGET = bin/run
LOADER_BIN = bin/loader
PROCESS_BIN = bin/processor
OUTPUT_BIN = bin/output

# Main files
RUN_MAIN := ./ExtractAndTransform/ExtractAndTransform.cpp
LOADER_MAIN := ./Loader/LoaderMain.cpp
PROCESSOR_MAIN := ./ProcessingNodes/ProcessingMain.cpp
OUTPUT_MAIN := ./Output/OutputMain.cpp

# All sources
ALL_SRCS := $(shell find . -name "*.cpp")

# Object files per binary
RUN_OBJ := $(RUN_MAIN:.cpp=.o)
LOADER_OBJ := $(LOADER_MAIN:.cpp=.o)
PROCESSOR_OBJ := $(PROCESSOR_MAIN:.cpp=.o)
OUTPUT_OBJ := $(OUTPUT_MAIN:.cpp=.o)

# Specific .cpp files to exclude per binary
COMMON_SRCS := $(filter-out $(RUN_MAIN) $(LOADER_MAIN) $(PROCESSOR_MAIN) $(OUTPUT_MAIN), $(ALL_SRCS))
COMMON_OBJS := $(COMMON_SRCS:.cpp=.o)

# Remove Output/Output.o from COMMON_OBJS for output binary to avoid redefinition
OUTPUT_OBJS := $(filter-out ./Output/Output.o, $(COMMON_OBJS)) Output/Output.o

# Colors
COLOR_GREEN = \033[0;32m
COLOR_YELLOW = \033[1;33m
COLOR_RED = \033[0;31m
COLOR_RESET = \033[0m

# Build targets
all: $(TARGET) $(LOADER_BIN) $(PROCESS_BIN) $(OUTPUT_BIN)

$(TARGET): $(COMMON_OBJS) $(RUN_OBJ)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking run...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $(TARGET)$(COLOR_RESET)\n"

$(LOADER_BIN): $(COMMON_OBJS) $(LOADER_OBJ)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking loader...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $(LOADER_BIN)$(COLOR_RESET)\n"

$(PROCESS_BIN): $(COMMON_OBJS) $(PROCESSOR_OBJ)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking processor...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $(PROCESS_BIN)$(COLOR_RESET)\n"

$(OUTPUT_BIN): $(OUTPUT_OBJS) $(OUTPUT_OBJ)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking output...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $(OUTPUT_BIN)$(COLOR_RESET)\n"

%.o: %.cpp
	@printf "$(COLOR_YELLOW)📦 Compiling %s ...$(COLOR_RESET)\n" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ || (printf "$(COLOR_RED)❌ Failed: %s$(COLOR_RESET)\n" "$<" && exit 1)

# Run
run: all
	@printf "$(COLOR_GREEN)🚀 Running run...$(COLOR_RESET)\n"
	./$(TARGET)

run-loader: all
	@printf "$(COLOR_GREEN)🚀 Running loader...$(COLOR_RESET)\n"
	./$(LOADER_BIN)

run-processor: all
	@printf "$(COLOR_GREEN)🚀 Running processor...$(COLOR_RESET)\n"
	./$(PROCESS_BIN) 4

run-output: all
	@printf "$(COLOR_GREEN)🚀 Running output...$(COLOR_RESET)\n"
	./$(OUTPUT_BIN)

# Clean
clean:
	@printf "$(COLOR_YELLOW)🪹 Cleaning...$(COLOR_RESET)\n"
	@rm -f $(COMMON_OBJS) $(RUN_OBJ) $(LOADER_OBJ) $(PROCESSOR_OBJ) $(OUTPUT_OBJ)
	@rm -rf bin
