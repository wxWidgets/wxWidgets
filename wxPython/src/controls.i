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

class wxControl : public wxWindow {
public:
    wxControl(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos=wxDefaultPosition,
                       const wxSize& size=wxDefaultSize,
                       long style=0,
                       const wxValidator& validator=wxDefaultValidator,
                       const char* name="control");
    %name(wxPreControl)wxControl();

    bool Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos=wxDefaultPosition,
                       const wxSize& size=wxDefaultSize,
                       long style=0,
                       const wxValidator& validator=wxDefaultValidator,
                       const char* name="control");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreControl:val._setOORInfo(val)"

    void Command(wxCommandEvent& event);
    wxString GetLabel();
    void SetLabel(const wxString& label);
};


//----------------------------------------------------------------------


class wxControlWithItems : public wxControl {
public:

    // void Clear();  ambiguous, redefine below...
    void Delete(int n);

    int GetCount();
    %pragma(python) addtoclass = "Number = GetCount"
    wxString GetString(int n);
    void SetString(int n, const wxString& s);
    int FindString(const wxString& s);

    void Select(int n);
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
//----------------------------------------------------------------------

class wxButton : public wxControl {
public:
    wxButton(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             char* name = "button");
    %name(wxPreButton)wxButton();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             char* name = "button");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreButton:val._setOORInfo(val)"

    void SetDefault();
    void SetBackgroundColour(const wxColour& colour);
    void SetForegroundColour(const wxColour& colour);
#ifdef __WXMSW__
    void SetImageLabel(const wxBitmap& bitmap);
    void SetImageMargins(wxCoord x, wxCoord y);
#endif
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
                   char* name = "button");
    %name(wxPreBitmapButton)wxBitmapButton();

    bool Create(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator,
                   char* name = "button");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreBitmapButton:val._setOORInfo(val)"

    wxBitmap& GetBitmapLabel();
    wxBitmap& GetBitmapDisabled();
    wxBitmap& GetBitmapFocus();
    wxBitmap& GetBitmapSelected();
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
               const wxValidator& val = wxDefaultValidator,
               char* name = "checkBox");
    %name(wxPreCheckBox)wxCheckBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& val = wxDefaultValidator,
               char* name = "checkBox");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreCheckBox:val._setOORInfo(val)"

    bool GetValue();
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
             char* name = "choice");
    %name(wxPreChoice)wxChoice();

    bool Create(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             int LCOUNT=0, wxString* choices=NULL,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             char* name = "choice");

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

// wxGTK's wxComboBox doesn't derive from wxChoice like wxMSW, or
// even wxControlWithItems, so we have to duplicate the methods
// here... <blech!>

#ifdef __WXGTK__
class wxComboBox : public wxControl
{
public:
    wxComboBox(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "comboBox");
    %name(wxPreComboBox)wxComboBox();

    bool Create(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "comboBox");

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



#else  // For all but wxGTK


class wxComboBox : public wxChoice {
public:
    wxComboBox(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "comboBox");
    %name(wxPreComboBox)wxComboBox();

    bool Create(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "comboBox");

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
            char* name = "gauge");
    %name(wxPreGauge)wxGauge();

    bool Create(wxWindow* parent, wxWindowID id, int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            char* name = "gauge");

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
                char* name = "staticBox");
    %name(wxPreStaticBox)wxStaticBox();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                char* name = "staticBox");

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
                  const char* name = "staticLine" );
    %name(wxPreStaticLine)wxStaticLine();

    bool Create( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  const wxSize &size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const char* name = "staticLine" );

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
                 char* name = "staticText");
    %name(wxPreStaticText)wxStaticText();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 char* name = "staticText");

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
              char* name = "listBox");
    %name(wxPreListBox)wxListBox();

    bool Create(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int LCOUNT, wxString* choices = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              char* name = "listBox");

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
                   char* name = "listBox");
    %name(wxPreCheckListBox)wxCheckListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int LCOUNT = 0,
                   wxString* choices = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   char* name = "listBox");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreCheckListBox:val._setOORInfo(val)"

    bool  IsChecked(int uiIndex);
    void  Check(int uiIndex, int bCheck = TRUE);
    void InsertItems(int LCOUNT, wxString* choices, int pos);

#ifndef __WXMAC__
    int GetItemHeight();
#endif
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

    const wxColour& GetTextColour() const;
    const wxColour& GetBackgroundColour() const;
    const wxFont& GetFont() const;

    // returns false if we have any attributes set, true otherwise
    bool IsDefault();
};



class wxTextCtrl : public wxControl {
public:
    wxTextCtrl(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "text");
    %name(wxPreTextCtrl)wxTextCtrl();

    bool Create(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "text");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreTextCtrl:val._setOORInfo(val)"


    wxString GetValue() const;
    void SetValue(const wxString& value);

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
    bool SaveFile(const wxString& file = wxEmptyString);

    // clears the dirty flag
    void DiscardEdits();

    // set the max number of characters which may be entered in a single line
    // text control
    void SetMaxLength(unsigned long len);

    // writing text inserts it at the current position, appending always
    // inserts it at the end
    void WriteText(const wxString& text);
    void AppendText(const wxString& text);

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
    bool PositionToXY(long pos, long *OUTPUT, long *OUTPUT) const;

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

    %addmethods {
        void write(const wxString& text) {
            self->AppendText(text);
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
                char* name = "scrollBar");
    %name(wxPreScrollBar)wxScrollBar();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                char* name = "scrollBar");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreScrollBar:val._setOORInfo(val)"

    int GetRange();
    int GetPageSize();
    int GetThumbPosition();
    int GetThumbSize();
    %name(GetThumbLength) int GetThumbSize();  // to match the docs
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
                 char* name = "spinButton");
    %name(wxPreSpinButton)wxSpinButton();

    bool Create(wxWindow* parent, wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 char* name = "spinButton");

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
                   char* name = "staticBitmap");
    %name(wxPreStaticBitmap)wxStaticBitmap();

    bool Create(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   char* name = "staticBitmap");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStaticBitmap:val._setOORInfo(val)"

    const wxBitmap& GetBitmap();
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
               char* name = "radioBox");
    %name(wxPreRadioBox)wxRadioBox();

    bool Create(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int LCOUNT = 0, wxString* choices = NULL,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxDefaultValidator,
               char* name = "radioBox");

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
                  char* name = "radioButton");
    %name(wxPreRadioButton)wxRadioButton();

    bool Create(wxWindow* parent, wxWindowID id,
                  const wxString& label,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxDefaultValidator,
                  char* name = "radioButton");

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
             char* name = "slider");
    %name(wxPreSlider)wxSlider();

    bool Create(wxWindow* parent, wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& point = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             char* name = "slider");

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
               const char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const char* name = "wxSpinCtrl");
    %name(wxPreSpinCtrl)wxSpinCtrl();

    bool Create(wxWindow *parent,
               wxWindowID id = -1,
               const char* value = "",
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const char* name = "wxSpinCtrl");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSpinCtrl:val._setOORInfo(val)"

    int GetMax();
    int GetMin();
    int GetValue();
    void SetRange(int min, int max);
    void SetValue(int value);

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
                   const char* name = "toggle");
    %name(wxPreToggleButton)wxToggleButton();

    bool Create(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const char* name = "toggle");

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



