#****************************************************************************
# Debug and Profile
#****************************************************************************

DEBUG            := NO
PROFILE          := NO

#****************************************************************************

CC               := gcc
CXX              := g++
LD               := g++
AR               := ar rc
RANLIB           := ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

LIBS             := -Llib/GA/ga

DEBUG_CXXFLAGS   := $(DEBUG_CFLAGS) 
RELEASE_CXXFLAGS := $(RELEASE_CFLAGS)

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, $(DEBUG))
   CFLAGS        := $(DEBUG_CFLAGS)
   CXXFLAGS      := $(DEBUG_CXXFLAGS)
   LDFLAGS       := $(DEBUG_LDFLAGS)
else
   CFLAGS        := $(RELEASE_CFLAGS)
   CXXFLAGS      := $(RELEASE_CXXFLAGS)
   LDFLAGS       := $(RELEASE_LDFLAGS)
endif

ifeq (YES, $(PROFILE))
   CFLAGS        := $(CFLAGS) -pg -O3
   CXXFLAGS      := $(CXXFLAGS) -pg -O3
   LDFLAGS       := $(LDFLAGS) -pg
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************

DEFS     := -DTIXML_USE_STL

#****************************************************************************
# Include paths
#****************************************************************************

INCS     := -Ilib -Ilib/GA
EXTRA_LIBS := -lga

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := $(CFLAGS)   $(DEFS)
CXXFLAGS := $(CXXFLAGS) $(DEFS)

#****************************************************************************
# Source files
#****************************************************************************

SRC_DIR  := src/
SRCS     := $(wildcard $(SRC_DIR)*.cpp) \
$(wildcard lib/ACO/src/*.cpp) \
$(wildcard lib/ABC/src/*.cpp) \
$(wildcard lib/newton/*.cpp) \
$(filter-out %xmltest.cpp, $(wildcard lib/tinyxml/*.cpp)) 

OBJS     := $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SRCS)))

#****************************************************************************
# Targets of the build
#****************************************************************************
 
OUTPUT := energy

all: $(OUTPUT)
GA:
	$(MAKE) -C lib/GA/ga DEBUG=$(DEBUG) PROFILE=$(PROFILE)

FOR_LISTING_FILES = $(wildcard $(SRC_DIR)*.h) $(wildcard $(SRC_DIR)*.cpp)
listing.txt: $(FOR_LISTING_FILES)
	echo "" > $@
	for file in $(FOR_LISTING_FILES); \
	do \
		echo "/* File $$file */\n" >> $@; \
		cat $$file >> $@ ; \
	done

#****************************************************************************
# Output
#****************************************************************************

$(OUTPUT): $(OBJS) GA
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS) $(EXTRA_LIBS)

#****************************************************************************
# Common rules
#****************************************************************************

# Rules for compiling source files to object files
# For C++
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

# For C
%.o : %.c
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJS) $(OUTPUT)

#****************************************************************************
# Dependencies
#****************************************************************************

$(SRC_DIR)energy.o: $(SRC_DIR)EnergyAlgorithmBee.h
$(SRC_DIR)EnergyAlgorithmBee.h: $(SRC_DIR)EnergyAlgorithm.h
$(SRC_DIR)EnergyAlgorithm.h: $(SRC_DIR)EnergyBus.h $(SRC_DIR)EnergyBusSet.h $(SRC_DIR)EnergyLine.h $(SRC_DIR)EnergyLineSet.h
$(SRC_DIR)EnergyAlgorithm.o: $(SRC_DIR)EnergyAlgorithm.h
$(SRC_DIR)EnergyBus.o: $(SRC_DIR)EnergyBus.h
$(SRC_DIR)EnergyBusSet.o: $(SRC_DIR)EnergyBusSet.h
$(SRC_DIR)EnergyBusSet.h: $(SRC_DIR)EnergyBus.h
$(SRC_DIR)EnergyLine.o: $(SRC_DIR)EnergyLine.h
$(SRC_DIR)EnergyLineSet.o: $(SRC_DIR)EnergyLineSet.h
$(SRC_DIR)EnergyLineSet.h: $(SRC_DIR)EnergyLine.h