CC = g++
LD = g++
LDFLAGS =-lgsl
CFLAGS=-g -Wall `pkg-config --cflags libfreenect` `pkg-config opencv --cflags ` -I /usr/local/include -lopengl32 -lglut32 -I/usr/include/gsl -ansi -Wall -pedantic -g
LIBS = `pkg-config --libs libfreenect` `pkg-config opencv --libs` -L/usr/local/lib -lGL -lGLU -lglut -L/usr/local/lib -lgsl -lgslcblas
OBJECTS1 = main.o
#OBJECTS2 = structure.o
PROG = executor

all:$(PROG)

$(PROG): server.o main.o errorHandler.o token.o
	$(CC) -o $(PROG) server.o main.o errorHandler.o token.o
server.o : server.cpp structure.h client.h
	$(CC) -c server.cpp
main.o : main.cpp structure.h client.h
	$(CC) -c main.cpp
errorHandler.o : errorHandler.cpp
	$(CC) -c errorHandler.cpp
token.o : token.cpp
	$(CC) -c token.cpp
#$(OBJECTS1): glview.cpp
#	$(CC) $(CFLAGS)  $(LIBS) -c glview.cpp
#$(OBJECTS2): structure.cpp
#	$(CC) $(CFLAGS)  $(LIBS) -c structure.cpp
clean:
	rm -rf *.o $(PROG)


