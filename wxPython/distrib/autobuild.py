

import sys, os, string, time
from ftplib import FTP

cwd = os.getcwd()

logfile = 'c:\\temp\\autobuild.log'
WXDIR   = os.environ['WXWIN']
dllVer  = '21_16'
wxpVer  = '2.1.16'
dateSt  = time.strftime("%Y%m%d", time.localtime(time.time()))

base = os.path.split(sys.argv[0])[0]
base = os.path.join(base, '..')
WXPYDIR = os.path.abspath(base)

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
##         logSeparator("Cleanup")
##         os.chdir(WXDIR + '/src/msw')
##         do('make cleandll FINAL=1')

##         logSeparator("Building Documentation...")
##         os.chdir(WXDIR + '/src/msw')
##         do('make touchmanual htmlhelp')
##         validateFile(WXDIR + '/docs/htmlhelp/wx.chm')

##         logSeparator("Building wxWindows and other libraries...")
##         os.chdir(WXDIR + '/src/msw')
##         do('make dll pch FINAL=1')
##         validateFile(WXDIR + '/lib/wx'+dllVer+'.dll')



##         logSeparator("Cleaning wxPython build directory...")
##         os.chdir(WXPYDIR)
##         do('buildall.bat -c')
        os.rename('build.local', 'build.local.save')
        f = open("build.local", "w")
        f.write("""
CRTFLAG='/MD'
FINAL=1
""")
        f.close()


        logSeparator("Building core wxPython...")
        os.chdir(WXPYDIR + '\\src')
        do("build -b")
        validateFile(WXPYDIR+'\\wxPython\\wxc.pyd')


        logSeparator("Building wxPython addon modules...")
        os.chdir(WXPYDIR+'\\contrib')
        do("buildall -b")
        validateFile(WXPYDIR+'\\wxPython\\glcanvasc.pyd')
        validateFile(WXPYDIR+'\\wxPython\\oglc.pyd')
        validateFile(WXPYDIR+'\\wxPython\\stc_c.pyd')


        os.chdir(WXPYDIR)
        os.unlink('build.local')
        os.rename('build.local.save', 'build.local')


        logSeparator("Building installer executable...")
        os.chdir(WXPYDIR+'\\distrib')
        do("autoit2 wise.aut")
        srcName =  WXPYDIR+'\\distrib\\wxPython-'+wxpVer+'.EXE'
        destName = WXPYDIR+'\\distrib\\wxPython-'+wxpVer+'-'+dateSt+'.EXE'
        validateFile(srcName)
        try:
            time.sleep(5)
            os.rename(srcName, destName)
            validateFile(destName)
        except:
            logSeparator("****** UNABLE TO RENAME FILE ******")


        logSeparator("Building source and docs zip files...")
        os.chdir(WXPYDIR)
        do("distrib\\zipit.bat %s" % wxpVer)
        srcZName =  WXPYDIR+'\\distrib\\wxPython-src-'+wxpVer+'.zip'
        destZName = WXPYDIR+'\\distrib\\wxPython-src-'+wxpVer+'-'+dateSt+'.zip'
        validateFile(srcZName)
        try:
            os.rename(srcZName, destZName)
        except:
            pass

        srcDName  = WXPYDIR+'\\distrib\\wxPython-docs-'+wxpVer+'.zip'
        destDName = WXPYDIR+'\\distrib\\wxPython-docs-'+wxpVer+'-'+dateSt+'.zip'
        validateFile(srcDName)
        try:
            os.rename(srcDName, destDName)
        except:
            pass



        # #*#*#*#*#*  Comment this out to allow upload...
        return

        logSeparator("Uploading to website...")
        do('python c:\\utils\\sendwxp.py %s' % destName)
        do('python c:\\utils\\sendwxp.py %s' % destZName)
        do('python c:\\utils\\sendwxp.py %s' % destDName)


        logSeparator("Finished!!!")

    finally:
        os.system("list " + logfile)
        pass






if __name__ == '__main__':
    main()
