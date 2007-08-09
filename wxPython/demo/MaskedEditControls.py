
import  string
import  sys
import  traceback

import  wx
import  wx.lib.masked             as  masked
import  wx.lib.scrolledpanel      as  scroll


class demoMixin:
    """
    Centralized routines common to demo pages, to remove repetition.
    """
    def labelGeneralTable(self, sizer):
        description = wx.StaticText( self, -1, "Description", )
        mask        = wx.StaticText( self, -1, "Mask Value" )
        formatcode  = wx.StaticText( self, -1, "Format" )
        regex       = wx.StaticText( self, -1, "Regexp Validator(opt.)" )
        ctrl        = wx.StaticText( self, -1, "Masked TextCtrl" )

        description.SetFont( wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD))
        mask.SetFont( wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD))
        formatcode.SetFont( wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD) )
        regex.SetFont( wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD))
        ctrl.SetFont( wx.Font(9, wx.SWISS, wx.NORMAL, wx.BOLD))

        sizer.Add(description)
        sizer.Add(mask)
        sizer.Add(formatcode)
        sizer.Add(regex)
        sizer.Add(ctrl)


    def layoutGeneralTable(self, controls, sizer):
        for control in controls:
            sizer.Add( wx.StaticText( self, -1, control[0]) )
            sizer.Add( wx.StaticText( self, -1, control[1]) )
            sizer.Add( wx.StaticText( self, -1, control[3]) )
            sizer.Add( wx.StaticText( self, -1, control[4]) )

            if control in controls:
                newControl  = masked.TextCtrl( self, -1, "",
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
        if event.IsChecked(): value = checked_value
        else:               value = notchecked_value

        kwargs = {parameter: value}

        for control in self.editList:
            control.SetCtrlParameters(**kwargs)
            control.Refresh()

        self.Refresh()



#----------------------------------------------------------------------------
class demoPage1(scroll.ScrolledPanel, demoMixin):
    def __init__(self, parent, log):
        scroll.ScrolledPanel.__init__(self, parent, -1)
        self.sizer = wx.BoxSizer( wx.VERTICAL )
        self.editList  = []

        label = wx.StaticText( self, -1, """\
Here are some basic masked TextCtrls to give you an idea of what you can do
with this control.  Note that all controls have been auto-sized by including 'F' in
the format codes.

Try entering nonsensical or partial values in validated fields to see what happens.
Note that the State and Last Name fields are list-limited (valid last names are:
Smith, Jones, Williams).  Signs on numbers can be toggled with the minus key.
""")
        label.SetForegroundColour( "Blue" )
        header = wx.BoxSizer( wx.HORIZONTAL )
        header.Add( label, 0, flag=wx.ALIGN_LEFT|wx.ALL, border = 5 )

        highlight = wx.CheckBox( self, -1, "Highlight Empty" )
        disallow =  wx.CheckBox( self, -1, "Disallow Empty" )
        showFill = wx.CheckBox( self, -1, "change fillChar" )

        vbox = wx.BoxSizer( wx.VERTICAL )
        vbox.Add( highlight, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        vbox.Add( disallow, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        vbox.Add( showFill, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        header.Add((15, 0))
        header.Add(vbox, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=5 )

        self.Bind(wx.EVT_CHECKBOX, self.onHighlightEmpty, id=highlight.GetId())
        self.Bind(wx.EVT_CHECKBOX, self.onDisallowEmpty, id=disallow.GetId())
        self.Bind(wx.EVT_CHECKBOX, self.onShowFill, id=showFill.GetId())

        grid = wx.FlexGridSizer( 0, 5, vgap=10, hgap=10 )
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
        self.sizer.Add( header, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.sizer.Add( grid, 0, flag= wx.ALIGN_LEFT|wx.LEFT, border=5 )
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


class demoPage2(scroll.ScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        scroll.ScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wx.BoxSizer( wx.VERTICAL )

        label = wx.StaticText( self, -1, """\
All these controls have been created by passing a single parameter, the autoformat code,
and use the factory class masked.Ctrl with its default controlType.
The masked package contains an internal dictionary of types and formats (autoformats).
Many of these already do complicated validation; To see some examples, try
29 Feb 2002 vs. 2004 for the date formats, or email address validation.
""")

        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        description = wx.StaticText( self, -1, "Description")
        autofmt     = wx.StaticText( self, -1, "AutoFormat Code")
        ctrl        = wx.StaticText( self, -1, "Masked Ctrl")

        description.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )
        autofmt.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )
        ctrl.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )

        grid = wx.FlexGridSizer( 0, 3, vgap=10, hgap=5 )
        grid.Add( description, 0, wx.ALIGN_LEFT )
        grid.Add( autofmt,     0, wx.ALIGN_LEFT )
        grid.Add( ctrl,        0, wx.ALIGN_LEFT )

        for autoformat, desc in masked.autoformats:
            grid.Add( wx.StaticText( self, -1, desc), 0, wx.ALIGN_LEFT )
            grid.Add( wx.StaticText( self, -1, autoformat), 0, wx.ALIGN_LEFT )
            grid.Add( masked.Ctrl( self, -1, "",
                                    autoformat       = autoformat,
                                    demo             = True,
                                    name             = autoformat),
                            0, wx.ALIGN_LEFT )

        self.sizer.Add( grid, 0, wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout( 1 )
        self.SetupScrolling()


class demoPage3(scroll.ScrolledPanel, demoMixin):
    def __init__(self, parent, log):
        self.log = log
        scroll.ScrolledPanel.__init__(self, parent, -1)
        self.sizer = wx.BoxSizer( wx.VERTICAL )
        self.editList  = []

        label = wx.StaticText( self, -1, """\
Here masked TextCtrls that have default values.  The states
control has a list of valid values, and the unsigned integer
has a legal range specified.
""")
        label.SetForegroundColour( "Blue" )
        requireValid =  wx.CheckBox( self, -1, "Require Valid Value" )
        self.Bind(wx.EVT_CHECKBOX, self.onRequireValid, id=requireValid.GetId())

        header = wx.BoxSizer( wx.HORIZONTAL )
        header.Add( label, 0, flag=wx.ALIGN_LEFT|wx.ALL, border = 5)
        header.Add((75, 0))
        header.Add( requireValid, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=10 )

        grid = wx.FlexGridSizer( 0, 5, vgap=10, hgap=10 )
        self.labelGeneralTable( grid )

        controls = [
        #description        mask                    excl format     regexp                              range,list,initial
       ("U.S. State (2 char)",      "AA",            "", 'F!_',       "[A-Z]{2}",                         '', masked.states, masked.states[0]),
       ("Integer (signed)",         "#{6}",          "", 'F-_',       "",                                 '','', ' 0    '),
       ("Integer (unsigned)\n(1-399)","######",      "", 'F_',        "",                                 (1,399),'', '1     '),
       ("Float (signed)",           "#{6}.#{9}",     "", 'F-_R',      "",                                 '','', '000000.000000000'),
       ("Date (MDY) + Time",        "##/##/#### ##:##:## AM",  'BCDEFGHIJKLMNOQRSTUVWXYZ','DF!',"",          '','', wx.DateTime_Now().Format("%m/%d/%Y %I:%M:%S %p")),
       ]
        self.layoutGeneralTable( controls, grid )

        self.sizer.Add( header, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.sizer.Add( grid, 0, flag=wx.ALIGN_LEFT|wx.ALL, border=5 )

        self.SetSizer( self.sizer )
        self.SetAutoLayout( 1 )
        self.SetupScrolling()

    def onRequireValid( self, event ):
        """ Set validRequired parameter on/off """
        self.changeControlParams( event, "validRequired", True, False )


class demoPage4(scroll.ScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        scroll.ScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wx.BoxSizer( wx.VERTICAL )

        label = wx.StaticText( self, -1, """\
These controls have field-specific choice lists and allow autocompletion.

Down arrow or Page Down in an uncompleted field with an auto-completable field will attempt
to auto-complete a field if it has a choice list.
Page Down and Shift-Down arrow will also auto-complete, or cycle through the complete list.
Page Up and Shift-Up arrow will similarly cycle backwards through the list.
""")

        label.SetForegroundColour( "Blue" )
        self.sizer.Add( label, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        description  = wx.StaticText( self, -1, "Description" )
        autofmt      = wx.StaticText( self, -1, "AutoFormat Code" )
        fields       = wx.StaticText( self, -1, "Field Objects" )
        ctrl         = wx.StaticText( self, -1, "Masked TextCtrl" )

        description.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )
        autofmt.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )
        fields.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )
        ctrl.SetFont( wx.Font( 9, wx.SWISS, wx.NORMAL, wx.BOLD ) )

        grid = wx.FlexGridSizer( 0, 4, vgap=10, hgap=10 )
        grid.Add( description, 0, wx.ALIGN_LEFT )
        grid.Add( autofmt,     0, wx.ALIGN_LEFT )
        grid.Add( fields,      0, wx.ALIGN_LEFT )
        grid.Add( ctrl,        0, wx.ALIGN_LEFT )

        autoformat = "USPHONEFULLEXT"
        fieldsDict = {0: masked.Field(choices=["617","781","508","978","413"], choiceRequired=True)}
        fieldsLabel = """\
{0: Field(choices=[
            "617","781",
            "508","978","413"],
          choiceRequired=True)}"""
        grid.Add( wx.StaticText( self, -1, "Restricted Area Code"), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, autoformat), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, fieldsLabel), 0, wx.ALIGN_LEFT )
        grid.Add( masked.TextCtrl( self, -1, "",
                                    autoformat       = autoformat,
                                    fields           = fieldsDict,
                                    demo             = True,
                                    name             = autoformat),
                  0, wx.ALIGN_LEFT )

        autoformat = "EXPDATEMMYY"
        fieldsDict = {1: masked.Field(choices=["03", "04", "05"], choiceRequired=True)}
        fieldsLabel = """\
{1: Field(choices=[
            "03", "04", "05"],
          choiceRequired=True)}"""
        exp =  masked.TextCtrl( self, -1, "",
                                 autoformat       = autoformat,
                                 fields           = fieldsDict,
                                 demo             = True,
                                 name             = autoformat)

        grid.Add( wx.StaticText( self, -1, "Restricted Expiration"), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, autoformat), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, fieldsLabel), 0, wx.ALIGN_LEFT )
        grid.Add( exp, 0, wx.ALIGN_LEFT )

        fieldsDict = {0: masked.Field(choices=["02134","02155"], choiceRequired=True),
                      1: masked.Field(choices=["1234", "5678"],  choiceRequired=False)}
        fieldsLabel = """\
{0: Field(choices=["02134","02155"],
          choiceRequired=True),
 1: Field(choices=["1234", "5678"],
          choiceRequired=False)}"""
        autoformat = "USZIPPLUS4"
        zip =  masked.TextCtrl( self, -1, "",
                                 autoformat       = autoformat,
                                 fields           = fieldsDict,
                                 demo             = True,
                                 name             = autoformat)

        grid.Add( wx.StaticText( self, -1, "Restricted Zip + 4"), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, autoformat), 0, wx.ALIGN_LEFT )
        grid.Add( wx.StaticText( self, -1, fieldsLabel), 0, wx.ALIGN_LEFT )
        grid.Add( zip, 0, wx.ALIGN_LEFT )

        self.sizer.Add( grid, 0, wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.SetSizer( self.sizer )
        self.SetAutoLayout(1)
        self.SetupScrolling()


class demoPage5(scroll.ScrolledPanel, demoMixin):
    def __init__( self, parent, log ):
        self.log = log
        scroll.ScrolledPanel.__init__( self, parent, -1 )
        self.sizer = wx.BoxSizer( wx.VERTICAL )


        labelMaskedCombos = wx.StaticText( self, -1, """\
These are some examples of masked.ComboBox:""")
        labelMaskedCombos.SetForegroundColour( "Blue" )


        label_statecode = wx.StaticText( self, -1, """\
A state selector; only
"legal" values can be
entered:""")
        statecode = masked.ComboBox( self, -1, masked.states[0],
                                  choices = masked.states,
                                  autoformat="USSTATE")

        label_statename = wx.StaticText( self, -1, """\
A state name selector,
with auto-select:""")

        # Create this one using factory function:
        statename = masked.Ctrl( self, -1, masked.state_names[0],
                                  controlType = masked.controlTypes.COMBO,
                                  choices = masked.state_names,
                                  autoformat="USSTATENAME",
                                  autoSelect=True)
        statename.SetCtrlParameters(formatcodes = 'F!V_')


        numerators = [ str(i) for i in range(1, 4) ]
        denominators = [ string.ljust(str(i), 2) for i in [2,3,4,5,8,16,32,64] ]
        fieldsDict = {0: masked.Field(choices=numerators, choiceRequired=False),
                      1: masked.Field(choices=denominators, choiceRequired=True)}
        choices = []
        for n in numerators:
            for d in denominators:
                if n != d:
                    choices.append( '%s/%s' % (n,d) )


        label_fraction = wx.StaticText( self, -1, """\
A masked ComboBox for fraction selection.
Choices for each side of the fraction can
be selected with PageUp/Down:""")

        fraction = masked.Ctrl( self, -1, "",
                                 controlType = masked.controlTypes.COMBO,
                                 choices = choices,
                                 choiceRequired = True,
                                 mask = "#/##",
                                 formatcodes = "F_",
                                 validRegex = "^\d\/\d\d?",
                                 fields = fieldsDict )


        label_code = wx.StaticText( self, -1, """\
A masked ComboBox to validate
text from a list of numeric codes:""")

        choices = ["91", "136", "305", "4579"]
        code = masked.ComboBox( self, -1, choices[0],
                                 choices = choices,
                                 choiceRequired = True,
                                 formatcodes = "F_r",
                                 mask = "####")

        label_selector = wx.StaticText( self, -1, """\
Programmatically set
choice sets:""")
        self.list_selector = wx.ComboBox(self, -1, '', choices = ['list1', 'list2', 'list3'])
        self.dynamicbox = masked.Ctrl( self, -1, '    ',
                                      controlType = masked.controlTypes.COMBO,
                                      mask =    'XXXX',
                                      formatcodes = 'F_',
                                      # these are to give dropdown some initial height,
                                      # as base control apparently only sets that size
                                      # during initial construction <sigh>:
                                      choices = ['', '1', '2', '3', '4', '5'] )

        self.dynamicbox.Clear()   # get rid of initial choices used to size the dropdown


        labelIpAddrs = wx.StaticText( self, -1, """\
Here are some examples of IpAddrCtrl, a control derived from masked.TextCtrl:""")
        labelIpAddrs.SetForegroundColour( "Blue" )


        label_ipaddr1 = wx.StaticText( self, -1, "An empty control:")
        ipaddr1 = masked.IpAddrCtrl( self, -1, style = wx.TE_PROCESS_TAB )


        label_ipaddr2 = wx.StaticText( self, -1, "A restricted mask:")
        ipaddr2 = masked.IpAddrCtrl( self, -1, mask=" 10.  1.109.###" )


        label_ipaddr3 = wx.StaticText( self, -1, """\
A control with restricted legal values:
10. (1|2) . (129..255) . (0..255)""")
        ipaddr3 = masked.Ctrl( self, -1,
                                controlType = masked.controlTypes.IPADDR,
                                mask=" 10.  #.###.###")
        ipaddr3.SetFieldParameters(0, validRegex="1|2",validRequired=False )   # requires entry to match or not allowed

        # This allows any value in penultimate field, but colors anything outside of the range invalid:
        ipaddr3.SetFieldParameters(1, validRange=(129,255), validRequired=False )



        labelNumerics = wx.StaticText( self, -1, """\
Here are some useful configurations of a masked.TextCtrl for integer and floating point input that still treat
the control as a text control.  (For a true numeric control, check out the masked.NumCtrl class!)""")
        labelNumerics.SetForegroundColour( "Blue" )

        label_intctrl1 = wx.StaticText( self, -1, """\
An integer entry control with
shifting insert enabled:""")
        self.intctrl1 = masked.TextCtrl(self, -1, name='intctrl', mask="#{9}", formatcodes = '_-,F>')
        label_intctrl2 = wx.StaticText( self, -1, """\
     Right-insert integer entry:""")
        self.intctrl2 = masked.TextCtrl(self, -1, name='intctrl', mask="#{9}", formatcodes = '_-,Fr')

        label_floatctrl = wx.StaticText( self, -1, """\
A floating point entry control
with right-insert for ordinal:""")
        self.floatctrl = masked.TextCtrl(self, -1, name='floatctrl', mask="#{9}.#{2}", formatcodes="F,_-R", useParensForNegatives=False)
        self.floatctrl.SetFieldParameters(0, formatcodes='r<', validRequired=True)  # right-insert, require explicit cursor movement to change fields
        self.floatctrl.SetFieldParameters(1, defaultValue='00')                     # don't allow blank fraction

        label_numselect = wx.StaticText( self, -1, """\
<= Programmatically set the value
     of the float entry ctrl:""")
        numselect = wx.ComboBox(self, -1, choices = [ '', '111', '222.22', '-3', '54321.666666666', '-1353.978',
                                                     '1234567', '-1234567', '123456789', '-123456789.1',
                                                     '1234567890.', '-1234567890.1' ])

        parens_check = wx.CheckBox(self, -1, "Use () to indicate negatives in above controls")



        gridCombos = wx.FlexGridSizer( 0, 4, vgap=10, hgap = 10 )
        gridCombos.Add( label_statecode, 0, wx.ALIGN_LEFT )
        gridCombos.Add( statecode, 0, wx.ALIGN_LEFT )
        gridCombos.Add( label_fraction, 0, wx.ALIGN_LEFT )
        gridCombos.Add( fraction, 0, wx.ALIGN_LEFT )
        gridCombos.Add( label_statename, 0, wx.ALIGN_LEFT )
        gridCombos.Add( statename, 0, wx.ALIGN_LEFT )
        gridCombos.Add( label_code, 0, wx.ALIGN_LEFT )
        gridCombos.Add( code, 0, wx.ALIGN_LEFT )
        gridCombos.Add( label_selector, 0, wx.ALIGN_LEFT)
        hbox = wx.BoxSizer( wx.HORIZONTAL )
        hbox.Add( self.list_selector, 0, wx.ALIGN_LEFT )
        hbox.Add(wx.StaticText(self, -1, ' => '), 0, wx.ALIGN_LEFT)
        hbox.Add( self.dynamicbox, 0, wx.ALIGN_LEFT )
        gridCombos.Add( hbox, 0, wx.ALIGN_LEFT )

        gridIpAddrs = wx.FlexGridSizer( 0, 4, vgap=10, hgap = 15 )
        gridIpAddrs.Add( label_ipaddr1, 0, wx.ALIGN_LEFT )
        gridIpAddrs.Add( ipaddr1, 0, wx.ALIGN_LEFT )
        gridIpAddrs.Add( label_ipaddr2, 0, wx.ALIGN_LEFT )
        gridIpAddrs.Add( ipaddr2, 0, wx.ALIGN_LEFT )
        gridIpAddrs.Add( label_ipaddr3, 0, wx.ALIGN_LEFT )
        gridIpAddrs.Add( ipaddr3, 0, wx.ALIGN_LEFT )

        gridNumerics = wx.FlexGridSizer( 0, 4, vgap=10, hgap = 10 )
        gridNumerics.Add( label_intctrl1, 0, wx.ALIGN_LEFT )
        gridNumerics.Add( self.intctrl1, 0, wx.ALIGN_LEFT )
        gridNumerics.Add( label_intctrl2, 0, wx.ALIGN_RIGHT )
        gridNumerics.Add( self.intctrl2, 0, wx.ALIGN_LEFT )
        gridNumerics.Add( label_floatctrl, 0, wx.ALIGN_LEFT )
        gridNumerics.Add( self.floatctrl, 0, wx.ALIGN_LEFT )
        gridNumerics.Add( label_numselect, 0, wx.ALIGN_RIGHT )
        gridNumerics.Add( numselect, 0, wx.ALIGN_LEFT )

        self.sizer.Add( labelMaskedCombos, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        self.sizer.Add( gridCombos, 0, wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.sizer.Add( wx.StaticLine(self, -1), 0, wx.EXPAND|wx.TOP|wx.BOTTOM, border=8 )
        self.sizer.Add( labelIpAddrs, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        self.sizer.Add( gridIpAddrs, 0, wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.sizer.Add( wx.StaticLine(self, -1), 0, wx.EXPAND|wx.TOP|wx.BOTTOM, border=8 )
        self.sizer.Add( labelNumerics, 0, wx.ALIGN_LEFT|wx.ALL, 5 )
        self.sizer.Add( gridNumerics, 0, wx.ALIGN_LEFT|wx.ALL, border=5 )
        self.sizer.Add( parens_check, 0, wx.ALIGN_LEFT|wx.ALL, 5 )

        self.SetSizer( self.sizer )
        self.SetAutoLayout(1)
        self.SetupScrolling()

        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelection, id=fraction.GetId())
        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelection, id=code.GetId())
        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelection, id=statecode.GetId())
        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelection, id=statename.GetId())
        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=code.GetId())
        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=statecode.GetId())
        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=statename.GetId())
        self.Bind(wx.EVT_COMBOBOX, self.OnListSelection, id=self.list_selector.GetId())

        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=self.intctrl1.GetId())
        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=self.intctrl2.GetId())
        self.Bind(wx.EVT_TEXT, self.OnTextChange, id=self.floatctrl.GetId())
        self.Bind(wx.EVT_COMBOBOX, self.OnNumberSelect, id=numselect.GetId())
        self.Bind(wx.EVT_CHECKBOX, self.OnParensCheck, id=parens_check.GetId())

        self.Bind(wx.EVT_TEXT, self.OnIpAddrChange, id=ipaddr1.GetId())
        self.Bind(wx.EVT_TEXT, self.OnIpAddrChange, id=ipaddr2.GetId())
        self.Bind(wx.EVT_TEXT, self.OnIpAddrChange, id=ipaddr3.GetId())




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
        self.intctrl1.SetCtrlParameters(useParensForNegatives=event.IsChecked())
        self.intctrl2.SetCtrlParameters(useParensForNegatives=event.IsChecked())
        self.floatctrl.SetCtrlParameters(useParensForNegatives=event.IsChecked())

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
            choices = masked.states
            mask = 'AA'
            formatcodes += '!'
        self.dynamicbox.SetCtrlParameters( mask = mask,
                                           choices = choices,
                                           choiceRequired=True,
                                           autoSelect=True,
                                           formatcodes=formatcodes)
        self.dynamicbox.SetValue(choices[0])

# ---------------------------------------------------------------------
class TestMaskedTextCtrls(wx.Notebook):
    def __init__(self, parent, id, log):
        wx.Notebook.__init__(self, parent, id)
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
    app = wx.PySimpleApp()
    frame = wx.Frame(None, -1, "Test MaskedEditCtrls", size=(640, 480))
    win = TestMaskedTextCtrls(frame, -1, sys.stdout)
    frame.Show(True)
    app.MainLoop()
#----------------------------------------------------------------------------
import wx.lib.masked.maskededit as maskededit
# strip out module header used for pydoc:
demodoc = '\n'.join(maskededit.__doc__.split('\n')[2:])
overview = """<html>
<PRE><FONT SIZE=-1>
""" + demodoc + """
</FONT></PRE>
"""

if __name__ == "__main__":
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
