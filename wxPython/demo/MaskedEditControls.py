from wxPython.wx import *
from wxPython.lib.maskededit import Field, wxMaskedTextCtrl, wxMaskedComboBox, wxIpAddrCtrl, states, months
from wxPython.lib.maskededit import __doc__ as overviewdoc
from wxPython.lib.scrolledpanel import wxScrolledPanel
import string

class demoMixin:
    """
    Centralized routines common to demo pages, to remove repetition.
    """
    def labelGeneralTable(self, sizer):
        description = wxStaticText( self, -1, "Description", )
        mask        = wxStaticText( self, -1, "Mask Value" )
        formatcode  = wxStaticText( self, -1, "Format" )
        regex       = wxStaticText( self, -1, "Regexp\nValidator(opt.)" )
        ctrl        = wxStaticText( self, -1, "wxMaskedEdit Ctrl" )

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
            control.SetMaskParameters(**kwargs)
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
with this control.  Note that all controls have been auto-sized by including
F in the format code.

Try entering nonsensical or partial values in validated fields to see what
happens.  Note that the State and Last Name fields are list-limited (valid
last names are: Smith, Jones, Williams).  Signs on numbers can be toggled
with the minus key.
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
        self.changeControlParams( event, "emptyBackgroundColor", "Blue", "White" )

    def onShowFill( self, event ):
        """ Set fillChar parameter to '?' or ' ' """
        self.changeControlParams( event, "fillChar", '?', ' ' )


class demoPage2(wxScrolledPanel, demoMixin):
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
       ("Integer (signed)",         "#{6}",          "", 'F-_R',      "",                                 '','', '0     '),
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



class demoPage3( wxScrolledPanel, demoMixin ):
    def __init__( self, parent, log ):
        self.log = log
        wxScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wxBoxSizer( wxVERTICAL )

        label = wxStaticText( self, -1, """\
All these controls have been created by passing a single parameter, the autoformat code.
The class contains an internal dictionary of types and formats (autoformats).
To see a great example of validations in action, try entering a bad email address,
then tab out.""")

        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wxALIGN_LEFT|wxALL, 5 )

        description = wxStaticText( self, -1, "Description")
        autofmt     = wxStaticText( self, -1, "AutoFormat Code")
        ctrl        = wxStaticText( self, -1, "wxMaskedEdit Control")

        description.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        autofmt.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )
        ctrl.SetFont( wxFont( 9, wxSWISS, wxNORMAL, wxBOLD ) )

        grid = wxFlexGridSizer( 0, 3, vgap=10, hgap=5 )
        grid.Add( description, 0, wxALIGN_LEFT )
        grid.Add( autofmt,     0, wxALIGN_LEFT )
        grid.Add( ctrl,        0, wxALIGN_LEFT )

        controls = [
        #description        autoformat code
        ("Phone No w/opt. ext","USPHONEFULLEXT"),
        ("Phone No only", "USPHONEFULL"),
        ("US Date + Time","USDATETIMEMMDDYYYY/HHMMSS"),
        ("US Date + Time\n(without seconds)","USDATETIMEMMDDYYYY/HHMM"),
        ("US Date + Military Time","USDATEMILTIMEMMDDYYYY/HHMMSS"),
        ("US Date + Military Time\n(without seconds)","USDATEMILTIMEMMDDYYYY/HHMM"),
        ("US Date MMDDYYYY","USDATEMMDDYYYY/"),
        ("Time", "TIMEHHMMSS"),
        ("Time\n(without seconds)", "TIMEHHMM"),
        ("Military Time", "MILTIMEHHMMSS"),
        ("Military Time\n(without seconds)", "MILTIMEHHMM"),
        ("European Date", "EUDATEYYYYMMDD/"),
        ("Social Sec#","USSOCIALSEC"),
        ("Credit Card","CREDITCARD"),
        ("Expiration MM/YY","EXPDATEMMYY"),
        ("Percentage","PERCENT"),
        ("Person's Age","AGE"),
        ("US State", "USSTATE"),
        ("US Zip Code","USZIP"),
        ("US Zip+4","USZIPPLUS4"),
        ("Age", "AGE"),
        ("Email Address","EMAIL"),
        ]
        for control in controls:
            grid.Add( wxStaticText( self, -1, control[0]), 0, wxALIGN_LEFT )
            grid.Add( wxStaticText( self, -1, control[1]), 0, wxALIGN_LEFT )
            grid.Add( wxMaskedTextCtrl( self, -1, "",
                                              autoformat       = control[1],
                                              demo             = True,
                                              name             = control[1]),
                            0, wxALIGN_LEFT )

        self.sizer.Add( grid, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout( 1 )
        self.SetupScrolling()


class demoPage4( wxScrolledPanel, demoMixin ):
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
        ctrl         = wxStaticText( self, -1, "wxMaskedEdit Control" )

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


class demoPage5( wxScrolledPanel, demoMixin ):
    def __init__( self, parent, log ):
        self.log = log
        wxScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wxBoxSizer( wxVERTICAL )
        label = wxStaticText( self, -1, """\
These are examples of wxMaskedComboBox and wxIpAddrCtrl, and a more
useful configuration of a wxMaskedTextCtrl for floating point input.
""")
        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wxALIGN_LEFT|wxALL, 5 )

        numerators = [ str(i) for i in range(1, 4) ]
        denominators = [ string.ljust(str(i), 2) for i in [2,3,4,5,8,16,32,64] ]
        fieldsDict = {0: Field(choices=numerators, choiceRequired=False),
                      1: Field(choices=denominators, choiceRequired=True)}
        choices = []
        for n in numerators:
            for d in denominators:
                if n != d:
                    choices.append( '%s/%s' % (n,d) )


        text1 = wxStaticText( self, -1, """\
A masked ComboBox for fraction selection.
Choices for each side of the fraction can be
selected with PageUp/Down:""")

        fraction = wxMaskedComboBox( self, -1, "",
                                     choices = choices,
                                     choiceRequired = True,
                                     mask = "#/##",
                                     formatcodes = "F_",
                                     validRegex = "^\d\/\d\d?",
                                     fields = fieldsDict )


        text2 = wxStaticText( self, -1, """
A masked ComboBox to validate
text from a list of numeric codes:""")

        choices = ["91", "136", "305", "4579"]
        code = wxMaskedComboBox( self, -1, choices[0],
                                 choices = choices,
                                 choiceRequired = True,
                                 formatcodes = "F_r",
                                 mask = "####")


        text3 = wxStaticText( self, -1, """\
A masked state selector; only "legal" values
can be entered:""")

        state = wxMaskedComboBox( self, -1, states[0],
                                  choices = states,
                                  autoformat="USSTATE")

        text4 = wxStaticText( self, -1, "An empty IP Address entry control:")
        ip_addr1 = wxIpAddrCtrl( self, -1, style = wxTE_PROCESS_TAB )


        text5 = wxStaticText( self, -1, "An IP Address control with a restricted mask:")
        ip_addr2 = wxIpAddrCtrl( self, -1, mask=" 10.  1.109.###" )


        text6 = wxStaticText( self, -1, """\
An IP Address control with restricted choices
of form: 10. (1|2) . (129..255) . (0..255)""")
        ip_addr3 = wxIpAddrCtrl( self, -1, mask=" 10.  #.###.###")
        ip_addr3.SetFieldParameters(0, validRegex="1|2" )   # requires entry to match or not allowed


        # This allows any value in penultimate field, but colors anything outside of the range invalid:
        ip_addr3.SetFieldParameters(1, validRange=(129,255), validRequired=False )

        text7 = wxStaticText( self, -1, """\
A floating point entry control
with right-insert for ordinal:""")
        floatctrl = wxMaskedTextCtrl(self, -1, mask="#{9}.#{2}", formatcodes="F_-R")
        floatctrl.SetFieldParameters(0, formatcodes='r,<', validRequired=True)  # right-insert, allow commas, require explicit cursor movement to change fields
        floatctrl.SetFieldParameters(1, defaultValue='00')                      # don't allow blank fraction

        grid = wxFlexGridSizer( 0, 2, vgap=20, hgap = 5 )
        grid.Add( text1, 0, wxALIGN_LEFT )
        grid.Add( fraction, 0, wxALIGN_LEFT )
        grid.Add( text2, 0, wxALIGN_LEFT )
        grid.Add( code, 0, wxALIGN_LEFT )
        grid.Add( text3, 0, wxALIGN_LEFT )
        grid.Add( state, 0, wxALIGN_LEFT )
        grid.Add( text4, 0, wxALIGN_LEFT )
        grid.Add( ip_addr1, 0, wxALIGN_LEFT )
        grid.Add( text5, 0, wxALIGN_LEFT )
        grid.Add( ip_addr2, 0, wxALIGN_LEFT )
        grid.Add( text6, 0, wxALIGN_LEFT )
        grid.Add( ip_addr3, 0, wxALIGN_LEFT )
        grid.Add( text7, 0, wxALIGN_LEFT )
        grid.Add( floatctrl, 0, wxALIGN_LEFT )

        self.sizer.Add( grid, 0, wxALIGN_LEFT|wxALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout(1)
        self.SetupScrolling()

        EVT_COMBOBOX( self, fraction.GetId(), self.OnComboChange )
        EVT_COMBOBOX( self, code.GetId(), self.OnComboChange )
        EVT_COMBOBOX( self, state.GetId(), self.OnComboChange )
        EVT_TEXT( self, fraction.GetId(), self.OnComboChange )
        EVT_TEXT( self, code.GetId(), self.OnComboChange )
        EVT_TEXT( self, state.GetId(), self.OnComboChange )

        EVT_TEXT( self, ip_addr1.GetId(), self.OnIpAddrChange )
        EVT_TEXT( self, ip_addr2.GetId(), self.OnIpAddrChange )
        EVT_TEXT( self, ip_addr3.GetId(), self.OnIpAddrChange )
        EVT_TEXT( self, floatctrl.GetId(), self.OnTextChange )


    def OnComboChange( self, event ):
        ctl = self.FindWindowById( event.GetId() )
        if not ctl.IsValid():
            self.log.write('current value not a valid choice')


    def OnIpAddrChange( self, event ):
        ip_addr = self.FindWindowById( event.GetId() )
        if ip_addr.IsValid():
            self.log.write('new addr = %s\n' % ip_addr.GetAddress() )

    def OnTextChange( self, event ):
        ctl = self.FindWindowById( event.GetId() )
        if ctl.IsValid():
            self.log.write('new value = %s\n' % ctl.GetValue() )

# ---------------------------------------------------------------------
class TestMaskedTextCtrls(wxNotebook):
    def __init__(self, parent, id, log):
        wxNotebook.__init__(self, parent, id)
        self.log = log

        win = demoPage1(self, log)
        self.AddPage(win, "General examples")

        win = demoPage2(self, log)
        self.AddPage(win, "Using default values")

        win = demoPage3(self, log)
        self.AddPage(win, 'Auto-formatted controls')

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
""" + overviewdoc + """
</FONT></PRE>
"""

if __name__ == "__main__":
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
