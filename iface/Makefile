all: gpu_lib

NVCC 		= nvcc -m64
CC			= gcc


UNAME 		= $(shell uname)
OPTFLAGS    = 
INCFLAGS    = -I/usr/local/pgsql/include/server -I/usr/local/include
CFLAGS      = $(OPTFLAGS) $(INCFLAGS) 
LIBS        = -L/usr/local/lib -llwgeom

ifeq ($(UNAME), Darwin)
CUDALIBS    = -L/usr/local/cuda/lib -lcudart
LDFLAGS		= -shared -flat_namespace -undefined suppress
else 
CUDALIBS    = -L/usr/local/cuda/lib64 -lcudart
endif
HFILES		= gpu-iface.h 

util.o: util.c
	$(CC) $(CFLAGS) -c $*.c
	
gpu-info.o: gpu-info.cu $(HFILES)
	$(NVCC) -c $*.cu 

gpu-poly.o: gpu-poly.cu $(HFILES)
	$(NVCC) -c $*.cu

contains.o: contains.cu
	$(NVCC) -c $*.cu
	
poly.o: poly.c $(HFILES)
	$(CC) $(CFLAGS) -c $*.c
	
gpu-api.o: gpu-api.c $(HFILES)
	$(CC) $(CFLAGS) -c $*.c

gpu_lib: gpu-api.o poly.o gpu-info.o gpu-poly.o util.o
	$(CC) $(LDFLAGS) -o gpu_spatial.so gpu-api.o poly.o gpu-info.o util.o gpu-poly.o $(CUDALIBS) $(LIBS)
	
clean:
	rm -f *.o gpu_spatial.so
	
