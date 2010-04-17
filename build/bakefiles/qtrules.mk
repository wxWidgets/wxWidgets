#
# Rule for Qt's moc precompiler:
#
$(srcdir)/src/qt/%_qt_moc.cpp: ../include/wx/qt/%_qt.h
	moc $< -o $@

