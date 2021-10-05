
./dosbox \
    -c "mount c build" \
    -c "c:"\
    -c "set path=c:/tc;c:/tasm" \
    -c "copy tr\\cods\\arcade.exe tr\\cods\\arca.exe" \
    -c "copy arca.exe tr\\cods\\arcade.exe" \
    #-c "cd tr" \
    #-c "manager"


