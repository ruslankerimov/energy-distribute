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

LIBS             :=

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

INCS :=

#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := $(CFLAGS)   $(DEFS)
CXXFLAGS := $(CXXFLAGS) $(DEFS)

#****************************************************************************
# Source files
#****************************************************************************

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

#****************************************************************************
# Targets of the build
#****************************************************************************

LIB_NAME  := libga.a

all: $(LIB_NAME)

$(LIB_NAME): $(OBJS)
	$(AR) $(LIB_NAME) $?
	$(RANLIB) $(LIB_NAME)

#****************************************************************************
# Output
#****************************************************************************

$(OUTPUT): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS) $(EXTRA_LIBS)

#****************************************************************************
# Common rules
#****************************************************************************

# Rules for compiling source files to object files
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $(INCS) $< -o $@

clean:
	-rm -f $(OBJS) $(LIB_NAME)