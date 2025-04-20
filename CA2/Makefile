CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -fsanitize=address -fno-omit-frame-pointer -I.
# CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -I.

TARGET = bin/run
MAIN = ExtractAndTransform/ExtractAndTransform.cpp

SRCS := $(shell find . -name "*.cpp")
OBJS := $(SRCS:.cpp=.o)

# رنگ‌ها برای چاپ زیباتر
COLOR_GREEN = \033[0;32m
COLOR_YELLOW = \033[1;33m
COLOR_RED = \033[0;31m
COLOR_RESET = \033[0m

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking all object files...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	@printf "$(COLOR_GREEN)✅ Build successful! Executable: $(TARGET)$(COLOR_RESET)\n"

%.o: %.cpp
	@printf "$(COLOR_YELLOW)📦 Compiling %s ...$(COLOR_RESET)\n" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ || (printf "$(COLOR_RED)❌ Compilation failed for %s$(COLOR_RESET)\n" "$<" && exit 1)

run: all
	@printf "$(COLOR_GREEN)🚀 Running $(TARGET)...$(COLOR_RESET)\n"
	./$(TARGET)

clean:
	@printf "$(COLOR_YELLOW)🧹 Cleaning up...$(COLOR_RESET)\n"
	@rm -f $(OBJS)
	@rm -rf bin
