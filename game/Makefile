CXX	= clang++
#CXX	= g++
# fpermissive requred for compilation with g++.
CXXFLAGS	= -std=c++14 -Wall -Wpedantic -Wextra -O2
LIBS	= -lncurses
BIN_NAME	= nscrl
SRC	= ./src
IDIR	= $(SRC)/include
ODIR	= ./obj
DEPS	= $(wildcard $(IDIR)/*.hpp)
CPP_DIRS	=  $(wildcard $(SRC)/*.cpp) # Get list of .cpp file directories.
CPP_FILES	= $(subst $(SRC)/,,$(CPP_DIRS)) # Strip srings of paths.
OBJ_FILES	= $(patsubst %.cpp,%.o,$(CPP_FILES)) # Replace .cpp with .o.
# Prepend object directory to .o file names.
OBJ	= $(patsubst %,$(ODIR)/%,$(OBJ_FILES))


# Main make file pattern...
# Target: Prerequisite
#	Rule
# 
# $(BIN_NAME) depends on $(OBJ).
$(BIN_NAME): $(OBJ)
	$(CXX) -o $@ $^ $(LIBS) -DNDEBUG


# $(ODIR)/%.o depends on $(SRC)/%.cpp $(DEPS).
$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(DEBUG)


# Clean and debug targets won't update on files.
.PHONY: clean debug


# Call make recursively on $(BIN_NAME) target (with DEBUG var set to -DDEBUG.)
debug:
	$(MAKE) $(BIN_NAME) CXXFLAGS="-std=c++14 -g -O0 -Wall -Wpedantic -Wextra" DEBUG=-DDEBUG 


# Files with '#' and '~' seem to be left around be emacs >:'^(
clean:
	$(RM) $(BIN_NAME) $(ODIR)/*.o *.core *~ $(SRC)/*~ $(IDIR)/*~ \#*\# $(SRC)/\#*\# $(IDIR)/\#*\#

