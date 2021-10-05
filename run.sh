
./dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "copy loader.exe tr" \
    -c "cd tr" \
    -c "loader"


