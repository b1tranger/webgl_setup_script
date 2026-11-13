# ==============================================================================
# WebGL 2.0 / WebAssembly Project Makefile (Emscripten)
# ==============================================================================

CXX = em++
CXXFLAGS = -std=c++17 -O2 \
           -s USE_GLFW=3 \
           -s FULL_ES3=1 \
           -s USE_WEBGL2=1 \
           -s WASM=1 \
           -s ALLOW_MEMORY_GROWTH=1 \
           -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
           --shell-file shell_minimal.html

INCLUDES = -Iinclude
SRCS = src/main.cpp
OUT_DIR = dist
TARGET = $(OUT_DIR)/index.html

.PHONY: all clean run serve

all: $(TARGET)

$(TARGET): $(SRCS)
	@if not exist $(OUT_DIR) mkdir $(OUT_DIR)
	$(CXX) $(SRCS) $(INCLUDES) $(CXXFLAGS) -o $(TARGET)
	@echo [SUCCESS] Web build generated at $(TARGET)

clean:
	@if exist $(OUT_DIR) rmdir /s /q $(OUT_DIR)
	@echo [CLEAN] Cleaned $(OUT_DIR) directory.

run: all serve

serve:
	@echo Starting local HTTP server on http://localhost:8080 ...
	@python -m http.server 8080 --directory $(OUT_DIR)

