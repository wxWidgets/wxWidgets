##############################################################################
# Name:			src/makelib.sc
# Purpose:		build library Digital Mars 8.33 compiler
# Author:		Chris Elliott
# Created:		21.01.03
# RCS-ID:		$Id$
# Licence:		wxWindows licence
##############################################################################




all:  $(LIBTARGET) 

$(LIBTARGET): $(OBJECTS)
    lib -c $(LIBTARGET) $(OBJECTS)


clean: 
	-del $(THISDIR)\*.obj
	-del $(LIBTARGET)

