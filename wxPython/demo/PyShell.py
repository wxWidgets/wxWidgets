
import  wx.py   as  py

#----------------------------------------------------------------------

intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % py.version.VERSION

def runTest(frame, nb, log):
    win = py.shell.Shell(nb, -1, introText=intro)
    return win

#----------------------------------------------------------------------

overview = py.shell.__doc__


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

