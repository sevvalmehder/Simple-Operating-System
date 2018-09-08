all: compile

compile: 8080emu.o thread.o fileSystem.o gtuos.o main.o 
	g++ 8080emu.o thread.o fileSystem.o gtuos.o main.o -o sim8080

main.o: main.cpp
	g++ -c -std=c++11 main.cpp

8080emu.o: 8080emu.cpp
	g++ -c -std=c++11 8080emu.cpp

gtuos.o: gtuos.cpp
	g++ -c -std=c++11 gtuos.cpp

thread.o: thread.cpp
	g++ -c -std=c++11 thread.cpp

fileSystem.o: fileSystem.cpp
	g++ -c -std=c++11 fileSystem.cpp
clean:
	rm *.o sim8080
	clear

