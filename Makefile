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

DEFS :=

#****************************************************************************
# Include paths
#****************************************************************************

INCS     := -Ilib

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := $(CFLAGS)   $(DEFS)
CXXFLAGS := $(CXXFLAGS) $(DEFS)

#****************************************************************************
# Source files
#****************************************************************************

SRC_DIR  := src/
SRCS     := $(SRC_DIR)energy.cpp
OBJS     := $(patsubst %.cpp, %.o, $(SRCS))

#****************************************************************************
# Targets of the build
#****************************************************************************
 
OUTPUT := energy

all: $(OUTPUT)
GAlib:
	$(MAKE) -C lib/ga/ DEBUG=$(DEBUG) PROFILE=$(PROFILE) 
tinyxml:
	$(MAKE) -C lib/tinyxml/ TINYXML_USE_STL=YES DEBUG=$(DEBUG) PROFILE=$(PROFILE)

EXTRA_LIBS := -lga
# lib/tinyxml/tinyxml.o lib/tinyxml/tinystr.o lib/tinyxml/tinyxmlparser.o lib/tinyxml/tinyxmlerror.o

#****************************************************************************
# Output
#****************************************************************************

$(OUTPUT): $(OBJS) GAlib tinyxml
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS) $(EXTRA_LIBS)

#****************************************************************************
# Common rules
#****************************************************************************

# Rules for compiling source files to object files
$(SRC_DIR)%.o : $(SRC_DIR)%.cpp
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

$(SRC_DIR)%.o : $(SRC_DIR)%.c
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJS) $(OUTPUT)