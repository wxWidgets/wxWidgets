# Installer Makefile

CC = gcc
RM = rm -f
RC = windres


DEFS =
LIBS = -lwx_gtk -lpng -ljpeg -lstdc++ -lgcc  -lc_r  -lgtk -lgdk -lgmodule -lglib -lXext -lX11 -lm -lgthread

CFLAGS = -g -I.. -I../incace -I/usr/local//include  -I/usr/X11R6/include -I/usr/local/lib/glib/include -DSTRICT  -D__WXGTK__ -DGTK_NO_CHECK_CASTS -D_REENTRANT -D_IODBC_ -Wall
LDFLAGS = -L/usr/local/lib -L/usr/X11R6/lib -L/usr

OBJECTS = globals.o install.o uac_comm.o uac_crc.o instsup.o \
          uac_crt.o uac_dcpr.o uac_sys.o unace.o wxmain.o

SOURCES = globals.c ../install.c uac_comm.c uac_crc.c ../instsup.cpp \
          uac_crt.c uac_dcpr.c uac_sys.c unace.c ../wxmain.cpp

all: sfx

$(OBJECTS):
	$(CC) $(CFLAGS) -c $<	

packinst: packinst/Makefile Makefile
	@cd packinst
	$(MAKE) -f Makefile all

sfx:  $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(DEFS) -o sfx $(OBJECTS) $(LIBS)

clean: 
	$(RM) $(OBJECTS) install sfx *~

globals.o: globals.c ../incace/globals.h ../incace/acestruc.h ../incace/unace.h
install.o: ../install.c ../install.h ../instsup.h
instsup.o: ../instsup.cpp
uac_comm.o: uac_comm.c ../incace/globals.h ../incace/uac_dcpr.h ../incace/uac_comm.h
uac_crc.o: uac_crc.c ../incace/uac_crc.h
uac_crt.o: uac_crt.c ../incace/os.h ../incace/attribs.h ../incace/globals.h ../incace/uac_crt.h ../incace/uac_sys.h
uac_dcpr.o: uac_dcpr.c ../incace/os.h ../incace/globals.h ../incace/portable.h ../incace/uac_comm.h ../incace/uac_crc.h \
	      ../incace/uac_dcpr.h ../incace/uac_sys.h
uac_sys.o: uac_sys.c ../incace/os.h ../incace/globals.h ../incace/uac_sys.h
unace.o: unace.c ../incace/os.h ../incace/globals.h ../incace/portable.h ../incace/uac_comm.h ../incace/uac_crc.h ../incace/uac_crt.h \
           ../incace/uac_dcpr.h ../incace/uac_sys.h
wxmain.o: ../wxmain.cpp

