CXX = em++
CXXFLAGS = -std=c++17 -O2 -s USE_GLFW=3 -s FULL_ES3=1 -s USE_WEBGL2=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 --shell-file shell_minimal.html
SRCS = src/main.cpp
OUT_DIR = dist
TARGET = $(OUT_DIR)/index.html

all: $(TARGET)

$(TARGET): $(SRCS)
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	$(CXX) $(SRCS) $(CXXFLAGS) -o $(TARGET)

clean:
	@if exist $(OUT_DIR) rmdir /s /q $(OUT_DIR)

run: all
	python -m http.server 8080 --directory $(OUT_DIR)

