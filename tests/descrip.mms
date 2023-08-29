#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 6 January 2021                                                      *
#                                                                            *
#*****************************************************************************
.first
	define wx [-.include.wx]

.ifdef __WXMOTIF__
TEST_CXXFLAGS = /define=(__WXMOTIF__=1,"wxUSE_GUI=0","CATCH_CONFIG_VARIADIC_MACROS=1")/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
TEST_GUI_CXXFLAGS = /define=(__WXMOTIF__=1,"wxUSE_GUI=1","CATCH_CONFIG_VARIADIC_MACROS=1")\
	/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXGTK__
TEST_CXXFLAGS = /define=(__WXGTK__=1,"wxUSE_GUI=0","__USE_STD_IOSTREAM=1",\
	"_USE_STD_STAT=1","CATCH_CONFIG_VARIADIC_MACROS=1")\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/include=[]/list/show=all
TEST_GUI_CXXFLAGS = /define=(__WXGTK__=1,"wxUSE_GUI=1","__USE_STD_IOSTREAM=1",\
	"CATCH_CONFIG_VARIADIC_MACROS=1")\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/include=[]/list/show=all
.else
.ifdef __WXX11__
TEST_CXXFLAGS = /define=(__WXX11__=1,__WXUNIVERSAL__==1,"wxUSE_GUI=0",\
	"CATCH_CONFIG_VARIADIC_MACROS=1")/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
TEST_GUI_CXXFLAGS = /define=(__WXGTK__=1,"wxUSE_GUI=1",\
	"CATCH_CONFIG_VARIADIC_MACROS=1")\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
.else
.ifdef __WXGTK2__
TEST_CXXFLAGS = /define=(__WXGTK__=1,VMS_GTK2==1,"wxUSE_GUI=0",\
	"CATCH_CONFIG_VARIADIC_MACROS=1")/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)
TEST_GUI_CXXFLAGS = /define=(__WXGTK__=1,"wxUSE_GUI=1",\
	"CATCH_CONFIG_VARIADIC_MACROS=1")\
	/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)
.else
CXX_DEFINE =
CC_DEFINE =
.endif
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

CXXC=cxx

TEST_OBJECTS =  \
	test_anytest.obj,\
	test_archivetest.obj,\
	test_arrays.obj,\
	test_base64.obj,\
	test_cmdlinetest.obj,\
	test_fileconf.obj,\
	test_regconf.obj,\
	test_datetimetest.obj,\
	test_evthandler.obj,\
	test_evtsource.obj,\
	test_stopwatch.obj,\
	test_timertest.obj,\
	test_exec.obj,\
	test_dir.obj,\
	test_filefn.obj,\
	test_filetest.obj,\
	test_filekind.obj,\
	test_filenametest.obj,\
	test_filesystest.obj,\
	test_fontmaptest.obj,\
	test_formatconvertertest.obj,\
	test_hashes.obj,\
	test_output.obj,\
	test_input.obj,\
	test_intltest.obj,\
	test_lists.obj,\
	test_logtest.obj,\
	test_longlongtest.obj,\
	test_convautotest.obj,\
	test_mbconvtest.obj,\
	test_dynamiclib.obj,\
	test_environ.obj,\
	test_metatest.obj,\
	test_misctests.obj,\
	test_module.obj,\
	test_pathlist.obj,\
	test_typeinfotest.obj

TEST_OBJECTS1=test_ipc.obj,\
	test_socket.obj,\
	test_regextest.obj,\
	test_wxregextest.obj,\
	test_scopeguardtest.obj,\
	test_iostream.obj,\
	test_strings.obj,\
	test_stdstrings.obj,\
	test_tokenizer.obj,\
	test_unichar.obj,\
	test_unicode.obj,\
	test_crt.obj,\
	test_vsnprintf.obj,\
	test_datastreamtest.obj,\
	test_ffilestream.obj,\
	test_fileback.obj,\
	test_filestream.obj,\
	test_iostreams.obj,\
	test_largefile.obj,\
	test_memstream.obj,\
	test_socketstream.obj,\
	test_sstream.obj,\
	test_stdstream.obj,\
	test_tempfile.obj,\
	test_textstreamtest.obj,\
	test_zlibstream.obj,\
	test_textfiletest.obj,\
	test_atomic.obj,\
	test_misc.obj,\
	test_queue.obj,\
	test_tls.obj,\
	test_ftp.obj,\
	test_uris.obj,\
	test_url.obj,\
	test_evtconnection.obj,\
	test_weakref.obj,\
	test_xlocale.obj,\
	test_xmltest.obj

TEST_L_OBJs=test_ziptest.obj,\
	test_tartest.obj

TEST_GUI_OBJECTS =  \
	test_gui_asserthelper.obj,\
	test_gui_testableframe.obj,\
	test_gui_rect.obj,\
	test_gui_size.obj,\
	test_gui_point.obj,\
	test_gui_bitmap.obj,\
	test_gui_colour.obj,\
	test_gui_ellipsization.obj,\
	test_gui_measuring.obj,\
	test_gui_config.obj,\
	test_gui_bitmapcomboboxtest.obj,\
	test_gui_bitmaptogglebuttontest.obj,\
	test_gui_bookctrlbasetest.obj,\
	test_gui_buttontest.obj

TEST_GUI_OBJECTS1=test_gui_checkboxtest.obj,\
	test_gui_checklistboxtest.obj,\
	test_gui_choicebooktest.obj,\
	test_gui_choicetest.obj,\
	test_gui_comboboxtest.obj,\
	test_gui_frametest.obj,\
	test_gui_gaugetest.obj,\
	test_gui_gridtest.obj,\
	test_gui_headerctrltest.obj,\
	test_gui_htmllboxtest.obj,\
	test_gui_hyperlinkctrltest.obj,\
	test_gui_itemcontainertest.obj,\
	test_gui_label.obj,\
	test_gui_listbasetest.obj,\
	test_gui_listbooktest.obj,\
	test_gui_listboxtest.obj,\
	test_gui_listctrltest.obj,\
	test_gui_listviewtest.obj,\
	test_gui_notebooktest.obj,\
	test_gui_pickerbasetest.obj,\
	test_gui_pickertest.obj,\
	test_gui_radioboxtest.obj,\
	test_gui_radiobuttontest.obj,\
	test_gui_rearrangelisttest.obj

TEST_GUI_OBJECTS2=test_gui_richtextctrltest.obj,\
	test_gui_slidertest.obj,\
	test_gui_spinctrldbltest.obj,\
	test_gui_spinctrltest.obj,\
	test_gui_styledtextctrltest.obj,\
	test_gui_textctrltest.obj,\
	test_gui_textentrytest.obj,\
	test_gui_togglebuttontest.obj,\
	test_gui_toolbooktest.obj,\
	test_gui_treebooktest.obj,\
	test_gui_treectrltest.obj,\
	test_gui_virtlistctrltest.obj,\
	test_gui_windowtest.obj,\
	test_gui_clone.obj,\
	test_gui_propagation.obj,\
	test_gui_keyboard.obj,\
	test_gui_fonttest.obj,\
	test_gui_image.obj,\
	test_gui_rawbmp.obj,\
	test_gui_htmlwindow.obj,\
	test_gui_accelentry.obj,\
	test_gui_menu.obj,\
	test_gui_guifuncs.obj,\
	test_gui_selstoretest.obj,\
	test_gui_garbage.obj,\
	test_gui_settings.obj,\
	test_gui_socket.obj,\
	test_gui_boxsizer.obj,\
	test_gui_clientsize.obj,\
	test_gui_setsize.obj,\
	test_gui_xrctest.obj

.ifdef __WXMOTIF__
.else
.ifdef __WXGTK__
all : test_gtk.exe test_gui_gtk.exe
	write sys$output "tests created"

test_gtk.exe : test_test.obj $(TEST_OBJECTS) $(TEST_OBJECTS1) $(TEST_L_OBJS)
	library/create temp.olb $(TEST_OBJECTS)
	library temp.olb $(TEST_OBJECTS1)
	cxxlink/exec=test_gtk.exe test_test.obj,$(TEST_L_OBJS),temp.olb/lib,\
	sys$library:libcppunit.olb/lib,[-.lib]vms_gtk/opt
	delete temp.olb;*

test_gui_gtk.exe : test_gui_test.obj $(TEST_GUI_OBJECTS) $(TEST_GUI_OBJECTS1)\
	$(TEST_GUI_OBJECTS2)
	library/create temp.olb $(TEST_GUI_OBJECTS)
	library temp.olb $(TEST_GUI_OBJECTS1)
	library temp.olb $(TEST_GUI_OBJECTS2)
	cxxlink/exec=test_gui_gtk.exe test_gui_test.obj,temp.olb/lib,\
	sys$library:libcppunit.olb/lib,[-.lib]vms_gtk/opt
.else
.ifdef __WXX11__
.else
.ifdef __WXGTK2__
.else
.endif
.endif
.endif
.endif

$(TEST_OBJECTS) : [-.include.wx]setup.h
$(TEST_GUI_OBJECTS) : [-.include.wx]setup.h

test_test.obj : test.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) test.cpp

test_anytest.obj : [.any]anytest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.any]anytest.cpp

test_archivetest.obj : [.archive]archivetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.archive]archivetest.cpp

test_ziptest.obj : [.archive]ziptest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.archive]ziptest.cpp

test_tartest.obj : [.archive]tartest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.archive]tartest.cpp

test_arrays.obj : [.arrays]arrays.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.arrays]arrays.cpp

test_base64.obj : [.base64]base64.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.base64]base64.cpp

test_cmdlinetest.obj : [.cmdline]cmdlinetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.cmdline]cmdlinetest.cpp

test_fileconf.obj : [.config]fileconf.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.config]fileconf.cpp

test_regconf.obj : [.config]regconf.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.config]regconf.cpp

test_datetimetest.obj : [.datetime]datetimetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS)/warn=(disable=INTSIGNCHANGE)\
	[.datetime]datetimetest.cpp

test_evthandler.obj : [.events]evthandler.cpp 
.ifdef ALPHA
	pipe gsed\
	-e "s/handler.Connect(wxEVT_THREAD, wxThreadEventHandler(MyHandler::OnOverloadedHandler));//"\
	-e "s/handler.Connect(wxEVT_IDLE, wxIdleEventHandler(MyHandler::OnOverloadedHandler));//" \
	< [.events]evthandler.cpp > [.events]evthandler.cpp_
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.events]evthandler.cpp_
	delete [.events]evthandler.cpp_;*
.else
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.events]evthandler.cpp
.endif

test_evtsource.obj : [.events]evtsource.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.events]evtsource.cpp

test_stopwatch.obj : [.events]stopwatch.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.events]stopwatch.cpp

test_timertest.obj : [.events]timertest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.events]timertest.cpp

test_exec.obj : [.exec]exec.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.exec]exec.cpp

test_dir.obj : [.file]dir.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.file]dir.cpp

test_filefn.obj : [.file]filefn.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.file]filefn.cpp

test_filetest.obj : [.file]filetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.file]filetest.cpp

test_filekind.obj : [.filekind]filekind.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.filekind]filekind.cpp

test_filenametest.obj : [.filename]filenametest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.filename]filenametest.cpp

test_filesystest.obj : [.filesys]filesystest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.filesys]filesystest.cpp

test_fontmaptest.obj : [.fontmap]fontmaptest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.fontmap]fontmaptest.cpp

test_formatconvertertest.obj : [.formatconverter]formatconvertertest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.formatconverter]formatconvertertest.cpp

test_hashes.obj : [.hashes]hashes.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.hashes]hashes.cpp

test_output.obj : [.interactive]output.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.interactive]output.cpp

test_input.obj : [.interactive]input.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.interactive]input.cpp

test_intltest.obj : [.intl]intltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.intl]intltest.cpp

test_lists.obj : [.lists]lists.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.lists]lists.cpp

test_logtest.obj : [.log]logtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.log]logtest.cpp

test_longlongtest.obj : [.longlong]longlongtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.longlong]longlongtest.cpp

test_convautotest.obj : [.mbconv]convautotest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.mbconv]convautotest.cpp

test_mbconvtest.obj : [.mbconv]mbconvtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.mbconv]mbconvtest.cpp

test_dynamiclib.obj : [.misc]dynamiclib.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]dynamiclib.cpp

test_environ.obj : [.misc]environ.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]environ.cpp

test_metatest.obj : [.misc]metatest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]metatest.cpp

test_misctests.obj : [.misc]misctests.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]misctests.cpp

test_module.obj : [.misc]module.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]module.cpp

test_pathlist.obj : [.misc]pathlist.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]pathlist.cpp

test_typeinfotest.obj : [.misc]typeinfotest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.misc]typeinfotest.cpp

test_ipc.obj : [.net]ipc.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.net]ipc.cpp

test_socket.obj : [.net]socket.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS)/warn=(disable=REFTEMPORARY)\
	[.net]socket.cpp

test_regextest.obj : [.regex]regextest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.regex]regextest.cpp

test_wxregextest.obj : [.regex]wxregextest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.regex]wxregextest.cpp

test_scopeguardtest.obj : [.scopeguard]scopeguardtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.scopeguard]scopeguardtest.cpp

test_iostream.obj : [.strings]iostream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]iostream.cpp

test_strings.obj : [.strings]strings.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS)/warn=(disable=INTSIGNCHANGE)\
	[.strings]strings.cpp

test_stdstrings.obj : [.strings]stdstrings.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]stdstrings.cpp

test_tokenizer.obj : [.strings]tokenizer.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]tokenizer.cpp

test_unichar.obj : [.strings]unichar.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]unichar.cpp

test_unicode.obj : [.strings]unicode.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]unicode.cpp

test_crt.obj : [.strings]crt.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]crt.cpp

test_vsnprintf.obj : [.strings]vsnprintf.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.strings]vsnprintf.cpp

test_datastreamtest.obj : [.streams]datastreamtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]datastreamtest.cpp

test_ffilestream.obj : [.streams]ffilestream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]ffilestream.cpp

test_fileback.obj : [.streams]fileback.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]fileback.cpp

test_filestream.obj : [.streams]filestream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]filestream.cpp

test_iostreams.obj : [.streams]iostreams.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]iostreams.cpp

test_largefile.obj : [.streams]largefile.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS)/warn=(disable=INTSIGNCHANGE)\
	[.streams]largefile.cpp

test_memstream.obj : [.streams]memstream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]memstream.cpp

test_socketstream.obj : [.streams]socketstream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]socketstream.cpp

test_sstream.obj : [.streams]sstream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]sstream.cpp

test_stdstream.obj : [.streams]stdstream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]stdstream.cpp

test_tempfile.obj : [.streams]tempfile.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]tempfile.cpp

test_textstreamtest.obj : [.streams]textstreamtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]textstreamtest.cpp

test_zlibstream.obj : [.streams]zlibstream.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.streams]zlibstream.cpp

test_textfiletest.obj : [.textfile]textfiletest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.textfile]textfiletest.cpp

test_atomic.obj : [.thread]atomic.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.thread]atomic.cpp

test_misc.obj : [.thread]misc.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.thread]misc.cpp

test_queue.obj : [.thread]queue.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.thread]queue.cpp

test_tls.obj : [.thread]tls.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.thread]tls.cpp

test_ftp.obj : [.uris]ftp.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.uris]ftp.cpp

test_uris.obj : [.uris]uris.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.uris]uris.cpp

test_url.obj : [.uris]url.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.uris]url.cpp

test_evtconnection.obj : [.weakref]evtconnection.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.weakref]evtconnection.cpp

test_weakref.obj : [.weakref]weakref.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.weakref]weakref.cpp

test_xlocale.obj : [.xlocale]xlocale.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.xlocale]xlocale.cpp

test_xmltest.obj : [.xml]xmltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_CXXFLAGS) [.xml]xmltest.cpp

test_gui_sample_rc.obj : [...]samples]sample.rc 
	$(WINDRES) -i$< -o$@    --define __WX$(TOOLKIT)__ $(__WXUNIV_DEFINE_p_5) $(__DEBUG_DEFINE_p_5)  $(__EXCEPTIONS_DEFINE_p_5) $(__RTTI_DEFINE_p_5) $(__THREAD_DEFINE_p_5)   --include-dir $(srcdir) $(__DLLFLAG_p_5) --include-dir [...]samples $(__RCDEFDIR_p_1) --include-dir $(top_srcdir)]include

test_gui_asserthelper.obj : asserthelper.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) asserthelper.cpp

test_gui_test.obj : test.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) test.cpp

test_gui_testableframe.obj : testableframe.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) testableframe.cpp

test_gui_rect.obj : [.geometry]rect.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.geometry]rect.cpp

test_gui_size.obj : [.geometry]size.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.geometry]size.cpp

test_gui_point.obj : [.geometry]point.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.geometry]point.cpp

test_gui_bitmap.obj : [.graphics]bitmap.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.graphics]bitmap.cpp

test_gui_colour.obj : [.graphics]colour.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.graphics]colour.cpp

test_gui_ellipsization.obj : [.graphics]ellipsization.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.graphics]ellipsization.cpp

test_gui_measuring.obj : [.graphics]measuring.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.graphics]measuring.cpp

test_gui_config.obj : [.config]config.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.config]config.cpp

test_gui_bitmapcomboboxtest.obj : [.controls]bitmapcomboboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]bitmapcomboboxtest.cpp

test_gui_bitmaptogglebuttontest.obj : [.controls]bitmaptogglebuttontest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]bitmaptogglebuttontest.cpp

test_gui_bookctrlbasetest.obj : [.controls]bookctrlbasetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]bookctrlbasetest.cpp

test_gui_buttontest.obj : [.controls]buttontest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]buttontest.cpp

test_gui_checkboxtest.obj : [.controls]checkboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]checkboxtest.cpp

test_gui_checklistboxtest.obj : [.controls]checklistboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]checklistboxtest.cpp

test_gui_choicebooktest.obj : [.controls]choicebooktest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]choicebooktest.cpp

test_gui_choicetest.obj : [.controls]choicetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]choicetest.cpp

test_gui_comboboxtest.obj : [.controls]comboboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]comboboxtest.cpp

test_gui_frametest.obj : [.controls]frametest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]frametest.cpp

test_gui_gaugetest.obj : [.controls]gaugetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]gaugetest.cpp

test_gui_gridtest.obj : [.controls]gridtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]gridtest.cpp

test_gui_headerctrltest.obj : [.controls]headerctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]headerctrltest.cpp

test_gui_htmllboxtest.obj : [.controls]htmllboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]htmllboxtest.cpp

test_gui_hyperlinkctrltest.obj : [.controls]hyperlinkctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]hyperlinkctrltest.cpp

test_gui_itemcontainertest.obj : [.controls]itemcontainertest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]itemcontainertest.cpp

test_gui_label.obj : [.controls]label.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]label.cpp

test_gui_listbasetest.obj : [.controls]listbasetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]listbasetest.cpp

test_gui_listbooktest.obj : [.controls]listbooktest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]listbooktest.cpp

test_gui_listboxtest.obj : [.controls]listboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]listboxtest.cpp

test_gui_listctrltest.obj : [.controls]listctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]listctrltest.cpp

test_gui_listviewtest.obj : [.controls]listviewtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]listviewtest.cpp

test_gui_notebooktest.obj : [.controls]notebooktest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]notebooktest.cpp

test_gui_pickerbasetest.obj : [.controls]pickerbasetest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]pickerbasetest.cpp

test_gui_pickertest.obj : [.controls]pickertest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]pickertest.cpp

test_gui_radioboxtest.obj : [.controls]radioboxtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]radioboxtest.cpp

test_gui_radiobuttontest.obj : [.controls]radiobuttontest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]radiobuttontest.cpp

test_gui_rearrangelisttest.obj : [.controls]rearrangelisttest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]rearrangelisttest.cpp

test_gui_richtextctrltest.obj : [.controls]richtextctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]richtextctrltest.cpp

test_gui_slidertest.obj : [.controls]slidertest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]slidertest.cpp

test_gui_spinctrldbltest.obj : [.controls]spinctrldbltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]spinctrldbltest.cpp

test_gui_spinctrltest.obj : [.controls]spinctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]spinctrltest.cpp

test_gui_styledtextctrltest.obj : [.controls]styledtextctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]styledtextctrltest.cpp

test_gui_textctrltest.obj : [.controls]textctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]textctrltest.cpp

test_gui_textentrytest.obj : [.controls]textentrytest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]textentrytest.cpp

test_gui_togglebuttontest.obj : [.controls]togglebuttontest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]togglebuttontest.cpp

test_gui_toolbooktest.obj : [.controls]toolbooktest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]toolbooktest.cpp

test_gui_treebooktest.obj : [.controls]treebooktest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]treebooktest.cpp

test_gui_treectrltest.obj : [.controls]treectrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]treectrltest.cpp

test_gui_virtlistctrltest.obj : [.controls]virtlistctrltest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]virtlistctrltest.cpp

test_gui_windowtest.obj : [.controls]windowtest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.controls]windowtest.cpp

test_gui_clone.obj : [.events]clone.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.events]clone.cpp

test_gui_propagation.obj : [.events]propagation.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.events]propagation.cpp

test_gui_keyboard.obj : [.events]keyboard.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.events]keyboard.cpp

test_gui_fonttest.obj : [.font]fonttest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.font]fonttest.cpp

test_gui_image.obj : [.image]image.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.image]image.cpp

test_gui_rawbmp.obj : [.image]rawbmp.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.image]rawbmp.cpp

test_gui_htmlwindow.obj : [.html]htmlwindow.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.html]htmlwindow.cpp

test_gui_accelentry.obj : [.menu]accelentry.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.menu]accelentry.cpp

test_gui_menu.obj : [.menu]menu.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.menu]menu.cpp

test_gui_guifuncs.obj : [.misc]guifuncs.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.misc]guifuncs.cpp

test_gui_selstoretest.obj : [.misc]selstoretest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.misc]selstoretest.cpp

test_gui_garbage.obj : [.misc]garbage.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.misc]garbage.cpp

test_gui_settings.obj : [.misc]settings.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.misc]settings.cpp

test_gui_socket.obj : [.net]socket.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS)/warn=(disable=REFTEMPORARY)\
	[.net]socket.cpp

test_gui_boxsizer.obj : [.sizers]boxsizer.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.sizers]boxsizer.cpp

test_gui_clientsize.obj : [.window]clientsize.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.window]clientsize.cpp

test_gui_setsize.obj : [.window]setsize.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.window]setsize.cpp

test_gui_xrctest.obj : [.xml]xrctest.cpp 
	$(CXXC) /object=[]$@ $(TEST_GUI_CXXFLAGS) [.xml]xrctest.cpp

