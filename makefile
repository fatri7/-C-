CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -g -pthread
LDFLAGS := -pthread

TARGET := unified_lp_solver
SOURCES := $(wildcard *.cc)
OBJS := $(SOURCES:.cc=.o)
HEADERS := ThreadPool paraSim paraSim_threadp json.hpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)