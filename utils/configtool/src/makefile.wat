# 
# Makefile for WATCOM 
# 
# Created by Julian Smart, January 1999 
#  
# 
# 
# 
PROGRAM = wxconfigtool
OBJECTS = $(OUTPUTDIR)\$(PROGRAM).obj $(OUTPUTDIR)\appsettings.obj &
  $(OUTPUTDIR)\configitem.obj $(OUTPUTDIR)\configitemselector.obj &
  $(OUTPUTDIR)\configtooldoc.obj $(OUTPUTDIR)\configtoolview.obj &
  $(OUTPUTDIR)\configtree.obj $(OUTPUTDIR)\custompropertydialog.obj &
  $(OUTPUTDIR)\htmlparser.obj $(OUTPUTDIR)\mainframe.obj $(OUTPUTDIR)\propeditor.obj &
  $(OUTPUTDIR)\property.obj $(OUTPUTDIR)\settingsdialog.obj $(OUTPUTDIR)\utils.obj

# 
!include $(%WXWIN)\src\makeprog.wat 
