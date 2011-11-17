OBJ = vmaxs.o utils.o list.o bipartite.o
LIB = libmcs.a
EXTRACFLAGS = -O3  -fPIC  -Wall -Wextra -Werror -Wshadow -Wwrite-strings -pedantic
EXTRACXXFLAGS = -O3 
AR = ar
CC = g++
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
	rm $^

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
