CC=gcc
CFLAGS=-I. -L ./libs/wiringPi -l wiringPi
DEPS = machine.h longshot.h
OBJ = longshot.o machine.o

all: longshot

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

longshot: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
