rm -fr build

mkdir build
cp -r "${PWD}/tr"   "build/tr"
ln -s "${PWD}/tc"   "build/tc"
ln -s "${PWD}/tasm" "build/tasm"
for i in ../src/*; do ln -s "${PWD}/$i" build/; done

SDL_VIDEODRIVER=dummy dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "tcc arca.c loader.asm >> build.txt" \
    -c "exit"

cat build/build.txt

    #-c "tcc mana.c loader.asm  > build.txt" \
