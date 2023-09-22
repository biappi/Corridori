docker run               \
    -it                  \
    --rm                 \
    -v "$PWD/..":/src    \
    emscripten/emsdk     \
    wasm/actual-build.sh

