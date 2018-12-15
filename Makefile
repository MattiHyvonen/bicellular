SRC=src/lodepng.cpp src/main.cpp
CFLAGS=-std=c++14
LFLAGS=-lGLEW -lGL -lglfw
EXECUTABLE=test

all:
	g++ $(SRC) $(CFLAGS) $(LFLAGS) -o $(EXECUTABLE)
