from wxPython.wx import *
from wxPython.lib.maskededit import Field, wxMaskedTextCtrl, wxMaskedComboBox, wxIpAddrCtrl, states, state_names, months
from wxPython.lib.maskededit import __doc__ as maskededit_doc
from wxPython.lib.maskededit import autoformats
from wxPython.lib.maskedctrl import wxMaskedCtrl, controlTypes, MASKEDCOMBO
from wxPython.lib.scrolledpanel import wxScrolledPanel
import string, sys, traceback


class demoMixin:
    """
    Centralized routines common to demo pages, to remove repetition.
    """
    def labelGeneralTable(self, sizer):
        description = wxStaticText( self, -1, "Description", )
        mask        = wxStaticText( self, -1, "Mask Value" )
        formatcode  = wxStaticText( self, -1, "Format" )
        regex       = wxStaticText( self, -1, "Regexp Validator(opt.)" )
        ctrl        = wxStaticText( self, -1, "wxMaskedTextCtrl" )

        description.SetFont( wxFont(9, wxSWISS, wxNORMAL, wxBOLD))
        mask.SetFont( wxFont(9, wxSWISS, wxNORMAL, wxBOLD))
        formatcode.SetFont( wxFont(9, wxSWISS, wxNORMAL, wxBOLD) )
        regex.SetFont( wxFont(9, wxSWISS, wxNORMAL, wxBOLD))
        ctrl.SetFont( wxFont(9, wxSWISS, wxNORMAL, wxBOLD))

        sizer.Add(description)
        sizer.Add(mask)
        sizer.Add(formatcode)
        sizer.Add(regex)
        sizer.Add(ctrl)


    def layoutGeneralTable(self, controls, sizer):
        for control in controls:
            sizer.Add( wxStaticText( self, -1, control[0]) )
            sizer.Add( wxStaticText( self, -1, control[1]) )
            sizer.Add( wxStaticText( self, -1, control[3]) )
            sizer.Add( wxStaticText( self, -1, control[4]) )

            if control in controls:
                newControl  = wxMaskedTextCtrl( self, -1, "",
                                                mask         = control[1],
                                                excludeChars = control[2],
                                                formatcodes  = control[3],
                                                includeChars = "",
                                                validRegex   = control[4],
                                                validRange   = control[5],
                                                choices      = control[6],
                                                choiceRequired = True,
                                                defaultValue = control[7],
                                                demo         = True,
                                                name         = control[0])
            self.editList.append(newControl)
            sizer.Add(newControl)


    def changeControlParams(self, event, parameter, checked_value, notchecked_value):
        if event.Checked(): value = checked_value
        else:               value = notchecked_value
        kwargs = {parameter: value}
        for control in self.editList:
            control.SetCtrlParameters(**kwargs)
            control.Refresh()
        self.Refresh()



#----------------------------------------------------------------------------
class demoPage1(wxScrolledPanel, demoMixin):
    def __init__(self, parent, log):
        wxScrolledPanel.__init__(self, parent, -1)
        self.sizer = wxBoxSizer( wxVERTICAL )
        self.editList  = []

        label = wxStaticText( self, -1, """\
Here are some basic wxMaskedTextCtrls to give you an idea of what you can do
with this control.  Note that all controls have been auto-sized by including 'F' in
the format codes.

Try entering nonsensical or partial values in validated fields to see what happens.
Note that the State and Last Name fields are list-limited (valid last names are:
Smith, Jones, Williams).  Signs on numbers can be toggled with the minus key.
""")
        label.SetForegroundColour( "Blue" )
        header = wxBoxSizer( wxHORIZONTAL )
        header.Add( label, 0, flag=wxALIGN_LEFT|wxALL, border = 5 )

        highlight = wxCheckBox( self, -1, "Highlight Empty" )
        disallow =  wxCheckBox( self, -1, "Disallow Empty" )
        showFill = wxCheckBox( self, -1, "change fillChar" )

        vbox = wxBoxSizer( wxVERTICAL )
        vbox.Add( highlight, 0, wxALIGN_LEFT|wxALL, 5 )
        vbox.Add( disallow, 0, wxALIGN_LEFT|wxALL, 5 )
        vbox.Add( showFill, 0, wxALIGN_LEFT|wxALL, 5 )
        header.AddSpacer(15, 0)
        header.Add(vbox, 0, flag=wxALIGN_LEFT|wxALL, border=5 )

        EVT_CHECKBOX( self, highlight.GetId(), self.onHighlightEmpty )
        EVT_CHECKBOX( self, disallow.GetId(), self.onDisallowEmpty )
        EVT_CHECKBOX( self, showFill.GetId(), self.onShowFill )

        grid = wxFlexGridSizer( 0, 5, vgap=10, hgap=10 )
        self.labelGeneralTable(grid)

        # The following list is of the controls for the demo. Feel free to play around with
        # the options!
        controls = [
        #description        mask                    excl format     regexp                              range,list,initial
       ("Phone No",         "(###) ###-#### x:###", "", 'F^-',     "^\(\d{3}\) \d{3}-\d{4}",           '','',''),
       ("Social Sec#",      "###-##-####",          "", 'F',        "\d{3}-\d{2}-\d{4}",                '','',''),
       ("Full Name",        "C{14}",                "", 'F_',       '^[A-Z][a-zA-Z]+ [A-Z][a-zA-Z]+',   '','',''),
       ("Last Name Only",   "C{14}",                "", 'F {list}', '^[A-Z][a-zA-Z]+',                  '',('Smith','Jones','Williams'),''),
       ("Zip plus 4",       "#{5}-#{4}",            "", 'F',        "\d{5}-(\s{4}|\d{4})",              '','',''),
       ("Customer No",      "\CAA-###",             "", 'F!',       "C[A-Z]{2}-\d{3}",                  '','',''),
       ("Invoice Total",    "#{9}.##",              "", 'F-_,',     "",                                 '','',''),
       ("Integer",          "#{9}",                 "", 'F-_',      "",                                 '','',''),
       ]

        self.layoutGeneralTable(controls, grid)
        self.sizer.Add( header, 0, flag=wxALIGN_LEFT|wxALL, border=5 )
        self.sizer.Add( grid, 0, flag= wxALIGN_LEFT|wxLEFT, border=5 )
        self.SetSizer(self.sizer)
        self.SetupScrolling()
        self.SetAutoLayout(1)


    def onDisallowEmpty( self, event ):
        """ Set emptyInvalid parameter on/off """
        self.changeControlParams( event, "emptyInvalid", True, False )

    def onHighlightEmpty( self, event ):
        """ Highlight empty values"""
        self.changeControlParams( event, "emptyBackgroundColour", "Blue", "White" )

    def onShowFill( self, event ):
        """ Set fillChar parameter to '?' or ' ' """
        self.changeControlParams( event, "fillChar", '?', ' ' )


class demoPage2(wxScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        wxScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wxBoxSizer( wxVERTICAL )

        label = wxStaticText( self, -1, """\
All these controls have been created by passing a single parameter, the autoformat code,
and use the factory class wxMaskedCtrl with its default controlType.
The maskededit module contains an internal dictionary of types and formats (autoformats).
Many of these already do complicated validation; To see some examples, try
29 Feb 2002 vs. 2004 for the date formats, or email address validation.
""")

        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wxALIGN_LEFT|wxALL, 5 )

        description = wxStaticText( self, -1, "Description")
        autofmt     = wxStaticText( self, -1, "AutoFormat Code")
        ctrl        = wxStaticText( self, -1, "wxMaskedCtrl")

        description.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        autofmt.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        ctrl.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )

        grid = wxFlexGridSizer( 0, 3, vgap=10, hgap=5 )
        grid.Add( description, 0, wxALIGN_LEFT )
        grid.Add( autofmt,     0, wxALIGN_LEFT )
        grid.Add( ctrl,        0, wxALIGN_LEFT )

        for autoformat, desc in autoformats:
            grid.Add( wxStaticText( self, -1, desc), 0, wxALIGN_LEFT )
            grid.Add( wxStaticText( self, -1, autoformat), 0, wxALIGN_LEFT )
            grid.Add( wxMaskedCtrl( self, -1, "",
                                    autoformat       = autoformat,
                                    demo             = True,
                                    name             = autoformat),
                            0, wxALIGN_LEFT )

        self.sizer.Add( grid, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout( 1 )
        self.SetupScrolling()


class demoPage3(wxScrolledPanel, demoMixin):
    def __init__(self, parent, log):
        self.log = log
        wxScrolledPanel.__init__(self, parent, -1)
        self.sizer = wxBoxSizer( wxVERTICAL )
        self.editList  = []

        label = wxStaticText( self, -1, """\
Here wxMaskedTextCtrls that have default values.  The states
control has a list of valid values, and the unsigned integer
has a legal range specified.
""")
        label.SetForegroundColour( "Blue" )
        requireValid =  wxCheckBox( self, -1, "Require Valid Value" )
        EVT_CHECKBOX( self, requireValid.GetId(), self.onRequireValid )

        header = wxBoxSizer( wxHORIZONTAL )
        header.Add( label, 0, flag=wxALIGN_LEFT|wxALL, border = 5)
        header.AddSpacer(75, 0)
        header.Add( requireValid, 0, flag=wxALIGN_LEFT|wxALL, border=10 )

        grid = wxFlexGridSizer( 0, 5, vgap=10, hgap=10 )
        self.labelGeneralTable( grid )

        controls = [
        #description        mask                    excl format     regexp                              range,list,initial
       ("U.S. State (2 char)",      "AA",            "", 'F!_',       "[A-Z]{2}",                         '',states, states[0]),
       ("Integer (signed)",         "#{6}",          "", 'F-_',       "",                                 '','', ' 0    '),
       ("Integer (unsigned)\n(1-399)","######",      "", 'F_',        "",                                 (1,399),'', '1     '),
       ("Float (signed)",           "#{6}.#{9}",     "", 'F-_R',      "",                                 '','', '000000.000000000'),
       ("Date (MDY) + Time",        "##/##/#### ##:##:## AM",  'BCDEFGHIJKLMNOQRSTUVWXYZ','DF!',"",          '','', wxDateTime_Now().Format("%m/%d/%Y %I:%M:%S %p")),
       ]
        self.layoutGeneralTable( controls, grid )

        self.sizer.Add( header, 0, flag=wxALIGN_LEFT|wxALL, border=5 )
        self.sizer.Add( grid, 0, flag=wxALIGN_LEFT|wxALL, border=5 )

        self.SetSizer( self.sizer )
        self.SetAutoLayout( 1 )
        self.SetupScrolling()

    def onRequireValid( self, event ):
        """ Set validRequired parameter on/off """
        self.changeControlParams( event, "validRequired", True, False )


class demoPage4(wxScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        wxScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wxBoxSizer( wxVERTICAL )

        label = wxStaticText( self, -1, """\
These controls have field-specific choice lists and allow autocompletion.

Down arrow or Page Down in an uncompleted field with an auto-completable field will attempt
to auto-complete a field if it has a choice list.
Page Down and Shift-Down arrow will also auto-complete, or cycle through the complete list.
Page Up and Shift-Up arrow will similarly cycle backwards through the list.
""")

        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wxALIGN_LEFT|wxALL, 5 )

        description  = wxStaticText( self, -1, "Description" )
        autofmt      = wxStaticText( self, -1, "AutoFormat Code" )
        fields       = wxStaticText( self, -1, "Field Objects" )
        ctrl         = wxStaticText( self, -1, "wxMaskedTextCtrl" )

        description.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        autofmt.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        fields.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        ctrl.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )

        grid = wxFlexGridSizer( 0, 4, vgap=10, hgap=10 )
        grid.Add( description, 0, wxALIGN_LEFT )
        grid.Add( autofmt,     0, wxALIGN_LEFT )
        grid.Add( fields,      0, wxALIGN_LEFT )
        grid.Add( ctrl,        0, wxALIGN_LEFT )

        autoformat = "USPHONEFULLEXT"
        fieldsDict = {0: Field(choices=["617","781","508","978","413"], choiceRequired=True)}
        fieldsLabel = """\
{0: Field(choices=[
            "617","781",
            "508","978","413"],
          choiceRequired=True)}"""
        grid.Add( wxStaticText( self, -1, "Restricted Area Code"), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, autoformat), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, fieldsLabel), 0, wxALIGN_LEFT )
        grid.Add( wxMaskedTextCtrl( self, -1, "",
                                    autoformat       = autoformat,
                                    fields           = fieldsDict,
                                    demo             = True,
                                    name             = autoformat),
                  0, wxALIGN_LEFT )

        autoformat = "EXPDATEMMYY"
        fieldsDict = {1: Field(choices=["03", "04", "05"], choiceRequired=True)}
        fieldsLabel = """\
{1: Field(choices=[
            "03", "04", "05"],
          choiceRequired=True)}"""
        exp =  wxMaskedTextCtrl( self, -1, "",
                                 autoformat       = autoformat,
                                 fields           = fieldsDict,
                                 demo             = True,
                                 name             = autoformat)

        grid.Add( wxStaticText( self, -1, "Restricted Expiration"), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, autoformat), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, fieldsLabel), 0, wxALIGN_LEFT )
        grid.Add( exp, 0, wxALIGN_LEFT )

        fieldsDict = {0: Field(choices=["02134","02155"], choiceRequired=True),
                      1: Field(choices=["1234", "5678"],  choiceRequired=False)}
        fieldsLabel = """\
{0: Field(choices=["02134","02155"],
          choiceRequired=True),
 1: Field(choices=["1234", "5678"],
          choiceRequired=False)}"""
        autoformat = "USZIPPLUS4"
        zip =  wxMaskedTextCtrl( self, -1, "",
                                 autoformat       = autoformat,
                                 fields           = fieldsDict,
                                 demo             = True,
                                 name             = autoformat)

        grid.Add( wxStaticText( self, -1, "Restricted Zip + 4"), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, autoformat), 0, wxALIGN_LEFT )
        grid.Add( wxStaticText( self, -1, fieldsLabel), 0, wxALIGN_LEFT )
        grid.Add( zip, 0, wxALIGN_LEFT )

        self.sizer.Add( grid, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout(1)
        self.SetupScrolling()


class demoPage5(wxScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        wxScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wxBoxSizer( wxVERTICAL )


        labelMaskedCombos = wxStaticText( self, -1, """\
These are some examples of wxMaskedComboBox:""")
        labelMaskedCombos.SetForegroundColour( "Blue" )


        label_statecode = wxStaticText( self, -1, """\
A state selector; only
"legal" values can be
entered:""")
        statecode = wxMaskedComboBox( self, -1, states[0],
                                  choices = states,
                                  autoformat="USSTATE")

        label_statename = wxStaticText( self, -1, """\
A state name selector,
with auto-select:""")

        # Create this one using factory function:
        statename = wxMaskedCtrl( self, -1, state_names[0],
                                  controlType = controlTypes.MASKEDCOMBO,
                                  choices = state_names,
                                  autoformat="USSTATENAME",
                                  autoSelect=True)
        statename.SetCtrlParameters(formatcodes = 'F!V_')


        numerators = [ str(i) for i in range(1, 4) ]
        denominators = [ string.ljust(str(i), 2) for i in [2,3,4,5,8,16,32,64] ]
        fieldsDict = {0: Field(choices=numerators, choiceRequired=False),
                      1: Field(choices=denominators, choiceRequired=True)}
        choices = []
        for n in numerators:
            for d in denominators:
                if n != d:
                    choices.append( '%s/%s' % (n,d) )


        label_fraction = wxStaticText( self, -1, """\
A masked ComboBox for fraction selection.
Choices for each side of the fraction can
be selected with PageUp/Down:""")

        fraction = wxMaskedCtrl( self, -1, "",
                                 controlType = MASKEDCOMBO,
                                 choices = choices,
                                 choiceRequired = True,
                                 mask = "#/##",
                                 formatcodes = "F_",
                                 validRegex = "^\d\/\d\d?",
                                 fields = fieldsDict )


        label_code = wxStaticText( self, -1, """\
A masked ComboBox to validate
text from a list of numeric codes:""")

        choices = ["91", "136", "305", "4579"]
        code = wxMaskedComboBox( self, -1, choices[0],
                                 choices = choices,
                                 choiceRequired = True,
                                 formatcodes = "F_r",
                                 mask = "####")

        label_selector = wxStaticText( self, -1, """\
Programmatically set
choice sets:""")
        self.list_selector = wxComboBox(self, -1, '', choices = ['list1', 'list2', 'list3'])
        self.dynamicbox = wxMaskedCtrl( self, -1, '    ',
                                      controlType = controlTypes.MASKEDCOMBO,
                                      mask =    'XXXX',
                                      formatcodes = 'F_',
                                      # these are to give dropdown some initial height,
                                      # as base control apparently only sets that size
                                      # during initial construction <sigh>:
                                      choices = ['', '1', '2', '3', '4', '5'] )

        self.dynamicbox.Clear()   # get rid of initial choices used to size the dropdown


        labelIpAddrs = wxStaticText( self, -1, """\
Here are some examples of wxIpAddrCtrl, a control derived from wxMaskedTextCtrl:""")
        labelIpAddrs.SetForegroundColour( "Blue" )


        label_ipaddr1 = wxStaticText( self, -1, "An empty control:")
        ipaddr1 = wxIpAddrCtrl( self, -1, style = wxTE_PROCESS_TAB )


        label_ipaddr2 = wxStaticText( self, -1, "A restricted mask:")
        ipaddr2 = wxIpAddrCtrl( self, -1, mask=" 10.  1.109.###" )


        label_ipaddr3 = wxStaticText( self, -1, """\
A control with restricted legal values:
10. (1|2) . (129..255) . (0..255)""")
        ipaddr3 = wxMaskedCtrl( self, -1,
                                controlType = controlTypes.IPADDR,
                                mask=" 10.  #.###.###")
        ipaddr3.SetFieldParameters(0, validRegex="1|2",validRequired=False )   # requires entry to match or not allowed

        # This allows any value in penultimate field, but colors anything outside of the range invalid:
        ipaddr3.SetFieldParameters(1, validRange=(129,255), validRequired=False )



        labelNumerics = wxStaticText( self, -1, """\
Here are some useful configurations of a wxMaskedTextCtrl for integer and floating point input that still treat
the control as a text control.  (For a true numeric control, check out the wxMaskedNumCtrl class!)""")
        labelNumerics.SetForegroundColour( "Blue" )

        label_intctrl1 = wxStaticText( self, -1, """\
An integer entry control with
shifting insert enabled:""")
        self.intctrl1 = wxMaskedTextCtrl(self, -1, name='intctrl', mask="#{9}", formatcodes = '_-,F>')
        label_intctrl2 = wxStaticText( self, -1, """\
     Right-insert integer entry:""")
        self.intctrl2 = wxMaskedTextCtrl(self, -1, name='intctrl', mask="#{9}", formatcodes = '_-,Fr')

        label_floatctrl = wxStaticText( self, -1, """\
A floating point entry control
with right-insert for ordinal:""")
        self.floatctrl = wxMaskedTextCtrl(self, -1, name='floatctrl', mask="#{9}.#{2}", formatcodes="F,_-R", useParensForNegatives=False)
        self.floatctrl.SetFieldParameters(0, formatcodes='r<', validRequired=True)  # right-insert, require explicit cursor movement to change fields
        self.floatctrl.SetFieldParameters(1, defaultValue='00')                     # don't allow blank fraction

        label_numselect = wxStaticText( self, -1, """\
<= Programmatically set the value
     of the float entry ctrl:""")
        numselect = wxComboBox(self, -1, choices = [ '', '111', '222.22', '-3', '54321.666666666', '-1353.978',
                                                     '1234567', '-1234567', '123456789', '-123456789.1',
                                                     '1234567890.', '-1234567890.1' ])

        parens_check = wxCheckBox(self, -1, "Use () to indicate negatives in above controls")



        gridCombos = wxFlexGridSizer( 0, 4, vgap=10, hgap = 10 )
        gridCombos.Add( label_statecode, 0, wxALIGN_LEFT )
        gridCombos.Add( statecode, 0, wxALIGN_LEFT )
        gridCombos.Add( label_fraction, 0, wxALIGN_LEFT )
        gridCombos.Add( fraction, 0, wxALIGN_LEFT )
        gridCombos.Add( label_statename, 0, wxALIGN_LEFT )
        gridCombos.Add( statename, 0, wxALIGN_LEFT )
        gridCombos.Add( label_code, 0, wxALIGN_LEFT )
        gridCombos.Add( code, 0, wxALIGN_LEFT )
        gridCombos.Add( label_selector, 0, wxALIGN_LEFT)
        hbox = wxBoxSizer( wxHORIZONTAL )
        hbox.Add( self.list_selector, 0, wxALIGN_LEFT )
        hbox.Add(wxStaticText(self, -1, ' => '), 0, wxALIGN_LEFT)
        hbox.Add( self.dynamicbox, 0, wxALIGN_LEFT )
        gridCombos.Add( hbox, 0, wxALIGN_LEFT )

        gridIpAddrs = wxFlexGridSizer( 0, 4, vgap=10, hgap = 15 )
        gridIpAddrs.Add( label_ipaddr1, 0, wxALIGN_LEFT )
        gridIpAddrs.Add( ipaddr1, 0, wxALIGN_LEFT )
        gridIpAddrs.Add( label_ipaddr2, 0, wxALIGN_LEFT )
        gridIpAddrs.Add( ipaddr2, 0, wxALIGN_LEFT )
        gridIpAddrs.Add( label_ipaddr3, 0, wxALIGN_LEFT )
        gridIpAddrs.Add( ipaddr3, 0, wxALIGN_LEFT )

        gridNumerics = wxFlexGridSizer( 0, 4, vgap=10, hgap = 10 )
        gridNumerics.Add( label_intctrl1, 0, wxALIGN_LEFT )
        gridNumerics.Add( self.intctrl1, 0, wxALIGN_LEFT )
        gridNumerics.Add( label_intctrl2, 0, wxALIGN_RIGHT )
        gridNumerics.Add( self.intctrl2, 0, wxALIGN_LEFT )
        gridNumerics.Add( label_floatctrl, 0, wxALIGN_LEFT )
        gridNumerics.Add( self.floatctrl, 0, wxALIGN_LEFT )
        gridNumerics.Add( label_numselect, 0, wxALIGN_RIGHT )
        gridNumerics.Add( numselect, 0, wxALIGN_LEFT )

        self.sizer.Add( labelMaskedCombos, 0, wxALIGN_LEFT|wxALL, 5 )
        self.sizer.Add( gridCombos, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.sizer.Add( wxStaticLine(self, -1), 0, wxEXPAND|wxTOP|wxBOTTOM, border=8 )
        self.sizer.Add( labelIpAddrs, 0, wxALIGN_LEFT|wxALL, 5 )
        self.sizer.Add( gridIpAddrs, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.sizer.Add( wxStaticLine(self, -1), 0, wxEXPAND|wxTOP|wxBOTTOM, border=8 )
        self.sizer.Add( labelNumerics, 0, wxALIGN_LEFT|wxALL, 5 )
        self.sizer.Add( gridNumerics, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.sizer.Add( parens_check, 0, wxALIGN_LEFT|wxALL, 5 )

        self.SetSizer( self.sizer )
        self.SetAutoLayout(1)
        self.SetupScrolling()

        EVT_COMBOBOX( self, fraction.GetId(), self.OnComboSelection )
        EVT_COMBOBOX( self, code.GetId(), self.OnComboSelection )
        EVT_COMBOBOX( self, statecode.GetId(), self.OnComboSelection )
        EVT_COMBOBOX( self, statename.GetId(), self.OnComboSelection )
        EVT_TEXT( self, fraction.GetId(), self.OnTextChange )
        EVT_TEXT( self, code.GetId(), self.OnTextChange )
        EVT_TEXT( self, statecode.GetId(), self.OnTextChange )
        EVT_TEXT( self, statename.GetId(), self.OnTextChange )
        EVT_COMBOBOX( self, self.list_selector.GetId(), self.OnListSelection )

        EVT_TEXT( self, self.intctrl1.GetId(), self.OnTextChange )
        EVT_TEXT( self, self.intctrl2.GetId(), self.OnTextChange )
        EVT_TEXT( self, self.floatctrl.GetId(), self.OnTextChange )
        EVT_COMBOBOX( self, numselect.GetId(), self.OnNumberSelect )
        EVT_CHECKBOX( self, parens_check.GetId(), self.OnParensCheck )

        EVT_TEXT( self, ipaddr1.GetId(), self.OnIpAddrChange )
        EVT_TEXT( self, ipaddr2.GetId(), self.OnIpAddrChange )
        EVT_TEXT( self, ipaddr3.GetId(), self.OnIpAddrChange )




    def OnComboSelection( self, event ):
        ctl = self.FindWindowById( event.GetId() )
        if not ctl.IsValid():
            self.log.write('current value not a valid choice')
        self.log.write('new value = %s' % ctl.GetValue())

    def OnTextChange( self, event ):
        ctl = self.FindWindowById( event.GetId() )
        if ctl.IsValid():
            self.log.write('new value = %s\n' % ctl.GetValue() )

    def OnNumberSelect( self, event ):
        value = event.GetString()
        # Format choice to fit into format for #{9}.#{2}, with sign position reserved:
        # (ordinal + fraction == 11 + decimal point + sign == 13)
        if value:
            floattext = "%13.2f" % float(value)
        else:
            floattext = value   # clear the value again
        try:
            self.floatctrl.SetValue(floattext)
        except:
            type, value, tb = sys.exc_info()
            for line in traceback.format_exception_only(type, value):
                self.log.write(line)

    def OnParensCheck( self, event ):
        self.intctrl1.SetCtrlParameters(useParensForNegatives=event.Checked())
        self.intctrl2.SetCtrlParameters(useParensForNegatives=event.Checked())
        self.floatctrl.SetCtrlParameters(useParensForNegatives=event.Checked())

    def OnIpAddrChange( self, event ):
        ipaddr = self.FindWindowById( event.GetId() )
        if ipaddr.IsValid():
            self.log.write('new addr = %s\n' % ipaddr.GetAddress() )

    def OnListSelection( self, event ):
        list = self.list_selector.GetStringSelection()
        formatcodes = 'F_'
        if list == 'list1':
            choices = ['abc', 'defg', 'hi']
            mask = 'aaaa'
        elif list == 'list2':
            choices = ['1', '2', '34', '567']
            formatcodes += 'r'
            mask = '###'
        else:
            choices = states
            mask = 'AA'
            formatcodes += '!'
        self.dynamicbox.SetCtrlParameters( mask = mask,
                                           choices = choices,
                                           choiceRequired=True,
                                           autoSelect=True,
                                           formatcodes=formatcodes)
        self.dynamicbox.SetValue(choices[0])

# ---------------------------------------------------------------------
class TestMaskedTextCtrls(wxNotebook):
    def __init__(self, parent, id, log):
        wxNotebook.__init__(self, parent, id)
        self.log = log

        win = demoPage1(self, log)
        self.AddPage(win, "General examples")

        win = demoPage2(self, log)
        self.AddPage(win, 'Auto-formatted controls')

        win = demoPage3(self, log)
        self.AddPage(win, "Using default values")

        win = demoPage4(self, log)
        self.AddPage(win, 'Using auto-complete fields')

        win = demoPage5(self, log)
        self.AddPage(win, 'Other masked controls')


#----------------------------------------------------------------------------

def runTest(frame, nb, log):
    testWin = TestMaskedTextCtrls(nb, -1, log)
    return testWin

def RunStandalone():
    app = wxPySimpleApp()
    frame = wxFrame(None, -1, "Test wxMaskedTextCtrl", size=(640, 480))
    win = TestMaskedTextCtrls(frame, -1, sys.stdout)
    frame.Show(True)
    app.MainLoop()
#----------------------------------------------------------------------------
if __name__ == "__main__":
    RunStandalone()


overview = """<html>
<PRE><FONT SIZE=-1>
""" + maskededit_doc + """
</FONT></PRE>
"""

if __name__ == "__main__":
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
