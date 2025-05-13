CC       = gcc
CXX      = g++
CPPFLAGS =
CFLAGS   = -std=c99 -O0 -g3
CXXFLAGS = -std=c++11 -O0 -g3
LDFLAGS  =
LIBS     =

$(DIR)/breakpoint$(SUFFIX): $(DIR)/breakpoint.o
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(DIR)/breakpoint.o: debug/breakpoint.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	-del $(DIR)/breakpoint$(SUFFIX) $(DIR)/breakpoint.o
