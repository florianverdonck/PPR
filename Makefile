########################################
# Makefile
#######################################

CFLAGS=-g

# Les programmes

all : maint pdr server client

# Edition de liens

maint : maint.o resource.o
	gcc $(CFLAGS) -o maint maint.o resource.o

pdr : pdr.o operation.o
	gcc $(CFLAGS) -o pdr pdr.o operation.o resource.o

server : server.o socket.o operation.o
	gcc $(CLFLAGS) -o server server.o socket.o resource.o operation.o

client : client.o operation.o socket.o
	gcc $(CFLAGS) -o client client.o operation.o resource.o

# Compilation

maint.o : maint.c resource.h
	gcc $(CFLAGS) -c maint.c resource.c

pdr.o : pdr.c operation.h 
	gcc $(CFLAGS) -c pdr.c operation.c

server.o : server.c socket.h config.h
	gcc $(CFLAGS) -c server.c

client.o : client.c operation.h socket.h config.h
	gcc $(CFLAGS) -c client.c

resource.o : resource.c resource.h
	gcc $(CFLAGS) -c resource.c

operation.o : operation.c operation.h resource.h
	gcc $(CFLAGS) -c operation.c resource.c

socket.o : socket.c socket.h resource.h config.h
	gcc $(CFLAGS) -c socket.c


# Nettoyage

clean :
	rm *.o
	rm maint
	rm pdr
	rm server
	rm client