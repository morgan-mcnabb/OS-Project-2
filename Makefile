project: project.o Logger.o
	g++ project.o Logger.o -o project
 
project.o: project.cpp
	g++ -c project.cpp

Logger.o: Logger.cpp
	g++ -c Logger.cpp

clean:
	rm *.o project project.o Logger.o
