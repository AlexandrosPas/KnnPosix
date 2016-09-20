CC=gcc -O4


all: knnTest.o knns.o

	$(CC) knnTest.o knns.o -o knnTest -lpthread

knnTest.o: knnTest.c utils.h knns.h

	$(CC) knnTest.c -c

knns.o: knns.c utils.h knns.h

	$(CC) knns.c -c

clean:
	rm -f *.o *.out *~
	rm -f *.bin
