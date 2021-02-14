CC = gcc
CFLAFS = -g

prog: clean main.o util.o
	$(CC) $(CFLAFS) -o prog main.o util.o -lcrypto
main.o:
	$(CC) $(CFLAFS) -c main.c
util.o:
	$(CC) $(CFLAFS) -c util.c
.PHONY: clean
clean:
	rm -f prog main.o util.o