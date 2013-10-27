#*****************************************************************************
#                                                                            *
# Make file for VMS                                                          *
# Author : J.Jansen (joukj@hrem.nano.tudelft.nl)                             *
# Date : 17 June 2009                                                        *
#                                                                            *
#*****************************************************************************
.first
	define wx [--.include.wx]

.ifdef __WXMOTIF__
CXX_DEFINE = /define=(__WXMOTIF__=1)/name=(as_is,short)\
	   /assume=(nostdnew,noglobal_array_new)/warn=disable=(INTSIGNCHANGE)
.else
.ifdef __WXGTK__
CXX_DEFINE = /define=(__WXGTK__=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/warn=disable=(INTSIGNCHANGE)
.else
.ifdef __WXGTK2__
CXX_DEFINE = /define=(__WXGTK__=1,VMS_GTK2=1)/float=ieee/name=(as_is,short)/ieee=denorm\
	   /assume=(nostdnew,noglobal_array_new)/warn=disable=(INTSIGNCHANGE)
.else
.ifdef __WXX11__
CXX_DEFINE = /define=(__WXX11__=1,__WXUNIVERSAL__==1)/float=ieee\
	/name=(as_is,short)/assume=(nostdnew,noglobal_array_new)\
	/warn=disable=(INTSIGNCHANGE)
.else
CXX_DEFINE =
.endif
.endif
.endif
.endif

.suffixes : .cpp

.cpp.obj :
	cxx $(CXXFLAGS)$(CXX_DEFINE) $(MMS$TARGET_NAME).cpp

all :
.ifdef __WXMOTIF__
	$(MMS)$(MMSQUALIFIERS) baseclient.exe
	$(MMS)$(MMSQUALIFIERS) baseserver.exe
	$(MMS)$(MMSQUALIFIERS) client.exe
	$(MMS)$(MMSQUALIFIERS) server.exe
.else
.ifdef __WXGTK__
	$(MMS)$(MMSQUALIFIERS) baseclient_gtk.exe
	$(MMS)$(MMSQUALIFIERS) baseserver_gtk.exe
	$(MMS)$(MMSQUALIFIERS) client_gtk.exe
	$(MMS)$(MMSQUALIFIERS) server_gtk.exe
.else
.ifdef __WXGTK2__
	$(MMS)$(MMSQUALIFIERS) baseclient_gtk2.exe
	$(MMS)$(MMSQUALIFIERS) baseserver_gtk2.exe
	$(MMS)$(MMSQUALIFIERS) client_gtk2.exe
	$(MMS)$(MMSQUALIFIERS) server_gtk2.exe
.else
.ifdef __WXX11__
	$(MMS)$(MMSQUALIFIERS) baseclient_x11.exe
	$(MMS)$(MMSQUALIFIERS) baseserver_x11.exe
	$(MMS)$(MMSQUALIFIERS) client_x11.exe
	$(MMS)$(MMSQUALIFIERS) server_x11.exe
.endif
.endif
.endif
.endif

.ifdef __WXMOTIF__
baseclient.exe : baseclient.obj
	cxxlink baseclient,[--.lib]vms/opt
baseserver.exe : baseserver.obj
	cxxlink baseserver,[--.lib]vms/opt
client.exe : client.obj
	cxxlink client,[--.lib]vms/opt
server.exe : server.obj
	cxxlink server,[--.lib]vms/opt
.else
.ifdef __WXGTK__
baseclient_gtk.exe : baseclient.obj
	cxxlink/exe=baseclient_gtk.exe baseclient,[--.lib]vms_gtk/opt
baseserver_gtk.exe : baseserver.obj
	cxxlink/exe=baseserver_gtk.exe baseserver,[--.lib]vms_gtk/opt
client_gtk.exe : client.obj
	cxxlink/exe=client_gtk client,[--.lib]vms_gtk/opt
server_gtk.exe : server.obj
	cxxlink/exe=server_gtk.exe server,[--.lib]vms_gtk/opt
.else
.ifdef __WXGTK2__
baseclient_gtk2.exe : baseclient.obj
	cxxlink/exe=baseclient_gtk2.exe baseclient,[--.lib]vms_gtk2/opt
baseserver_gtk2.exe : baseserver.obj
	cxxlink/exe=baseserver_gtk2.exe baseserver,[--.lib]vms_gtk2/opt
client_gtk2.exe : client.obj
	cxxlink/exe=client_gtk2 client,[--.lib]vms_gtk2/opt
server_gtk2.exe : server.obj
	cxxlink/exe=server_gtk2.exe server,[--.lib]vms_gtk2/opt
.else
.ifdef __WXX11__
baseclient_x11.exe : baseclient.obj
	cxxlink/exe=baseclient_x11.exe baseclient,[--.lib]vms_x11/opt
baseserver_x11.exe : baseserver.obj
	cxxlink/exe=baseserver_x11.exe baseserver,[--.lib]vms_x11/opt
client_x11.exe : client.obj
	cxxlink/exe=client_x11 client,[--.lib]vms_x11/opt
server_x11.exe : server.obj
	cxxlink/exe=server_x11.exe server,[--.lib]vms_x11/opt
.endif
.endif
.endif
.endif

baseclient.obj : baseclient.cpp
baseserver.obj : baseserver.cpp
client.obj : client.cpp
server.obj : server.cpp
