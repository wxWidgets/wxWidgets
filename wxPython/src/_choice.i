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
    List choices=[], long style=0, Validator validator=DefaultValidator,
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
    List choices=[], long style=0, Validator validator=DefaultValidator,
    String name=ChoiceNameStr) -> bool",
        "Actually create the GUI Choice control for 2-phase creation", "");
    

//     // These are only meaningful on wxMotif...
//     int GetColumns();
//     void SetColumns(const int n = 1);


    DocDeclStr(
        void , SetSelection(const int n),
        "Select the n'th item (zero based) in the list.", "");
    
    DocDeclStr(
        bool , SetStringSelection(const wxString& string),
        "Select the item with the specifed string", "");
    
    DocDeclStr(
        void , SetString(int n, const wxString& string),
        "Set the label for the n'th item (zero based) in the list.", "");
    

    %pythoncode { Select = SetSelection }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
