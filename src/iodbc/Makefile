include ../Version.mk
include ../Config.mk





INCDIR  = .
OUTFILE = iodbc

CFLAGS = -O $(PIC) $(ANSI) -I$(INCDIR) -D$(DLDAPI) $(CFLAGSX)\
	-DVERSION=\"$(VERSION)$(EXTVER)\"

# ODBC adminiatator is statically linked on BSDI 3.1
# On this one can comment the following line:
#
ODBCDM = $(ODBC_LIBPATH)/$(OUTFILE).$(DLSUFFIX)

OBJS =  dlf.o dlproc.o herr.o henv.o hdbc.o hstmt.o connect.o prepare.o\
	execute.o result.o fetch.o info.o catalog.o misc.o itrace.o $(OBJX)

all:    $(OBJS) $(ODBCDM) $(ODBC_LIBPATH)/lib$(OUTFILE).a
	@rm -f $(ODBC_INCLUDE)/isql.h
	@rm -f $(ODBC_INCLUDE)/isqlext.h
	@rm -f $(ODBC_INCLUDE)/odbc_types.h
	@rm -f $(ODBC_INCLUDE)/odbc_funcs.h
	cp isql.h    $(ODBC_INCLUDE)/
	cp isqlext.h $(ODBC_INCLUDE)/
	cp odbc_types.h $(ODBC_INCLUDE)/
	cp odbc_funcs.h $(ODBC_INCLUDE)/
	@echo 
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	@echo 
	@echo Don\'t forget tu update $(ODBC_LIBPATH)/iodbc.ini with your conf
	@echo 
	@echo %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	@echo 

clean:
	\rm -f $(OBJS)

delete: clean

delete_all: delete

misc.o:	misc.c
	$(CC) $(CFLAGS) -DFIX_INI_FILE -DDIR_INI_FILE=\"$(ODBC_LIBPATH)\" -c $<

$(ODBC_LIBPATH)/lib$(OUTFILE).a: $(OBJS)
	$(AR) $(ODBC_LIBPATH)/lib$(OUTFILE).a $(OBJS)
	$(RANLIB) $(ODBC_LIBPATH)/lib$(OUTFILE).a

$(ODBCDM): $(OBJS)
	@echo "Generating iODBC driver manager -->" $(ODBCDM)
	@rm -f $(ODBCDM)
	$(LD) $(LDFLAGS) -L$(ODBC_LIBPATH) $(OBJS) -o $(ODBCDM) $(LIBS)
	if [ ! -f $(ODBC_LIBPATH)/iodbc.ini ]; then cp  iodbc.$(OS)  $(ODBC_LIBPATH)/iodbc.ini; fi

