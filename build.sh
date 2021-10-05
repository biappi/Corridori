rm -fr build

mkdir build
ln -s "${PWD}/dosroot/tc"   "build/tc"
ln -s "${PWD}/dosroot/tr"   "build/tr"
ln -s "${PWD}/dosroot/tasm" "build/tasm"
cp -r src/* build/

SDL_VIDEODRIVER=dummy dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "tcc loader.asm main.c > build.txt" \
    -c "exit"

cat build/build.txt

