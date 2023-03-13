CC = gcc
LINK = g++
KEY = -Wall -Werror -O0
NOLINK = -c

all: 
	$(CC) source/b-tree-node.c $(KEY) $(NOLINK)
	$(CC) source/b-tree.c $(KEY) $(NOLINK)

demo: all
	mkdir build -p
	$(LINK) demo/main.cpp $(KEY) $(NOLINK)
	$(LINK) *.o -o build/demo
	rm *.o

show: demo
	./build/demo

timetest: all
	mkdir build -p
	$(LINK) demo/time_test.cpp $(KEY) $(NOLINK)
	$(LINK) *.o -o build/time_test
	rm *.o
	./build/time_test

clean:	
	rm *.o *.a -f
	rm -fr build


