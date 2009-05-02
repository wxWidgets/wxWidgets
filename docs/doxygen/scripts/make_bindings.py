import doxymlparser
import optparse
import sys
import os
import string
import types

option_dict = { 
            "output_dir"     : ("output", "Directory to output bindings to"),
            "sip"            : (True, "Produce SIP bindings"),
            "swig"           : (True, "Produce SWIG bindings."),
            
}

# format: class : {method : (prototype1, prototype2)}
# using a "*" means all prototypes
ignored_methods = {
    "wxIcon": {'wxIcon': (['const char', 'int', 'int'], )},

}

# these classes are either replaced by different data types in bindings, or have equivalent / better
# functionality provided by the target language.
excluded_classes = [
        "wxArchiveClassFactory",
        "wxArchiveEntry",
        "wxArchiveInputStream",
        "wxArchiveIterator",
        "wxArchiveNotifier",
        "wxArchiveOutputStream",
        "wxArray< T >", 
        "wxArrayString",
        "wxAutomationObject",
        "wxBufferedInputStream",
        "wxBufferedOutputStream",
        "wxCharBuffer",
        "wxCharTypeBuffer",
        "wxClassInfo",
        "wxCmdLineParser",
        "wxCondition",
        "wxConnection",
        "wxConnectionBase",
        "wxConvAuto",
        "wxCountingOutputStream",
        "wxCriticalSection",
        "wxCriticalSectionLocker",
        "wxCSConv",
        "wxDatagramSocket",
        "wxDataInputStream",
        "wxDataOutputStream",
        "wxDir",
        "wxDirTraverser",
        "wxFFile",
        "wxFFileInputStream",
        "wxFFileOutputStream",
        "wxFile",
        "wxFileInputStream",
        "wxFileName",
        "wxFileOutputStream",
        "wxFileStream",
        "wxFilterClassFactory",
        "wxFilterInputStream",
        "wxFilterOutputStream",
        "wxFSFile",
        "wxFSVolume",
        "wxFTP",
        "wxHashMap",
        "wxHashSet",
        "wxHashTable",
        "wxHTTP",
        "wxImage::HSVValue",
        "wxImage::RGBValue",
        "wxInputStream",
        "wxIPAddress",
        "wxIPV4Address",
        "wxList< T >",
        "wxLongLong",
        "wxMBConv",
        "wxMBConvFile",
        "wxMBConvUTF7",
        "wxMBConvUTF8",
        "wxMBConvUTF16",
        "wxMBConvUTF32",
        "wxMemoryBuffer",
        "wxMemoryFSHandler",
        "wxMemoryInputStream",
        "wxMemoryOutputStream",
        "wxMessageQueue< T >",
        "wxModule",
        "wxMutex",
        "wxMutexLocker",
        "wxNode< T >",
        "wxObjectDataPtr< T >",
        "wxObjectRefData",
        "wxOutputStream",
        "wxProcess",
        "wxProcessEvent",
        "wxProtocol",
        "wxProtocolLog",
        "wxRecursionGuard",
        "wxRecursionGuardFlag",
        "wxRegKey",
        "wxScopedArray",
        "wxScopedCharTypeBuffer",
        "wxScopedPtr",
        "wxScopedPtr< T >",
        "wxSharedPtr< T >",
        "wxServer",
        "wxSockAddress",
        "wxSocketBase",
        "wxSocketClient",
        "wxSocketEvent",
        "wxSocketInputStream",
        "wxSocketOutputStream",
        "wxSortedArrayString",
        "wxStopWatch",
        "wxStreamBase",
        "wxStreamBuffer",
        "wxStreamToTextRedirector",
        "wxString",
        "wxStringBuffer",
        "wxStringBufferLength",
        "wxStringClientData",
        "wxStringInputStream",
        "wxStringOutputStream",
        "wxTarClassFactory",
        "wxTarEntry",
        "wxTarInputStream",
        "wxTarOutputStream",
        "wxTCPClient",
        "wxTCPConnection",
        "wxTCPServer",
        "wxTempFile",
        "wxTempFileOutputStream",
        "wxTextInputStream",
        "wxTextOutputStream",
        "wxThread",
        "wxThreadEvent",
        "wxThreadHelper",
        "wxULongLong",
        "wxUniChar",
        "wxUniCharRef",
        "wxURI",
        "wxURL",
        "wxUString",
        "wxVariant",
        "wxVariantData",
        "wxVector< T >",
        "wxVector< T >::reverse_iterator",
        "wxWCharBuffer",
        "wxWeakRef< T >",
        "wxWeakRefDynamic< T >",
        "wxZipInputStream",
        "wxZipOutputStream",
        "wxZlibInputStream",
        "wxZlibOutputStream",
        ]


parser = optparse.OptionParser(usage="usage: %prog <doxyml files to parse>\n" , version="%prog 1.0")

for opt in option_dict:
    default = option_dict[opt][0]
    
    action = "store"
    if type(default) == types.BooleanType:
        action = "store_true"
    parser.add_option("--" + opt, default=default, action=action, dest=opt, help=option_dict[opt][1])

options, arguments = parser.parse_args()

def get_first_value(alist):
    if len(alist) > 0:
        return alist[0]
    else:
        return ""

def make_enums(aclass):
    retval = ""
    for enum in aclass.enums:
        retval += "enum %s {\n" % enum
        num_values = len(aclass.enums[enum])
        for value in aclass.enums[enum]:
            retval += "    %s" % value
            if not value == aclass.enums[enum][-1]:
                retval += ", "
            retval += "\n"
        retval += "};\n\n"
    
    return retval

class SIPBuilder:
    def __init__(self, doxyparse, outputdir):
        self.doxyparser = doxyparse
        self.output_dir = outputdir

    def make_bindings(self):
        output_dir = os.path.abspath(os.path.join(self.output_dir, "sip"))
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
    
        for aclass in self.doxyparser.classes:
            if aclass.name in excluded_classes:
                print "Skipping %s" % aclass.name
                continue
        
            header_name = aclass.name[2:].lower()
            filename = os.path.join(output_dir, header_name + ".sip")
            enums_text = make_enums(aclass)
            method_text = self.make_sip_methods(aclass)
            base_class = get_first_value(aclass.bases)
            if base_class != "":
                base_class = ": %s" % base_class
            
            text = """
%s
class %s %s
{
%%TypeHeaderCode
#include <%s>
%%End

public:
%s
};
""" % (enums_text, aclass.name, base_class, get_first_value(aclass.includes), method_text)

            afile = open(filename, "wb")
            afile.write(text)
            afile.close()


    def make_sip_methods(self, aclass):
        retval = ""
    
        for amethod in aclass.constructors + aclass.methods:
            transfer = ""
            
            # we need to come up with a way of filtering the methods out by various criteria
            # including parameters and method name, and how to deal with overloads 
            if aclass.name in ignored_methods:
                should_ignore = False
                for method in ignored_methods[aclass.name]:
                    print "method = %s" % method
                    if method == amethod.name:
                        params = ignored_methods[aclass.name][method]
                        should_ignore = True
                        for i in xrange(len(params)):
                            if i >= len(amethod.params):
                                should_ignore = False
                                break
                            elif amethod.params[i]["type"] != params[i]:
                                print "param type = %s, amethod.param type = %s" % (params[i], amethod.params[i]["type"])
                                should_ignore = False
                                break
                        
                if should_ignore:
                    print "Ignoring method %s..." % amethod.name
                    continue
            
            if amethod in aclass.constructors and self.doxyparser.is_derived_from_base(aclass, "wxWindow"):
                transfer = "/Transfer/"
                
            if amethod.name.startswith("operator"):
                continue
            
            retval += "    %s %s%s%s;\n\n" % (amethod.return_type.replace("virtual ", ""), amethod.name, amethod.argsstring, transfer)
        
        return retval



class SWIGBuilder:
    def __init__(self, doxyparse, outputdir):
        self.doxyparser = doxyparse
        self.output_dir = outputdir

    def make_bindings(self):
        output_dir = os.path.abspath(os.path.join(self.output_dir, "swig"))
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
    
        for aclass in self.doxyparser.classes:
            header_name = aclass.name[2:].lower()
            if aclass.name in excluded_classes:
                #print "Skipping %s" % aclass.name
                continue
                
            filename = os.path.join(output_dir, header_name + ".i")
            enums_text = make_enums(aclass)
            method_text = self.make_swig_methods(aclass)
            text = """
%%newgroup

%s
class %s : publib %s
{

public:
%s
};
""" % (enums_text, aclass.name, get_first_value(aclass.bases), method_text)

            afile = open(filename, "wb")
            afile.write(text)
            afile.close()

        
    def make_swig_methods(self, aclass):
        retval = ""
        
        retval += """
    %%pythonAppend %s    "self._setOORInfo(self)"
    %%pythonAppend %s() ""
    %%typemap(out) %s*; // turn off this typemap
    """ % (aclass.name, aclass.name, aclass.name)
    
        for amethod in aclass.constructors:
            retval += "    %s%s;\n\n" % (amethod.name, amethod.argsstring)
            
        retval += """
        // Turn it back on again
        %%typemap(out) %s* { $result = wxPyMake_wxObject($1, $owner); } 
    """ % aclass.name
    
        for amethod in aclass.methods:
            retval += "    %s %s%s;\n\n" % (amethod.return_type, amethod.name, amethod.argsstring)
        
        return retval


if __name__ == "__main__":
    if len(arguments) < 1:
        parser.print_usage()
        sys.exit(1)
    
    doxyparse = doxymlparser.DoxyMLParser()
    for arg in arguments:
        doxyparse.parse(arg)
        
    if options.sip:
        builder = SIPBuilder(doxyparse, options.output_dir)
        builder.make_bindings()
        
    if options.swig:
        builder = SWIGBuilder(doxyparse, options.output_dir)
        builder.make_bindings()
