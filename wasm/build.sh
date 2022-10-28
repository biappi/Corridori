docker run \
    -it \
    --rm \
    -v "$PWD/..":/src emscripten/emsdk \
    /usr/bin/bash -c "cd src && emcc -O3 corridori.c -o index.html -I../wasm/raylib-4.2.0_webassembly/include/ -L../wasm/raylib-3.7.0_webassembly/lib  -I../wasm/cimgui/ ../wasm/cimgui/cimgui.a -s USE_GLFW=3 --preload-file ../dos/tr@/ -lraylib -s ASYNCIFY"

