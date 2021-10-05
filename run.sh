
./dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "copy mana.exe tr" \
    -c "cd tr" \
    -c "mana"


