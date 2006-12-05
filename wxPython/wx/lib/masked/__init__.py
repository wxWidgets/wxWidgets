#----------------------------------------------------------------------
# Name:        wxPython.lib.masked
# Purpose:     A package containing the masked edit controls
#
# Author:      Will Sadkin, Jeff Childers
#
# Created:     6-Mar-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004
# License:     wxWidgets license
#----------------------------------------------------------------------
__author__  = "Will Sadkin <wsadkin |at| parlancecorp.com>"
__date__    = "02 Dec 2006, 19:00 GMT-05:00"
__version__ = "1.11"
__doc__     = """\
package providing "masked edit" controls, allowing characters within a data entry control to remain fixed, and providing fine-grain control over allowed user input.
"""

# import relevant external symbols into package namespace:
from maskededit import *
from textctrl   import BaseMaskedTextCtrl, PreMaskedTextCtrl, TextCtrl
from combobox   import BaseMaskedComboBox, PreMaskedComboBox, ComboBox, MaskedComboBoxSelectEvent
from numctrl    import NumCtrl, wxEVT_COMMAND_MASKED_NUMBER_UPDATED, EVT_NUM, NumberUpdatedEvent
from timectrl   import TimeCtrl, wxEVT_TIMEVAL_UPDATED, EVT_TIMEUPDATE, TimeUpdatedEvent
from ipaddrctrl import IpAddrCtrl
from ctrl       import Ctrl, controlTypes
