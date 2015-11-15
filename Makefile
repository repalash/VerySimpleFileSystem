CC = gcc
CFLAGS = 
OBJ = 14072_basic.o 14072_fsapi.o 14072_diagnostic.o 14072_main.o
DEP = 14072_basic.h 14072_fsapi.h 14072_diagnostic.h

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

run: main
	./main

%.o: %.c $(DEP)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm main *.o test_disk

cleandisk:
	rm test_disk