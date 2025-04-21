CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -fsanitize=address -fno-omit-frame-pointer -I.

TARGET = bin/run
LOADER_BIN = bin/loader

SRCS := $(filter-out ./Loader/LoaderMain.cpp, $(shell find . -name "*.cpp"))
OBJS := $(SRCS:.cpp=.o)

SHARED_OBJ = Shared.o

LOADER_SRC = Loader/LoaderMain.cpp
LOADER_OBJ = Loader/LoaderMain.o
LOADER_OBJS = Loader/Loader.o $(LOADER_OBJ)

COLOR_GREEN = \033[0;32m
COLOR_YELLOW = \033[1;33m
COLOR_RED = \033[0;31m
COLOR_RESET = \033[0m

all: $(TARGET) $(LOADER_BIN)

$(TARGET): $(OBJS)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking run...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
	@printf "$(COLOR_GREEN)✅ Built: $(TARGET)$(COLOR_RESET)\n"

$(LOADER_BIN): $(LOADER_OBJS) $(SHARED_OBJ)
	@mkdir -p bin
	@printf "$(COLOR_YELLOW)🔧 Linking loader...$(COLOR_RESET)\n"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@printf "$(COLOR_GREEN)✅ Built: $(LOADER_BIN)$(COLOR_RESET)\n"

%.o: %.cpp
	@printf "$(COLOR_YELLOW)📦 Compiling %s ...$(COLOR_RESET)\n" "$<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@ || (printf "$(COLOR_RED)❌ Failed: %s$(COLOR_RESET)\n" "$<" && exit 1)

run: all
	@printf "$(COLOR_GREEN)🚀 Running run...$(COLOR_RESET)\n"
	./$(TARGET)

run-loader: all
	@printf "$(COLOR_GREEN)🚀 Running loader...$(COLOR_RESET)\n"
	./$(LOADER_BIN)

clean:
	@printf "$(COLOR_YELLOW)🧹 Cleaning...$(COLOR_RESET)\n"
	@rm -f $(OBJS) $(LOADER_OBJ) $(LOADER_OBJS)
	@rm -rf bin
