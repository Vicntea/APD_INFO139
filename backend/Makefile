CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/SDL2 -D_REENTRANT -Iinclude

LDFLAGS = -lSDL2 -lSDL2_ttf

SRC = $(filter-out src/backend.cpp, $(wildcard src/*.cpp))

OBJ = $(patsubst %.cpp, build/%.o, $(SRC))
EXEC = bin/apd_simulator

all: $(EXEC)

$(EXEC): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build bin
