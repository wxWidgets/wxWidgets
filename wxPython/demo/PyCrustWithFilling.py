
from wxPython.wx import wxSplitterWindow
from wxPython.lib.PyCrust import shell, version, filling


#----------------------------------------------------------------------

intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % version.VERSION

def runTest(frame, nb, log):
    win = wxSplitterWindow(nb, -1, size=(640, 480))
    shellWin = shell.Shell(win, -1, introText=intro)
    fillingWin = filling.Filling(win, -1, size=(640, 480),
                                 rootObject=shellWin.interp.locals,
                                 rootIsNamespace=1
                                 )
    win.SplitHorizontally(shellWin, fillingWin)
    return win

#----------------------------------------------------------------------

overview = filling.__doc__

if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

