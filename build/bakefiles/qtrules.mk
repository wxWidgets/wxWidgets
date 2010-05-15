#
# Rule for Qt's moc precompiler:
#
$(srcdir)/src/qt/%.moc.cpp: $(srcdir)/include/wx/qt/%.h
	moc $< -o $@
    
