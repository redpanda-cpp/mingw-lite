CXXFLAGS = -Os -fno-lto

.PHONY: all test clean

all: main.exe

main.exe: main.cc
	g++ $(CXXFLAGS) $^ -o $@

test: main.exe
	.\main.exe

clean:
	-del *.o
	-del *.exe
	-del *.txt
