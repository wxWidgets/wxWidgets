

from wxPython.lib.PyCrust import PyCrustShell, PyCrustEditor, PyCrustVersion


#----------------------------------------------------------------------

intro = 'Welcome To PyCrust %s - The Flakiest Python Shell' % PyCrustVersion.version

def runTest(frame, nb, log):
    shell = PyCrustShell.Shell(nb, intro)
    return shell.editor

#----------------------------------------------------------------------

overview = PyCrustShell.__doc__
