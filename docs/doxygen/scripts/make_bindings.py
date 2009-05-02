import doxymlparser
import optparse
import sys
import os
import string
import sip_tools
import swig_tools
import types

from common import *

option_dict = { 
            "output_dir"     : ("output", "Directory to output bindings to"),
            "sip"            : (True, "Produce SIP bindings"),
            "swig"           : (True, "Produce SWIG bindings."),
            
}

parser = optparse.OptionParser(usage="usage: %prog <doxyml files to parse>\n" , version="%prog 1.0")

for opt in option_dict:
    default = option_dict[opt][0]
    
    action = "store"
    if type(default) == types.BooleanType:
        action = "store_true"
    parser.add_option("--" + opt, default=default, action=action, dest=opt, help=option_dict[opt][1])

options, arguments = parser.parse_args()

if __name__ == "__main__":
    if len(arguments) < 1:
        parser.print_usage()
        sys.exit(1)
    
    doxyparse = doxymlparser.DoxyMLParser()
    for arg in arguments:
        doxyparse.parse(arg)
        
    if options.sip:
        builder = sip_tools.SIPBuilder(doxyparse, options.output_dir)
        builder.make_bindings()
        
    if options.swig:
        builder = swig_tools.SWIGBuilder(doxyparse, options.output_dir)
        builder.make_bindings()
