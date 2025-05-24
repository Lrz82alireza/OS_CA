# make run ARGS="64 8"

# === Compiler and Flags ===
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -pthread

# === Colors ===
COLOR_GREEN := \033[0;32m
COLOR_YELLOW := \033[1;33m
COLOR_RESET := \033[0m

# === Source Directories ===
SRC_DIRS := \
    thread_pool \
    hidden_layer_job \
    output_layer_job \
    neural_net \
    reader_thread \
    shared_queue \
    data_loader \
    barrier


INCLUDE_DIRS := $(foreach dir,$(SRC_DIRS),-I$(dir))
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -pthread $(INCLUDE_DIRS)

SRCS := main.cpp $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

# === Output
BIN_DIR := bin
TARGET := $(BIN_DIR)/mnist_nn

# === Rules
.PHONY: all run clean

all: $(TARGET)

# === Linking
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@printf "$(COLOR_YELLOW)🔧 Linking...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $@$(COLOR_RESET)\n"

# === Compiling
%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "$(COLOR_YELLOW)⏳ Compiling $<...$(COLOR_RESET)\n"
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# === Include Dependencies
-include $(DEPS)

# === Run Project
run: all
	@printf "$(COLOR_YELLOW)🚀 Running mnist_nn...$(COLOR_RESET)\n"
	@./$(TARGET) $(ARGS)

# === Clean
clean:
	@printf "$(COLOR_YELLOW)🧹 Cleaning up...$(COLOR_RESET)\n"
	rm -rf $(OBJS) $(DEPS) $(TARGET)
	@printf "$(COLOR_GREEN)✅ Clean complete.$(COLOR_RESET)\n"
