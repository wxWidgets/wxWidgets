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
    virtual size_t GetCount() const;
    virtual int FindString(const wxString& s) const;
    
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);
    %pythoncode { GetItemLabel = GetString };
    %pythoncode { SetItemLabel = SetString };

    // change the individual radio button state
    %Rename(EnableItem,  virtual void, Enable(unsigned int n, bool enable = true));
    %Rename(ShowItem,  virtual void, Show(unsigned int n, bool show = true));
    virtual bool IsItemEnabled(unsigned int n) const;
    virtual bool IsItemShown(unsigned int n) const;

    // layout parameters
    virtual unsigned int GetColumnCount() const;
    virtual unsigned int GetRowCount() const;

    // return the item above/below/to the left/right of the given one
    int GetNextItem(int item, wxDirection dir, long style) const;

    // set the tooltip text for a radio item, empty string unsets any tooltip
    void SetItemToolTip(unsigned int item, const wxString& text);

    // get the individual items tooltip; returns NULL if none
    wxToolTip *GetItemToolTip(unsigned int item) const;

    // set helptext for a particular item, pass an empty string to erase it
    void SetItemHelpText(unsigned int n, const wxString& helpText);

    // retrieve helptext for a particular item, empty string means no help text
    wxString GetItemHelpText(unsigned int n) const;

// Hmmm...  This is protected on wxMSW.    
//    virtual int GetItemFromPoint(const wxPoint& pt) const;

//    bool IsValid(int n) const;  ** not public
        
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(ColumnCount, GetColumnCount, doc="See `GetColumnCount`");
    %property(Count, GetCount, doc="See `GetCount`");
    %property(RowCount, GetRowCount, doc="See `GetRowCount`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(StringSelection, GetStringSelection, SetStringSelection, doc="See `GetStringSelection` and `SetStringSelection`");    
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

    %property(Value, GetValue, SetValue, doc="See `GetValue` and `SetValue`");
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
