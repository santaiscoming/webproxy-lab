CC = gcc
CFLAGS = -O2 -Wall -I ..

all: adder form-adder

form-adder: form-adder.c
	$(CC) $(CFLAGS) -o form-adder form-adder.c

adder: adder.c
	$(CC) $(CFLAGS) -o adder adder.c

clean:
	rm -f adder form-adder *~ 
