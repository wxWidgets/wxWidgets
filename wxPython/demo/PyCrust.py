

from wxPython.lib.PyCrust import shell, version


#----------------------------------------------------------------------

intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % version.VERSION

def runTest(frame, nb, log):
    win = shell.Shell(nb, -1, introText=intro)
    return win

#----------------------------------------------------------------------

overview = shell.__doc__


if __name__ == '__main__':
    import run
    run.main(['', 'PyCrust'])

