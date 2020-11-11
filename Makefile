OBJS = Logger.o project.o SafeQueue.o
SOURCE = Logger.cpp project.cpp SafeQueue.cpp
HEADER = Logger.h SafeQueue.h
OUT = project
CC = g++
FLAGS = -g -c -Wall
LFLAGS = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Logger.o: Logger.cpp
	$(CC) $(FLAGS) Logger.cpp

project.o: project.cpp
	$(CC) $(FLAGS) project.cpp

SafeQueue.o: SafeQueue.cpp
	$(CC) $(FLAGS) SafeQueue.cpp

clean: 
	rm -f $(OBJS) $(OUT)

