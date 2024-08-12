CFLAGS = -Os -fno-lto

.PHONY: all clean

all: diff.exe

diff.exe: diff.c
	gcc $(CFLAGS) $^ -o $@

clean:
	-del *.exe
