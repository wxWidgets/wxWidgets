#
# Rule for Qt's moc precompiler:
#
$(srcdir)/src/qt/%_qt_h.moc.cpp: ../include/wx/qt/%_qt.h
	moc $< -o $@

$(srcdir)/src/qt/%_qt_cpp.moc.cpp: ../src/qt/%_qt.cpp
	moc $< -o $@
    
