SRC=src/lodepng.cpp src/image.cpp src/graphics.cpp src/main.cpp
CFLAGS=-std=c++14
LFLAGS=-lGLEW -lGL -lglfw
EXECUTABLE=test

all:
	g++ $(SRC) $(CFLAGS) $(LFLAGS) -o $(EXECUTABLE)
