#
# Makefile : Builds wxWindows utils for Unix.
#

OGL_SAMPLES=ogledit studio

all:
	@for d in $(OGL_SAMPLES); do (cd $$d && $(MAKE)); done

clean:
	@for d in $(OGL_SAMPLES); do (cd $$d && $(MAKE) clean); done

