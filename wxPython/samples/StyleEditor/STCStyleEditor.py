#-----------------------------------------------------------------------------
# Name:        STCStyleEditor.py
# Purpose:     Style editor for the wxStyledTextCtrl
#
# Author:      Riaan Booysen
#
# Created:     2001/08/20
# RCS-ID:      $Id$
# Copyright:   (c) 2001 - 2002 Riaan Booysen
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
from wxPython.help import *
from wxPython.lib.anchors import LayoutAnchors
from wxPython.stc import *

settingsIdNames = {-1: 'Selection', -2: 'Caret', -3: 'Edge'}

commonPropDefs = {}

styleCategoryDescriptions = {
 '----Language----': 'Styles spesific to the language',
 '----Standard----': 'Styles shared by all languages',
 '----Settings----': 'Properties set by STC methods',
 '----Common----': 'User definable values that can be shared between languages'}

[wxID_STCSTYLEEDITDLG, wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN,
 wxID_STCSTYLEEDITDLGBGCOLBTN, wxID_STCSTYLEEDITDLGBGCOLCB,
 wxID_STCSTYLEEDITDLGBGCOLDEFCB, wxID_STCSTYLEEDITDLGBGCOLOKBTN,
 wxID_STCSTYLEEDITDLGCANCELBTN, wxID_STCSTYLEEDITDLGCONTEXTHELPBUTTON1,
 wxID_STCSTYLEEDITDLGELEMENTLB, wxID_STCSTYLEEDITDLGFACECB,
 wxID_STCSTYLEEDITDLGFACEDEFCB, wxID_STCSTYLEEDITDLGFACEOKBTN,
 wxID_STCSTYLEEDITDLGFGCOLBTN, wxID_STCSTYLEEDITDLGFGCOLCB,
 wxID_STCSTYLEEDITDLGFGCOLDEFCB, wxID_STCSTYLEEDITDLGFGCOLOKBTN,
 wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK, wxID_STCSTYLEEDITDLGOKBTN,
 wxID_STCSTYLEEDITDLGPANEL1, wxID_STCSTYLEEDITDLGPANEL2,
 wxID_STCSTYLEEDITDLGPANEL3, wxID_STCSTYLEEDITDLGPANEL4,
 wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN, wxID_STCSTYLEEDITDLGSIZECB,
 wxID_STCSTYLEEDITDLGSIZEOKBTN, wxID_STCSTYLEEDITDLGSPEEDSETTINGCH,
 wxID_STCSTYLEEDITDLGSTATICBOX1, wxID_STCSTYLEEDITDLGSTATICBOX2,
 wxID_STCSTYLEEDITDLGSTATICLINE1, wxID_STCSTYLEEDITDLGSTATICTEXT2,
 wxID_STCSTYLEEDITDLGSTATICTEXT3, wxID_STCSTYLEEDITDLGSTATICTEXT4,
 wxID_STCSTYLEEDITDLGSTATICTEXT6, wxID_STCSTYLEEDITDLGSTATICTEXT7,
 wxID_STCSTYLEEDITDLGSTATICTEXT8, wxID_STCSTYLEEDITDLGSTATICTEXT9,
 wxID_STCSTYLEEDITDLGSTC, wxID_STCSTYLEEDITDLGSTYLEDEFST,
 wxID_STCSTYLEEDITDLGTABOLDCB, wxID_STCSTYLEEDITDLGTABOLDDEFCB,
 wxID_STCSTYLEEDITDLGTAEOLFILLEDCB, wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB,
 wxID_STCSTYLEEDITDLGTAITALICCB, wxID_STCSTYLEEDITDLGTAITALICDEFCB,
 wxID_STCSTYLEEDITDLGTASIZEDEFCB, wxID_STCSTYLEEDITDLGTAUNDERLINEDCB,
 wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB,
] = map(lambda _init_ctrls: wxNewId(), range(47))

class STCStyleEditDlg(wxDialog):
    """ Style editor for the wxStyledTextCtrl """
    _custom_classes = {'wxWindow' : ['wxStyledTextCtrl']}
    def _init_utils(self):
        # generated method, don't edit
        pass

    def _init_ctrls(self, prnt):
        # generated method, don't edit
        wxDialog.__init__(self, id=wxID_STCSTYLEEDITDLG, name='STCStyleEditDlg',
              parent=prnt, pos=wxPoint(583, 291), size=wxSize(459, 482),
              style=wxWANTS_CHARS | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
              title=self.stc_title)
        self._init_utils()
        self.SetClientSize(wxSize(451, 455))
        self.SetAutoLayout(True)
        self.SetSizeHints(425, 400, -1, -1)
        self.Center(wxBOTH)
        EVT_SIZE(self, self.OnStcstyleeditdlgSize)

        self.speedsettingCh = wxChoice(choices=[],
              id=wxID_STCSTYLEEDITDLGSPEEDSETTINGCH, name='speedsettingCh',
              parent=self, pos=wxPoint(96, 28), size=wxSize(346, 21), style=0,
              validator=wxDefaultValidator)
        self.speedsettingCh.SetConstraints(LayoutAnchors(self.speedsettingCh,
              True, True, True, False))
        self.speedsettingCh.SetHelpText('The speed setting allows you to revert to one of the predefined style sets. This will overwrite your current settings when tha dialog is posted.')
        EVT_CHOICE(self.speedsettingCh, wxID_STCSTYLEEDITDLGSPEEDSETTINGCH,
              self.OnSpeedsettingchChoice)

        self.elementLb = wxListBox(choices=[], id=wxID_STCSTYLEEDITDLGELEMENTLB,
              name='elementLb', parent=self, pos=wxPoint(8, 72),
              size=wxSize(160, 112), style=0, validator=wxDefaultValidator)
        self.elementLb.SetConstraints(LayoutAnchors(self.elementLb, True, True,
              True, False))
        self.elementLb.SetHelpText('Select a style here to edit it. Common definitions can be added and maintained here.  A common definition is a property that can be shared between styles and special cased per platform.')
        EVT_LISTBOX(self.elementLb, wxID_STCSTYLEEDITDLGELEMENTLB,
              self.OnElementlbListbox)

        self.addCommonItemBtn = wxButton(id=wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN,
              label='Add', name='addCommonItemBtn', parent=self, pos=wxPoint(8,
              184), size=wxSize(80, 17), style=0)
        self.addCommonItemBtn.SetToolTipString('Add new Common definition')
        EVT_BUTTON(self.addCommonItemBtn, wxID_STCSTYLEEDITDLGADDCOMMONITEMBTN,
              self.OnAddsharebtnButton)

        self.removeCommonItemBtn = wxButton(id=wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN,
              label='Remove', name='removeCommonItemBtn', parent=self,
              pos=wxPoint(88, 184), size=wxSize(80, 17), style=0)
        self.removeCommonItemBtn.SetToolTipString('Remove the selected Common definition')
        EVT_BUTTON(self.removeCommonItemBtn,
              wxID_STCSTYLEEDITDLGREMOVECOMMONITEMBTN,
              self.OnRemovesharebtnButton)

        self.styleDefST = wxStaticText(id=wxID_STCSTYLEEDITDLGSTYLEDEFST,
              label='(nothing selected)', name='styleDefST', parent=self,
              pos=wxPoint(96, 8), size=wxSize(366, 16),
              style=wxST_NO_AUTORESIZE)
        self.styleDefST.SetFont(wxFont(self.style_font_size, wxSWISS, wxNORMAL,
              wxBOLD, False, ''))
        self.styleDefST.SetConstraints(LayoutAnchors(self.styleDefST, True,
              True, True, False))

        self.staticLine1 = wxStaticLine(id=wxID_STCSTYLEEDITDLGSTATICLINE1,
              name='staticLine1', parent=self, pos=wxPoint(48, 62),
              size=wxSize(120, 2), style=wxLI_HORIZONTAL)
        self.staticLine1.SetConstraints(LayoutAnchors(self.staticLine1, True,
              True, True, False))

        self.staticText6 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT6,
              label='Style', name='staticText6', parent=self, pos=wxPoint(8,
              56), size=wxSize(40, 13), style=0)

        self.staticText8 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT8,
              label='Style def:', name='staticText8', parent=self,
              pos=wxPoint(8, 8), size=wxSize(88, 13), style=0)

        self.staticText9 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT9,
              label='SpeedSetting:', name='staticText9', parent=self,
              pos=wxPoint(8, 32), size=wxSize(88, 13), style=0)

        self.panel3 = wxPanel(id=wxID_STCSTYLEEDITDLGPANEL3, name='panel3',
              parent=self, pos=wxPoint(176, 56), size=wxSize(146, 104),
              style=wxTAB_TRAVERSAL)
        self.panel3.SetConstraints(LayoutAnchors(self.panel3, False, True, True,
              False))

        self.panel4 = wxPanel(id=wxID_STCSTYLEEDITDLGPANEL4, name='panel4',
              parent=self, pos=wxPoint(330, 56), size=wxSize(114, 104),
              style=wxTAB_TRAVERSAL)
        self.panel4.SetConstraints(LayoutAnchors(self.panel4, False, True, True,
              False))

        self.panel1 = wxPanel(id=wxID_STCSTYLEEDITDLGPANEL1, name='panel1',
              parent=self, pos=wxPoint(176, 161), size=wxSize(143, 40),
              style=wxTAB_TRAVERSAL)
        self.panel1.SetConstraints(LayoutAnchors(self.panel1, False, True, True,
              False))

        self.panel2 = wxPanel(id=wxID_STCSTYLEEDITDLGPANEL2, name='panel2',
              parent=self, pos=wxPoint(330, 162), size=wxSize(112, 40),
              style=wxTAB_TRAVERSAL)
        self.panel2.SetConstraints(LayoutAnchors(self.panel2, False, True, True,
              False))

        self.stc = wxStyledTextCtrl(id=wxID_STCSTYLEEDITDLGSTC, name='stc',
              parent=self, pos=wxPoint(8, 208), size=wxSize(435, 207),
              style=wxSUNKEN_BORDER)
        self.stc.SetConstraints(LayoutAnchors(self.stc, True, True, True, True))
        self.stc.SetHelpText('The style preview window. Click or move the cursor over a spesific style to select the style for editing in the editors above.')
        EVT_LEFT_UP(self.stc, self.OnUpdateUI)
        EVT_KEY_UP(self.stc, self.OnUpdateUI)

        self.contextHelpButton1 = wxContextHelpButton(parent=self,
              pos=wxPoint(8, 423), size=wxSize(24, 24), style=wxBU_AUTODRAW)
        self.contextHelpButton1.SetConstraints(LayoutAnchors(self.contextHelpButton1,
              True, False, False, True))

        self.okBtn = wxButton(id=wxID_STCSTYLEEDITDLGOKBTN, label='OK',
              name='okBtn', parent=self, pos=wxPoint(282, 423), size=wxSize(75,
              23), style=0)
        self.okBtn.SetConstraints(LayoutAnchors(self.okBtn, False, False, True,
              True))
        self.okBtn.SetToolTipString('Save changes to the config file')
        EVT_BUTTON(self.okBtn, wxID_STCSTYLEEDITDLGOKBTN, self.OnOkbtnButton)

        self.cancelBtn = wxButton(id=wxID_STCSTYLEEDITDLGCANCELBTN,
              label='Cancel', name='cancelBtn', parent=self, pos=wxPoint(366,
              423), size=wxSize(75, 23), style=0)
        self.cancelBtn.SetConstraints(LayoutAnchors(self.cancelBtn, False,
              False, True, True))
        self.cancelBtn.SetToolTipString('Close dialog without saving changes')
        EVT_BUTTON(self.cancelBtn, wxID_STCSTYLEEDITDLGCANCELBTN,
              self.OnCancelbtnButton)

        self.staticText4 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT4,
              label='Face:', name='staticText4', parent=self.panel1,
              pos=wxPoint(0, 0), size=wxSize(48, 13), style=0)

        self.fixedWidthChk = wxCheckBox(id=wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK,
              label='', name='fixedWidthChk', parent=self.panel1, pos=wxPoint(0,
              23), size=wxSize(13, 19), style=0)
        self.fixedWidthChk.SetToolTipString('Check this for Fixed Width fonts')
        EVT_CHECKBOX(self.fixedWidthChk, wxID_STCSTYLEEDITDLGFIXEDWIDTHCHK,
              self.OnFixedwidthchkCheckbox)

        self.faceCb = wxComboBox(choices=[], id=wxID_STCSTYLEEDITDLGFACECB,
              name='faceCb', parent=self.panel1, pos=wxPoint(17, 18),
              size=wxSize(105, 21), style=0, validator=wxDefaultValidator,
              value='')

        self.staticText7 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT7,
              label='Size:', name='staticText7', parent=self.panel2,
              pos=wxPoint(0, 0), size=wxSize(40, 13), style=0)

        self.sizeCb = wxComboBox(choices=[], id=wxID_STCSTYLEEDITDLGSIZECB,
              name='sizeCb', parent=self.panel2, pos=wxPoint(0, 17),
              size=wxSize(91, 21), style=0, validator=wxDefaultValidator,
              value='')

        self.sizeOkBtn = wxButton(id=wxID_STCSTYLEEDITDLGSIZEOKBTN, label='ok',
              name='sizeOkBtn', parent=self.panel2, pos=wxPoint(90, 17),
              size=wxSize(21, 21), style=0)

        self.faceOkBtn = wxButton(id=wxID_STCSTYLEEDITDLGFACEOKBTN, label='ok',
              name='faceOkBtn', parent=self.panel1, pos=wxPoint(122, 18),
              size=wxSize(21, 21), style=0)

        self.fgColBtn = wxButton(id=wxID_STCSTYLEEDITDLGFGCOLBTN,
              label='Foreground', name='fgColBtn', parent=self.panel3,
              pos=wxPoint(8, 16), size=wxSize(72, 16), style=0)
        EVT_BUTTON(self.fgColBtn, wxID_STCSTYLEEDITDLGFGCOLBTN,
              self.OnFgcolbtnButton)

        self.fgColCb = wxComboBox(choices=[], id=wxID_STCSTYLEEDITDLGFGCOLCB,
              name='fgColCb', parent=self.panel3, pos=wxPoint(8, 32),
              size=wxSize(89, 21), style=0, validator=wxDefaultValidator,
              value='')

        self.fgColOkBtn = wxButton(id=wxID_STCSTYLEEDITDLGFGCOLOKBTN,
              label='ok', name='fgColOkBtn', parent=self.panel3, pos=wxPoint(96,
              32), size=wxSize(21, 21), style=0)

        self.staticText3 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT3,
              label='default', name='staticText3', parent=self.panel3,
              pos=wxPoint(100, 16), size=wxSize(37, 16), style=0)

        self.fgColDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGFGCOLDEFCB,
              label='checkBox1', name='fgColDefCb', parent=self.panel3,
              pos=wxPoint(120, 31), size=wxSize(16, 16), style=0)

        self.bgColBtn = wxButton(id=wxID_STCSTYLEEDITDLGBGCOLBTN,
              label='Background', name='bgColBtn', parent=self.panel3,
              pos=wxPoint(8, 56), size=wxSize(72, 16), style=0)
        EVT_BUTTON(self.bgColBtn, wxID_STCSTYLEEDITDLGBGCOLBTN,
              self.OnBgcolbtnButton)

        self.bgColCb = wxComboBox(choices=[], id=wxID_STCSTYLEEDITDLGBGCOLCB,
              name='bgColCb', parent=self.panel3, pos=wxPoint(8, 72),
              size=wxSize(89, 21), style=0, validator=wxDefaultValidator,
              value='')

        self.bgColOkBtn = wxButton(id=wxID_STCSTYLEEDITDLGBGCOLOKBTN,
              label='ok', name='bgColOkBtn', parent=self.panel3, pos=wxPoint(96,
              72), size=wxSize(21, 21), style=0)

        self.staticBox2 = wxStaticBox(id=wxID_STCSTYLEEDITDLGSTATICBOX2,
              label='Text attributes', name='staticBox2', parent=self.panel4,
              pos=wxPoint(0, 0), size=wxSize(112, 99), style=0)
        self.staticBox2.SetConstraints(LayoutAnchors(self.staticBox2, False,
              True, True, False))
        self.staticBox2.SetHelpText('Text attribute flags.')

        self.staticText2 = wxStaticText(id=wxID_STCSTYLEEDITDLGSTATICTEXT2,
              label='default', name='staticText2', parent=self.panel4,
              pos=wxPoint(68, 11), size=wxSize(37, 16), style=0)

        self.taBoldDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTABOLDDEFCB,
              label='checkBox1', name='taBoldDefCb', parent=self.panel4,
              pos=wxPoint(88, 27), size=wxSize(16, 16), style=0)

        self.taItalicDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAITALICDEFCB,
              label='checkBox1', name='taItalicDefCb', parent=self.panel4,
              pos=wxPoint(88, 43), size=wxSize(16, 16), style=0)

        self.taUnderlinedDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB,
              label='checkBox1', name='taUnderlinedDefCb', parent=self.panel4,
              pos=wxPoint(88, 59), size=wxSize(16, 16), style=0)

        self.taEOLfilledDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB,
              label='checkBox1', name='taEOLfilledDefCb', parent=self.panel4,
              pos=wxPoint(88, 75), size=wxSize(16, 16), style=0)

        self.taEOLfilledCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAEOLFILLEDCB,
              label='EOL filled', name='taEOLfilledCb', parent=self.panel4,
              pos=wxPoint(8, 75), size=wxSize(72, 16), style=0)
        EVT_CHECKBOX(self.taEOLfilledCb, wxID_STCSTYLEEDITDLGTAEOLFILLEDCB,
              self.OnTaeoffilledcbCheckbox)

        self.taUnderlinedCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAUNDERLINEDCB,
              label='Underlined', name='taUnderlinedCb', parent=self.panel4,
              pos=wxPoint(8, 59), size=wxSize(72, 16), style=0)
        EVT_CHECKBOX(self.taUnderlinedCb, wxID_STCSTYLEEDITDLGTAUNDERLINEDCB,
              self.OnTaunderlinedcbCheckbox)

        self.taItalicCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTAITALICCB,
              label='Italic', name='taItalicCb', parent=self.panel4,
              pos=wxPoint(8, 43), size=wxSize(72, 16), style=0)
        EVT_CHECKBOX(self.taItalicCb, wxID_STCSTYLEEDITDLGTAITALICCB,
              self.OnTaitaliccbCheckbox)

        self.taBoldCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTABOLDCB,
              label='Bold', name='taBoldCb', parent=self.panel4, pos=wxPoint(8,
              27), size=wxSize(72, 16), style=0)
        EVT_CHECKBOX(self.taBoldCb, wxID_STCSTYLEEDITDLGTABOLDCB,
              self.OnTaboldcbCheckbox)

        self.bgColDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGBGCOLDEFCB,
              label='checkBox1', name='bgColDefCb', parent=self.panel3,
              pos=wxPoint(120, 71), size=wxSize(16, 16), style=0)

        self.staticBox1 = wxStaticBox(id=wxID_STCSTYLEEDITDLGSTATICBOX1,
              label='Colour', name='staticBox1', parent=self.panel3,
              pos=wxPoint(0, 0), size=wxSize(142, 99), style=0)
        self.staticBox1.SetConstraints(LayoutAnchors(self.staticBox1, False,
              True, True, False))

        self.faceDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGFACEDEFCB,
              label='checkBox1', name='faceDefCb', parent=self.panel1,
              pos=wxPoint(120, 0), size=wxSize(16, 16), style=0)

        self.taSizeDefCb = wxCheckBox(id=wxID_STCSTYLEEDITDLGTASIZEDEFCB,
              label='checkBox1', name='taSizeDefCb', parent=self.panel2,
              pos=wxPoint(88, 0), size=wxSize(16, 16), style=0)

    def __init__(self, parent, langTitle, lang, configFile, STCsToUpdate=()):
        global commonPropDefs
        commonPropDefs = {'fore': '#888888',
                          'size': 8,
                          'face': wxSystemSettings_GetSystemFont(wxSYS_DEFAULT_GUI_FONT).GetFaceName()}

        self.stc_title = 'wxStyledTextCtrl Style Editor'
        self.stc_title = 'wxStyledTextCtrl Style Editor - %s' % langTitle
        if wxPlatform == '__WXMSW__':
            self.style_font_size = 8
        else:
            self.style_font_size = 10
        self._init_ctrls(parent)
        self.lang = lang
        self.configFile = configFile
        self.style = ''
        self.styleNum = 0
        self.names = []
        self.values = {}
        self.STCsToUpdate = STCsToUpdate
        self._blockUpdate = False


        for combo, okBtn, evtRet, evtCB, evtRDC in (
         (self.fgColCb, self.fgColOkBtn, self.OnfgColRet, self.OnfgColCombobox, self.OnGotoCommonDef),
         (self.bgColCb, self.bgColOkBtn, self.OnbgColRet, self.OnbgColCombobox, self.OnGotoCommonDef),
         (self.faceCb, self.faceOkBtn, self.OnfaceRet, self.OnfaceCombobox, self.OnGotoCommonDef),
         (self.sizeCb, self.sizeOkBtn, self.OnsizeRet, self.OnsizeCombobox, self.OnGotoCommonDef)):
            self.bindComboEvts(combo, okBtn, evtRet, evtCB, evtRDC)

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
        self.stc.SetMarginSensitive(margin, True)
        EVT_STC_MARGINCLICK(self.stc, wxID_STCSTYLEEDITDLGSTC, self.OnMarginClick)

        self.stc.SetUseTabs(False)
        self.stc.SetTabWidth(4)
        self.stc.SetIndentationGuides(True)
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
        self.allCtrls = [((self.fgColBtn, self.fgColCb, self.fgColOkBtn), self.fgColDefCb,
                             'fore', wxID_STCSTYLEEDITDLGFGCOLDEFCB),
                         ((self.bgColBtn, self.bgColCb, self.bgColOkBtn), self.bgColDefCb,
                             'back', wxID_STCSTYLEEDITDLGBGCOLDEFCB),
                         (self.taBoldCb, self.taBoldDefCb,
                             'bold', wxID_STCSTYLEEDITDLGTABOLDDEFCB),
                         (self.taItalicCb, self.taItalicDefCb,
                             'italic', wxID_STCSTYLEEDITDLGTAITALICDEFCB),
                         (self.taUnderlinedCb, self.taUnderlinedDefCb,
                             'underline', wxID_STCSTYLEEDITDLGTAUNDERLINEDDEFCB),
                         (self.taEOLfilledCb, self.taEOLfilledDefCb,
                             'eolfilled', wxID_STCSTYLEEDITDLGTAEOLFILLEDDEFCB),
                         ((self.sizeCb, self.sizeOkBtn), self.taSizeDefCb,
                             'size', wxID_STCSTYLEEDITDLGTASIZEDEFCB),
                         ((self.faceCb, self.faceOkBtn, self.fixedWidthChk), self.faceDefCb,
                             'face', wxID_STCSTYLEEDITDLGFACEDEFCB)]

        self.clearCtrls(disableDefs=True)
        # centralised default checkbox event handler
        self.chbIdMap = {}
        for ctrl, chb, prop, wid in self.allCtrls:
            self.chbIdMap[wid] = ctrl, chb, prop, wid
            EVT_CHECKBOX(chb, wid, self.OnDefaultCheckBox)
            chb.SetToolTipString('Toggle defaults')

        self.Center(wxBOTH)

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
            return True
        except KeyError, errkey:
            wxLogError('Name not found in Common definition, '\
                'please enter valid reference. (%s)'%errkey)
            self.restoreStyles(oldstyle)
            return False

#---Control population methods--------------------------------------------------
    def setStyles(self):
        if self._blockUpdate: return
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
                return True
        return False

    def rememberStyles(self):
        return self.names[:], copy.copy(self.values)

    def restoreStyles(self, style):
        self.names, self.values = style
        self.updateStyle()

    def clearCtrls(self, isDefault=False, disableDefs=False):
        self._blockUpdate = True
        try:
            for ctrl, chb, prop, wid in self.allCtrls:
                if prop in ('fore', 'back'):
                    cbtn, txt, btn = ctrl
                    cbtn.SetBackgroundColour(\
                          wxSystemSettings_GetSystemColour(wxSYS_COLOUR_BTNFACE))
                    cbtn.SetForegroundColour(wxColour(255, 255, 255))
                    cbtn.Enable(isDefault)
                    txt.SetValue('')
                    txt.Enable(isDefault)
                    btn.Enable(isDefault)
                elif prop == 'size':
                    cmb, btn = ctrl
                    cmb.SetValue('')
                    cmb.Enable(isDefault)
                    btn.Enable(isDefault)
                elif prop == 'face':
                    cmb, btn, chk = ctrl
                    cmb.SetValue('')
                    cmb.Enable(isDefault)
                    btn.Enable(isDefault)
                    chk.Enable(isDefault)
                    chk.SetValue(False)
                elif prop in ('bold', 'italic', 'underline', 'eolfilled'):
                    ctrl.SetValue(False)
                    ctrl.Enable(isDefault)

                chb.Enable(not isDefault and not disableDefs)
                chb.SetValue(True)
        finally:
            self._blockUpdate = False

    def populateProp(self, items, default, forceDisable=False):
        self._blockUpdate = True
        try:
            for name, val in items:
                if name:
                    ctrl, chb = self.getCtrlForProp(name)

                    if name in ('fore', 'back'):
                        cbtn, txt, btn = ctrl
                        repval = val%self.commonDefs
                        cbtn.SetBackgroundColour(strToCol(repval))
                        cbtn.SetForegroundColour(wxColour(0, 0, 0))
                        cbtn.Enable(not forceDisable)
                        txt.SetValue(val)
                        txt.Enable(not forceDisable)
                        btn.Enable(not forceDisable)
                        chb.SetValue(default)
                    elif name  == 'size':
                        cmb, btn = ctrl
                        cmb.SetValue(val)
                        cmb.Enable(not forceDisable)
                        btn.Enable(not forceDisable)
                        chb.SetValue(default)
                    elif name  == 'face':
                        cmb, btn, chk = ctrl
                        cmb.SetValue(val)
                        cmb.Enable(not forceDisable)
                        btn.Enable(not forceDisable)
                        chk.Enable(not forceDisable)
                        chb.SetValue(default)
                    elif name in ('bold', 'italic', 'underline', 'eolfilled'):
                        ctrl.Enable(not forceDisable)
                        ctrl.SetValue(True)
                        chb.SetValue(default)
        finally:
            self._blockUpdate = False

    def valIsCommonDef(self, val):
        return len(val) >= 5 and val[:2] == '%('

    def populateCtrls(self):
        self.clearCtrls(self.styleNum == wxSTC_STYLE_DEFAULT,
            disableDefs=self.styleNum < 0)

        # handle colour controls for settings
        if self.styleNum < 0:
            self.fgColDefCb.Enable(True)
            if self.styleNum == -1:
                self.bgColDefCb.Enable(True)

        # populate with default style
        self.populateProp(self.defValues.items(), True,
            self.styleNum != wxSTC_STYLE_DEFAULT)
        # override with current settings
        self.populateProp(self.values.items(), False)

    def getCommonDefPropType(self, commonDefName):
        val = self.commonDefs[commonDefName]
        if type(val) == type(0): return 'size'
        if len(val) == 7 and val[0] == '#': return 'fore'
        return 'face'

    def bindComboEvts(self, combo, btn, btnEvtMeth, comboEvtMeth, rdclickEvtMeth):
        EVT_COMBOBOX(combo, combo.GetId(), comboEvtMeth)
        EVT_BUTTON(btn, btn.GetId(), btnEvtMeth)
        EVT_RIGHT_DCLICK(combo, rdclickEvtMeth)
        combo.SetToolTipString('Select from list or click "ok" button on the right to change a manual entry, right double-click \n'\
            'the drop down button to select Common definition in the Style Editor (if applicable)')
        btn.SetToolTipString('Accept value')

    def populateCombosWithCommonDefs(self, fixedWidthOnly=None):
        self._blockUpdate = True
        try:
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
        finally:
            self._blockUpdate = False

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
                self.elementLb.InsertItems([name, '----Language----'], 0)
                self.elementLb.Append('----Standard----')
                stdStart = stdPos = self.elementLb.GetCount()
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
        self.elementLb.Append('----Settings----')
        settings = settingsIdNames.items()
        settings.sort();settings.reverse()
        for num, name in settings:
            self.elementLb.Append(name)
            self.styleNumLookup[name] = num

        # add definitions
        self.elementLb.Append('----Common----')
        self.commonDefsStartIdx = self.elementLb.GetCount()
        for common in self.commonDefs.keys():
            tpe = type(self.commonDefs[common])
            self.elementLb.Append('%('+common+')'+(tpe is type('') and 's' or 'd'))
            self.styleNumLookup[common] = num

#---Colour methods--------------------------------------------------------------
    def getColourDlg(self, colour, title=''):
        data = wxColourData()
        data.SetColour(colour)
        data.SetChooseFull(True)
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
            self.editProp(True, prop, colStr)

    def OnFgcolbtnButton(self, event):
        self.editColProp(self.fgColBtn, self.fgColCb, 'fore')

    def OnBgcolbtnButton(self, event):
        self.editColProp(self.bgColBtn, self.bgColCb, 'back')

    def editColTCProp(self, colCb, colBtn, prop, val=None):
        if val is None:
            colStr = colCb.GetValue()
        else:
            colStr = val
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

    def OnfgColCombobox(self, event):
        if self._blockUpdate: return
        try: self.editColTCProp(self.fgColCb, self.fgColBtn, 'fore', event.GetString())
        except AssertionError: wxLogError('Not a valid colour value')

    def OnbgColRet(self, event):
        try: self.editColTCProp(self.bgColCb, self.bgColBtn, 'back')
        except AssertionError: wxLogError('Not a valid colour value')

    def OnbgColCombobox(self, event):
        if self._blockUpdate: return
        try: self.editColTCProp(self.bgColCb, self.bgColBtn, 'back', event.GetString())
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
                self.elementLb.SetSelection(idx, True)
                self.OnElementlbListbox(None)

    def OnfaceRet(self, event):
        self.setFace(self.faceCb.GetValue())

    def OnfaceCombobox(self, event):
        if self._blockUpdate: return
        self.setFace(event.GetString())

    def setFace(self, val):
        try: val%self.commonDefs
        except KeyError: wxLogError('Invalid common definition')
        else: self.editProp(val!='', 'face', val)

    def OnsizeRet(self, event):
        self.setSize(self.sizeCb.GetValue())

    def OnsizeCombobox(self, event):
        if self._blockUpdate: return
        self.setSize(event.GetString())

    def setSize(self, val):
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
            self.clearCtrls(disableDefs=True)
            if prop == 'fore':
                self.fgColBtn.Enable(True)
                self.fgColCb.Enable(True)
                self.fgColOkBtn.Enable(True)
            elif prop == 'face':
                self.faceCb.Enable(True)
                self.fixedWidthChk.Enable(True)
                self.faceOkBtn.Enable(True)
            elif prop == 'size':
                self.sizeCb.Enable(True)
                self.sizeOkBtn.Enable(True)

            commonDefVal = str(self.commonDefs[common])
            self.styleDefST.SetLabel(commonDefVal)
            self.populateProp( [(prop, commonDefVal)], True)

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
            self.clearCtrls(disableDefs=True)
            if styleIdent:
                self.styleDefST.SetLabel(styleCategoryDescriptions[styleIdent])

        self.populateCombosWithCommonDefs()

    def OnDefaultCheckBox(self, event):
        if self.chbIdMap.has_key(event.GetId()):
            ctrl, chb, prop, wid = self.chbIdMap[event.GetId()]
            restore = not event.IsChecked()
            if prop in ('fore', 'back'):
                cbtn, cmb, btn = ctrl
                cbtn.Enable(restore)
                cmb.Enable(restore)
                btn.Enable(restore)
                if restore:
                    colStr = cmb.GetValue()
                    #if prop == 'fore': colStr = self.fgColCb.GetValue()
                    #else: colStr = self.bgColCb.GetValue()
                    if colStr: self.editProp(True, prop, colStr)
                else:
                    self.editProp(False, prop)
            elif prop  == 'size':
                cmb, btn = ctrl
                val = cmb.GetValue()
                if val: self.editProp(restore, prop, val)
                cmb.Enable(restore)
                btn.Enable(restore)
            elif prop  == 'face':
                cmb, btn, chk = ctrl
                val = cmb.GetStringSelection()
                if val: self.editProp(restore, prop, val)
                cmb.Enable(restore)
                btn.Enable(restore)
                chk.Enable(restore)
            elif prop in ('bold', 'italic', 'underline', 'eolfilled'):
                ctrl.Enable(restore)
                if ctrl.GetValue(): self.editProp(restore, prop)

    def OnOkbtnButton(self, event):
        # write styles and common defs to the config
        wxBeginBusyCursor()
        try:
            writeStylesToConfig(self.config, 'style.%s'%self.lang, self.styles)
            self.config.SetPath('')
            self.config.Write(commonDefsFile, `self.commonDefs`)
            self.config.Flush()

            for stc in self.STCsToUpdate:
                setSTCStyles(stc, self.styles, self.styleIdNames, self.commonDefs,
                      self.lang, self.lexer, self.keywords)
        finally:
            wxEndBusyCursor()
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
                    self.elementLb.SetSelection(self.elementLb.GetCount()-1, True)
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
            if selIdx == self.elementLb.GetCount():
                selIdx = selIdx - 1
            self.elementLb.SetSelection(selIdx, True)
            self.OnElementlbListbox(None)

#---STC events------------------------------------------------------------------
    def OnUpdateUI(self, event):
        styleBefore = self.stc.GetStyleAt(self.stc.GetCurrentPos())
        if self.styleIdNames.has_key(styleBefore):
            self.elementLb.SetStringSelection(self.styleIdNames[styleBefore],
                  True)
        else:
            self.elementLb.SetSelection(0, False)
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
        self.elementLb.SetStringSelection('Line numbers', True)
        self.OnElementlbListbox(None)


#---Common definition dialog----------------------------------------------------

[wxID_COMMONDEFDLG, wxID_COMMONDEFDLGCANCELBTN, wxID_COMMONDEFDLGCOMDEFNAMETC, wxID_COMMONDEFDLGOKBTN, wxID_COMMONDEFDLGPROPTYPERBX, wxID_COMMONDEFDLGSTATICBOX1] = map(lambda _init_ctrls: wxNewId(), range(6))

class CommonDefDlg(wxDialog):
    def _init_ctrls(self, prnt):
        wxDialog.__init__(self, id = wxID_COMMONDEFDLG, name = 'CommonDefDlg', parent = prnt, pos = wxPoint(398, 249), size = wxSize(192, 220), style = wxDEFAULT_DIALOG_STYLE, title = 'Common definition')
        self.SetClientSize(wxSize(184, 200))

        self.propTypeRBx = wxRadioBox(choices = ['Colour value', 'Font face', 'Size value'], id = wxID_COMMONDEFDLGPROPTYPERBX, label = 'Property type', majorDimension = 1, name = 'propTypeRBx', parent = self, point = wxPoint(8, 8), size = wxSize(168, 92), style = wxRA_SPECIFY_COLS, validator = wxDefaultValidator)
        self.propTypeRBx.SetSelection(self._propTypeIdx)

        self.staticBox1 = wxStaticBox(id = wxID_COMMONDEFDLGSTATICBOX1, label = 'Name', name = 'staticBox1', parent = self, pos = wxPoint(8, 108), size = wxSize(168, 46), style = 0)

        self.comDefNameTC = wxTextCtrl(id = wxID_COMMONDEFDLGCOMDEFNAMETC, name = 'comDefNameTC', parent = self, pos = wxPoint(16, 124), size = wxSize(152, 21), style = 0, value = '')
        self.comDefNameTC.SetLabel(self._comDefName)

        self.okBtn = wxButton(id = wxID_COMMONDEFDLGOKBTN, label = 'OK', name = 'okBtn', parent = self, pos = wxPoint(8, 164), size = wxSize(80, 23), style = 0)
        EVT_BUTTON(self.okBtn, wxID_COMMONDEFDLGOKBTN, self.OnOkbtnButton)

        self.cancelBtn = wxButton(id = wxID_COMMONDEFDLGCANCELBTN, label = 'Cancel', name = 'cancelBtn', parent = self, pos = wxPoint(96, 164), size = wxSize(80, 23), style = 0)
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
        stc.SetSelForeground(True, strToCol(values['fore']))
    if 'back' in names:
        stc.SetSelBackground(True, strToCol(values['back']))

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
    #wxLogMessage('Set style')
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

    # Set background colour to reduce flashing effect on refresh or page switch
    bkCol = None
    if styleDict.has_key(0): prop = styleDict[0]
    else: prop = styleDict[wxSTC_STYLE_DEFAULT]
    names, vals = parseProp(prop)
    if 'back' in names:
        bkCol = strToCol(vals['back'])
    if bkCol is None:
        bkCol = wxWHITE
    stc.SetBackgroundColour(bkCol)

    # Set the styles on the wxSTC
#    stc.Show(False)
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
#    stc.Show(True)

    return newStyles, styleDict, styleNumIdxMap

#---Config reading and writing -------------------------------------------------
commonDefsFile = 'common.defs.%s'%(wxPlatform == '__WXMSW__' and 'msw' or 'gtk')

def readPyValFromConfig(conf, name):
    return eval(string.replace(conf.Read(name), '\r\n', '\n')+'\n')

def initFromConfig(configFile, lang):
    cfg = wxFileConfig(localFilename=configFile, style=wxCONFIG_USE_LOCAL_FILE)
    cfg.SetExpandEnvVars(False)

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
    commonDefs = readPyValFromConfig(cfg, commonDefsFile)
    assert type(commonDefs) is type({}), \
          'Common definitions (%s) not a valid dict'%commonDefsFile

    commonStyleIdNames = readPyValFromConfig(cfg, 'common.styleidnames')
    assert type(commonStyleIdNames) is type({}), \
          'Common definitions (%s) not a valid dict'%'common.styleidnames'

    # Lang spesific settings
    cfg.SetPath(lang)
    styleIdNames = readPyValFromConfig(cfg, 'styleidnames')
    assert type(commonStyleIdNames) is type({}), \
          'Not a valid dict [%s] styleidnames)'%lang
    styleIdNames.update(commonStyleIdNames)
    braceInfo = readPyValFromConfig(cfg, 'braces')
    assert type(commonStyleIdNames) is type({}), \
          'Not a valid dict [%s] braces)'%lang

    displaySrc = cfg.Read('displaysrc')
    lexer = readPyValFromConfig(cfg, 'lexer')
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
    from wxPython.help import *

    app = wxPySimpleApp()

    provider = wxSimpleHelpProvider()
    wxHelpProvider_Set(provider)

    base = os.path.split(__file__)[0]
    config = os.path.abspath(os.path.join(base, 'stc-styles.rc.cfg'))
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
        f.Show(True)
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
