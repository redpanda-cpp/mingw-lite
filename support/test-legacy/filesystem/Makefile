CXXFLAGS = -Os -fno-lto

.PHONY: all test clean clean-data

all: main.exe

main.exe: main.cc
	g++ $(CXXFLAGS) $^ -o $@

test: main.exe clean-data
	.\main.exe >output.txt
	..\_meta\diff.exe output.txt expected.txt

clean: clean-data
	-del *.o
	-del *.exe

clean-data:
	-del output.txt
	-del test\*.txt
	-rmdir test
	-del ≤‚ ‘\*.txt
	-rmdir ≤‚ ‘
