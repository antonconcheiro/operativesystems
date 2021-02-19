CFLAGS= -g -Wall

shell: list.o shell.o
	gcc ${CFLAGS} -o shell list.o shell.o

list.o: list.h list.c
	gcc ${CFLAGS} -c list.c

shell.o: list.h shell.c
	gcc ${CFLAGS} -c shell.c

clean:
	rm shell *.o

