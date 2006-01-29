import sys, os, string, shutil, re

#TODO: copy .make files to build/msw

# update setup_h defines to match official wxPython settings
defines = {
 'wxDIALOG_UNIT_COMPATIBILITY' :'0',
 'wxUSE_DEBUG_CONTEXT'   :'0',
 'wxUSE_MEMORY_TRACING'          :'0',
 'wxUSE_DIALUP_MANAGER'          :'0',
 'wxUSE_GLCANVAS'                :'1',
 'wxUSE_POSTSCRIPT'              :'1',
 'wxUSE_AFM_FOR_POSTSCRIPT'      :'0',
 'wxUSE_DISPLAY'                 :'1',
 'wxUSE_DEBUGREPORT'             :'0'
}

uni_defines = {
 'wxUSE_UNICODE'                 :'1',
 'wxUSE_UNICODE_MSLU'            :'1'
}

debug_defines = {
 'wxUSE_DEBUG_CONTEXT'   :'1',
 'wxUSE_MEMORY_TRACING'          :'1',
}

if "UNICODE=1" in sys.argv:
    defines.update(uni_defines)

if "DEBUG=1" in sys.argv:
    defines.update(debug_defines)

setup_dir = os.path.join("..", "..", "include", "wx", "msw")

# copy the file over if it doesn't exist
setup_h = os.path.join(setup_dir, "setup.h")
setup0_h = os.path.join(setup_dir, "setup0.h")
if not os.path.exists(setup_h) and os.path.exists(setup0_h):
    shutil.copyfile(setup0_h, setup_h)

setup_file = open(setup_h, "rb").read()

for define in defines:
    setup_file, num_subs = re.subn("%s\s+\d" % (define), "%s\t%s" % (define, defines[define]), setup_file)
    print "# of subs made for pattern %s: %d" % (define, num_subs) 

output = open(setup_h, "wb")
output.write(setup_file)
output.close()