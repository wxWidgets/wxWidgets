
# The global was removed  in favor of static accessor functions.  This is for
# backwards compatibility:
wxTheXmlResource = wxXmlResource_Get()

wx.wxXmlNodePtr = wxXmlNodePtr




#----------------------------------------------------------------------
#  Create a factory for handling the subclass property of the object tag.


def _my_import(name):
    mod = __import__(name)
    components = name.split('.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
    return mod


class wxXmlSubclassFactory_Python(wxXmlSubclassFactory):
    def __init__(self):
        wxXmlSubclassFactory.__init__(self)

    def Create(self, className):
        assert className.find('.') != -1, "Module name must be specified!"
        mname = className[:className.rfind('.')]
        cname = className[className.rfind('.')+1:]
        module = _my_import(mname)
        klass = getattr(module, cname)
        inst = klass()
        return inst


wxXmlResource_AddSubclassFactory(wxXmlSubclassFactory_Python())

