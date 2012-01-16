CXX         = g++
CXXFLAGS    = -g -Wall

GA_INC_DIR  = /usr/local/include
GA_LIB_DIR  = /usr/local/lib

INC_DIRS    = -I$(GA_INC_DIR)
LIB_DIRS    = -L$(GA_LIB_DIR)

EXS=energe

.SUFFIXES: .cpp
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $<

all: $(EXS)

$(EXS): %: %.o
	$(CXX) $@.o -o $@ $(LIB_DIRS) -lga -lm $(CXX_LIBS)

clean:
	$(RM) $(EXS)
	$(RM) *.o