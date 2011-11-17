EXE = batch batch-p batch-timed test test-longresult test_read_sdf test-timed sdf_mcs
CFLAGS = -O3 -I/usr/include/gc -std=c99 -fPIC -I.. 
CXXFLAGS = -O3 -I/usr/include/gc  -fPIC -I.. 
LDFLAGS = -lgc -lmcs -L..  
CXX = g++
CC = gcc
LINKER = ld

.c.o : 
	$(CC) $(CFLAGS) -c $<
.cc.o :
	$(CXX) $(CXXFLAGS) -c $<
.o :
	$(CC) -o $@ $^ $(LDFLAGS)

all : ${EXE}
test : test.o

test-longresult : test-longresult.o ../libmcs.a

test-timed : test-timed.o ../libmcs.a

batch : batch.o ../libmcs.a

batch-timed : batch-timed.o ../libmcs.a

batch-p : batch-p.o client.o ../libmcs.a

test_read_sdf.o : test_read_sdf.cc
test_read_sdf : test_read_sdf.o ../libmcs.a
	$(CXX) -o $@ $^ $(LDFLAGS)

sdf_mcs.o : sdf_mcs.cc
sdf_mcs : sdf_mcs.o ../libmcs.a
	$(CXX) -o $@ $^ $(LDFLAGS)
	

clean : 
	rm -f ${EXE}
	rm -f *.o

