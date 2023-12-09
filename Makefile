#CROSS_COMPILE = arm-linux-gnueabi-
CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc

EXE = .exe

CFLAGS := -O3 -Wall -g -std=gnu99 

TARGET = sha256

OBJ = main.o sha256.o

all : $(OBJ)
	$(CC) -o $(TARGET)$(EXE) $(OBJ) $(CFLAGS)
    
sha256.o : sha256.c sha256.h
main.o : main.c sha256.h

.PHONY : clean

clean:
	rm -f $(OBJ)
	rm -f $(TARGET)$(EXE)
