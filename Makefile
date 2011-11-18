OBJ = vmaxs.o utils.o list.o bipartite.o
LIB = libmcs.a libmcs.so
EXTRACFLAGS = -O3  -std=c99 -fPIC 
EXTRACXXFLAGS = -O3 
AR = ar
CC = gcc
CXX = g++
CFLAGS = -g -O2
CXXFLAGS = -g -O2

.c.o : 
	$(CC) $(CFLAGS) $(EXTRACFLAGS) -c $<

.cc.o :
	$(CXX) $(CXXFLAGS) $(EXTRACXXFLAGS) -c $<

all : ${LIB}   

libmcs.a : ${OBJ}
	ar cr $@ $^

libmcs.so : ${OBJ}
	$(CC) $(CFLAGS) $(EXTRACFLAGS) -shared -o $@ $^ -lgc

clean : 
	rm -f ${OBJ}
	rm -f ${LIB}
	(cd python; $(MAKE) clean)
	(cd R; $(MAKE) clean)

dist-clean : 
	rm -rf autom4te.cache config.log config.status
	rm -f python/setup.py tests/Makefile R/mcs/src/Makevars
	rm -f Makefile

python : 
	(cd python; $(MAKE))

R : 
	(cd R; $(MAKE))

swig:
	(cd python; $(MAKE) swig)
	(cd R; $(MAKE) swig)
