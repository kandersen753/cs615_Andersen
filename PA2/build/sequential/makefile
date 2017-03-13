# set flags and compiler, as well as file var
CC=mpic++
CXXFLAGS=-Wall
LIBS=-lpmi
# feel free to either add files here (for more compilcated src's), or create more vars, or just manually type them in. Up to you.
FILE=../../src/pa2_sequential.cpp
# uncomment below if/when you have a folder called include that holds potential header files
#INCLUDES=-I../include
OUTPUT=pa2

all: $(OUTPUT)

$(OUTPUT): $(FILE)
	$(CC)	$(CXXFLAGS) -o $(OUTPUT) $(FILE) $(LIBS)

#PIMFuncs.o:	../src/PIMFuncs.cpp
#	$(CC)	$(CXXFLAGS)	../src/PIMFuncs.cpp	-o	PIMFuncs.o

clean:
	-@if rm $(OUTPUT) 2>/dev/null || true; then echo "Cleaned"; else echo "Nothing to clean"; fi
