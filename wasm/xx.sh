docker run -it --rm -v "$PWD/..":/work emscripten/emsdk  /bin/bash --login
emcc -O3 corridori.c -o index.html -I../wasm/raylib-3.7.0_webassembly/include/ -L../wasm/raylib-3.7.0_webassembly/lib  -s USE_GLFW=3 --preload-file ../dos/tr@/ -lraylib -s ASYNCIFY

