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

LIBS             := -Llib/ga

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

INCS     := -Ilib
EXTRA_LIBS := -lga

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := $(CFLAGS)   $(DEFS)
CXXFLAGS := $(CXXFLAGS) $(DEFS)

#****************************************************************************
# Source files
#****************************************************************************

SRC_DIR         := src/
TINIXML_LIB_DIR := lib/tinyxml/
SRCS            := $(wildcard $(SRC_DIR)*.cpp) $(filter-out %xmltest.cpp, $(wildcard $(TINIXML_LIB_DIR)*.cpp))

OBJS     := $(patsubst %.cpp, %.o, $(patsubst %.c, %.o, $(SRCS)))

#****************************************************************************
# Targets of the build
#****************************************************************************
 
OUTPUT := main

all: $(OUTPUT)
GAlib:
	$(MAKE) -C lib/ga/ DEBUG=$(DEBUG) PROFILE=$(PROFILE)

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

$(OUTPUT): $(OBJS) GAlib
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
VPATH = $(SRC_DIR)

$(SRC_DIR)main.o: Energy.h 
Energy.h: EnergyBus.h EnergyBusSet.h EnergyLine.h EnergyLineSet.h
$(SRC_DIR)Energy.o: Energy.h
$(SRC_DIR)EnergyBus.o: EnergyBus.h
$(SRC_DIR)EnergyBusSet.o: EnergyBusSet.h
EnergyBusSet.h: EnergyBus.h
$(SRC_DIR)EnergyLine.o: EnergyLine.h
$(SRC_DIR)EnergyLineSet.o: EnergyLineSet.h
EnergyLineSet.h: EnergyLine.h
$(SRC_DIR)Newton.o: Newton.h
Newton.h: Gauss.h
$(SRC_DIR)Gauss.o: Gauss.h 