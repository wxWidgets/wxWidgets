include Version.mk
include Config.mk











INCDIR	= .
OUTDIR	= $(HOME)

CFLAGS  = -O $(PIC) $(ANSI) -I$(INCDIR) -D$(DLDAPI) $(CFLAGSX)\
	  -DVERSION=\"$(VERSION)$(EXTVER)\" 

ODBCDM	= $(OUTDIR)/$(OUTFILE)-$(VERSION).$(DLSUFFIX)

OBJS =	dlf.o dlproc.o herr.o henv.o hdbc.o hstmt.o \
	connect.o prepare.o execute.o result.o \
	fetch.o info.o catalog.o misc.o itrace.o $(OBJX)

all:	$(OBJS)
	@echo "Generating iODBC driver manager -->" $(ODBCDM)
	@\rm -f $(ODBCDM)
	@$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(ODBCDM)

clean:
	\rm -f $(OBJS) 
