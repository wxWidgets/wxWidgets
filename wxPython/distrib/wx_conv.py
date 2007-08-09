"""
Convert modules from the old 'from wxPython.wx import *' style to
the new 'import wx' style.

Author: dfh@forestfield.co.uk (David Hughes)


This should probably go in the tools package and have a startup script
like the others, but I don't think it's ready for prime-time yet.  So
just put it in distrib for now so it doesn't get lost...

--Robin
"""


import os, sys, time
import string
t0 = time.time()


import wx
print 'length = ', len(wx._newnames)

for key in ['true', 'True', 'false', 'False']:      # use native Python not wx.<bool>
    if key in wx._newnames:
        del wx._newnames[key]

import wx.calendar, wx.gizmos, wx.glcanvas, wx.grid, wx.help, wx.html,   \
       wx.htmlhelp, wx.iewin, wx.ogl, wx.stc, wx.wizard, wx.xrc
import wx.lib
##import wx.lib.activexwrapper, wx.lib.analogclock, wx.lib.anchors,  \
import wx.lib.activexwrapper,  wx.lib.anchors,  \
       wx.lib.buttons, wx.lib.calendar, wx.lib.ClickableHtmlWindow,  \
       wx.lib.colourdb, wx.lib.colourselect, wx.lib.dialogs,   \
       wx.lib.ErrorDialogs, wx.lib.evtmgr, wx.lib.fancytext,   \
       wx.lib.filebrowsebutton, wx.lib.gridmovers, wx.lib.grids,  \
       wx.lib.imagebrowser, wx.lib.imageutils, wx.lib.infoframe,   \
       wx.lib.intctrl, wx.lib.layoutf, wx.lib.multisash,   \
       wx.lib.popupctl, wx.lib.printout, wx.lib.rcsizer,   \
       wx.lib.rightalign, wx.lib.sheet, wx.lib.stattext,   \
       wx.lib.throbber, wx.lib.timectrl, wx.lib.wxPlotCanvas,   \
       wx.lib.wxpTag
import wx.lib.mixins, wx.lib.editor, wx.lib.colourchooser
import wx.lib.mixins.grid, wx.lib.mixins.imagelist,    \
       wx.lib.mixins.listctrl
##       wx.lib.mixins.listctrl, wx.lib.mixins.rubberband
import wx.lib.editor.editor, wx.lib.editor.images, wx.lib.editor.selection
import wx.lib.colourchooser.canvas, wx.lib.colourchooser.intl, wx.lib.colourchooser.pycolourbox,   \
       wx.lib.colourchooser.pycolourchooser, wx.lib.colourchooser.pycolourslider, wx.lib.colourchooser.pypalette

wxlist = []
for kd in wx._newnames.items():
    wxlist.append(kd)
wxlist.sort()

n = 0
for item in wxlist:
    n += 1
##    print n, item
print 'length = ', len(wx._newnames)
print 'imports completed in ', time.time()-t0, 'secs'

base_path = 'G:/wxConvert/Test1'
exclude_dir = ( 'wx', 'wx_cc', 'reportlab', 'sqlite')       # MUST exclude wx if it is below base_dir, others optional

other_subs = { 'true': 'True',
               'false': 'False'
               }

punct = string.punctuation.replace('_','')                  # exclude single underscore
punct = punct.replace('.','')                               # and period
punct = punct.replace('*','')                               # and asterisk, all allowed in names
punctable = string.maketrans(punct, ' '*len(punct))         # map punctuation characters to spaces
numfiles = 0

#----------------------------------------------------------------------------

class AFile:
    " file object - collection of properties relating to current instance"
    def __init__(self, subsdict):
        self.subsdict = subsdict.copy()     # dictionary of universal and local substitutions to make
        self.importing = []                 # list of wx modules being imported

#----------------------------------------------------------------------------

def visit(noargs, thispath, contentlist):
    """ Function is called by os walk for every directory in base_path,
        including base_path itself. Contentlist is a list of files/dirs in thispath.
        Wx conversion function is called for every qualifying file in list
    """
    path = thispath
    base = 'something'
    while base:                             # check if thispath or a parent is excluded
        path, base = os.path.split(path)
        if base in exclude_dir:
            print 'Excluded:', thispath
            return

    for item in contentlist:
        pathname = os.path.join(thispath, item)
        if os.path.isfile(pathname) and pathname[-3:].lower() == '.py':
            wxconvert(pathname)

def wxconvert(pathname):
    """ Scan each line of text in pathname. Replace each occurrence of any key in wx._newnames
        dictionary with the content stored under that key
    """
    global numfiles
    afile = AFile(other_subs)
    infile = open(pathname, 'r')
    linelist = infile.readlines()
    infile.close()

    lnum = 0
    outlist = []
    for line in linelist:
        lnum += 1
        tokenlist = line.replace('.__', ' ').translate(punctable).split()    # split on white space and punctuation
        line, status = checkimports(afile, line, tokenlist)                  # return line with 'import' modifications
        if not status:
            print 'Unable to convert line %d in %s' % (lnum, pathname)
            break
        else:
            for key in afile.subsdict:                    # do other changes first
                if line.find(key) >= 0:
                    line = line.replace(key, afile.subsdict[key])
            for token in tokenlist:                     # change wx names
                if token in wx._newnames:
                    candidate = wx._newnames[token]
                    module = candidate[:candidate.rfind('.')]
                    if module in afile.importing:
                        line = line.replace(token, candidate)
        outlist.append(line)
    else:
        outfile = open(pathname, 'w')
        outfile.writelines(outlist)
        numfiles += 1
        print 'Converted:', pathname
        outfile.close()

def checkimports(afile, line, tlist):
    """ Scan tokenlist for wxPython import statement. Add to afile.subsdict any
        name changes that are necessary for the rest of the current source file.
        Add to afile.importing any new import modules
        Return a tuple (status, newstring) -
           line, possibly modified if an import statmeny
           status: 0: unable to handle complexity, 1: OK
    """
    aline = line
    if len(tlist) == 0: return (aline, 1)
    indent = ''
    for ch in line:
        if ch in string.whitespace:
            indent += ' '
        else:
            break

    if tlist[0] == 'import':        # import module [as name] [module [as name]...]
        skip = False
        for t in tlist[1:]:
            if skip:
                skip = False
            elif t == 'as':
                skip = True
            elif t.startswith('wx'):
                aline = aline.replace(t, rename_module(afile, t))
    elif (tlist[0] == 'from' and
            tlist[1] == 'wxPython' and
              tlist[2] == 'import'):                # from wxPython import module
        if len(tlist) > 4:                          # ...[as name] [module [as name]...]
            return ('', 0)                          # too complicated
        module = rename_module(afile,tlist[-1])
        aline = indent = 'import ' + module + '\n'
    elif (tlist[0] == 'from' and
            tlist[1].startswith('wxPython') and
              tlist[2] == 'import'):                # from module import ....
        if tlist[-1] <> '*':                        # ...name [as name] [name [as name]...]
            aline = aline.replace(tlist[1], rename_module(afile, tlist[1]))
            skip = False
            for t in tlist[3:]:
                if skip:
                    skip = False
                elif t == 'as':
                    skip = True
                else:
                    elem = t.split('.')
                    if elem[-1].startswith('wx'):           # remove wx prefix from last element of name
                        elem[-1] = elem[-1][2:]
                        afile.subsdict[t] = '.'.join(elem)  # and apply to each occurrence in source
        else:                       # from module import *
            module = rename_module(afile,tlist[1])
            aline = indent = 'import ' + module + '\n'
    return (aline, 1)

def rename_module(afile, t, type='A'):
    """ Substitute  wx for wxPython.wx or wx, and wx.foo.bar for wxPython.foo.bar in token
        foo.bar => wx.foo.bar is also permitted (from wxPython import foo.bar)
    """
    if t in ['wx', 'wxPython.wx']:
        module = 'wx'
    elif t.startswith('wxPython'):
        module = t.replace('wxPython', 'wx')
    elif t.startswith('wx'):
        module = t
    else:
        module = 'wx.' + t
    if module not in afile.importing:
        afile.importing.append(module)
    return module

def main():
    " Convert every file in base_dir and all subdirectories except in exclude_dir list"

    os.path.walk(base_path, visit, None)
    print '%d files converted in %.2f seconds' % (numfiles, time.time() - t0)

#----------------------------------------------------------------------------

if __name__ == '__main__':
    main()
