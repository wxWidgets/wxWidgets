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
//#include <wx/toggbutt.h>

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

%{
wxValidator wxPyDefaultValidator;       // Non-const default because of SWIG
%}

%readonly
wxValidator wxDefaultValidator;
%readwrite

//----------------------------------------------------------------------

class wxControl : public wxWindow {
public:
    wxControl(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos=wxPyDefaultPosition,
                       const wxSize& size=wxPyDefaultSize,
                       long style=0,
                       const wxValidator& validator=wxPyDefaultValidator,
                       const char* name="control");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Command(wxCommandEvent& event);
    wxString GetLabel();
    void SetLabel(const wxString& label);
};


//----------------------------------------------------------------------

class wxButton : public wxControl {
public:
    wxButton(wxWindow* parent, wxWindowID id, const wxString& label,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             long style = 0,
             const wxValidator& validator = wxPyDefaultValidator,
             char* name = "button");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void SetDefault();
    void SetBackgroundColour(const wxColour& colour);
    void SetForegroundColour(const wxColour& colour);
};


%inline %{
    wxSize wxButton_GetDefaultSize() {
        return wxButton::GetDefaultSize();
    }
%}

//----------------------------------------------------------------------

class wxBitmapButton : public wxButton {
public:
    wxBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxPyDefaultValidator,
                   char* name = "button");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    wxBitmap& GetBitmapLabel();
    wxBitmap& GetBitmapDisabled();
    wxBitmap& GetBitmapFocus();
    wxBitmap& GetBitmapSelected();
    void SetBitmapDisabled(const wxBitmap& bitmap);
    void SetBitmapFocus(const wxBitmap& bitmap);
    void SetBitmapSelected(const wxBitmap& bitmap);
    void SetBitmapLabel(const wxBitmap& bitmap);

};

//----------------------------------------------------------------------

//  class wxToggleButton : public wxControl {
//  public:
//      wxToggleButton(wxWindow *parent, wxWindowID id, const wxString& label,
//                     const wxPoint& pos = wxPyDefaultPosition,
//                     const wxSize& size = wxPyDefaultSize, long style = 0,
//                     const wxValidator& validator = wxPyDefaultValidator,
//                     const char* name = "toggle");
//      void SetValue(bool value);
//      bool GetValue() const ;
//      void SetLabel(const wxString& label);
//  };

//  class wxBitmapToggleButton : public wxToggleButton {
//  public:
//      wxBitmapToggleButton(wxWindow *parent, wxWindowID id, const wxBitmap *label,
//                           const wxPoint& pos = wxPyDefaultPosition,
//                           const wxSize& size = wxPyDefaultSize, long style = 0,
//                           const wxValidator& validator = wxPyDefaultValidator,
//                           const char *name = "toggle");
//      void SetLabel(const wxBitmap& bitmap);
//  };


//----------------------------------------------------------------------

class wxCheckBox : public wxControl {
public:
    wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               const wxValidator& val = wxPyDefaultValidator,
               char* name = "checkBox");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    bool GetValue();
    void SetValue(const bool state);
};

//----------------------------------------------------------------------

class wxChoice : public wxControl {
public:
    wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             int LCOUNT=0, wxString* choices=NULL,
             long style = 0,
             const wxValidator& validator = wxPyDefaultValidator,
             char* name = "choice");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Append(const wxString& item);
    void Clear();
    int FindString(const wxString& string);
    int GetColumns();
    int GetSelection();
    wxString GetString(const int n);
    wxString GetStringSelection();
    int Number();
    void SetColumns(const int n = 1);
    void SetSelection(const int n);
    void SetStringSelection(const wxString& string);
};

//----------------------------------------------------------------------

class wxComboBox : public wxChoice {
public:
    wxComboBox(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               int LCOUNT=0, wxString* choices=NULL,
               long style = 0,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "comboBox");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Append(const wxString& item);
    // TODO: void Append(const wxString& item, char* clientData);
    void Clear();
    void Copy();
    void Cut();
    void Delete(int n);
    // NotMember??:    void Deselect(int n);
    int FindString(const wxString& string);
    // TODO: char* GetClientData(const int n);
    long GetInsertionPoint();
    long GetLastPosition();
    int GetSelection();
    wxString GetString(int n);
    wxString GetStringSelection();
    wxString GetValue();
    int Number();
    void Paste();
    void Replace(long from, long to, const wxString& text);
    void Remove(long from, long to);
    // TODO:    void SetClientData(const int n, char* data);
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    void SetSelection(int n);
    %name(SetMark)void SetSelection(long from, long to);
    void SetValue(const wxString& text);
};

//----------------------------------------------------------------------

class wxGauge : public wxControl {
public:
    wxGauge(wxWindow* parent, wxWindowID id, int range,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxPyDefaultValidator,
            char* name = "gauge");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

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
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = 0,
                char* name = "staticBox");
};


//----------------------------------------------------------------------


class wxStaticLine : public wxControl {
public:
    wxStaticLine( wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxPyDefaultPosition,
                  const wxSize &size = wxPyDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const char* name = "staticLine" );
};


//----------------------------------------------------------------------

class wxStaticText : public wxControl {
public:
    wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxPyDefaultPosition,
                 const wxSize& size = wxPyDefaultSize,
                 long style = 0,
                 char* name = "staticText");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    wxString GetLabel();
    void SetLabel(const wxString& label);
};

//----------------------------------------------------------------------

class wxListBox : public wxControl {
public:
    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              int LCOUNT, wxString* choices = NULL,
              long style = 0,
              const wxValidator& validator = wxPyDefaultValidator,
              char* name = "listBox");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Append(const wxString& item);
    // TODO:    void Append(const wxString& item, char* clientData);
    void Clear();
    void Delete(int n);
    void Deselect(int n);
    int FindString(const wxString& string);
    // TODO:    char* GetClientData(const int n);
    int GetSelection();

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

    wxString GetString(int n);
    wxString GetStringSelection();
    int Number();
    bool Selected(const int n);
    void Set(int LCOUNT, wxString* choices);
    // TODO:    void SetClientData(const int n, char* data);
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
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   int LCOUNT = 0,
                   wxString* choices = NULL,
                   long style = 0,
                   const wxValidator& validator = wxPyDefaultValidator,
                   char* name = "listBox");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    bool  IsChecked(int uiIndex);
    void  Check(int uiIndex, int bCheck = TRUE);
    void InsertItems(int LCOUNT, wxString* choices, int pos);

    int GetItemHeight();
};

//----------------------------------------------------------------------

class wxTextCtrl : public wxControl {
public:
    wxTextCtrl(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "text");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Clear();
    void Copy();
    void Cut();
    void DiscardEdits();
    long GetInsertionPoint();
    long GetLastPosition();
    int GetLineLength(long lineNo);
    wxString GetLineText(long lineNo);
    int GetNumberOfLines();
    wxString GetValue();
    bool IsModified();
    bool LoadFile(const wxString& filename);
    void Paste();
    void PositionToXY(long pos, long *OUTPUT, long *OUTPUT);
    void Remove(long from, long to);
    void Replace(long from, long to, const wxString& value);
    bool SaveFile(const wxString& filename);
    void SetEditable(bool editable);
    void SetInsertionPoint(long pos);
    void SetInsertionPointEnd();
    void SetSelection(long from, long to);
    void SetValue(const wxString& value);
    void ShowPosition(long pos);
    void WriteText(const wxString& text);
    void AppendText(const wxString& text);
    long XYToPosition(long x, long y);

    bool CanCopy();
    bool CanCut();
    bool CanPaste();
    bool CanRedo();
    bool CanUndo();
    void GetSelection(long* OUTPUT, long* OUTPUT);
    bool IsEditable();
    void Undo();
    void Redo();

    %addmethods {
        void write(const wxString& text) {
            self->AppendText(text + '\n');
        }
    }
};

//----------------------------------------------------------------------

class wxScrollBar : public wxControl {
public:
    wxScrollBar(wxWindow* parent, wxWindowID id = -1,
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxPyDefaultValidator,
                char* name = "scrollBar");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

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
                 const wxPoint& pos = wxPyDefaultPosition,
                 const wxSize& size = wxPyDefaultSize,
                 long style = wxSP_HORIZONTAL,
                 char* name = "spinButton");

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
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   long style = 0,
                   char* name = "staticBitmap");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    const wxBitmap& GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
    void SetIcon(const wxIcon& icon);
};

//----------------------------------------------------------------------

class wxRadioBox : public wxControl {
public:
    wxRadioBox(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               int LCOUNT = 0, wxString* choices = NULL,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "radioBox");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void Enable(bool enable);
    %name(EnableItem)void Enable(int n, bool enable);
    int FindString(const wxString& string);

//***    wxString GetLabel();
//***    void SetLabel(const wxString& label);

    %name(GetItemLabel)wxString GetLabel(int n);
    int GetSelection();
    wxString GetString(int n);
    wxString GetStringSelection();
    int Number();
    %name(SetItemLabel)void SetLabel(int n, const wxString& label);
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
                  const wxPoint& pos = wxPyDefaultPosition,
                  const wxSize& size = wxPyDefaultSize,
                  long style = 0,
                  const wxValidator& validator = wxPyDefaultValidator,
                  char* name = "radioButton");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    bool GetValue();
    void SetValue(bool value);
};

//----------------------------------------------------------------------

class wxSlider : public wxControl {
public:
    wxSlider(wxWindow* parent, wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& point = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxPyDefaultValidator,
             char* name = "slider");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

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
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const char* name = "wxSpinCtrl");


};


//----------------------------------------------------------------------

