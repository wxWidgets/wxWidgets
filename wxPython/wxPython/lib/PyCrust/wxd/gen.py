
import inspect
from wxPython import wx


def scan():
    d = wx.__dict__
    newd = {}
    keys = d.keys()
    keys.sort()
    for key in keys:
        if key.endswith('Ptr'):
            # Skip
            pass
        elif key+'Ptr' in keys:
            # Rename
            newd[key] = d[key+'Ptr']
        else:
            # Include as is
            newd[key] = d[key]
    d = newd
    keys = d.keys()
    keys.sort()
    for key in keys:
        value = d[key]
        if inspect.isclass(value):
            # genClass(value)
            pass
        elif callable(value):
            genFunction(value)
            pass
        else:
            # print type(value), value            
            pass


def genClass(cls):
    sp4 = ' ' * 4
    name = cls.__name__
    if name.endswith('Ptr'):
        name = name[:-3]
##     if name != 'wxNotebook':
##         return
    parent = ''
    if cls.__bases__:
        parent = cls.__bases__[0].__name__
        if parent.endswith('Ptr'):
            parent = parent[:-3]
        parent = '(%s)' % parent
    items = cls.__dict__.keys()
    items.sort()
    print
    print 'class %s%s:' % (name, parent)
    print sp4 + '""""""'
    print
    for item in items:
        attr = cls.__dict__[item]
        if inspect.isfunction(attr):
            print sp4 + 'def ' + item + '(self):'
            print sp4 + sp4 + '""""""'
            print sp4 + sp4 + 'pass'
            print


def genFunction(func):
    sp4 = ' ' * 4
    name = func.__name__
    print 'def %s():' % name
    print sp4 + '""""""'
    print sp4 + 'pass'
    print
