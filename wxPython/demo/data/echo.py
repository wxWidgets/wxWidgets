

"""
This is a simple little echo program that is used by the wxProcess
demo.  It reads lines from stdin and echos them back to stdout, until
there is an EOF on stdin.

Enter text in the field below to send to the stdin of the echo
process.  Clicking on 'Close Stream' will close the stream in the
demo, and then echo.py should terminate too...
"""

import sys

sys.stdout.write( __doc__)
sys.stdout.flush()

line = sys.stdin.readline()
while line:
    line = line[:-1]
    sys.stdout.write('\nYou typed "%s"\n' % line)
    sys.stdout.flush()
    line = sys.stdin.readline()


sys.stdout.write('\nExiting...\n')
sys.stdout.flush()
