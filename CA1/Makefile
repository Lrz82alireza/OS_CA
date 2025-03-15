CXX = g++
CXXFLAGS = -std=c++11 -Wall -IShared

SRC_SERVER = $(wildcard Server/*.cpp Shared/*.cpp)
SRC_CLIENT = $(wildcard Client/*.cpp Shared/*.cpp)

OBJ_SERVER = $(SRC_SERVER:.cpp=.o)
OBJ_CLIENT = $(SRC_CLIENT:.cpp=.o)

BIN_SERVER = Server/server.out
BIN_CLIENT = Client/client.out

all: $(BIN_SERVER) $(BIN_CLIENT)

$(BIN_SERVER): $(OBJ_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN_CLIENT): $(OBJ_CLIENT)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_SERVER) $(OBJ_CLIENT) $(BIN_SERVER) $(BIN_CLIENT)

# اجرای کلاینت و سرور در tmux
run: all
	@if tmux ls 2>/dev/null | grep -q "mysession"; then \
		tmux kill-session -t mysession; \
	fi
	@tmux new-session -d -s mysession
	@tmux send-keys -t mysession:0.0 'bash -c "./Server/server.out 8080 || echo Server failed"' C-m
	@sleep 5

	@tmux split-window -h -t mysession:0
	@tmux split-window -v -t mysession:0.0
	@tmux split-window -v -t mysession:0.1

	@tmux send-keys -t mysession:0.1 'bash -c "./Client/client.out Mir 8080 coder"' C-m
	@tmux send-keys -t mysession:0.2 'bash -c "./Client/client.out Hashem 8080 navigator"' C-m
	@tmux send-keys -t mysession:0.3 'bash -c "./Client/client.out Abbookh 8080 coder"' C-m

	@tmux attach-session -t mysession


