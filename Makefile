.PHONY: all test clean

all: cansid.o

clean:
	rm -vf *.o
	rm -f tests

test: tests
	./tests

cansid.o: cansid.c cansid.h
	$(CC) -c cansid.c -o cansid.o $(CFLAGS)

test.o: tests.c cansid.h minunit.h
	$(CC) -c tests.c -o test.o $(CFLAGS)

tests: test.o cansid.o
	$(CC) -o tests test.o cansid.o $(LDFLAGS)
