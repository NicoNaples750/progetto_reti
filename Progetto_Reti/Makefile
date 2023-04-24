CC=gcc
#CFLAGS=$(ARGS) -O3 -Iinclude -Wno-unused-result -pthread -std=c11
CFLAGS=$(ARGS) -O3 -Iinclude -Wno-unused-result -pthread -std=c11 -DNDEBUG
SRCDIR=src
INCLUDEDIR=include
OBJDIR=obj
BINDIR=bin

SOURCES=$(SRCDIR)/foo.c
INCLUDES=$(INCLUDEDIR)/defines.h $(INCLUDEDIR)/green_pass.h $(INCLUDEDIR)/utils.h

all: common server_v server_g client_vax centro_vaccinale client_s client_t client_utility

server_v:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/server_v.c -o $(BINDIR)/server_v

server_g:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/server_g.c -o $(BINDIR)/server_g

centro_vaccinale:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/centro_vaccinale.c -o $(BINDIR)/centro_vaccinale

client_vax:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/client_vax.c -o $(BINDIR)/client_vax

client_s:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/client_s.c -o $(BINDIR)/client_s

client_t:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/client_t.c -o $(BINDIR)/client_t

client_utility:
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCDIR)/client_utility.c -o $(BINDIR)/client_utility

common:
	mkdir -p bin/ &> /dev/null

# Cleaning object files and binary output
clean:
	rm -f *.o $(BINDIR)/*  $(INCLUDEDIR)/*.gch