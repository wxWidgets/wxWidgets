SUBDIRS = html lseditor glcanvas

all: 
	-for i in $(SUBDIRS); do \
	   if test ! -r $$i/Makefile ; then \
	      make -C $$i -f Makefile.pre.in boot ; \
	   fi ; \
	   make -C $$i; \
	done

# Generic target for i.e. install, clean, distclean...
%: 
	-for i in $(SUBDIRS); do \
	   if test ! -r $$i/Makefile ; then \
	      make -C $$i -f Makefile.pre.in boot ; \
	   fi ; \
	   make -C $$i $@; \
	done
	



