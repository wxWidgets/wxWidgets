###############################################################################
# Name:         misc/gdb/print.py
# Purpose:      pretty-printers for wx data structures: this file is meant to
#               be sourced from gdb using "source -p" (or, better, autoloaded
#               in the future...)
# Author:       Vadim Zeitlin
# Created:      2009-01-04
# Copyright:    (c) 2009 Vadim Zeitlin
# Licence:      wxWindows licence
###############################################################################

# Define wxFooPrinter class implementing (at least) to_string() method for each
# wxFoo class we want to pretty print. Then just add wxFoo to the types array
# in wxLookupFunction at the bottom of this file.

import datetime

# shamelessly stolen from std::string example
class wxStringPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return self.val['m_impl']['_M_dataplus']['_M_p']

    def display_hint(self):
        return 'string'

class wxDateTimePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        # A value of type wxLongLong can't be used in Python arithmetic
        # expressions directly so we need to convert it to long long first and
        # then cast to int explicitly to be able to use it as a timestamp.
        msec = self.val['m_time'].cast(gdb.lookup_type('long long'))
        if msec == 0x8000000000000000:
            return 'NONE'
        sec = int(msec / 1000)
        return datetime.datetime.fromtimestamp(sec).isoformat(' ')

class wxFileNamePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        # It is simpler to just call the internal function here than to iterate
        # over m_dirs array ourselves. The disadvantage of this approach is
        # that it requires a live inferior process and so doesn't work when
        # debugging using only a core file. If this ever becomes a serious
        # problem, this should be rewritten to use m_dirs and m_name and m_ext.
        return gdb.parse_and_eval('((wxFileName*)%s)->GetFullPath(0)' %
                                  self.val.address)

class wxXYPrinterBase:
    def __init__(self, val):
        self.x = val['x']
        self.y = val['y']

class wxPointPrinter(wxXYPrinterBase):
    def to_string(self):
        return '(%d, %d)' % (self.x, self.y)

class wxSizePrinter(wxXYPrinterBase):
    def to_string(self):
        return '%d*%d' % (self.x, self.y)

class wxRectPrinter(wxXYPrinterBase):
    def __init__(self, val):
        wxXYPrinterBase.__init__(self, val)
        self.width = val['width']
        self.height = val['height']

    def to_string(self):
        return '(%d, %d) %d*%d' % (self.x, self.y, self.width, self.height)


# The function looking up the pretty-printer to use for the given value.
def wxLookupFunction(val):
    # Using a list is probably ok for so few items but consider switching to a
    # set (or a dict and cache class types as the keys in it?) if needed later.
    types = ['wxString',
             'wxDateTime',
             'wxFileName',
             'wxPoint',
             'wxSize',
             'wxRect']

    for t in types:
        if val.type.tag == t:
            # Not sure if this is the best name to create the object of a class
            # by name but at least it beats eval()
            return globals()[t + 'Printer'](val)

    return None

gdb.pretty_printers.append(wxLookupFunction)
