
# Some sample data for the treectrl samples

tree = [
    "wx.AcceleratorTable",
    "wx.BrushList",
    "wx.BusyInfo",
    "wx.Clipboard",
    "wx.Colour",
    "wx.ColourData",
    "wx.ColourDatabase",
    "wx.ContextHelp",
    ["wx.DC", [
          "wx.ClientDC",
          ["wx.MemoryDC", [
                "wx.lib.colourchooser.canvas.BitmapBuffer",
                ["wx.BufferedDC", [
                      "wx.BufferedPaintDC", ]]]],

          "wx.MetaFileDC",
          "wx.MirrorDC",
          "wx.PaintDC",
          "wx.PostScriptDC",
          "wx.PrinterDC",
          "wx.ScreenDC",
          "wx.WindowDC",]],
    "wx.DragImage",
    "wx.Effects",
    "wx.EncodingConverter",
    ["wx.Event", [
          "wx.ActivateEvent",
          "wx.CalculateLayoutEvent",
          "wx.CloseEvent",
          ["wx.CommandEvent", [
                "wx.calendar.CalendarEvent",
                "wx.ChildFocusEvent",
                "wx.ContextMenuEvent",
                "wx.gizmos.DynamicSashSplitEvent",
                "wx.gizmos.DynamicSashUnifyEvent",
                "wx.FindDialogEvent",
                "wx.grid.GridEditorCreatedEvent",
                "wx.HelpEvent",
                ["wx.NotifyEvent",[
                      ["wx.BookCtrlEvent", [
                            "wx.ListbookEvent",
                            "wx.NotebookEvent ",]],
                      "wx.grid.GridEvent",
                      "wx.grid.GridRangeSelectEvent",
                      "wx.grid.GridSizeEvent",
                      "wx.ListEvent",
                      "wx.SpinEvent",
                      "wx.SplitterEvent",
                      "wx.TreeEvent",
                      "wx.wizard.WizardEvent ",]],
                ["wx.PyCommandEvent", [
                      "wx.lib.colourselect.ColourSelectEvent",
                      "wx.lib.buttons.GenButtonEvent",
                      "wx.lib.gridmovers.GridColMoveEvent",
                      "wx.lib.gridmovers.GridRowMoveEvent",
                      "wx.lib.intctrl.IntUpdatedEvent",
                      "wx.lib.masked.combobox.MaskedComboBoxSelectEvent",
                      "wx.lib.masked.numctrl.NumberUpdatedEvent",
                      "wx.lib.masked.timectrl.TimeUpdatedEvent ",]],
                "wx.SashEvent",
                "wx.ScrollEvent",
                "wx.stc.StyledTextEvent",
                "wx.TextUrlEvent",
                "wx.UpdateUIEvent",
                "wx.WindowCreateEvent",
                "wx.WindowDestroyEvent ",]],
          "wx.DisplayChangedEvent",
          "wx.DropFilesEvent",
          "wx.EraseEvent",
          "wx.FocusEvent",
          "wx.IconizeEvent",
          "wx.IdleEvent",
          "wx.InitDialogEvent",
          "wx.JoystickEvent",
          "wx.KeyEvent",
          "wx.MaximizeEvent",
          "wx.MenuEvent",
          "wx.MouseCaptureChangedEvent",
          "wx.MouseEvent",
          "wx.MoveEvent",
          "wx.NavigationKeyEvent",
          "wx.NcPaintEvent",
          "wx.PaintEvent",
          "wx.PaletteChangedEvent",
          "wx.ProcessEvent",
          ["wx.PyEvent", [
                "wx.lib.throbber.UpdateThrobberEvent ",]],
          "wx.QueryLayoutInfoEvent",
          "wx.QueryNewPaletteEvent",
          "wx.ScrollWinEvent",
          "wx.SetCursorEvent",
          "wx.ShowEvent",
          "wx.SizeEvent",
          "wx.SysColourChangedEvent",
          "wx.TaskBarIconEvent",
          "wx.TimerEvent ",]],
    ["wx.EvtHandler", [
          "wx.lib.gridmovers.GridColMover",
          "wx.lib.gridmovers.GridRowMover",
          "wx.html.HtmlHelpController",
          "wx.Menu",
          "wx.Process",          
          ["wx.PyApp", [
                ["wx.App", [
                      "wx.py.PyAlaCarte.App",
                      "wx.py.PyAlaMode.App",
                      "wx.py.PyAlaModeTest.App",
                      "wx.py.PyCrust.App",
                      "wx.py.PyShell.App",
                      ["wx.py.filling.App", [
                            "wx.py.PyFilling.App ",]],
                      ["wx.PySimpleApp", [
                            "wx.lib.masked.maskededit.test",]],
                      "wx.PyWidgetTester ",]]]],
          
          "wx.TaskBarIcon",
          ["wx.Timer", [
                "wx.PyTimer ",]],
          ["wx.Validator", [
                ["wx.PyValidator",[
                      "wx.lib.intctrl.IntValidator",]]]],
          ["wx.Window", [
                ["wx.lib.colourchooser.canvas.Canvas", [
                      "wx.lib.colourchooser.pycolourslider.PyColourSlider",
                      "wx.lib.colourchooser.pypalette.PyPalette",]],
                "wx.lib.gridmovers.ColDragWindow",
                ["wx.Control",[
                      ["wx.BookCtrl", [
                            "wx.Listbook",
                            ["wx.Notebook",[
                                  "wx.py.editor.EditorNotebook",
                                  "wx.py.editor.EditorShellNotebook",]] ]],
                      ["wx.Button", [
                            ["wx.BitmapButton",[
                                  "wx.lib.colourselect.ColourSelect",
                                  "wx.ContextHelpButton",
                                  "wx.lib.foldmenu.FoldOutMenu ",]] ]],
                      "wx.calendar.CalendarCtrl",
                      "wx.CheckBox",
                      ["wx.ComboBox",[
                            ["wx.lib.masked.combobox.BaseMaskedComboBox", [
                                  "wx.lib.masked.combobox.ComboBox",
                                  "wx.lib.masked.combobox.PreMaskedComboBox",]] ]],
                      ["wx.ControlWithItems", [
                            ["wx.Choice",[
                                  "wx.DirFilterListCtrl ",]],
                            "wx.ListBox",
                                  "wx.CheckListBox ",]],
                      "wx.Gauge",
                      "wx.GenericDirCtrl",
                      "wx.gizmos.LEDNumberCtrl",
                      ["wx.ListCtrl",[
                            "wx.ListView ",]],
                      ["wx.PyControl",[
                            "wx.lib.calendar.Calendar",
                            ["wx.lib.buttons.GenButton",[
                                  ["wx.lib.buttons.GenBitmapButton",[
                                        ["wx.lib.buttons.GenBitmapTextButton",[
                                              "wx.lib.buttons.GenBitmapTextToggleButton ",]],
                                        "wx.lib.buttons.GenBitmapToggleButton ",]],
                                  "wx.lib.buttons.GenToggleButton ",]],
                            "wx.lib.statbmp.GenStaticBitmap",
                            "wx.lib.stattext.GenStaticText",
                            "wx.lib.popupctl.PopButton",
                            "wx.lib.popupctl.PopupControl",
                            "wx.lib.ticker.Ticker ",]],
                      "wx.RadioBox",
                      "wx.RadioButton",
                      "wx.ScrollBar",
                      "wx.Slider",
                      "wx.SpinButton",
                      "wx.SpinCtrl",
                      ["wx.StaticBitmap",[
                            "wx.lib.fancytext.StaticFancyText ",]],
                      "wx.StaticBox",
                      "wx.StaticLine",
                      "wx.StaticText",
                      ["wx.stc.StyledTextCtrl",[
                            ["wx.py.editwindow.EditWindow",[
                                  "wx.py.crust.Display",
                                  "wx.py.editor.EditWindow",
                                  "wx.py.filling.FillingText",
                                  "wx.py.shell.Shell",]],
                            "wx.lib.pyshell.PyShellWindow ",]],
                      ["wx.TextCtrl", [
                            ["wx.lib.masked.textctrl.BaseMaskedTextCtrl",[
                                  "wx.lib.masked.ipaddrctrl.IpAddrCtrl",
                                  "wx.lib.masked.numctrl.NumCtrl",
                                  "wx.lib.masked.textctrl.PreMaskedTextCtrl",
                                  "wx.lib.masked.textctrl.TextCtrl",
                                  "wx.lib.masked.timectrl.TimeCtrl ",]],
                            "wx.py.crust.Calltip",
                            "wx.lib.sheet.CTextCellEditor",
                            "wx.py.crust.DispatcherListing",
                            "wx.lib.intctrl.IntCtrl",
                            "wx.lib.rightalign.RightTextCtrl",
                            "wx.py.crust.SessionListing",]],
                      "wx.ToggleButton",
                      "wx.ToolBar",
                      ["wx.TreeCtrl",[
                            "wx.py.filling.FillingTree",
                            "wx.gizmos.RemotelyScrolledTreeCtrl ",]],
                      "wx.gizmos.TreeListCtrl ",]],
                "wx.gizmos.DynamicSashWindow",
                "wx.lib.multisash.EmptyChild",
                "wx.glcanvas.GLCanvas",
                "wx.lib.imagebrowser.ImageView",
                "wx.MDIClientWindow",
                "wx.MenuBar",
                "wx.lib.multisash.MultiClient",
                "wx.lib.multisash.MultiCloser",
                "wx.lib.multisash.MultiCreator",
                "wx.lib.multisash.MultiSash",
                "wx.lib.multisash.MultiSizer",
                "wx.lib.multisash.MultiSplit",
                "wx.lib.multisash.MultiViewLeaf",
                ["wx.Panel",[
                      "wx.gizmos.EditableListBox",
                      ["wx.lib.filebrowsebutton.FileBrowseButton",[
                            "wx.lib.filebrowsebutton.DirBrowseButton",
                            "wx.lib.filebrowsebutton.FileBrowseButtonWithHistory",]],
                      "wx.lib.floatcanvas.FloatCanvas.FloatCanvas",
                      "wx.lib.floatcanvas.NavCanvas.NavCanvas",
                      "wx.NotebookPage",
                      ["wx.PreviewControlBar",[
                            "wx.PyPreviewControlBar ",]],
                      "wx.lib.colourchooser.pycolourbox.PyColourBox",
                      "wx.lib.colourchooser.pycolourchooser.PyColourChooser",
                      ["wx.PyPanel",[
                            "wx.lib.throbber.Throbber",]],
                      "wx.lib.shell.PyShell",
                      "wx.lib.shell.PyShellInput",
                      "wx.lib.shell.PyShellOutput",
                      ["wx.ScrolledWindow",[
                            "wx.lib.editor.editor.Editor",
                            ["wx.grid.Grid",[
                                  "wx.lib.sheet.CSheet ",]],
                            ["wx.html.HtmlWindow",[
                                  "wx.lib.ClickableHtmlWindow.PyClickableHtmlWindow",]],
                            "wx.PreviewCanvas",
                            "wx.lib.printout.PrintTableDraw",
                            ["wx.PyScrolledWindow",[
                                  "wx.lib.scrolledpanel.ScrolledPanel",]],
                            "wx.lib.ogl.ShapeCanvas",
                            "wx.gizmos.SplitterScrolledWindow ",]],
                      ["wx.VScrolledWindow",[
                            ["wx.VListBox", [
                                  "wx.HtmlListBox ",]] ]],
                      ["wx.wizard.WizardPage", [
                            "wx.wizard.PyWizardPage",
                            "wx.wizard.WizardPageSimple ",]],
                "wx.lib.plot.PlotCanvas",
                "wx.lib.wxPlotCanvas.PlotCanvas",
                ["wx.PopupWindow",[
                      "wx.lib.foldmenu.FoldOutWindow",
                      ["wx.PopupTransientWindow",[
                            "wx.TipWindow ",]] ]],
                ["wx.PyWindow", [
                      "wx.lib.analogclock.AnalogClockWindow",]],
                "wx.lib.gridmovers.RowDragWindow",
                ["wx.SashWindow",[
                      "wx.SashLayoutWindow ",]],
                "wx.SplashScreenWindow",
                ["wx.SplitterWindow",[
                      "wx.py.crust.Crust",
                      "wx.py.filling.Filling",
                      "wx.gizmos.ThinSplitterWindow ",]],
                "wx.StatusBar",
                ["wx.TopLevelWindow",[
                      ["wx.Dialog",[
                            "wx.lib.calendar.CalenDlg",
                            "wx.ColourDialog",
                            "wx.DirDialog",
                            "wx.FileDialog",
                            "wx.FindReplaceDialog",
                            "wx.FontDialog",
                            "wx.lib.imagebrowser.ImageDialog",
                            "wx.MessageDialog",
                            "wx.MultiChoiceDialog",
                            "wx.lib.dialogs.MultipleChoiceDialog",
                            "wx.PageSetupDialog",
                            "wx.lib.popupctl.PopupDialog",
                            "wx.PrintDialog",
                            "wx.lib.dialogs.ScrolledMessageDialog",
                            "wx.SingleChoiceDialog",
                            "wx.TextEntryDialog",
                            "wx.wizard.Wizard ",]],
                      ["wx.Frame", [
                            "wx.lib.analogclockopts.ACCustomizationFrame",
                            "wx.py.filling.FillingFrame",
                            ["wx.py.frame.Frame",[
                                  "wx.py.crust.CrustFrame",
                                  ["wx.py.editor.EditorFrame",[
                                        "wx.py.editor.EditorNotebookFrame",]],
                                  "wx.py.shell.ShellFrame",]],
                            "wx.html.HtmlHelpFrame",
                            "wx.MDIChildFrame",
                            "wx.MDIParentFrame",
                            "wx.MiniFrame",
                            ["wx.PreviewFrame",[
                                  "wx.PyPreviewFrame ",]],
                            "wx.ProgressDialog",
                            "wx.SplashScreen",
                            "wx.lib.splashscreen.SplashScreen",
                            "wx.lib.masked.maskededit.test2",
                            "wx.lib.plot.TestFrame ",]] ]],
                "wx.gizmos.TreeCompanionWindow ",]] ]] ]],
    "wx.FileHistory",
    "wx.FileSystem",
    "wx.FindReplaceData",
    "wx.FontData",
    "wx.FontList",
    "wx.FSFile",
    ["wx.GDIObject",[
          "wx.Bitmap",
          "wx.Brush",
          "wx.Cursor",
          "wx.Font",
          "wx.Icon",
          "wx.Palette",
          "wx.Pen",
          "wx.Region ",]],
    "wx.glcanvas.GLContext",
    ["wx.grid.GridTableBase", [
          "wx.grid.GridStringTable",
          "wx.grid.PyGridTableBase ",]],
    ["wx.html.HtmlCell", [
          "wx.html.HtmlColourCell",
          "wx.html.HtmlContainerCell",
          "wx.html.HtmlFontCell",
          "wx.html.HtmlWidgetCell",
          "wx.html.HtmlWordCell ",]],
    "wx.html.HtmlDCRenderer",
    "wx.html.HtmlEasyPrinting",
    "wx.html.HtmlFilter",
    "wx.html.HtmlLinkInfo",
    ["wx.html.HtmlParser", [
          "wx.html.HtmlWinParser ",]],
    "wx.html.HtmlTag",
    ["wx.html.HtmlTagHandler", [
          ["wx.html.HtmlWinTagHandler", [
                "wx.lib.wxpTag.wxpTagHandler ",]] ]],
    "wx.Image",
    ["wx.ImageHandler", [
          ["wx.BMPHandler", [
                ["wx.ICOHandler", [
                      ["wx.CURHandler", [
                            "wx.ANIHandler ",]] ]] ]],
          "wx.GIFHandler",
          "wx.JPEGHandler",
          "wx.PCXHandler",
          "wx.PNGHandler",
          "wx.PNMHandler",
          "wx.TIFFHandler",
          "wx.XPMHandler ",]],
    "wx.ImageList",
    "wx.IndividualLayoutConstraint",
    "wx.LayoutAlgorithm",
    ["wx.LayoutConstraints", [
          "wx.lib.anchors.LayoutAnchors",
          "wx.lib.layoutf.Layoutf",]],
    "wx.ListItem",
    "wx.Mask",
    "wx.MenuItem",
    "wx.MetaFile",
    "wx.PageSetupDialogData",
    "wx.PenList",
    "wx.PrintData",
    "wx.PrintDialogData",
    "wx.Printer",
    ["wx.Printout", [
          "wx.html.HtmlPrintout",
          "wx.lib.plot.PlotPrintout",
          "wx.lib.printout.SetPrintout ",]],
    ["wx.PrintPreview", [
          "wx.PyPrintPreview ",]],
    "wx.RegionIterator",
    ["wx.Sizer", [
          "wx.BookCtrlSizer",
          ["wx.BoxSizer", [
                "wx.StaticBoxSizer", ]],
          ["wx.GridSizer", [
                ["wx.FlexGridSizer", [
                      "wx.GridBagSizer",]] ]],
          "wx.NotebookSizer",
          "wx.PySizer",]],
    ["wx.SizerItem", [
          "wx.GBSizerItem",]],
    "wx.SystemOptions",
    "wx.ToolBarToolBase",
    "wx.ToolTip",
    "wx.gizmos.TreeListColumnInfo",
    "wx.xrc.XmlDocument",
    "wx.xrc.XmlResource",
    "wx.xrc.XmlResourceHandler ",
]



