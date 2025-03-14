# کامپایلر و فلگ‌ها
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# نام فایل‌های اجرایی
TARGETS = server client

# نام کتابخانه
LIB = libmyprint.a

# فایل‌های منبع کتابخانه
LIB_SRC = myprint.cpp
LIB_OBJ = myprint.o

# هدف اصلی
all: $(LIB) $(TARGETS)

# ساخت کتابخانه استاتیک
$(LIB): $(LIB_OBJ)
	ar rcs $(LIB) $(LIB_OBJ)

# کامپایل کد کتابخانه
$(LIB_OBJ): $(LIB_SRC) myprint.h
	$(CXX) $(CXXFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

# کامپایل سرور با لینک به کتابخانه
server: server.cpp $(LIB)
	$(CXX) $(CXXFLAGS) -o server server.cpp -L. -lmyprint

# کامپایل کلاینت با لینک به کتابخانه
client: client.cpp $(LIB)
	$(CXX) $(CXXFLAGS) -o client client.cpp -L. -lmyprint

# اجرای کلاینت و سرور در tmux
run: all
	@tmux has-session -t mysession 2>/dev/null && tmux kill-session -t mysession || true
	@tmux new-session -d -s mysession 'bash -c "./server 8080"'
	@tmux split-window -h -t mysession
	@tmux send-keys -t mysession:0.1 'bash -c "./client ali 8080 code"' C-m
	@tmux attach-session -t mysession

# تمیز کردن فایل‌های موقت
clean:
	rm -f $(TARGETS) $(LIB) $(LIB_OBJ)
