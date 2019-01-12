#Old comm
#clang++ -std=c++14 *.cpp ./draw/*.cpp ./initial/collapse/*.cpp ./initial/load/*.cpp ./io/*.cpp ./physics/*.cpp ./sprite/*.cpp -lncurses -pthread
CC=clang++
CFLAGS=-std=c++1z -lncurses -pthread
DEPS = yxWin.h 

%.o: %.c $(DEPS)
	$(CC) -C -o $@ $< $(CFLAGS)

nscrole: main.o colorS.o draw.o draw.slice
