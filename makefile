CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -I/usr/include/SDL2
CXXFLAGS_RELEASE = -O3 -Wall -Wextra -std=c++11 -I/usr/include/SDL2 -DNDEBUG
LDFLAGS = -lSDL2 -lSDL2_ttf

TARGET = build/chip8
SOURCES = main.cpp chip8.cpp chip8gfx.cpp logger.cpp chip8audio.cpp
OBJECTS = $(addprefix build/,$(SOURCES:.cpp=.o))
OBJECTS_RELEASE = $(addprefix build/release/,$(SOURCES:.cpp=.o))

all: build $(TARGET)

release: build/release $(TARGET)-release

build:
	mkdir -p build

build/release:
	mkdir -p build/release

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

$(TARGET)-release: $(OBJECTS_RELEASE)
	$(CXX) $(CXXFLAGS_RELEASE) -o $(TARGET)-release $(OBJECTS_RELEASE) $(LDFLAGS)

build/%.o: %.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/release/%.o: %.cpp | build/release
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

build/chip8audio.o: chip8audio.cpp chip8audio.h
	$(CXX) $(CXXFLAGS) -c chip8audio.cpp -o build/chip8audio.o

clean:
	rm -rf build

.PHONY: all clean build release
