/////////////////////////////////////////////////////////////////////////////
// Name:        _radio.i
// Purpose:     SWIG interface defs for wxRadioButton and wxRadioBox
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

MAKE_CONST_WXSTRING(RadioBoxNameStr);
MAKE_CONST_WXSTRING(RadioButtonNameStr);

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxRadioBox);

class wxRadioBox : public wxControl
{
public:
    %pythonPrepend wxRadioBox         "if kwargs.has_key('point'): kwargs['pos'] = kwargs['point'];del kwargs['point']"
    %pythonPrepend wxRadioBox()       ""
    %pythonAppend  wxRadioBox         "self._setOORInfo(self)"
    %pythonAppend  wxRadioBox()       ""

    wxRadioBox(wxWindow* parent, wxWindowID id=-1,
               const wxString& label = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               const wxArrayString& choices = wxPyEmptyStringArray,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyRadioBoxNameStr);
    %RenameCtor(PreRadioBox, wxRadioBox());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                const wxString& label = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxArrayString& choices = wxPyEmptyStringArray,
                int majorDimension = 0,
                long style = wxRA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyRadioBoxNameStr);

    virtual void SetSelection(int n);
    virtual int GetSelection() const;

    virtual wxString GetStringSelection() const;
    virtual bool SetStringSelection(const wxString& s);

    // string access
    virtual int GetCount() const;
    virtual int FindString(const wxString& s) const;
    
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);
    %pythoncode { GetItemLabel = GetString };
    %pythoncode { SetItemLabel = SetString };

    // change the individual radio button state
    %Rename(EnableItem,  virtual void, Enable(int n, bool enable = true));
    %Rename(ShowItem,  virtual void, Show(int n, bool show = true));

#ifndef __WXGTK__
    // layout parameters
    virtual int GetColumnCount() const;
    virtual int GetRowCount() const;

    // return the item above/below/to the left/right of the given one
    int GetNextItem(int item, wxDirection dir, long style) const;
#else
    %extend {
        int GetColumnCount() const { return -1; }
        int GetRowCount() const { return -1; }
        int GetNextItem(int item, wxDirection dir, long style) const { return -1; }
    }
#endif

//    bool IsValid(int n) const;  ** not public
        
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};
    

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxRadioButton);

class wxRadioButton : public wxControl
{
public:
    %pythonAppend wxRadioButton         "self._setOORInfo(self)"
    %pythonAppend wxRadioButton()       ""

    wxRadioButton(wxWindow* parent, wxWindowID id=-1,
                  const wxString& label = wxPyEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyRadioButtonNameStr);
    %RenameCtor(PreRadioButton, wxRadioButton());

    bool Create(wxWindow* parent, wxWindowID id=-1,
                  const wxString& label = wxPyEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyRadioButtonNameStr);

    bool GetValue();
    void SetValue(bool value);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
