"""
wx_create.py

Originally written by David Hughes  <dfh@forestfield.co.uk>
Massivly hacked by Robin Dunn

This automatically creates all the stub modules that are required in
the wx package in addition to __init__.py

The module names to make stubs for are found by scanning the wxPython
package directory.  The default directory searched is ../wxPython, but
you can specify a different one on the command-line if needed.

The content of each module (.py file) is taken from wxmodule_template
with appropriate substitution of the %name% tokens

"""

import os, sys, glob

wxmodule_template = """
\"\"\"Renamer stub: provides a way to drop the wx prefix from wxPython objects.\"\"\"

from wx import _rename
from wxPython%(prefix)s import %(suffix)s
_rename(globals(), %(suffix)s.__dict__, modulename='%(name)s')
del %(suffix)s
del _rename
"""

call_main = """
if __name__ == '__main__':
    main()
"""    

wxPython_dir = "../wxPython"

subpackage_list = ['.',
                   'lib', 'lib/mixins', 'lib/editor', 'lib/colourchooser',
                   'py',
                   'tools', 'tools/XRCed',
                   ]

skip_modules = [ '__init__', '__version__',
                 'wx', 'windows', 'windows2', 'windows3', 'events', 'fonts', 'misc',
                 'misc2', 'gdi', 'mdi', 'controls', 'controls2', 'cmndlgs',
                 'stattool', 'frames', 'image', 'printfw', 'sizers', 'clip_dnd',
                 'filesys', 'streams', 'htmlhelp', 'oglbasic',  'oglshapes',
                 'oglshapes2', 'oglcanvas', 'stc_', 'utils', 'dllwidget_',

                 'PyAlaModeTest',
                 ]


add_call_main = ['py/PyAlaCarte.py',  'py/PyAlaMode.py',  'py/PyCrust.py',
                 'py/PyFilling.py',  'py/PyShell.py',  'py/PyWrap.py'
                 ]



# Check for command-line arg
if len(sys.argv) > 1:
    wxPython_dir = sys.argv[1]

# check wxPython_dir
if not os.path.exists(wxPython_dir) or not os.path.isdir(wxPython_dir):
    print wxPython_dir, "does not exist or is not a directory!"
    sys.exit()

# check current location
if os.path.basename(os.getcwd()) <> 'wx':
    print 'This must be run from inside the target "wx" directory'
    sys.exit()


# build the modules and subpackages as needed
for subdir in subpackage_list:
    # create subdir if needed
    if not os.path.exists(subdir):
        os.mkdir(subdir)

    # create __init__.py if needed
    if os.path.isdir(subdir):
        fname = os.path.join(subdir, '__init__.py')
        if not os.path.exists(fname):
            f = open(fname, 'w')
            f.write("# Python package\n")
            f.close()
    else:
        print subdir + 'exists but is not a directory'
        sys.exit()

    # find the .py files there and make renamer stubs for them here
    src = os.path.join(wxPython_dir, subdir, "*.py")
    for srcname in glob.glob(src):
        suffix = os.path.splitext(os.path.basename(srcname))[0]
        if suffix in skip_modules:
            continue
        prefix = subdir.replace('/', '.')
        if prefix == '.':
            prefix = ''
            name = suffix
        else:
            name = prefix + '.' + suffix
            prefix = '.' + prefix

        fname = os.path.join(subdir, suffix+".py")

        content = wxmodule_template % globals()
        f = open(fname, 'w')
        f.write(content)
        if fname in add_call_main:
            f.write(call_main)            
        f.close()
        print fname + ' created'



sys.exit()




