/////////////////////////////////////////////////////////////////////////////
// Name:        controls.i
// Purpose:     Control (widget) classes for wxPython
//
// Author:      Robin Dunn
//
// Created:     6/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module controls

%{
#include "helpers.h"
#include <wx/slider.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>
#include <wx/dynarray.h>
#include <wx/statline.h>
#include <wx/tglbtn.h>

#ifdef __WXMSW__
#if wxUSE_OWNER_DRAWN
#include <wx/checklst.h>
#endif
#endif

#ifdef __WXGTK__
#include <wx/checklst.h>
#endif
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import gdi.i
%import events.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

%readonly
wxValidator wxDefaultValidator;
%readwrite

//----------------------------------------------------------------------

%{
//#define DECLARE_DEF_STRING(name)  static wxString* wxPy##name

    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(ControlNameStr);
    DECLARE_DEF_STRING(ButtonNameStr);
    DECLARE_DEF_STRING(CheckBoxNameStr);
    DECLARE_DEF_STRING(ChoiceNameStr);
    DECLARE_DEF_STRING(ComboBoxNameStr);
    DECLARE_DEF_STRING(GaugeNameStr);
    DECLARE_DEF_STRING(StaticBoxNameStr);
    DECLARE_DEF_STRING(StaticTextNameStr);
    DECLARE_DEF_STRING(ListBoxNameStr);
    DECLARE_DEF_STRING(TextCtrlNameStr);
    DECLARE_DEF_STRING(ScrollBarNameStr);
    DECLARE_DEF_STRING(SPIN_BUTTON_NAME);
    DECLARE_DEF_STRING(StaticBitmapNameStr);
    DECLARE_DEF_STRING(RadioBoxNameStr);
    DECLARE_DEF_STRING(RadioButtonNameStr);
    DECLARE_DEF_STRING(SliderNameStr);

    wxChar* wxSpinCtrlNameStr = _T("wxSpinCtrl");
    DECLARE_DEF_STRING(SpinCtrlNameStr);

    static const wxString wxPyEmptyString(wxT(""));
%}

//----------------------------------------------------------------------

//  This is the base class for a control or 'widget'.
//
//  A control is generally a small window which processes user input and/or
//  displays one or more item of data.
class wxControl : public wxWindow {
public:

    //
    wxControl(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos=wxDefaultPosition,
              const wxSize& size=wxDefaultSize,
              long style=0,
              const wxValidator& validator=wxDefaultValidator,
              const wxString& name=wxPyControlNameStr);

    //
    %name(wxPreControl)wxControl();

    //
    bool Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos=wxDefaultPosition,
                       const wxSize& size=wxDefaultSize,
                       long style=0,
                       const wxValidator& validator=wxDefaultValidator,
                       const wxString& name=wxPyControlNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreControl:val._setOORInfo(val)"

    // Simulates the effect of the user issuing a command to the item. See
    // wxCommandEvent.
    void Command(wxCommandEvent& event);

    // Return a control's text.
    wxString GetLabel();

    // Sets the item's text.
    void SetLabel(const wxString& label);
};


//----------------------------------------------------------------------


class wxControlWithItems : public wxControl {
public:

    // void Clear();  ambiguous, redefine below...

    // Deletes an item from the control
    void Delete(int n);

    // Returns the number of items in the control.
    int GetCount();
    %pragma(python) addtoclass = "Number = GetCount"

    // Returns the string at the given position.
    wxString GetString(int n);

    // Sets the string value of an item.
    void SetString(int n, const wxString& s);

    // Finds an item matching the given string.  Returns the zero-based
    // position of the item, or -1 if the string was not found.
    int FindString(const wxString& s);

    // Select the item at postion n.
    void Select(int n);

    // Gets the position of the selected item.
    int GetSelection();

    // Gets the current selection as a string.
    wxString GetStringSelection() const;

    //   void Append(const wxString& item);
    //   void Append(const wxString& item, char* clientData);
    //   char* GetClientData(const int n);
    //   void SetClientData(const int n, char* data);


    %addmethods {
        // Adds the item to the control, associating the given data with the
        // item if not None.
        void Append(const wxString& item, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                self->Append(item, data);
            } else
                self->Append(item);
        }

        // Returns the client data associated with the given item, (if any.)
        PyObject* GetClientData(int n) {
            wxPyClientData* data = (wxPyClientData*)self->GetClientObject(n);
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        // Associate the given client data with the item at position n.
        void SetClientData(int n, PyObject* clientData) {
            wxPyClientData* data = new wxPyClientData(clientData);
            self->SetClientObject(n, data);
        }
    }

    // append several items at once to the control
    %name(AppendItems)void Append(const wxArrayString& strings);

};

//----------------------------------------------------------------------

// A button is a control that contains a text string, and is one of the most
// common elements of a GUI.  It may be placed on a dialog box or panel, or
// indeed almost any other window.
//
// Styles
//    wxBU_LEFT:  Left-justifies the label. WIN32 only.
//    wxBU_TOP:  Aligns the label to the top of the button. WIN32 only.
//    wxBU_RIGHT:  Right-justifies the bitmap label. WIN32 only.
//    wxBU_BOTTOM:  Aligns the label to the bottom of the button. WIN32 only.
//    wxBU_EXACTFIT: Creates the button as small as possible instead of making
//                   it of the standard size (which is the default behaviour.)
//
// Events
//     EVT_BUTTON(win,id,func):
//         Sent when the button is clicked.
//
class wxButton : public wxControl {
public:
    // Constructor, creating and showing a button.
    //
    // parent:  Parent window.  Must not be None.
    // id:      Button identifier.  A value of -1 indicates a default value.
    // label:   The text to be displayed on the button.
    // pos:     The button position on it's parent.
    // size:    Button size.  If the default size (-1, -1) is specified then the
    //          button is sized appropriately for the text.
    // style:   Window style.  See wxButton.
    // validator: Window validator.
    // name:    Window name.
    wxButton(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);

    // Default constructor
    %name(wxPreButton)wxButton();

    // Button creation function for two-step creation.
    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyButtonNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreButton:val._setOORInfo(val)"

    // This sets the button to be the default item for the panel or dialog box.
    //
    // Under Windows, only dialog box buttons respond to this function. As
    // normal under Windows and Motif, pressing return causes the default
    // button to be depressed when the return key is pressed. See also
    // wxWindow.SetFocus which sets the keyboard focus for windows and text
    // panel items, and wxPanel.SetDefaultItem.
    void SetDefault();

    //
    void SetBackgroundColour(const wxColour& colour);
    //
    void SetForegroundColour(const wxColour& colour);

#ifdef __WXMSW__
    // show the image in the button in addition to the label
    void SetImageLabel(const wxBitmap& bitmap);

    // set the margins around the image
    void SetImageMargins(wxCoord x, wxCoord y);
#endif

    // returns the default button size for this platform
    static wxSize GetDefaultSize();
};

//----------------------------------------------------------------------

class wxBitmapButton : public wxButton {
public:
    wxBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyButtonNameStr);
    %name(wxPreBitmapButton)wxBitmapButton();

    bool Create(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyButtonNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreBitmapButton:val._setOORInfo(val)"

    wxBitmap GetBitmapLabel();
    wxBitmap GetBitmapDisabled();
    wxBitmap GetBitmapFocus();
    wxBitmap GetBitmapSelected();
    void SetBitmapDisabled(const wxBitmap& bitmap);
    void SetBitmapFocus(const wxBitmap& bitmap);
    void SetBitmapSelected(const wxBitmap& bitmap);
    void SetBitmapLabel(const wxBitmap& bitmap);

    void SetMargins(int x, int y) { m_marginX = x; m_marginY = y; }
    int GetMarginX() const { return m_marginX; }
    int GetMarginY() const { return m_marginY; }
};

//----------------------------------------------------------------------

class wxCheckBox : public wxControl {
public:
    wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyCheckBoxNameStr);
    %name(wxPreCheckBox)wxCheckBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyCheckBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreCheckBox:val._setOORInfo(val)"

    bool GetValue();
    bool IsChecked();
    void SetValue(const bool state);
};

//----------------------------------------------------------------------

class wxChoice : public wxControlWithItems {
public:
    wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int LCOUNT=0, wxString* choices=NULL,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyChoiceNameStr);
    %name(wxPreChoice)wxChoice();

    bool Create(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int LCOUNT=0, wxString* choices=NULL,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPyChoiceNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreChoice:val._setOORInfo(val)"

    void Clear();

    int GetColumns();
    void SetColumns(const int n = 1);
    void SetSelection(const int n);
    void SetStringSelection(const wxString& string);
    void SetString(int n, const wxString& s);

    %pragma(python) addtoclass = "
    Select = SetSelection
    "
};

//----------------------------------------------------------------------

// wxGTK's wxComboBox doesn't derive from wxChoice like wxMSW, or even
// wxControlWithItems, so we have to duplicate the methods here... <blech!>
// wcMac's inheritace is weird too so we'll fake it with this one too.

#ifndef __WXMSW__
class wxComboBox : public wxControl
{
public:
    wxComboBox(wxWindow* parent, wxWindowID id,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyComboBoxNameStr);
    %name(wxPreComboBox)wxComboBox();

    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& value = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int LCOUNT=0, wxString* choices=NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyComboBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreComboBox:val._setOORInfo(val)"

    void Copy();
    void Cut();
    long GetInsertionPoint();
    long GetLastPosition();
    wxString GetValue();
    void Paste();
    void Replace(long from, long to, const wxString& text);
    void Remove(long from, long to);
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    void SetSelection(int n);
    %name(SetMark)void SetSelection(long from, long to);
    void SetValue(const wxString& text);
    void SetEditable(bool editable);


    void Clear();
    void Delete(int n);

    int GetCount();
    %pragma(python) addtoclass = "Number = GetCount"
    wxString GetString(int n);
    int FindString(const wxString& s);

    //void SetString(int n, const wxString& s);  *** No equivalent for wxGTK!!!

    // void Select(int n);
    %pragma(python) addtoclass = "Select = SetSelection"

    int GetSelection();
    wxString GetStringSelection() const;

    //   void Append(const wxString& item);
    //   void Append(const wxString& item, char* clientData);
    //   char* GetClientData(const int n);
    //   void SetClientData(const int n, char* data);
    %addmethods {
        void Append(const wxString& item, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                self->Append(item, data);
            } else
                self->Append(item);
        }

        PyObject* GetClientData(int n) {
            wxPyClientData* data = (wxPyClientData*)self->GetClientObject(n);
            if (data) {
                Py_INCREF(data->m_obj);
                return data->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        void SetClientData(int n, PyObject* clientData) {
            wxPyClientData* data = new wxPyClientData(clientData);
            self->SetClientObject(n, data);
        }
    }

};



#else
// MSW's version derives from wxChoice

class wxComboBox : public wxChoice {
public:
    wxComboBox(wxWindow* parent, wxWindowID id,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyComboBoxNameStr);
    %name(wxPreComboBox)wxComboBox();

    bool Create(wxWindow* parent, wxWindowID id,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyComboBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreComboBox:val._setOORInfo(val)"

    void Copy();
    void Cut();
    long GetInsertionPoint();
    long GetLastPosition();
    wxString GetValue();
    void Paste();
    void Replace(long from, long to, const wxString& text);
    void Remove(long from, long to);
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    void SetSelection(int n);
    %name(SetMark)void SetSelection(long from, long to);
    void SetValue(const wxString& text);
    void SetEditable(bool editable);
};
#endif


//----------------------------------------------------------------------

class wxGauge : public wxControl {
public:
    wxGauge(wxWindow* parent, wxWindowID id, int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyGaugeNameStr);
    %name(wxPreGauge)wxGauge();

    bool Create(wxWindow* parent, wxWindowID id, int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyGaugeNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreGauge:val._setOORInfo(val)"

    int GetBezelFace();
    int GetRange();
    int GetShadowWidth();
    int GetValue();
    void SetBezelFace(int width);
    void SetRange(int range);
    void SetShadowWidth(int width);
    void SetValue(int pos);
};

//----------------------------------------------------------------------

class wxStaticBox : public wxControl {
public:
    wxStaticBox(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);
    %name(wxPreStaticBox)wxStaticBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyStaticBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStaticBox:val._setOORInfo(val)"
};


//----------------------------------------------------------------------


class wxStaticLine : public wxControl {
public:
    wxStaticLine( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticTextNameStr);
    %name(wxPreStaticLine)wxStaticLine();

    bool Create( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString& name = wxPyStaticTextNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStaticLine:val._setOORInfo(val)"
};


//----------------------------------------------------------------------

class wxStaticText : public wxControl {
public:
    wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);
    %name(wxPreStaticText)wxStaticText();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticTextNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStaticText:val._setOORInfo(val)"

    wxString GetLabel();
    void SetLabel(const wxString& label);
};

//----------------------------------------------------------------------

class wxListBox : public wxControlWithItems {
public:
    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int LCOUNT, wxString* choices = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxPyListBoxNameStr);
    %name(wxPreListBox)wxListBox();

    bool Create(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int LCOUNT, wxString* choices = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxPyListBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreListBox:val._setOORInfo(val)"

    void Clear();
    void Deselect(int n);

    // int GetSelections(int **selections);
    %addmethods {
      PyObject* GetSelections() {
          wxArrayInt lst;
          self->GetSelections(lst);
          PyObject *tup = PyTuple_New(lst.GetCount());
          for(size_t i=0; i<lst.GetCount(); i++) {
              PyTuple_SetItem(tup, i, PyInt_FromLong(lst[i]));
          }
          return tup;
      }
    }


    void InsertItems(int LCOUNT, wxString* choices, int pos);

    bool IsSelected(const int n);
    bool Selected(const int n);
    void Set(int LCOUNT, wxString* choices);
    void SetFirstItem(int n);
    %name(SetFirstItemStr)void SetFirstItem(const wxString& string);
    void SetSelection(int n, bool select = TRUE);
    void SetString(int n, const wxString& string);
    void SetStringSelection(const wxString& string, bool select = TRUE);
};


//----------------------------------------------------------------------

class wxCheckListBox : public wxListBox {
public:
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int LCOUNT = 0,
                   wxString* choices = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyListBoxNameStr);
    %name(wxPreCheckListBox)wxCheckListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int LCOUNT = 0,
                   wxString* choices = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyListBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreCheckListBox:val._setOORInfo(val)"

    bool  IsChecked(int uiIndex);
    void  Check(int uiIndex, int bCheck = TRUE);
    void InsertItems(int LCOUNT, wxString* choices, int pos);

#ifndef __WXMAC__
    int GetItemHeight();
#endif

    // return the index of the item at this position or wxNOT_FOUND
    int HitTest(const wxPoint& pt) const;
    %name(HitTestXY)int HitTest(wxCoord x, wxCoord y) const;

};

//----------------------------------------------------------------------


class wxTextAttr
{
public:
    // ctors
    wxTextAttr(const wxColour& colText = wxNullColour,
               const wxColour& colBack = wxNullColour,
               const wxFont& font = wxNullFont);
    ~wxTextAttr();

    // setters
    void SetTextColour(const wxColour& colText);
    void SetBackgroundColour(const wxColour& colBack);
    void SetFont(const wxFont& font);

    // accessors
    bool HasTextColour() const;
    bool HasBackgroundColour() const;
    bool HasFont() const;

    wxColour GetTextColour() const;
    wxColour GetBackgroundColour() const;
    wxFont GetFont() const;

    // returns false if we have any attributes set, true otherwise
    bool IsDefault();

    // return the attribute having the valid font and colours: it uses the
    // attributes set in attr and falls back first to attrDefault and then to
    // the text control font/colours for those attributes which are not set
    static wxTextAttr Combine(const wxTextAttr& attr,
                              const wxTextAttr& attrDef,
                              const wxTextCtrl *text);
};



class wxTextCtrl : public wxControl {
public:
    wxTextCtrl(wxWindow* parent, wxWindowID id,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyTextCtrlNameStr);
    %name(wxPreTextCtrl)wxTextCtrl();

    bool Create(wxWindow* parent, wxWindowID id,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyTextCtrlNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreTextCtrl:val._setOORInfo(val)"


    wxString GetValue() const;
    void SetValue(const wxString& value);

    wxString GetRange(long from, long to);

    int GetLineLength(long lineNo) const;
    wxString GetLineText(long lineNo) const;
    int GetNumberOfLines() const;

    bool IsModified() const;
    bool IsEditable() const;

    // If the return values from and to are the same, there is no selection.
    void GetSelection(long* OUTPUT, long* OUTPUT) const;
    wxString GetStringSelection();

    void Clear();
    void Replace(long from, long to, const wxString& value);
    void Remove(long from, long to);

    // load/save the controls contents from/to the file
    bool LoadFile(const wxString& file);
    bool SaveFile(const wxString& file = wxPyEmptyString);

    // clears the dirty flag
    void DiscardEdits();

    // set the max number of characters which may be entered in a single line
    // text control
    void SetMaxLength(unsigned long len);

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    void WriteText(const wxString& text);
    void AppendText(const wxString& text);

    // insert the character which would have resulted from this key event,
    // return TRUE if anything has been inserted
    bool EmulateKeyPress(const wxKeyEvent& event);

    // text control under some platforms supports the text styles: these
    // methods allow to apply the given text style to the given selection or to
    // set/get the style which will be used for all appended text
    bool SetStyle(long start, long end, const wxTextAttr& style);
    bool SetDefaultStyle(const wxTextAttr& style);
    const wxTextAttr& GetDefaultStyle() const;

    // translate between the position (which is just an index in the text ctrl
    // considering all its contents as a single strings) and (x, y) coordinates
    // which represent column and line.
    long XYToPosition(long x, long y) const;
    void PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const;

    //bool PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const;
    // TODO: check return value, raise exception.

    void ShowPosition(long pos);

    // Clipboard operations
    void Copy();
    void Cut();
    void Paste();

    bool CanCopy() const;
    bool CanCut() const;
    bool CanPaste() const;

    // Undo/redo
    void Undo();
    void Redo();

    bool CanUndo() const;
    bool CanRedo() const;

    // Insertion point
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    long GetInsertionPoint() const;
    long GetLastPosition() const;

    void SetSelection(long from, long to);
    void SelectAll();
    void SetEditable(bool editable);

    bool IsSingleLine();
    bool IsMultiLine();


    %addmethods {
        void write(const wxString& text) {
            self->AppendText(text);
        }
    }

    // TODO: replace this when the method is really added to wxTextCtrl
    %addmethods {
        wxString GetString(long from, long to) {
            return self->GetValue().Mid(from, to - from);
        }
    }
};

//----------------------------------------------------------------------

class wxScrollBar : public wxControl {
public:
    wxScrollBar(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyScrollBarNameStr);
    %name(wxPreScrollBar)wxScrollBar();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPyScrollBarNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreScrollBar:val._setOORInfo(val)"

    int GetRange();
    int GetPageSize();
    int GetThumbPosition();
    int GetThumbSize();
    %name(GetThumbLength) int GetThumbSize();  // to match the docs

    bool IsVertical();

    void SetThumbPosition(int viewStart);
    void SetScrollbar(int position, int thumbSize,
                      int range,    int pageSize,
                      bool refresh = TRUE);
};

//----------------------------------------------------------------------

class wxSpinButton : public wxControl {
public:
    wxSpinButton(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 const wxString& name = wxPySPIN_BUTTON_NAME);
    %name(wxPreSpinButton)wxSpinButton();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 const wxString& name = wxPySPIN_BUTTON_NAME);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSpinButton:val._setOORInfo(val)"

    int GetMax();
    int GetMin();
    int GetValue();
    void SetRange(int min, int max);
    void SetValue(int value);
};

//----------------------------------------------------------------------

class wxStaticBitmap : public wxControl {
public:
    wxStaticBitmap(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);
    %name(wxPreStaticBitmap)wxStaticBitmap();

    bool Create(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxString& name = wxPyStaticBitmapNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStaticBitmap:val._setOORInfo(val)"

    wxBitmap GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
    void SetIcon(const wxIcon& icon);
};

//----------------------------------------------------------------------

class wxRadioBox : public wxControl {
public:
    wxRadioBox(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT = 0, wxString* choices = NULL,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyRadioBoxNameStr);
    %name(wxPreRadioBox)wxRadioBox();

    bool Create(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT = 0, wxString* choices = NULL,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxPyRadioBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreRadioBox:val._setOORInfo(val)"

    void Enable(bool enable);
    %name(EnableItem)void Enable(int n, bool enable);
    int FindString(const wxString& string);

    wxString GetString(int n);

#ifdef __WXGTK__
    %name(GetItemLabel)wxString GetLabel( int item );
    %name(SetItemLabel)void SetLabel( int item, const wxString& label );
#else
    void SetString(int n, const wxString& label);
    %pragma(python) addtoclass = "
    GetItemLabel = GetString
    SetItemLabel = SetString
    "
    int GetColumnCount();
    int GetRowCount();
#endif

    int GetSelection();
    wxString GetStringSelection();
    int GetCount();
    %pragma(python) addtoclass = "Number = GetCount"

    void SetSelection(int n);
    void SetStringSelection(const wxString& string);
    void Show(bool show);
    %name(ShowItem)void Show(int item, bool show);
};

//----------------------------------------------------------------------

class wxRadioButton : public wxControl {
public:
    wxRadioButton(wxWindow* parent, wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyRadioButtonNameStr);
    %name(wxPreRadioButton)wxRadioButton();

    bool Create(wxWindow* parent, wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  const wxString& name = wxPyRadioButtonNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreRadioButton:val._setOORInfo(val)"

    bool GetValue();
    void SetValue(bool value);
};

//----------------------------------------------------------------------

class wxSlider : public wxControl {
public:
    wxSlider(wxWindow* parent, wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& point = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPySliderNameStr);
    %name(wxPreSlider)wxSlider();

    bool Create(wxWindow* parent, wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& point = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxPySliderNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSlider:val._setOORInfo(val)"

    void ClearSel();
    void ClearTicks();
    int GetLineSize();
    int GetMax();
    int GetMin();
    int GetPageSize();
    int GetSelEnd();
    int GetSelStart();
    int GetThumbLength();
    int GetTickFreq();
    int GetValue();
    void SetRange(int minValue, int maxValue);
    void SetTickFreq(int n, int pos);
    void SetLineSize(int lineSize);
    void SetPageSize(int pageSize);
    void SetSelection(int startPos, int endPos);
    void SetThumbLength(int len);
    void SetTick(int tickPos);
    void SetValue(int value);
};


//----------------------------------------------------------------------

class wxSpinCtrl : public wxSpinButton {
public:
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxPySpinCtrlNameStr);
    %name(wxPreSpinCtrl)wxSpinCtrl();

    bool Create(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxPyEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxPySpinCtrlNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSpinCtrl:val._setOORInfo(val)"

    int GetMax();
    int GetMin();
    int GetValue();
    void SetRange(int min, int max);
    void SetValue(int value);
#ifdef __WXGTK__
    %addmethods {
        void SetSelection(long from, long to) {
        }
    }
#else
    void SetSelection(long from, long to);
#endif
};


//----------------------------------------------------------------------

#ifndef __WXMAC__
enum { wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, };

class wxToggleButton : public wxControl {
public:
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyCheckBoxNameStr);
    %name(wxPreToggleButton)wxToggleButton();

    bool Create(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyCheckBoxNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreToggleButton:val._setOORInfo(val)"

    void SetValue(bool value);
    bool GetValue() const ;
    void SetLabel(const wxString& label);

};

#endif

//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------



