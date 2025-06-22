CXX = g++
CXXFLAGS = -g -Wall -Wextra -std=c++11 -I/usr/include/SDL2
LDFLAGS = -lSDL2 -lSDL2_ttf

TARGET = chip8
SOURCES = main.cpp chip8.cpp logger.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: all clean
