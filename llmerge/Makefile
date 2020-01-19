CXX = clang++
CXXFLAGS = -std=c++14

all: llmerge

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *.o*
