

import sys, os, string, time
from ftplib import FTP


logfile = 'e:\\temp\\autobuild.log'
WXDIR   = os.environ['WXWIN']
dllVer  = '21_11'
wxpVer  = '2.1.11'
dateSt  = time.strftime("%Y%m%d", time.localtime(time.time()))

#----------------------------------------------------------------------

def do(cmd):
    st = "  " + cmd + " >> " + logfile
    print st
    f = open(logfile, "at")
    f.write(st + '\n')
    f.close()
    os.system(cmd + " >>& " + logfile)

#----------------------------------------------------------------------

def logTruncate():
    f = open(logfile, "wt")
    f.close()


def logSeparator(msg=None, f=None, recurse=1):
    if not f:
        f = open(logfile, "at")
    f.write('\n')
    f.write('--' * 35)
    f.write('\n')
    if msg:
        f.write(msg)
        f.write('\n')
        f.write('--' * 35)
        f.write('\n')
        if recurse:
            logSeparator(msg, sys.stdout, 0)

#----------------------------------------------------------------------

def validateFile(file):
    if not os.path.exists(file):
        logSeparator("*****  %s does not exist, exiting! *****" % file)
        raise SystemExit
    else:
        logSeparator("%s found, continuing..." % file, recurse=0)


#----------------------------------------------------------------------

def main():
    logTruncate()

    try:
        logSeparator("Cleanup")
        os.chdir(WXDIR + '/src/msw')
        do('make cleandll FINAL=1')
        os.chdir(WXDIR + '/utils/ogl/src')
        do('wxm clean FINAL=1')
        os.chdir(WXDIR + '/utils/glcanvas/win')
        do('wxm clean FINAL=1')

        logSeparator("Building Documentation...")
        os.chdir(WXDIR + '/src/msw')
        do('make touchmanual htmlhelp')
        validateFile(WXDIR + '/docs/html/wx/wx.chm')

        logSeparator("Building wxWindows and libraries...")
        os.chdir(WXDIR + '/src/msw')
        do('make dll pch FINAL=1')
        validateFile(WXDIR + '/lib/wx'+dllVer+'.dll')

        os.chdir(WXDIR + '/utils/ogl/src')
        do('wxm FINAL=1')
        os.chdir(WXDIR + '/utils/glcanvas/win')
        do('wxm FINAL=1')
        validateFile(WXDIR + '/lib/ogl.lib')
        validateFile(WXDIR + '/lib/glcanvas.lib')


        logSeparator("Copying wxPython workspace to build directory...")
        do("copy /s %s %s" % ('e:\\projects\\wxPython\*.*', WXDIR+'\\utils\\wxPython'))
        os.chdir(WXDIR + '/utils/wxPython')
        f = open("build.local", "w")
        f.write("""
TARGETDIR = 'e:\\projects\\wx\\utils\\wxPython'
WXPSRCDIR = 'e:\\projects\\wx\\utils\\wxPython\\src'
CRTFLAG='/MD'
FINAL=1
""")
        f.close()


        logSeparator("Cleaning wxPython...")
        os.chdir(WXDIR+'\\utils\\wxPython')
        do("buildall -cu")
        do("ll")


        logSeparator("Building core wxPython...")
        os.chdir(WXDIR+'\\utils\\wxPython\\src')
        do("build -bi")
        validateFile(WXDIR+'\\utils\\wxPython\\wxc.pyd')



        logSeparator("Building wxPython addon modules...")
        os.chdir(WXDIR+'\\utils\\wxPython\\modules')
        do("buildall -bi")
        validateFile(WXDIR+'\\utils\\wxPython\\utilsc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\htmlc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\glcanvasc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\oglc.pyd')


        logSeparator("Building installer executable...")
        os.chdir(WXDIR+'\\utils\\wxPython\\distrib')
        do("autoit2 wise.aut")
        srcName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-'+wxpVer+'.EXE'
        destName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-'+wxpVer+'-'+dateSt+'.EXE'
        validateFile(srcName)
        try:
            os.rename(srcName, destName)
        except:
            pass


        logSeparator("Building source zip file...")
        os.chdir(WXDIR+'\\utils')
        do("wxPython\\distrib\\zipit.bat %s" % wxpVer)
        srcZName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-src-'+wxpVer+'.zip'
        destZName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-src-'+wxpVer+'-'+dateSt+'.zip'
        validateFile(srcZName)
        try:
            os.rename(srcZName, destZName)
        except:
            pass


        logSeparator("Uploading to website...")
        do('python d:\util32\sendwxp.py %s' % destName)
        do('python d:\util32\sendwxp.py %s' % destZName)
        os.unlink(destName)
        os.unlink(destZName)


        logSeparator("Finished!!!")

    finally:
        os.system("list " + logfile)
        pass






if __name__ == '__main__':
    main()
