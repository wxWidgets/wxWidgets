/////////////////////////////////////////////////////////////////////////////
// Name:        _choice.i
// Purpose:     SWIG interface defs for wxChoice
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(ChoiceNameStr);

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxChoice,       
"A Choice control is used to select one of a list of strings.
Unlike a `wx.ListBox`, only the selection is visible until the
user pulls down the menu of choices.", "

Events
------
    ================    ==========================================
    EVT_CHOICE          Sent when an item in the list is selected.
    ================    ==========================================
");
       

MustHaveApp(wxChoice);

class wxChoice : public wxControlWithItems
{
public:
    %pythonAppend wxChoice         "self._setOORInfo(self)"
    %pythonAppend wxChoice()       ""

    DocCtorAStr(
        wxChoice(wxWindow *parent, wxWindowID id=-1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 const wxArrayString& choices = wxPyEmptyStringArray,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxPyChoiceNameStr),
        "__init__(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
    List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
    String name=ChoiceNameStr) -> Choice",
        "Create and show a Choice control", "");

    DocCtorStrName(
        wxChoice(),
        "Precreate a Choice control for 2-phase creation.", "",
        PreChoice);
                

    DocDeclAStr(
        bool, Create(wxWindow *parent, wxWindowID id=-1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     const wxArrayString& choices = wxPyEmptyStringArray,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxPyChoiceNameStr),
        "Create(Window parent, int id, Point pos=DefaultPosition, Size size=DefaultSize,
    List choices=EmptyList, long style=0, Validator validator=DefaultValidator,
    String name=ChoiceNameStr) -> bool",
        "Actually create the GUI Choice control for 2-phase creation", "");

    DocDeclStr(
        int , GetCurrentSelection() const,
        "Unlike `GetSelection` which only returns the accepted selection value,
i.e. the selection in the control once the user closes the dropdown
list, this function returns the current selection.  That is, while the
dropdown list is shown, it returns the currently selected item in
it. When it is not shown, its result is the same as for the other
function.", "");
    

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);


    %property(CurrentSelection, GetCurrentSelection, doc="See `GetCurrentSelection`");
    
};

//---------------------------------------------------------------------------
