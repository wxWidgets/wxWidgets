Note that there is a small bug in the OS2 makefiles that produce an exe.
Only those not using the *.rc file actually run correctly.

This is a known problem due to the fact that both OS/2 and MSW use
the same file extension for resource source files and the ones in
the samples directories are for MSW.

The work around for this is to not use these *.rc files but instead
to use the precompiled wx.res file.

This will require a bakefile change presumably.

Here is a diff of makefile.wat for the wizard sample.

 	@%append $(OBJS)\wizard.lbc option caseexact
 	@%append $(OBJS)\wizard.lbc $(LDFLAGS) $(__DEBUGINFO_1)  libpath $(LIBDIRNAME)
 	@for %i in ($(WIZARD_OBJECTS)) do @%append $(OBJS)\wizard.lbc file %i
-	@for %i in ( $(__WXLIB_ADV_p)  $(__WXLIB_HTML_p)  $(__WXLIB_XML_p)  $(__WXLIB_
-	@%append $(OBJS)\wizard.lbc
+	@for %i in ( $(__WXLIB_ADV_p)  $(__WXLIB_HTML_p)  $(__WXLIB_XML_p)  $(__WXLIB_
 	@for %i in () do @%append $(OBJS)\wizard.lbc option stack=%i
 	wlink @$(OBJS)\wizard.lbc
+	rc $(RESFILE) $^@

RESFILE is defined in ..\samples\makefile.wat as:- ..\..\include\wx\os2\wx.res


Dave Parsons/ Chris Elliott 
http://thread.gmane.org/gmane.comp.lib.wxwidgets.devel/94104/focus=94133

