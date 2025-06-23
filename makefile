CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -I/usr/include/SDL2
LDFLAGS = -lSDL2 -lSDL2_ttf

TARGET = build/chip8
SOURCES = main.cpp chip8.cpp chip8gfx.cpp logger.cpp
OBJECTS = $(addprefix build/,$(SOURCES:.cpp=.o))

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean build
