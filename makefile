CFLAGS= -Wall -Wextra -pedantic -O2 -Wno-long-long
OBJS = cartas.o
tudo: cartas
	sudo cp cartas /usr/lib/cgi-bin
	touch tudo

cartas: $(OBJS)
	gcc -o cartas $(OBJS)