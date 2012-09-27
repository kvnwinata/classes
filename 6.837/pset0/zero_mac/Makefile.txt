UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
	INCFLAGS  = -I include/vecmath

	FRAMEWORKS  = -framework GLUT
	FRAMEWORKS += -framework OpenGL

	LINKFLAGS += -L lib/ -lvecmath
else
	INCFLAGS  = -I /usr/include/GL
	INCFLAGS += -I /mit/6.837/public/include/vecmath

	LINKFLAGS  = -lglut -lGL -lGLU
	LINKFLAGS += -L /mit/6.837/public/lib -lvecmath
endif

CFLAGS    = -O2
CC        = g++
SRCS      = main.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = a0

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(FRAMEWORKS) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)

libvecmath:
