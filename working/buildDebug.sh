#!/bin/sh
#build
#-ggdb3 (better symbols and debugging info then -g.)
#To use visual aspect of gdb: ctrl + x + a
clang++ -ggdb3 -pedantic-errors -Wall -std=c++14 *.c++ ./draw/*.c++ ./initial/*.c++ ./initial/checkBoundsOfBounds/*.c++ ./initial/collapse/*.c++ ./initial/load/*.c++ ./physics/*.c++ ./sprite/player/*.c++ ./sprite/*.c++ -lncurses
#remove crud >:(
rm    ./*.gch ./*~ ./*#    ./draw/*.gch ./draw/*~ ./draw/*#    ./initial/checkBoundsOfBounds/*.gch ./initial/checkBoundsOfBounds/*~ ./initial/checkBoundsOfBounds/*#    ./initial/collapse/*.gch ./initial/collapse/*~ ./initial/collapse/*#    ./initial/load/*.gch ./initial/load/*~ ./initial/load/*#    ./physics/*.gch ./physics/*~ ./physics/*#    ./sprite/player/*.gch ./sprite/player/*~ ./sprite/player/*#     ./sprite/*.gch ./sprite/*~ ./sprite/*#         ./assets/sprites/*# ./assets/sprites/*~    ./assets/sprites/*# ./assets/sprites/*~
