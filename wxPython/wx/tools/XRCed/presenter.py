# Name:         presenter.py
# Purpose:      Presenter part
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

from globals import *
from model import Model
import view
import os

# Presenter class linking model to view objects
class _Presenter:
    def init(self):
        self.setModified(False)

    def setModified(self, state=True):
        self.modified = state
        name = os.path.basename(Model.path)
        if not name: name = 'UNTITLED.xrc'
        # Update GUI
        if state:
            view.frame.SetTitle(progname + ': ' + name + ' *')
        else:
            view.frame.SetTitle(progname + ': ' + name)

# Singleton class
Presenter = _Presenter()
