CC = g++
CFLAGS = -Wall -g -std=c++11 -fopenmp #comentar y descomentar para comparar los tiempos y desactivar el OpenMP
LDFLAGS = -lm

all: programa_imagen

programa_imagen: main.o ImageProcessor.o BuddySystem.o
	$(CC) -o programa_imagen main.o ImageProcessor.o BuddySystem.o $(LDFLAGS) -fopenmp

main.o: main.cpp ImageProcessor.h BuddySystem.h
	$(CC) $(CFLAGS) -c main.cpp

ImageProcessor.o: ImageProcessor.cpp ImageProcessor.h BuddySystem.h stb_image.h stb_image_write.h
	$(CC) $(CFLAGS) -c ImageProcessor.cpp

BuddySystem.o: BuddySystem.cpp BuddySystem.h
	$(CC) $(CFLAGS) -c BuddySystem.cpp

clean:
	rm -f *.o programa_imagen