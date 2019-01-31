#!/bin/sh
#build
#-ggdb3 (better symbols and debugging info then -g.)
#To use visual aspect of gdb: ctrl + x + a
clang++ -ggdb3 -pedantic-errors -Wall -std=c++14 *.cpp ./draw/*.cpp ./initial/checkBoundsOfBounds/*.cpp ./initial/collapse/*.cpp ./initial/load/*.cpp ./physics/*.cpp ./sprite/player/*.cpp ./sprite/*.cpp -lncurses
#remove crud >:(
rm    ./*.gch ./*~ ./*#    ./draw/*.gch ./draw/*~ ./draw/*#    ./initial/checkBoundsOfBounds/*.gch ./initial/checkBoundsOfBounds/*~ ./initial/checkBoundsOfBounds/*#    ./initial/collapse/*.gch ./initial/collapse/*~ ./initial/collapse/*#    ./initial/load/*.gch ./initial/load/*~ ./initial/load/*#    ./physics/*.gch ./physics/*~ ./physics/*#    ./sprite/player/*.gch ./sprite/player/*~ ./sprite/player/*#     ./sprite/*.gch ./sprite/*~ ./sprite/*#         ./assets/sprites/*# ./assets/sprites/*~    ./assets/sprites/*# ./assets/sprites/*~
