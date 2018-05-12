########################################
# Makefile
#######################################

CFLAGS=-g -Wall

# Les programmes

all : maint pdr server client

# Edition de liens

maint : maint.o resource.o
	gcc $(CFLAGS) -o maint maint.o resource.o

pdr : pdr.o operation.o resource.o
	gcc $(CFLAGS) -o pdr pdr.o operation.o resource.o

server : server.o resource.o operation.o 
	gcc $(CFLAGS) -o server server.o resource.o operation.o 

client : client.o operation.o resource.o
	gcc $(CFLAGS) -o client client.o operation.o resource.o

# Compilation

maint.o : maint.c resource.h
	gcc $(CFLAGS) -c maint.c

pdr.o : pdr.c operation.h 
	gcc $(CFLAGS) -c pdr.c

server.o : server.c resource.h config.h
	gcc $(CFLAGS) -c server.c

client.o : client.c operation.h config.h
	gcc $(CFLAGS) -c client.c

resource.o : resource.c resource.h config.h
	gcc $(CFLAGS) -c resource.c

operation.o : operation.c operation.h resource.h
	gcc $(CFLAGS) -c operation.c


# Nettoyage

clean :
	rm *.o
	rm maint
	rm pdr
	rm server
	rm client
