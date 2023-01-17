#main: main.c switch.c system.c
#	gcc -o main main.c system.c switch.c -I.


all: Router

Router: Router.o
 
Router.o: Router.o
	gcc -o Router.o Router.c util.h

clean:
		find . -type f | xargs touch
		rm -rf *o ass4
