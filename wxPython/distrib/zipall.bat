
find . | grep -v "/CVS" | grep -v "./build/" | grep -v "./distrib/" | grep -v ".pyd" | grep -v ".pdb" | grep -v "contrib/ogl/contrib" | grep -v "contrib/stc/contrib" | zip -@ wxPython.zip


