CC       = gcc
CXX      = g++

override CFLAGS   += -std=c99 -O0 -g3
override CXXFLAGS += -std=c++11 -O0 -g3

$(DIR)/breakpoint$(SUFFIX): $(DIR)/breakpoint.o
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

$(DIR)/breakpoint.o: debug/breakpoint.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	-del $(DIR)/breakpoint$(SUFFIX) $(DIR)/breakpoint.o
