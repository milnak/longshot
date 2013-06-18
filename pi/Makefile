CC=gcc
CFLAGS=-I. -L ./libs/wiringPi -l wiringPi -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -L/usr/lib/arm-linux-gnueabihf -lSDL
DEPS = machine.h longshot.h
OBJ = longshot.o machine.o main.o

all: longshot

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

longshot: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o *.dbm longshot
