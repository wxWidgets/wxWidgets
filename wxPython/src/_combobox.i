/////////////////////////////////////////////////////////////////////////////
// Name:        _combobox.i
// Purpose:     SWIG interface defs for wxComboBox
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

MAKE_CONST_WXSTRING(ComboBoxNameStr);

//---------------------------------------------------------------------------
%newgroup;


DocStr(wxComboBox,
"A combobox is like a combination of an edit control and a
listbox. It can be displayed as static list with editable or
read-only text field; or a drop-down list with text field.

A combobox permits a single selection only. Combobox items are
numbered from zero.", "

Styles
------
    ================    ===============================================
    wx.CB_SIMPLE        Creates a combobox with a permanently
                        displayed list.  Windows only.

    wx.CB_DROPDOWN      Creates a combobox with a drop-down list.

    wx.CB_READONLY      Same as wxCB_DROPDOWN but only the strings
                        specified as the combobox choices can be
                        selected, it is impossible to select
                        (even from a program) a string which is
                        not in the choices list.

    wx.CB_SORT          Sorts the entries in the list alphabetically.
    ================    ===============================================

Events
-------
    ================    ===============================================
    EVT_COMBOBOX        Sent when an item on the list is selected.
                        Note that calling `GetValue` in this handler 
                        will return the newly selected value.
    EVT_TEXT            Sent when the combobox text changes.
    ================    ===============================================
");



MustHaveApp(wxComboBox);

#ifdef __WXMSW__
class wxComboBox : public wxChoice
#else
class wxComboBox : public wxControl, public wxItemContainer
#endif
{
public:
    %pythonAppend wxComboBox         "self._setOORInfo(self)"
    %pythonAppend wxComboBox()       ""

    DocCtorAStr(
        wxComboBox(wxWindow* parent, wxWindowID id=-1,
                   const wxString& value = wxPyEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   const wxArrayString& choices = wxPyEmptyStringArray,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyComboBoxNameStr),
        "__init__(Window parent, int id, String value=EmptyString,
    Point pos=DefaultPosition, Size size=DefaultSize,
    List choices=[], long style=0, Validator validator=DefaultValidator,
    String name=ComboBoxNameStr) -> ComboBox",
        "Constructor, creates and shows a ComboBox control.", "");

    DocCtorStrName(
        wxComboBox(),
        "Precreate a ComboBox control for 2-phase creation.", "",
        PreComboBox);


    DocDeclAStr(
        bool, Create(wxWindow *parent, wxWindowID id=-1,
                     const wxString& value = wxPyEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     const wxArrayString& choices = wxPyEmptyStringArray,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxPyChoiceNameStr),
        "Create(Window parent, int id, String value=EmptyString,
    Point pos=DefaultPosition, Size size=DefaultSize,
    List choices=[], long style=0, Validator validator=DefaultValidator,
    String name=ChoiceNameStr) -> bool",
        "Actually create the GUI wxComboBox control for 2-phase creation", "");
    

    DocDeclStr(
        virtual wxString , GetValue() const,
        "Returns the current value in the combobox text field.", "");
    
    DocDeclStr(
        virtual void , SetValue(const wxString& value),
        "", "");
    

    DocDeclStr(
        virtual void , Copy(),
        "Copies the selected text to the clipboard.", "");
    
    DocDeclStr(
        virtual void , Cut(),
        "Copies the selected text to the clipboard and removes the selection.", "");
    
    DocDeclStr(
        virtual void , Paste(),
        "Pastes text from the clipboard to the text field.", "");
    
    
    DocDeclStr(
        virtual void , SetInsertionPoint(long pos),
        "Sets the insertion point in the combobox text field.", "");
    
    DocDeclStr(
        virtual long , GetInsertionPoint() const,
        "Returns the insertion point for the combobox's text field.", "");
    
    DocDeclStr(
        virtual long , GetLastPosition() const,
        "Returns the last position in the combobox text field.", "");
    
    DocDeclStr(
        virtual void , Replace(long from, long to, const wxString& value),
        "Replaces the text between two positions with the given text, in the
combobox text field.", "");
    
    DocDeclStr(
        void , SetSelection(int n),
        "Sets the item at index 'n' to be the selected item.", "");
    
    DocDeclStrName(
        virtual void , SetSelection(long from, long to),
        "Selects the text between the two positions in the combobox text field.", "",
        SetMark);    

    DocDeclStr(
        bool , SetStringSelection(const wxString& string),
        "Select the item with the specifed string", "");
    
    DocDeclStr(
        void , SetString(int n, const wxString& string),
        "Set the label for the n'th item (zero based) in the list.", "");
    
    DocDeclStr(
        virtual void , SetEditable(bool editable),
        "", "");
    

    DocDeclStr(
        virtual void , SetInsertionPointEnd(),
        "Sets the insertion point at the end of the combobox text field.", "");
    
    DocDeclStr(
        virtual void , Remove(long from, long to),
        "Removes the text between the two positions in the combobox text field.", "");
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
