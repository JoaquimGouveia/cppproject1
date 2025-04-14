# Compiler setup
CXX = g++
CC  = $(CXX)

DEPFLAGS  = -MT $@ -MMD -MP -MF $*.d

CPPFLAGS  = -I../include
CXXFLAGS  = -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++17 -g $(DEPFLAGS)
LDFLAGS   = -g -L../lib
LDLIBS    = -lclientserver

# Executables
PROGS = myclient myserver

# Source files for each program
MYCLIENT_SRCS = myclient.cc
MYSERVER_SRCS = myserver.cc ../src/article.cc ../src/newsgroup.cc ../src/database_memory.cc

# Object files
MYCLIENT_OBJS = $(MYCLIENT_SRCS:.cc=.o)
MYSERVER_OBJS = $(MYSERVER_SRCS:.cc=.o)

# Default target
all: $(PROGS)

# Build rules
myclient: $(MYCLIENT_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

myserver: $(MYSERVER_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compilation rule (with dep generation)
%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Phony targets
.PHONY: all clean distclean

clean:
	-rm -f *.o ../src/*.o $(PROGS)

distclean: clean
	-rm -f *.d ../src/*.d
	-rm -r $(addsuffix .dSYM,$(PROGS)) # for Mac users

# Include dependencies
-include *.d
-include ../src/*.d
