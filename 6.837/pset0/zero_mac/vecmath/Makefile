CC = g++
CFLAGS = -c -Wall
PROJECT = ../lib/libvecmath.a
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

INCFLAGS  = -I ../include/vecmath

all: $(PROJECT)

# $< means filename without extension
.cpp.o:
	CC $(CFLAGS) $< $(INCFLAGS)

$(PROJECT): $(OBJ)
	libtool -static $(OBJ) -o $(PROJECT)

.PHONY : clean
clean:
	rm -rf *.o