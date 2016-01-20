CC = gcc
CFLAGS = -Wimplicit-function-declaration -std=c99
OBJ = basic.o fsapi.o diagnostic.o main.o
DEP = basic.h fsapi.h diagnostic.h

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

run: main cleandisk
	./main

%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f main *.o test_disk

cleandisk: 
	rm -f test_disk
