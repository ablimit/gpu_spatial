all:	slidepack

CC			= g++

UNAME 		= $(shell uname)
OPTFLAGS    = 
INCFLAGS    = -I/usr/local/pgsql/include/
CFLAGS      = $(OPTFLAGS) $(INCFLAGS) 
LIBS        = -L/usr/local/pgsql/lib/ -lpq;

ifeq ($(UNAME), Darwin)
else 
endif

HFILES		= 

	
cmdline.o : options.ggo
	gengetopt -i options.ggo
	$(CC) $(CFLAGS) -c cmdline.c
		
pack.o : pack.cpp options.ggo
	$(CC) $(CFLAGS) -c pack.cpp
	
# cmdline.o needs to be before anything that requires cmdline.h in the
# dependency list
#
slidepack: cmdline.o pack.o 
	$(CC) $(LDFLAGS) -o slidepack pack.o cmdline.o $(LIBS)


clean:
	rm -f *.o slidepack cmdline.*
	
