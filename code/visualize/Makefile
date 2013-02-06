CC = g++

OSTYPE = $(shell uname -s)
ifeq ($(OSTYPE), Darwin)
  INCLUDES = -I/Library/Frameworks/SDL.framework/Versions/A/Headers/
  LIBS = -lSDLmain -lSDL
  FRAMEWORKS = -framework SDL -framework OpenGL -framework Cocoa
else
  INCLUDES = $(shell sdl-config --cflags)
  LIBS = $(shell sdl-config --libs) -lGL -lGLU
  FRAMEWORKS =
endif

visualize: visualize.cpp
	$(CC) $< $(INCLUDES) $(FRAMEWORKS) $(LIBS) -o $@

clean:
	rm visualize
