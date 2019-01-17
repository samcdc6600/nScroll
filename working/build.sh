#!/bin/sh
# The plan is to replace this with a make file eventually when I learn how to actually make one that's of any use :'(
clang++ -pedantic-errors -Wall -std=c++14 *.cpp ./draw/*.cpp ./initial/checkBoundsOfBounds/*.cpp ./initial/collapse/*.cpp ./initial/load/*.cpp ./io/*.cpp ./physics/*.cpp ./sprite/player/*.cpp ./sprite/*.cpp -lncurses
# g++ -pedantic-errors -Wall -std=c++14 *.cpp ./draw/*.cpp ./initial/checkBoundsOfBounds/*.cpp ./initial/collapse/*.cpp ./initial/load/*.cpp ./io/*.cpp ./physics/*.cpp ./sprite/player/*.cpp ./sprite/*.cpp -lncurses
# remove crud >:(
rm    ./*.gch ./*~ ./*#    ./draw/*.gch ./draw/*~ ./draw/*#    ./initial/checkBoundsOfBounds/*.gch ./initial/checkBoundsOfBounds/*~ ./initial/checkBoundsOfBounds/*#    ./initial/collapse/*.gch ./initial/collapse/*~ ./initial/collapse/*#    ./initial/load/*.gch ./initial/load/*~ ./initial/load/*#    ./io/*.gch ./io/*~ ./io/*#    ./physics/*.gch ./physics/*~ ./physics/*#    ./sprite/player/*.gch ./sprite/player/*~ ./sprite/player/*#     ./sprite/*.gch ./sprite/*~ ./sprite/*#         ./assets/sprites/*# ./assets/sprites/*~    ./assets/sprites/*# ./assets/sprites/*~
