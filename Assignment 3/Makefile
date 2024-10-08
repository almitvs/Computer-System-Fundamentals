# Makefile for A3
# Bob Dong bdong9@jh.edu
# Aidan Alme aalme2@jhu.edu

CC = g++
CONSERVATIVE_FLAGS = -std=c++11 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)
TARGET = csim
ZIP_FILE = assign3.zip
SRC_FILES = csim.cpp cache.cpp cache.h Makefile README.txt

all: $(TARGET)

$(TARGET): csim.o cache.o
	$(CC) $(CFLAGS) -o $(TARGET) csim.o cache.o

csim.o: csim.cpp cache.h
	$(CC) $(CFLAGES) -c csim.cpp

cache.o: cache.cpp cache.h
	$(CC) $(CFLAGES) -c cache.cpp

$(ZIP_FILE) : $(SRC_FILES)
	zip $(ZIP_FILE) $(SRC_FILES)

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
