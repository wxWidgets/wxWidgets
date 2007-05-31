# Name:         plugin.py
# Purpose:      Pluggable component support
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import os,sys

def load_plugins(dir):
    sys_path = sys.path
    cwd = os.getcwd()
    dir = os.path.abspath(os.path.normpath(dir))
    os.chdir(dir)
    sys.path = sys_path + [dir]
    try:
        for f in os.listdir('.'):
            name,ext = os.path.splitext(f)
            if os.path.isfile(f) and ext == '.py':
                __import__(name, globals(), locals(), ['*'])
            elif os.path.isdir(f):
                if os.path.isfile(os.path.join(f, '__init__.py')):
                    __import__(f, globals(), locals(), ['*'])
    finally:
        sys.path = sys_path
        os.chdir(cwd)

def load_all_plugins():
    pluginPath = os.getenv('XRCEDPATH')
    if pluginPath:
        for dir in plugins.split(':'):
            if os.path.isdir(dir):
                import_plugins(dir)

