#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test6.py
# Purpose:      Testing wxConfig
#
# Author:       Robin Dunn
#
# Created:      26-Nov-1998
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys
from wxPython.utils import wxConfig


#----------------------------------------------------------------------------

def main():

    cfg = wxConfig('test6', 'TCS')

    cmd = ''
    if len(sys.argv) > 1:
        cmd = sys.argv[1]

    if cmd == 'add':
        cfg.SetPath('one/two/three')
        cfg.Flush()

        cfg.Write('aaa', 'The quick brown fox jummped over the lazy dog.')
        cfg.Write('bbb', 'This is a test of the emergency broadcast system')

        aList = ['one', 'two', 'buckle', 'my', 'shoe', 1966]
        cfg.Write('ccc', str(aList))

        cfg.Write('zzz/yyy', 'foobar')
        cfg.Write('zzz/xxx', 'spam and eggs')

        cfg.Flush()

    elif cmd == 'enum':
        traverse(cfg, '/')

    elif cmd == 'del':
        cfg.DeleteAll()

    else:
        print 'Specify command:  add, enum, or del.'



def traverse(cfg, path):
    print path
    cont, val, idx = cfg.GetFirstEntry()
    while cont:
        print "%s/%s = %s" % (path, val, cfg.Read(val))
        cont, val, idx = cfg.GetNextEntry(idx)

    cont, val, idx = cfg.GetFirstGroup()
    while cont:
        if path == '/':
            newpath = path+val
        else:
            newpath = path+'/'+val

        cfg.SetPath(newpath)
        traverse(cfg, newpath)
        cfg.SetPath(path)
        cont, val, idx = cfg.GetNextGroup(idx)



if __name__ == '__main__':
    #import pdb
    #pdb.run('main()')
    main()


#----------------------------------------------------------------------------
