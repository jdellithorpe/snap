#
# Small makefile for testing with non-Microsoft compilers
#

include ../Makefile.config

all: testSnap

testSnap: testSnap.o Snap.o 
	$(CC) -o testSnap testSnap.o Snap.o $(RCLIB) -L$(RCOBJ) $(LDFLAGS) $(LIBS) 

testSnap.o: testSnap.cpp
	$(CC) -c $(CXXFLAGS) testSnap.cpp -L$(RCOBJ) -I$(CGLIB) -I$(RCSRC) -I$(RCOBJ) -I$(RCCBN) -I$(RCGTST)

Snap.o: *.h *.cpp ../glib-core/*.h ../glib-core/*.cpp
	$(CC) -c $(CXXFLAGS) Snap.cpp -L$(RCOBJ) -I$(CGLIB) -I$(RCSRC) -I$(RCOBJ) -I$(RCCBN) -I$(RCGTST)

lib: Snap.o
	rm -f libsnap.a
	ar -cvq libsnap.a Snap.o

libinstall: lib
	cp libsnap.a /usr/local/lib/libsnap.a

clean:
	rm -f *.o *.a testSnap testSnap.exe *.plt *.tab *.png

