CXX	= clang++
#CXX	= g++
# fpermissive requred for compilation with g++.
CXXFLAGS	= -std=c++20 -Wall -Wpedantic -Wextra -O2 #-D_XOPEN_SOURCE_EXTENDED
# Here we also need to specify (using -L) the directory where "lSDL2"
# (libSDL2.so) is, which is "/usr/local/lib".
LIBS	= -lncurses -lSDL2 -L/usr/local/lib
BIN_NAME	= scrollioExtremeo # nscrl
SRC	= ./src
IDIR	= $(SRC)/include
# Here we need to specify (using -I) the directory where the SDL header/s are,
# which is "/usr/local/include/SDL2". Extidir stands for EXTerior DIRectory.
EXTIDIR = -I/usr/local/include/SDL2
ODIR	= ./obj
DEPS	= $(wildcard $(IDIR)/*.hpp) # $(EXTIDIR)
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
	$(CXX) -o $@ $^ $(LIBS) -DNDEBUG $(EXTIDIR)


# $(ODIR)/%.o depends on $(SRC)/%.cpp $(DEPS).
$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(DEBUG) $(EXTIDIR)


# Clean and debug targets won't update on files.
.PHONY: clean debug


# Call make recursively on $(BIN_NAME) target (with DEBUG var set to -DDEBUG.)
debug:
	$(MAKE) $(BIN_NAME) CXXFLAGS="-std=c++14 -g -O0 -Wall -Wpedantic -Wextra" DEBUG=-DDEBUG 


# Files with '#' and '~' seem to be left around be emacs >:'^(
clean:
	$(RM) $(BIN_NAME) $(ODIR)/*.o *.core *~ $(SRC)/*~ $(IDIR)/*~ \#*\# $(SRC)/\#*\# $(IDIR)/\#*\#


#### TODO: ADD SOMETHING LIKE THIS TO THIS MAKE FILE!
# Yes, you can modify your makefile to check whether all compilations have succeeded and print a message if there were any errors. One way to achieve this is by using the special target .IGNORE, which allows you to specify a command to run after all other rules have been processed. You can use this target to check the return code of the compilation commands and take appropriate action.

# Here's an example of how you can modify your makefile:

# makefile

# # Your existing rules and dependencies here

# # Add a new target to check compilation success
# check:
#     @echo "Checking compilation status..."
#     @if grep -n "error:" $(MAKEFILE) >/dev/null; then \
#         echo "Compilation failed! Please check the errors."; \
#         exit 1; \
#     else \
#         echo "Compilation successful!"; \
#     fi

# .IGNORE: check

# In this example, the .IGNORE target is set to depend on the check target. The check target runs a command using grep to search for the string "error:" in the makefile (assuming the makefile is named $(MAKEFILE)). If any error messages are found, it prints a failure message and exits with a non-zero status. Otherwise, it prints a success message.

# When you run gmake or gmake -j8, you can add check as a dependency to ensure that the check target is executed after all compilations are finished. If any errors were encountered during compilation, the failure message will be displayed.

# Note that the specific implementation may vary depending on your makefile structure and requirements. Adjust the grep command or other parts of the check target as needed to fit your build process.
