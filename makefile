GCC        = g++
FLAGS = -std=c++11
ifdef dbg
	FLAGS  += -g 
else
	FLAGS  += -O3
endif

EXE	        = bool2cp
OBJ	        = bool2cp.o 

default: $(EXE)
	
bool2cp.o: bool2cp.cpp bool2cp.h 
	$(GCC) -c -o $@ bool2cp.cpp $(FLAGS)

# parser.o: parser.cpp parser.h helpers.o
# 	$(GCC) -c -o $@ parser.cpp $(FLAGS)

# graph.o: graph.cpp graph.h
# 	$(GCC) -c -o $@ graph.cpp $(FLAGS)

# helpers.o: helpers.cpp helpers.h
# 	$(GCC) -c -o $@ helpers.cpp $(FLAGS)

$(EXE): $(OBJ)
	$(GCC) $(OBJ) -o $(EXE)  $(FLAGS)

clean:
	rm -rf *.o $(EXE)

