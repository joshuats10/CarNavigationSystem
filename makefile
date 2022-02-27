CC=cc
CFLAGS=-g -O2 -Wall `pkg-config --cflags libglfw`
LDFLAGS=-I/usr/include/freetype2
LDLIBS= -lc -lftgl `pkg-config --static --libs libglfw` -lGLU -lGL -lX11 -lXrandr -lm
RM=rm

clean :
	${RM} *.o *-
