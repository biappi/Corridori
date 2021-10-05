
cp build/LOADER.EXE build/tr

dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "cd tr" \
    -c "loader"

