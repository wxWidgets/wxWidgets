import sys, os, glob
from distutils.dep_util import newer
from distutils.spawn import spawn

poFiles = glob.glob("*.po")

for po in poFiles:
   mo = os.path.splitext(po)[0] + '.mo'
   if mo != 'wxstd.mo':
       if newer(po, mo):
           cmd = 'msgfmt --verbose -o %s %s' % (mo, po)
           print cmd
           spawn(cmd.split())
           print
       else:
           print "%s is up to date" % mo
