#-----------------------------------------------------------------------------
# Name:        STCStyleEditor.py
# Purpose:     Style editor for the wxStyledTextCtrl
#
# Author:      Riaan Booysen
#
# Created:     2001/08/20
# RCS-ID:      $Id$
# Copyright:   (c) 2001 Riaan Booysen
# Licence:     wxWindows license
#-----------------------------------------------------------------------------
#Boa:Dialog:STCStyleEditDlg

""" Style editor for the wxStyledTextCtrl.

Reads in property style definitions from a config file.
Modified styled can be saved (and optionally applied to a given list of STCs)

It can also maintain a Common definition dictionary of font names, colours and
sizes which can be shared across multiple language style definitions.
This is also used to store platform spesific settings as fonts and sizes
vary with platform.

The following items are defined in the stc-styles.rc.cfg file.

common.defs.msw - Common definition dictionary used on wxMSW
common.defs.gtk - Common definition dictionary used on wxGTK
common.styleidnames - STC styles shared by all languages

Each supported language defines the following groups:
[<language>]
displaysrc - Example source to display in the editor
braces - Dictionary defining the (line, column) for showing 'good' and 'bad'
         brace matching (both keys optional)
keywords - Space separated list of keywords
lexer - wxSTC constant for the language lexer
styleidnames - Dictionary of language spesific style numbers and names

[style.<language>] - The users current style values
[style.<language>.default] - Default style values (can be reverted from)

0 or more predefined style groups or 'themes'
[style.<language>.<predefined name>]

Currently the following languages are supported:
    python, html, xml, cpp, text, props
Other languages can be added by just defining the above settings for them in
the config file (if wxSTC implements them).

Use the initSTC function to initialise your wxSTC from a config file.
"""

import os, sys, string, pprint, copy

from wxPython.wx import *
from wxPython.lib.anchors import LayoutAnchors
from wxPython.stc import *

settingsIdNames = {-1: 'Selection', -2: 'Caret', -3: 'Edge'}

commonPropDefs = {'fore': '#888888', 'size': 8,
  'face': wxSystemSettings_GetSystemFont(wxSYS_DEFAULT_GUI_FONT).GetFaceName()}

styleCategoryDescriptions = {
'-----Language-----': 'Styles spesific to the language',
'-----Standard-----': 'Styles shared by all languages',
'-----Settings-----': 'Properties set by STC methods',
'-----Common-----': 'User definable values that can be shared between languages'}

[wxID_STCSTYLEEDITDLG, wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN, wxID_STCSTYLEEDITDLGBGCOLBTN, wxID_STCSTYLEEDITDLGBGCOLCB, wxID_STCSTYLEEDITDLGBGCOLDEFCB, wxID_STCSTYLEEDITDLGCANCELBTN, wxID_STCSTYLEEDITDLGCOMMONDEFSBTN, wxID_STCSTYLEEDITDLGELEMENTLB, wxID_STCSTYLEEDITDLGFACECB, wxID_STCSTYLEEDITDLGFACEDEFCB, wxID_STCSTYLEEDITDLGFGCOLBTN, wxID_STCSTYLEEDITDLGFGCOLCB, wxID_STCSTYLEEDITDLGFGCOLDEFCB, wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK, wxID_STCSTYLEEDITDLGOKBTN, wxID_STCSTYLEEDITDLGPANEL1, wxID_STCSTYLEEDITDLGPANEL2, wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN, wxID_STCSTYLEEDITDLGSIZECB, wxID_STCSTYLEEDITDLGSPEEDSETTINGCH, wxID_STCSTYLEEDITDLGSTATICBOX1, wxID_STCSTYLEEDITDLGSTATICBOX2, wxID_STCSTYLEEDITDLGSTATICLINE1, wxID_STCSTYLEEDITDLGSTATICTEXT2, wxID_STCSTYLEEDITDLGSTATICTEXT3, wxID_STCSTYLEEDITDLGSTATICTEXT4, wxID_STCSTYLEEDITDLGSTATICTEXT6, wxID_STCSTYLEEDITDLGSTATICTEXT7, wxID_STCSTYLEEDITDLGSTATICTEXT8, wxID_STCSTYLEEDITDLGSTATICTEXT9, wxID_STCSTYLEEDITDLGSTC, wxID_STCSTYLEEDITDLGSTYLEDEFST, wxID_STCSTYLEEDITDLGTABOLDCB, wxID_STCSTYLEEDITDLGTABOLDDEFCB, wxID_STCSTYLEEDITDLGTAEOLFILLEDCB, wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB, wxID_STCSTYLEEDITDLGTAITALICCB, wxID_STCSTYLEEDITDLGTAITALICDEFCB, wxID_STCSTYLEEDITDLGTASIZEDEFCB, wxID_STCSTYLEEDITDLGTAUNDERLINEDCB, wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB] = map(lambda _init_ctrls: wxNewId(), range(41))

class STCStyleEditDlg(wxDialog):
    """ Style editor for the wxStyledTextCtrl """
    _custom_classes = {'wxWindow' : ['wxStyledTextCtrl']}
    def _init_utils(self):
        pass

    def _init_ctrls(self, prnt):
        wxDialog.__init__(self, id = wxID_STCSTYLEEDITDLG, name = 'STCStyleEditDlg', parent = prnt, pos = wxPoint(416, 307), size = wxSize(425, 481), style = wxWANTS_CHARS | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, title = self.stc_title)
        self._init_utils()
        self.SetClientSize(wxSize(417, 454))
        self.SetAutoLayout(true)
        self.SetSizeHints(425, 400, -1, -1)
        EVT_SIZE(self, self.OnStcstyleeditdlgSize)

        self.staticBox2 = wxStaticBox(id = wxID_STCSTYLEEDITDLGSTATICBOX2, label = 'Text attributes', name = 'staticBox2', parent = self, pos = wxPoint(296, 56), size = wxSize(112, 99), style = 0)
        self.staticBox2.SetConstraints(LayoutAnchors(self.staticBox2, false, true, true, false))

        self.staticBox1 = wxStaticBox(id = wxID_STCSTYLEEDITDLGSTATICBOX1, label = 'Colour', name = 'staticBox1', parent = self, pos = wxPoint(157, 56), size = wxSize(128, 99), style = 0)
        self.staticBox1.SetConstraints(LayoutAnchors(self.staticBox1, false, true, true, false))

        self.elementLb = wxListBox(choices = [], id = wxID_STCSTYLEEDITDLGELEMENTLB, name = 'elementLb', parent = self, pos = wxPoint(8, 72), size = wxSize(144, 112), style = 0, validator = wxDefaultValidator)
        self.elementLb.SetConstraints(LayoutAnchors(self.elementLb, true, true, true, false))
        EVT_LISTBOX(self.elementLb, wxID_STCSTYLEEDITDLGELEMENTLB, self.OnElementlbListbox)

        self.styleDefST = wxStaticText(id = wxID_STCSTYLEEDITDLGSTYLEDEFST, label = '(nothing selected)', name = 'styleDefST', parent = self, pos = wxPoint(56, 8), size = wxSize(352, 16), style = wxST_NO_AUTORESIZE)
        self.styleDefST.SetFont(wxFont(8, wxSWISS, wxNORMAL, wxBOLD, false, self.sys_font))
        self.styleDefST.SetConstraints(LayoutAnchors(self.styleDefST, true, true, true, false))

        self.taEOLfilledCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAEOLFILLEDCB, label = 'EOL filled', name = 'taEOLfilledCb', parent = self.staticBox2, pos = wxPoint(8, 75), size = wxSize(72, 16), style = 0)
        self.taEOLfilledCb.SetValue(false)
        EVT_CHECKBOX(self.taEOLfilledCb, wxID_STCSTYLEEDITDLGTAEOLFILLEDCB, self.OnTaeoffilledcbCheckbox)

        self.staticText2 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT2, label = 'default', name = 'staticText2', parent = self.staticBox2, pos = wxPoint(72, 11), size = wxSize(32, 16), style = 0)

        self.taItalicCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAITALICCB, label = 'Italic', name = 'taItalicCb', parent = self.staticBox2, pos = wxPoint(8, 43), size = wxSize(72, 16), style = 0)
        EVT_CHECKBOX(self.taItalicCb, wxID_STCSTYLEEDITDLGTAITALICCB, self.OnTaitaliccbCheckbox)

        self.taUnderlinedDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB, label = 'checkBox1', name = 'taUnderlinedDefCb', parent = self.staticBox2, pos = wxPoint(88, 59), size = wxSize(16, 16), style = 0)

        self.taBoldDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTABOLDDEFCB, label = 'checkBox1', name = 'taBoldDefCb', parent = self.staticBox2, pos = wxPoint(88, 27), size = wxSize(16, 16), style = 0)

        self.taItalicDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAITALICDEFCB, label = 'checkBox1', name = 'taItalicDefCb', parent = self.staticBox2, pos = wxPoint(88, 43), size = wxSize(16, 16), style = 0)

        self.taBoldCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTABOLDCB, label = 'Bold', name = 'taBoldCb', parent = self.staticBox2, pos = wxPoint(8, 27), size = wxSize(72, 16), style = 0)
        EVT_CHECKBOX(self.taBoldCb, wxID_STCSTYLEEDITDLGTABOLDCB, self.OnTaboldcbCheckbox)

        self.taEOLfilledDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB, label = 'checkBox1', name = 'taEOLfilledDefCb', parent = self.staticBox2, pos = wxPoint(88, 75), size = wxSize(16, 16), style = 0)

        self.taUnderlinedCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTAUNDERLINEDCB, label = 'Underlined', name = 'taUnderlinedCb', parent = self.staticBox2, pos = wxPoint(8, 59), size = wxSize(72, 16), style = 0)
        EVT_CHECKBOX(self.taUnderlinedCb, wxID_STCSTYLEEDITDLGTAUNDERLINEDCB, self.OnTaunderlinedcbCheckbox)

        self.fgColDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGFGCOLDEFCB, label = 'checkBox1', name = 'fgColDefCb', parent = self.staticBox1, pos = wxPoint(104, 31), size = wxSize(16, 16), style = 0)

        self.staticText3 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT3, label = 'default', name = 'staticText3', parent = self.staticBox1, pos = wxPoint(88, 16), size = wxSize(32, 16), style = 0)

        self.bgColDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGBGCOLDEFCB, label = 'checkBox1', name = 'bgColDefCb', parent = self.staticBox1, pos = wxPoint(104, 71), size = wxSize(16, 16), style = 0)

        self.fgColBtn = wxButton(id = wxID_STCSTYLEEDITDLGFGCOLBTN, label = 'Foreground', name = 'fgColBtn', parent = self.staticBox1, pos = wxPoint(8, 16), size = wxSize(72, 16), style = 0)
        EVT_BUTTON(self.fgColBtn, wxID_STCSTYLEEDITDLGFGCOLBTN, self.OnFgcolbtnButton)

        self.bgColBtn = wxButton(id = wxID_STCSTYLEEDITDLGBGCOLBTN, label = 'Background', name = 'bgColBtn', parent = self.staticBox1, pos = wxPoint(8, 56), size = wxSize(72, 16), style = 0)
        EVT_BUTTON(self.bgColBtn, wxID_STCSTYLEEDITDLGBGCOLBTN, self.OnBgcolbtnButton)

        self.staticLine1 = wxStaticLine(id = wxID_STCSTYLEEDITDLGSTATICLINE1, name = 'staticLine1', parent = self, pos = wxPoint(36, 62), size = wxSize(115, 2), style = wxLI_HORIZONTAL)
        self.staticLine1.SetConstraints(LayoutAnchors(self.staticLine1, true, true, true, false))

        self.staticText6 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT6, label = 'Style', name = 'staticText6', parent = self, pos = wxPoint(8, 56), size = wxSize(23, 13), style = 0)

        self.okBtn = wxButton(id = wxID_STCSTYLEEDITDLGOKBTN, label = 'OK', name = 'okBtn', parent = self, pos = wxPoint(248, 422), size = wxSize(75, 23), style = 0)
        self.okBtn.SetConstraints(LayoutAnchors(self.okBtn, false, false, true, true))
        self.okBtn.SetToolTipString('Save changes to the config file')
        EVT_BUTTON(self.okBtn, wxID_STCSTYLEEDITDLGOKBTN, self.OnOkbtnButton)

        self.cancelBtn = wxButton(id = wxID_STCSTYLEEDITDLGCANCELBTN, label = 'Cancel', name = 'cancelBtn', parent = self, pos = wxPoint(332, 422), size = wxSize(75, 23), style = 0)
        self.cancelBtn.SetConstraints(LayoutAnchors(self.cancelBtn, false, false, true, true))
        self.cancelBtn.SetToolTipString('Close dialog without saving changes')
        EVT_BUTTON(self.cancelBtn, wxID_STCSTYLEEDITDLGCANCELBTN, self.OnCancelbtnButton)

        self.commonDefsBtn = wxButton(id = wxID_STCSTYLEEDITDLGCOMMONDEFSBTN, label = 'Common definitions', name = 'commonDefsBtn', parent = self, pos = wxPoint(8, 422), size = wxSize(104, 23), style = 0)
        self.commonDefsBtn.SetConstraints(LayoutAnchors(self.commonDefsBtn, true, false, false, true))
        self.commonDefsBtn.SetToolTipString('Directly edit the common definitions dictionary')
        self.commonDefsBtn.Show(false)
        EVT_BUTTON(self.commonDefsBtn, wxID_STCSTYLEEDITDLGCOMMONDEFSBTN, self.OnCommondefsbtnButton)

        self.staticText8 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT8, label = 'Style def:', name = 'staticText8', parent = self, pos = wxPoint(8, 8), size = wxSize(44, 13), style = 0)

        self.staticText9 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT9, label = 'SpeedSetting:', name = 'staticText9', parent = self, pos = wxPoint(8, 32), size = wxSize(67, 13), style = 0)

        self.speedsettingCh = wxChoice(choices = [], id = wxID_STCSTYLEEDITDLGSPEEDSETTINGCH, name = 'speedsettingCh', parent = self, pos = wxPoint(88, 28), size = wxSize(320, 21), style = 0, validator = wxDefaultValidator)
        self.speedsettingCh.SetConstraints(LayoutAnchors(self.speedsettingCh, true, true, true, false))
        EVT_CHOICE(self.speedsettingCh, wxID_STCSTYLEEDITDLGSPEEDSETTINGCH, self.OnSpeedsettingchChoice)

        self.stc = wxStyledTextCtrl(id = wxID_STCSTYLEEDITDLGSTC, name = 'stc', parent = self, pos = wxPoint(8, 208), size = wxSize(401, 206), style = wxSUNKEN_BORDER)
        self.stc.SetConstraints(LayoutAnchors(self.stc, true, true, true, true))
        EVT_LEFT_UP(self.stc, self.OnUpdateUI)
        EVT_KEY_UP(self.stc, self.OnUpdateUI)

        self.panel1 = wxPanel(id = wxID_STCSTYLEEDITDLGPANEL1, name = 'panel1', parent = self, pos = wxPoint(157, 161), size = wxSize(128, 40), style = wxTAB_TRAVERSAL)
        self.panel1.SetConstraints(LayoutAnchors(self.panel1, false, true, true, false))

        self.staticText4 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT4, label = 'Face:', name = 'staticText4', parent = self.panel1, pos = wxPoint(0, 0), size = wxSize(27, 13), style = 0)

        self.faceDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGFACEDEFCB, label = 'checkBox1', name = 'faceDefCb', parent = self.panel1, pos = wxPoint(104, 0), size = wxSize(16, 16), style = 0)

        self.fixedWidthChk = wxCheckBox(id = wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK, label = '', name = 'fixedWidthChk', parent = self.panel1, pos = wxPoint(0, 23), size = wxSize(13, 19), style = 0)
        self.fixedWidthChk.SetValue(false)
        self.fixedWidthChk.SetToolTipString('Check this for Fixed Width fonts')
        EVT_CHECKBOX(self.fixedWidthChk, wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK, self.OnFixedwidthchkCheckbox)

        self.addCommonItemBtn = wxButton(id = wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN, label = 'Add', name = 'addCommonItemBtn', parent = self, pos = wxPoint(8, 184), size = wxSize(72, 16), style = 0)
        self.addCommonItemBtn.SetToolTipString('Add new Common definition')
        EVT_BUTTON(self.addCommonItemBtn, wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN, self.OnAddsharebtnButton)

        self.removeCommonItemBtn = wxButton(id = wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN, label = 'Remove', name = 'removeCommonItemBtn', parent = self, pos = wxPoint(80, 184), size = wxSize(72, 16), style = 0)
        self.removeCommonItemBtn.Enable(false)
        self.removeCommonItemBtn.SetToolTipString('Remove the selected Common definition')
        EVT_BUTTON(self.removeCommonItemBtn, wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN, self.OnRemovesharebtnButton)

        self.panel2 = wxPanel(id = wxID_STCSTYLEEDITDLGPANEL2, name = 'panel2', parent = self, pos = wxPoint(296, 162), size = wxSize(112, 40), style = wxTAB_TRAVERSAL)
        self.panel2.SetConstraints(LayoutAnchors(self.panel2, false, true, true, false))

        self.staticText7 = wxStaticText(id = wxID_STCSTYLEEDITDLGSTATICTEXT7, label = 'Size:', name = 'staticText7', parent = self.panel2, pos = wxPoint(0, 0), size = wxSize(23, 13), style = 0)

        self.taSizeDefCb = wxCheckBox(id = wxID_STCSTYLEEDITDLGTASIZEDEFCB, label = 'checkBox1', name = 'taSizeDefCb', parent = self.panel2, pos = wxPoint(88, 0), size = wxSize(16, 16), style = 0)

        self.sizeCb = wxComboBox(choices = [], id = wxID_STCSTYLEEDITDLGSIZECB, name = 'sizeCb', parent = self.panel2, pos = wxPoint(0, 17), size = wxSize(112, 21), style = 0, validator = wxDefaultValidator, value = '')
        self.sizeCb.SetLabel('')

        self.faceCb = wxComboBox(choices = [], id = wxID_STCSTYLEEDITDLGFACECB, name = 'faceCb', parent = self.panel1, pos = wxPoint(17, 18), size = wxSize(111, 21), style = 0, validator = wxDefaultValidator, value = '')
        self.faceCb.SetLabel('')

        self.fgColCb = wxComboBox(choices = [], id = wxID_STCSTYLEEDITDLGFGCOLCB, name = 'fgColCb', parent = self.staticBox1, pos = wxPoint(8, 32), size = wxSize(91, 21), style = 0, validator = wxDefaultValidator, value = '')
        self.fgColCb.SetLabel('')

        self.bgColCb = wxComboBox(choices = [], id = wxID_STCSTYLEEDITDLGBGCOLCB, name = 'bgColCb', parent = self.staticBox1, pos = wxPoint(8, 72), size = wxSize(91, 21), style = 0, validator = wxDefaultValidator, value = '')
        self.bgColCb.SetLabel('')

    def __init__(self, parent, langTitle, lang, configFile, STCsToUpdate=()):
        self.stc_title = 'wxStyledTextCtrl Style Editor'
        self.stc_title = 'wxStyledTextCtrl Style Editor - %s' % langTitle
        self.sys_font = wxSystemSettings_GetSystemFont(wxSYS_DEFAULT_GUI_FONT).GetFaceName()
        self._init_ctrls(parent)

        self._onUpdateUI = false
        self.lang = lang
        self.configFile = configFile
        self.style = ''
        self.names = []
        self.values = {}
        self.STCsToUpdate = STCsToUpdate

        for combo, evtRet, evtRDC in (
              (self.fgColCb, self.OnfgColRet, self.OnGotoCommonDef),
              (self.bgColCb, self.OnbgColRet, self.OnGotoCommonDef),
              (self.faceCb, self.OnfaceRet, self.OnGotoCommonDef),
              (self.sizeCb, self.OnsizeRet, self.OnGotoCommonDef)):
            self.bindComboEvts(combo, evtRet, evtRDC)

        (self.config, self.commonDefs, self.styleIdNames, self.styles,
         self.styleGroupNames, self.predefStyleGroups,
         self.otherLangStyleGroupNames, self.otherLangStyleGroups,
         self.displaySrc, self.lexer, self.keywords, self.braceInfo) = \
              initFromConfig(configFile, lang)

        self.currSpeedSetting = 'style.%s'%self.lang
        for grp in [self.currSpeedSetting]+self.styleGroupNames:
            self.speedsettingCh.Append(grp)
        self.speedsettingCh.SetSelection(0)

        margin = 0
        self.stc.SetMarginType(margin, wxSTC_MARGIN_NUMBER)
        self.stc.SetMarginWidth(margin, 25)
        self.stc.SetMarginSensitive(margin, true)
        EVT_STC_MARGINCLICK(self.stc, wxID_STCSTYLEEDITDLGSTC, self.OnMarginClick)

        self.stc.SetUseTabs(false)
        self.stc.SetTabWidth(4)
        self.stc.SetIndentationGuides(true)
        self.stc.SetEdgeMode(wxSTC_EDGE_BACKGROUND)
        self.stc.SetEdgeColumn(44)

        self.setStyles()

        self.populateStyleSelector()

        self.defNames, self.defValues = parseProp(\
              self.styleDict.get(wxSTC_STYLE_DEFAULT, ''))

        self.stc.SetText(self.displaySrc)
        self.stc.EmptyUndoBuffer()
        self.stc.SetCurrentPos(self.stc.GetTextLength())
        self.stc.SetAnchor(self.stc.GetTextLength())

        self.populateCombosWithCommonDefs()

        # Logical grouping of controls and the property they edit
        self.allCtrls = [((self.fgColBtn, self.fgColCb), self.fgColDefCb,
                             'fore', wxID_STCSTYLEEDITDLGFGCOLDEFCB),
                         ((self.bgColBtn, self.bgColCb), self.bgColDefCb,
                             'back', wxID_STCSTYLEEDITDLGBGCOLDEFCB),
                         (self.taBoldCb, self.taBoldDefCb,
                             'bold', wxID_STCSTYLEEDITDLGTABOLDDEFCB),
                         (self.taItalicCb, self.taItalicDefCb,
                             'italic', wxID_STCSTYLEEDITDLGTAITALICDEFCB),
                         (self.taUnderlinedCb, self.taUnderlinedDefCb,
                             'underline', wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB),
                         (self.taEOLfilledCb, self.taEOLfilledDefCb,
                             'eolfilled', wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB),
                         (self.sizeCb, self.taSizeDefCb,
                             'size', wxID_STCSTYLEEDITDLGTASIZEDEFCB),
                         ((self.faceCb, self.fixedWidthChk), self.faceDefCb,
                             'face', wxID_STCSTYLEEDITDLGFACEDEFCB)]

        self.clearCtrls(disableDefs=true)
        # centralised default checkbox event handler
        self.chbIdMap = {}
        for ctrl, chb, prop, wid in self.allCtrls:
            self.chbIdMap[wid] = ctrl, chb, prop, wid
            EVT_CHECKBOX(chb, wid, self.OnDefaultCheckBox)
            chb.SetToolTipString('Toggle defaults')

        self.Center(wxBOTH)
        self._onUpdateUI = true

#---Property methods------------------------------------------------------------
    def getCtrlForProp(self, findprop):
        for ctrl, chb, prop, wid in self.allCtrls:
            if findprop == prop:
                return ctrl, chb
        raise Exception('PropNotFound', findprop)

    def editProp(self, on, prop, val=''):
        oldstyle = self.rememberStyles()
        if on:
            if not self.names.count(prop):
                self.names.append(prop)
            self.values[prop] = val
        else:
            try: self.names.remove(prop)
            except ValueError: pass
            try: del self.values[prop]
            except KeyError: pass

        try:
            self.updateStyle()
            return true
        except KeyError, errkey:
            wxLogError('Name not found in Common definition, '\
                'please enter valid reference. (%s)'%errkey)
            self.restoreStyles(oldstyle)
            return false

#---Control population methods--------------------------------------------------
    def setStyles(self):
        self.styles, self.styleDict, self.styleNumIdxMap = \
              setSTCStyles(self.stc, self.styles, self.styleIdNames,
              self.commonDefs, self.lang, self.lexer, self.keywords)

    def updateStyle(self):
        # called after a control edited self.names, self.values
        # Special case for saving common defs settings
        if self.styleNum == 'common':
            strVal = self.style[2] = self.values.values()[0]
            if self.style[1] == 'size': self.style[2] = int(strVal)

            self.commonDefs[self.style[0]] = self.style[2]
            self.styleDefST.SetLabel(strVal)
        else:
            self.style = writePropVal(self.names, self.values)
            styleDecl = writeProp(self.styleNum, self.style, self.lang)
            self.styles[self.styleNumIdxMap[self.styleNum]] = styleDecl
            self.styleDefST.SetLabel(self.style)
        self.setStyles()

    def findInStyles(self, txt, styles):
        for style in styles:
            if string.find(style, txt) != -1:
                return true
        return false

    def rememberStyles(self):
        return self.names[:], copy.copy(self.values)

    def restoreStyles(self, style):
        self.names, self.values = style
        self.updateStyle()

    def clearCtrls(self, isDefault=false, disableDefs=false):
        for ctrl, chb, prop, wid in self.allCtrls:
            if prop in ('fore', 'back'):
                btn, txt = ctrl
                btn.SetBackgroundColour(\
                      wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNFACE))
                btn.SetForegroundColour(wxColour(255, 255, 255))
                btn.Enable(isDefault)
                txt.SetValue('')
                txt.Enable(isDefault)
            elif prop == 'size':
                ctrl.SetValue('')
                ctrl.Enable(isDefault)
            elif prop == 'face':
                ctrl[0].SetValue('')
                ctrl[0].Enable(isDefault)
                ctrl[1].Enable(isDefault)
                ctrl[1].SetValue(false)
            elif prop in ('bold', 'italic', 'underline', 'eolfilled'):
                ctrl.SetValue(false)
                ctrl.Enable(isDefault)

            chb.Enable(not isDefault and not disableDefs)
            chb.SetValue(true)

    def populateProp(self, items, default, forceDisable=false):
        for name, val in items:
            if name:
                ctrl, chb = self.getCtrlForProp(name)

                if name in ('fore', 'back'):
                    btn, txt = ctrl
                    repval = val%self.commonDefs
                    btn.SetBackgroundColour(strToCol(repval))
                    btn.SetForegroundColour(wxColour(0, 0, 0))
                    btn.Enable(not forceDisable)
                    txt.SetValue(val)
                    txt.Enable(not forceDisable)
                    chb.SetValue(default)
                elif name  == 'size':
                    ctrl.SetValue(val)
                    ctrl.Enable(not forceDisable)
                    chb.SetValue(default)
                elif name  == 'face':
                    ctrl[0].SetValue(val)
                    ctrl[0].Enable(not forceDisable)
                    ctrl[1].Enable(not forceDisable)
                    chb.SetValue(default)
                elif name in ('bold', 'italic', 'underline', 'eolfilled'):
                    ctrl.Enable(not forceDisable)
                    ctrl.SetValue(true)
                    chb.SetValue(default)

    def valIsCommonDef(self, val):
        return len(val) >= 5 and val[:2] == '%('

    def populateCtrls(self):
        self.clearCtrls(self.styleNum == wxSTC_STYLE_DEFAULT,
            disableDefs=self.styleNum < 0)

        # handle colour controls for settings
        if self.styleNum < 0:
            self.fgColDefCb.Enable(true)
            if self.styleNum == -1:
                self.bgColDefCb.Enable(true)

        # populate with default style
        self.populateProp(self.defValues.items(), true,
            self.styleNum != wxSTC_STYLE_DEFAULT)
        # override with current settings
        self.populateProp(self.values.items(), false)

    def getCommonDefPropType(self, commonDefName):
        val = self.commonDefs[commonDefName]
        if type(val) == type(0): return 'size'
        if len(val) == 7 and val[0] == '#': return 'fore'
        return 'face'

    def bindComboEvts(self, combo, returnEvtMeth, rdclickEvtMeth):
        wId = wxNewId()
        EVT_MENU(self, wId, returnEvtMeth)
        combo.SetAcceleratorTable(wxAcceleratorTable([(0, WXK_RETURN, wId)]))
        EVT_RIGHT_DCLICK(combo, rdclickEvtMeth)
        combo.SetToolTipString('Select or press Enter to change, right double-click \n'\
            'the drop down button to select Common definition (if applicable)')

    def populateCombosWithCommonDefs(self, fixedWidthOnly=None):
        commonDefs = {'fore': [], 'face': [], 'size': []}

        if self.elementLb.GetSelection() < self.commonDefsStartIdx:
            for common in self.commonDefs.keys():
                prop = self.getCommonDefPropType(common)
                commonDefs[prop].append('%%(%s)%s'%(common,
                                                   prop=='size' and 'd' or 's'))

        # Colours
        currFg, currBg = self.fgColCb.GetValue(), self.bgColCb.GetValue()
        self.fgColCb.Clear(); self.bgColCb.Clear()
        for colCommonDef in commonDefs['fore']:
            self.fgColCb.Append(colCommonDef)
            self.bgColCb.Append(colCommonDef)
        self.fgColCb.SetValue(currFg); self.bgColCb.SetValue(currBg)

        # Font
        if fixedWidthOnly is None:
            fixedWidthOnly = self.fixedWidthChk.GetValue()
        fontEnum = wxFontEnumerator()
        fontEnum.EnumerateFacenames(fixedWidthOnly=fixedWidthOnly)
        fontNameList = fontEnum.GetFacenames()

        currFace = self.faceCb.GetValue()
        self.faceCb.Clear()
        for colCommonDef in ['']+fontNameList+commonDefs['face']:
            self.faceCb.Append(colCommonDef)
        self.faceCb.SetValue(currFace)

        # Size (XXX add std font sizes)
        currSize = self.sizeCb.GetValue()
        self.sizeCb.Clear()
        for colCommonDef in commonDefs['size']:
            self.sizeCb.Append(colCommonDef)
        self.sizeCb.SetValue(currSize)

    def populateStyleSelector(self):
        numStyles = self.styleIdNames.items()
        numStyles.sort()
        self.styleNumLookup = {}
        stdStart = -1
        stdOffset = 0
        extrOffset = 0
        # add styles
        for num, name in numStyles:
            if num == wxSTC_STYLE_DEFAULT:
                self.elementLb.InsertItems([name, '-----Language-----'], 0)
                self.elementLb.Append('-----Standard-----')
                stdStart = stdPos = self.elementLb.Number()
            else:
                # std styles
                if num >= 33 and num < 40:
                    self.elementLb.InsertItems([name], stdStart + stdOffset)
                    stdOffset = stdOffset + 1
                # extra styles
                elif num >= 40:
                    self.elementLb.InsertItems([name], stdStart + extrOffset -1)
                    extrOffset = extrOffset + 1
                # normal lang styles
                else:
                    self.elementLb.Append(name)
            self.styleNumLookup[name] = num

        # add settings
        self.elementLb.Append('-----Settings-----')
        settings = settingsIdNames.items()
        settings.sort();settings.reverse()
        for num, name in settings:
            self.elementLb.Append(name)
            self.styleNumLookup[name] = num

        # add definitions
        self.elementLb.Append('-----Common-----')
        self.commonDefsStartIdx = self.elementLb.Number()
        for common in self.commonDefs.keys():
            tpe = type(self.commonDefs[common])
            self.elementLb.Append('%('+common+')'+(tpe is type('') and 's' or 'd'))
            self.styleNumLookup[common] = num

#---Colour methods--------------------------------------------------------------
    def getColourDlg(self, colour, title=''):
        data = wxColourData()
        data.SetColour(colour)
        data.SetChooseFull(true)
        dlg = wxColourDialog(self, data)
        try:
            dlg.SetTitle(title)
            if dlg.ShowModal() == wxID_OK:
                data = dlg.GetColourData()
                return data.GetColour()
        finally:
            dlg.Destroy()
        return None

    colDlgTitles = {'fore': 'Foreground', 'back': 'Background'}
    def editColProp(self, colBtn, colCb, prop):
        col = self.getColourDlg(colBtn.GetBackgroundColour(),
              self.colDlgTitles[prop]+ ' colour')
        if col:
            colBtn.SetForegroundColour(wxColour(0, 0, 0))
            colBtn.SetBackgroundColour(col)
            colStr = colToStr(col)
            colCb.SetValue(colStr)
            self.editProp(true, prop, colStr)

    def OnFgcolbtnButton(self, event):
        self.editColProp(self.fgColBtn, self.fgColCb, 'fore')

    def OnBgcolbtnButton(self, event):
        self.editColProp(self.bgColBtn, self.bgColCb, 'back')

    def editColTCProp(self, colCb, colBtn, prop):
        colStr = colCb.GetValue()
        if colStr:
            col = strToCol(colStr%self.commonDefs)
        if self.editProp(colStr!='', prop, colStr):
            if colStr:
                colBtn.SetForegroundColour(wxColour(0, 0, 0))
                colBtn.SetBackgroundColour(col)
            else:
                colBtn.SetForegroundColour(wxColour(255, 255, 255))
                colBtn.SetBackgroundColour(\
                      wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNFACE))

    def OnfgColRet(self, event):
        try: self.editColTCProp(self.fgColCb, self.fgColBtn, 'fore')
        except AssertionError: wxLogError('Not a valid colour value')

    def OnbgColRet(self, event):
        try: self.editColTCProp(self.bgColCb, self.bgColBtn, 'back')
        except AssertionError: wxLogError('Not a valid colour value')

#---Text attribute events-------------------------------------------------------
    def OnTaeoffilledcbCheckbox(self, event):
        self.editProp(event.IsChecked(), 'eolfilled')

    def OnTaitaliccbCheckbox(self, event):
        self.editProp(event.IsChecked(), 'italic')

    def OnTaboldcbCheckbox(self, event):
        self.editProp(event.IsChecked(), 'bold')

    def OnTaunderlinedcbCheckbox(self, event):
        self.editProp(event.IsChecked(), 'underline')

    def OnGotoCommonDef(self, event):
        val = event.GetEventObject().GetValue()
        if self.valIsCommonDef(val):
            idx = self.elementLb.FindString(val)
            if idx != -1:
                self.elementLb.SetSelection(idx, true)
                self.OnElementlbListbox(None)

    def OnfaceRet(self, event):
        val = self.faceCb.GetValue()
        try: val%self.commonDefs
        except KeyError: wxLogError('Invalid common definition')
        else: self.editProp(val!='', 'face', val)

    def OnsizeRet(self, event):
        val = self.sizeCb.GetValue()
        try: int(val%self.commonDefs)
        except ValueError: wxLogError('Not a valid integer size value')
        except KeyError: wxLogError('Invalid common definition')
        else: self.editProp(val!='', 'size', val)

#---Main GUI events-------------------------------------------------------------
    def OnElementlbListbox(self, event):
        isCommon = self.elementLb.GetSelection() >= self.commonDefsStartIdx
        self.removeCommonItemBtn.Enable(isCommon)

        styleIdent = self.elementLb.GetStringSelection()
        # common definition selected
        if isCommon:
            common = styleIdent[2:-2]
            prop = self.getCommonDefPropType(common)
            self.clearCtrls(disableDefs=true)
            if prop == 'fore':
                self.fgColBtn.Enable(true)
                self.fgColCb.Enable(true)
            elif prop == 'face':
                self.faceCb.Enable(true)
                self.fixedWidthChk.Enable(true)
            elif prop == 'size':
                self.sizeCb.Enable(true)

            commonDefVal = str(self.commonDefs[common])
            self.styleDefST.SetLabel(commonDefVal)
            self.populateProp( [(prop, commonDefVal)], true)

            self.styleNum = 'common'
            self.style = [common, prop, commonDefVal]
            self.names, self.values = [prop], {prop: commonDefVal}

        # normal style element selected
        elif len(styleIdent) >=2 and styleIdent[:2] != '--':
            self.styleNum = self.styleNumLookup[styleIdent]
            self.style = self.styleDict[self.styleNum]
            self.names, self.values = parseProp(self.style)

            if self.styleNum == wxSTC_STYLE_DEFAULT:
                self.defNames, self.defValues = \
                      self.names, self.values

            self.checkBraces(self.styleNum)

            self.styleDefST.SetLabel(self.style)

            self.populateCtrls()
        # separator selected
        else:
            self.clearCtrls(disableDefs=true)
            if styleIdent:
                self.styleDefST.SetLabel(styleCategoryDescriptions[styleIdent])

        self.populateCombosWithCommonDefs()

    def OnDefaultCheckBox(self, event):
        self._onUpdateUI = false
        try:
            if self.chbIdMap.has_key(event.GetId()):
                ctrl, chb, prop, wid = self.chbIdMap[event.GetId()]
                restore = not event.IsChecked()
                if prop in ('fore', 'back'):
                    ctrl[0].Enable(restore)
                    ctrl[1].Enable(restore)
                    if restore:
                        # XXX use ctrl[1] !!
                        colStr = ctrl[1].GetValue()
                        #if prop == 'fore': colStr = self.fgColCb.GetValue()
                        #else: colStr = self.bgColCb.GetValue()
                        if colStr: self.editProp(true, prop, colStr)
                    else:
                        self.editProp(false, prop)
                elif prop  == 'size':
                    val = ctrl.GetValue()
                    if val: self.editProp(restore, prop, val)
                    ctrl.Enable(restore)
                elif prop  == 'face':
                    val = ctrl[0].GetStringSelection()
                    if val: self.editProp(restore, prop, val)
                    ctrl[0].Enable(restore)
                    ctrl[1].Enable(restore)
                elif prop in ('bold', 'italic', 'underline', 'eolfilled'):
                    ctrl.Enable(restore)
                    if ctrl.GetValue(): self.editProp(restore, prop)
        finally:
            self._onUpdateUI = true

    def OnOkbtnButton(self, event):
        # write styles and common defs to the config
        writeStylesToConfig(self.config, 'style.%s'%self.lang, self.styles)
        self.config.SetPath('')
        self.config.Write(commonDefsFile, `self.commonDefs`)
        self.config.Flush()

        for stc in self.STCsToUpdate:
            setSTCStyles(stc, self.styles, self.styleIdNames, self.commonDefs,
                  self.lang, self.lexer, self.keywords)

        self.EndModal(wxID_OK)

    def OnCancelbtnButton(self, event):
        self.EndModal(wxID_CANCEL)

    def OnCommondefsbtnButton(self, event):
        dlg = wxTextEntryDialog(self, 'Edit common definitions dictionary',
              'Common definitions', pprint.pformat(self.commonDefs),
              style=wxTE_MULTILINE | wxOK | wxCANCEL | wxCENTRE)
        try:
            if dlg.ShowModal() == wxID_OK:
                answer = eval(dlg.GetValue())
                assert type(answer) is type({}), 'Not a valid dictionary'
                oldDefs = self.commonDefs
                self.commonDefs = answer
                try:
                    self.setStyles()
                except KeyError, badkey:
                    wxLogError(str(badkey)+' not defined but required, \n'\
                          'reverting to previous common definition')
                    self.commonDefs = oldDefs
                    self.setStyles()
                self.populateCombosWithCommonDefs()

        finally:
            dlg.Destroy()

    def OnSpeedsettingchChoice(self, event):
        group = event.GetString()
        if group:
            userStyles = 'style.%s'%self.lang
            if self.currSpeedSetting == userStyles:
                self.predefStyleGroups[userStyles] = self.styles
            self.styles = self.predefStyleGroups[group]
            self.setStyles()
            self.defNames, self.defValues = parseProp(\
                  self.styleDict.get(wxSTC_STYLE_DEFAULT, ''))
            self.OnElementlbListbox(None)
            self.currSpeedSetting = group

    def OnFixedwidthchkCheckbox(self, event):
        self.populateCombosWithCommonDefs(event.Checked())

    def OnAddsharebtnButton(self, event):
        dlg = CommonDefDlg(self)
        try:
            if dlg.ShowModal() == wxID_OK:
                prop, name = dlg.result
                if not self.commonDefs.has_key(name):
                    self.commonDefs[name] = commonPropDefs[prop]
                    self.elementLb.Append('%('+name+')'+\
                     (type(commonPropDefs[prop]) is type('') and 's' or 'd'))
                    self.elementLb.SetSelection(self.elementLb.Number()-1, true)
                    self.populateCombosWithCommonDefs()
                    self.OnElementlbListbox(None)
        finally:
            dlg.Destroy()

    def OnRemovesharebtnButton(self, event):
        ownGroup = 'style.%s'%self.lang
        comDef = self.elementLb.GetStringSelection()

        # Search ALL styles before removing
        srchDct = {ownGroup: self.styles}
        srchDct.update(self.predefStyleGroups)
        srchDct.update(self.otherLangStyleGroups)

        matchList = []
        for grpName, styles in srchDct.items():
            if self.findInStyles(comDef, styles):
                matchList.append(grpName)

        if matchList:
            wxLogError('Aborted: '+comDef+' is still used in the styles of the \n'\
                  'following groups in the config file (stc-styles.rc.cfg):\n'+ \
                  string.join(matchList, '\n'))
        else:
            del self.commonDefs[comDef[2:-2]]
            self.setStyles()
            self.populateCombosWithCommonDefs()
            selIdx = self.elementLb.GetSelection()
            self.elementLb.Delete(selIdx)
            if selIdx == self.elementLb.Number():
                selIdx = selIdx - 1
            self.elementLb.SetSelection(selIdx, true)
            self.OnElementlbListbox(None)

#---STC events------------------------------------------------------------------
    def OnUpdateUI(self, event):
        if self._onUpdateUI:
            styleBefore = self.stc.GetStyleAt(self.stc.GetCurrentPos())
            if self.styleIdNames.has_key(styleBefore):
                self.elementLb.SetStringSelection(self.styleIdNames[styleBefore],
                      true)
            else:
                self.elementLb.SetSelection(0, false)
                self.styleDefST.SetLabel('Style %d not defined, sorry.'%styleBefore)
            self.OnElementlbListbox(None)
        event.Skip()

    def checkBraces(self, style):
        if style == wxSTC_STYLE_BRACELIGHT and self.braceInfo.has_key('good'):
            line, col = self.braceInfo['good']
            pos = self.stc.PositionFromLine(line-1) + col
            braceOpposite = self.stc.BraceMatch(pos)
            if braceOpposite != -1:
                self.stc.BraceHighlight(pos, braceOpposite)
        elif style == wxSTC_STYLE_BRACEBAD and self.braceInfo.has_key('bad'):
            line, col = self.braceInfo['bad']
            pos = self.stc.PositionFromLine(line-1) + col
            self.stc.BraceBadLight(pos)
        else:
            self.stc.BraceBadLight(-1)
        return

    def OnStcstyleeditdlgSize(self, event):
        self.Layout()
        # Without this refresh, resizing leaves artifacts
        self.Refresh(1)
        event.Skip()

    def OnMarginClick(self, event):
        self.elementLb.SetStringSelection('Line numbers', true)
        self.OnElementlbListbox(None)


#---Common definition dialog----------------------------------------------------

[wxID_COMMONDEFDLG, wxID_COMMONDEFDLGCANCELBTN, wxID_COMMONDEFDLGCOMDEFNAMETC, wxID_COMMONDEFDLGOKBTN, wxID_COMMONDEFDLGPROPTYPERBX, wxID_COMMONDEFDLGSTATICBOX1] = map(lambda _init_ctrls: wxNewId(), range(6))

class CommonDefDlg(wxDialog):
    def _init_ctrls(self, prnt):
        wxDialog.__init__(self, id = wxID_COMMONDEFDLG, name = 'CommonDefDlg', parent = prnt, pos = wxPoint(398, 249), size = wxSize(192, 220), style = wxDEFAULT_DIALOG_STYLE, title = 'Common definition')
        self.SetClientSize(wxSize(184, 175))

        self.propTypeRBx = wxRadioBox(choices = ['Colour value', 'Font face', 'Size value'], id = wxID_COMMONDEFDLGPROPTYPERBX, label = 'Common definition property type', majorDimension = 1, name = 'propTypeRBx', parent = self, point = wxPoint(8, 8), size = wxSize(168, 72), style = wxRA_SPECIFY_COLS, validator = wxDefaultValidator)
        self.propTypeRBx.SetSelection(self._propTypeIdx)

        self.staticBox1 = wxStaticBox(id = wxID_COMMONDEFDLGSTATICBOX1, label = 'Name', name = 'staticBox1', parent = self, pos = wxPoint(8, 88), size = wxSize(168, 46), style = 0)

        self.comDefNameTC = wxTextCtrl(id = wxID_COMMONDEFDLGCOMDEFNAMETC, name = 'comDefNameTC', parent = self, pos = wxPoint(16, 104), size = wxSize(152, 21), style = 0, value = '')
        self.comDefNameTC.SetLabel(self._comDefName)

        self.okBtn = wxButton(id = wxID_COMMONDEFDLGOKBTN, label = 'OK', name = 'okBtn', parent = self, pos = wxPoint(8, 144), size = wxSize(80, 23), style = 0)
        EVT_BUTTON(self.okBtn, wxID_COMMONDEFDLGOKBTN, self.OnOkbtnButton)

        self.cancelBtn = wxButton(id = wxID_COMMONDEFDLGCANCELBTN, label = 'Cancel', name = 'cancelBtn', parent = self, pos = wxPoint(96, 144), size = wxSize(80, 23), style = 0)
        EVT_BUTTON(self.cancelBtn, wxID_COMMONDEFDLGCANCELBTN, self.OnCancelbtnButton)

    def __init__(self, parent, name='', propIdx=0):
        self._comDefName = ''
        self._comDefName = name
        self._propTypeIdx = 0
        self._propTypeIdx = propIdx
        self._init_ctrls(parent)

        self.propMap = {0: 'fore', 1: 'face', 2: 'size'}
        self.result = ( '', '' )

        self.Center(wxBOTH)

    def OnOkbtnButton(self, event):
        self.result = ( self.propMap[self.propTypeRBx.GetSelection()],
                        self.comDefNameTC.GetValue() )
        self.EndModal(wxID_OK)

    def OnCancelbtnButton(self, event):
        self.result = ( '', '' )
        self.EndModal(wxID_CANCEL)

#---Functions useful outside of the editor----------------------------------

def setSelectionColour(stc, style):
    names, values = parseProp(style)
    if 'fore' in names:
        stc.SetSelForeground(true, strToCol(values['fore']))
    if 'back' in names:
        stc.SetSelBackground(true, strToCol(values['back']))

def setCursorColour(stc, style):
    names, values = parseProp(style)
    if 'fore' in names:
        stc.SetCaretForeground(strToCol(values['fore']))

def setEdgeColour(stc, style):
    names, values = parseProp(style)
    if 'fore' in names:
        stc.SetEdgeColour(strToCol(values['fore']))

def strToCol(strCol):
    assert len(strCol) == 7 and strCol[0] == '#', 'Not a valid colour string'
    return wxColour(string.atoi('0x'+strCol[1:3], 16),
                    string.atoi('0x'+strCol[3:5], 16),
                    string.atoi('0x'+strCol[5:7], 16))
def colToStr(col):
    return '#%s%s%s' % (string.zfill(string.upper(hex(col.Red())[2:]), 2),
                        string.zfill(string.upper(hex(col.Green())[2:]), 2),
                        string.zfill(string.upper(hex(col.Blue())[2:]), 2))

def writeProp(num, style, lang):
    if num >= 0:
        return 'style.%s.%s='%(lang, string.zfill(`num`, 3)) + style
    else:
        return 'setting.%s.%d='%(lang, num) + style

def writePropVal(names, values):
    res = []
    for name in names:
        if name:
            res.append(values[name] and name+':'+values[name] or name)
    return string.join(res, ',')

def parseProp(prop):
    items = string.split(prop, ',')
    names = []
    values = {}
    for item in items:
        nameVal = string.split(item, ':')
        names.append(string.strip(nameVal[0]))
        if len(nameVal) == 1:
            values[nameVal[0]] = ''
        else:
            values[nameVal[0]] = string.strip(nameVal[1])
    return names, values

def parsePropLine(prop):
    name, value = string.split(prop, '=')
    return int(string.split(name, '.')[-1]), value

def setSTCStyles(stc, styles, styleIdNames, commonDefs, lang, lexer, keywords):
    styleDict = {}
    styleNumIdxMap = {}

    # build style dict based on given styles
    for numStyle in styles:
        num, style = parsePropLine(numStyle)
        styleDict[num] = style

    # Add blank style entries for undefined styles
    newStyles = []
    styleItems = styleIdNames.items() + settingsIdNames.items()
    styleItems.sort()
    idx = 0
    for num, name in styleItems:
        styleNumIdxMap[num] = idx
        if not styleDict.has_key(num):
            styleDict[num] = ''
        newStyles.append(writeProp(num, styleDict[num], lang))
        idx = idx + 1

    # Set the styles on the wxSTC
    stc.StyleResetDefault()
    stc.ClearDocumentStyle()
    stc.SetLexer(lexer)
    stc.SetKeyWords(0, keywords)
    stc.StyleSetSpec(wxSTC_STYLE_DEFAULT,
          styleDict[wxSTC_STYLE_DEFAULT] % commonDefs)
    stc.StyleClearAll()

    for num, style in styleDict.items():
        if num >= 0:
            stc.StyleSetSpec(num, style % commonDefs)
        elif num == -1:
            setSelectionColour(stc, style % commonDefs)
        elif num == -2:
            setCursorColour(stc, style % commonDefs)
        elif num == -3:
            setEdgeColour(stc, style % commonDefs)

    stc.Colourise(0, stc.GetTextLength())

    return newStyles, styleDict, styleNumIdxMap

#---Config reading and writing -------------------------------------------------
commonDefsFile = 'common.defs.%s'%(wxPlatform == '__WXMSW__' and 'msw' or 'gtk')

def initFromConfig(configFile, lang):
    cfg = wxFileConfig(localFilename=configFile, style=wxCONFIG_USE_LOCAL_FILE)
    cfg.SetExpandEnvVars(false)

    # read in all group names for this language
    groupPrefix = 'style.%s'%lang
    gpLen = len(groupPrefix)
    predefStyleGroupNames, otherLangStyleGroupNames = [], []
    cont, val, idx = cfg.GetFirstGroup()
    while cont:
        if val != groupPrefix and len(val) >= 5 and val[:5] == 'style':
            if len(val) > gpLen and val[:gpLen] == groupPrefix:
                predefStyleGroupNames.append(val)
            else:
                otherLangStyleGroupNames.append(val)

        cont, val, idx = cfg.GetNextGroup(idx)

    # read in common elements
    commonDefs = eval(cfg.Read(commonDefsFile))
    assert type(commonDefs) is type({}), \
          'Common definitions (%s) not a valid dict'%commonDefsFile

    commonStyleIdNames = eval(cfg.Read('common.styleidnames'))
    assert type(commonStyleIdNames) is type({}), \
          'Common definitions (%s) not a valid dict'%'common.styleidnames'

    # Lang spesific settings
    cfg.SetPath(lang)
    styleIdNames = eval(cfg.Read('styleidnames'))
    assert type(commonStyleIdNames) is type({}), \
          'Not a valid dict [%s] styleidnames)'%lang
    styleIdNames.update(commonStyleIdNames)
    braceInfo = eval(cfg.Read('braces'))
    assert type(commonStyleIdNames) is type({}), \
          'Not a valid dict [%s] braces)'%lang

    displaySrc = cfg.Read('displaysrc')
    lexer = eval(cfg.Read('lexer'))
    keywords = cfg.Read('keywords')

    cfg.SetPath('')

    # read in current styles
    styles = readStylesFromConfig(cfg, groupPrefix)

    # read in predefined styles
    predefStyleGroups = {}
    for group in predefStyleGroupNames:
        predefStyleGroups[group] = readStylesFromConfig(cfg, group)

    # read in all other style sections
    otherLangStyleGroups = {}
    for group in otherLangStyleGroupNames:
        otherLangStyleGroups[group] = readStylesFromConfig(cfg, group)

    return (cfg, commonDefs, styleIdNames, styles, predefStyleGroupNames,
            predefStyleGroups, otherLangStyleGroupNames, otherLangStyleGroups,
            displaySrc, lexer, keywords, braceInfo)

def readStylesFromConfig(config, group):
    config.SetPath('')
    config.SetPath(group)
    styles = []
    cont, val, idx = config.GetFirstEntry()
    while cont:
        styles.append(val+'='+config.Read(val))
        cont, val, idx = config.GetNextEntry(idx)
    config.SetPath('')

    return styles

def writeStylesToConfig(config, group, styles):
    config.SetPath('')
    config.DeleteGroup(group)
    config.SetPath(group)

    for style in styles:
        name, value = string.split(style, '=')
        config.Write(name, string.strip(value))

    config.SetPath('')

#-------------------------------------------------------------------------------
def initSTC(stc, config, lang):
    """ Main module entry point. Initialise a wxSTC from given config file."""
    (cfg, commonDefs, styleIdNames, styles, predefStyleGroupNames,
     predefStyleGroups, otherLangStyleGroupNames, otherLangStyleGroups,
     displaySrc, lexer, keywords, braceInfo) = initFromConfig(config, lang)

    setSTCStyles(stc, styles, styleIdNames, commonDefs, lang, lexer, keywords)

#-------------------------------------------------------------------------------
if __name__ == '__main__':
    app = wxPySimpleApp()
    config = os.path.abspath('stc-styles.rc.cfg')

    if 0:
        f = wxFrame(None, -1, 'Test frame (double click for editor)')
        stc = wxStyledTextCtrl(f, -1)
        def OnDblClick(evt, stc=stc):
            dlg = STCStyleEditDlg(None, 'Python', 'python', config, (stc,))
            try: dlg.ShowModal()
            finally: dlg.Destroy()
        stc.SetText(open('STCStyleEditor.py').read())
        EVT_LEFT_DCLICK(stc, OnDblClick)
        initSTC(stc, config, 'python')
        f.Show(true)
        app.MainLoop()
    else:
        dlg = STCStyleEditDlg(None,
            'Python', 'python',
            #'HTML', 'html',
            #'XML', 'xml',
            #'C++', 'cpp',
            #'Text', 'text',
            #'Properties', 'prop',
            config)
        try: dlg.ShowModal()
        finally: dlg.Destroy()
        del config
        app.MainLoop()

