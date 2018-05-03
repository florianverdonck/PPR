########################################
# Makefile
#######################################

CFLAGS=-g

# Les programmes

all : maint pdr

# Edition de liens

maint : maint.o ipc.o
	gcc $(CFLAGS) -o maint maint.o ipc.o

pdr : pdr.o operation.o
	gcc $(CFLAGS) -o pdr pdr.o operation.o

# Compilation

maint.o : maint.c ipc.h
	gcc $(CFLAGS) -c maint.c

pdr.o : pdr.c operation.h 
	gcc $(CFLAGS) -c pdr.c

ipc.o : ipc.c ipc.h
	gcc $(CFLAGS) -c ipc.c

operation.o : operation.c operation.h ipc.h
	gcc $(CFLAGS) -c operation.c


# Nettoyage

clean :
	rm *.o
	rm maint
	rm pdr