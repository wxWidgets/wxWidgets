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

#ifdef __WXMSW__
#include <wx/spinbutt.h>
#if wxUSE_OWNER_DRAWN
#include <wx/checklst.h>
#endif
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

%pragma(python) code = "import wxp"

//----------------------------------------------------------------------

%{
wxValidator wxPyDefaultValidator;       // Non-const default because of SWIG
%}

//----------------------------------------------------------------------

class wxControl : public wxWindow {
public:
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

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    void SetDefault();
};

//----------------------------------------------------------------------

class wxBitmapButton : public wxButton {
public:
    wxBitmapButton(wxWindow* parent, wxWindowID id, const wxBitmap& bitmap,
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxPyDefaultValidator,
                   char* name = "button");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    wxBitmap& GetBitmapLabel();
#ifdef __WXMSW__
    wxBitmap& GetBitmapDisabled();
    wxBitmap& GetBitmapFocus();
    wxBitmap& GetBitmapSelected();
    void SetBitmapDisabled(const wxBitmap& bitmap);
    void SetBitmapFocus(const wxBitmap& bitmap);
    void SetBitmapSelected(const wxBitmap& bitmap);
#endif
    void SetBitmapLabel(const wxBitmap& bitmap);

};

//----------------------------------------------------------------------

class wxCheckBox : public wxControl {
public:
    wxCheckBox(wxWindow* parent, wxWindowID id, const wxString& label,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               const wxValidator& val = wxPyDefaultValidator,
               char* name = "checkBox");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    bool GetValue();
    void SetValue(const bool state);
};

//----------------------------------------------------------------------

class wxChoice : public wxControl {
public:
    wxChoice(wxWindow *parent, wxWindowID id,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             int LCOUNT=0, wxString* LIST=NULL,
             long style = 0,
             const wxValidator& validator = wxPyDefaultValidator,
             char* name = "choice");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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

class wxComboBox : public wxControl {
public:
    wxComboBox(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               int LCOUNT=0, wxString* LIST=NULL,
               long style = 0,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "comboBox");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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
    void SetSelection(int n, bool select = TRUE);
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

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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

class wxStaticText : public wxControl {
public:
    wxStaticText(wxWindow* parent, wxWindowID id, const wxString& label,
                 const wxPoint& pos = wxPyDefaultPosition,
                 const wxSize& size = wxPyDefaultSize,
                 long style = 0,
                 char* name = "staticText");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    wxString GetLabel();
    void SetLabel(const wxString& label);
};

//----------------------------------------------------------------------

class wxListBox : public wxControl {
public:
    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              int LCOUNT, wxString* LIST = NULL,
              long style = 0,
              const wxValidator& validator = wxPyDefaultValidator,
              char* name = "listBox");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    void Append(const wxString& item);
    // TODO:    void Append(const wxString& item, char* clientData);
    void Clear();
    void Delete(int n);
    void Deselect(int n);
    int FindString(const wxString& string);
    // TODO:    char* GetClientData(const int n);
    int GetSelection();
    // TODO: int GetSelections(int **selections);
    wxString GetString(int n);
    wxString GetStringSelection();
    int Number();
    bool Selected(const int n);
    void Set(int LCOUNT, wxString* LIST);
    // TODO:    void SetClientData(const int n, char* data);
    void SetFirstItem(int n);
    %name(SetFirstItemStr)void SetFirstItem(const wxString& string);
    void SetSelection(int n, bool select = TRUE);
    void SetString(int n, const wxString& string);
    void SetStringSelection(const wxString& string, bool select = TRUE);
};


//----------------------------------------------------------------------

#ifdef __WXMSW__
class wxCheckListBox : public wxListBox {
public:
    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   int LCOUNT = 0,
                   wxString* LIST = NULL,
                   long style = 0,
                   const wxValidator& validator = wxPyDefaultValidator,
                   char* name = "listBox");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    bool  IsChecked(int uiIndex);
    void  Check(int uiIndex, bool bCheck = TRUE);

    int GetItemHeight();
};
#endif

//----------------------------------------------------------------------

class wxTextCtrl : public wxControl {
public:
    wxTextCtrl(wxWindow* parent, wxWindowID id, char* value = "",
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               long style = 0,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "text");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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
    long XYToPosition(long x, long y);
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

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    int GetRange();
    int GetPageSize();
    int GetPosition();
    int GetThumbSize();
    void SetPosition(int viewStart);
    void SetScrollbar(int position, int thumbSize,
                      int range,    int pageSize,
                      bool refresh = TRUE);
};

//----------------------------------------------------------------------

#ifdef __WXMSW__
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
#endif

//----------------------------------------------------------------------

class wxStaticBitmap : public wxControl {
public:
    wxStaticBitmap(wxWindow* parent, wxWindowID id,
                   const wxBitmap& bitmap,
                   const wxPoint& pos = wxPyDefaultPosition,
                   const wxSize& size = wxPyDefaultSize,
                   long style = 0,
                   char* name = "staticBitmap");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    wxBitmap& GetBitmap();
    void SetBitmap(const wxBitmap& bitmap);
};

//----------------------------------------------------------------------

class wxRadioBox : public wxControl {
public:
    wxRadioBox(wxWindow* parent, wxWindowID id,
               const wxString& label,
               const wxPoint& point = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize,
               int LCOUNT = 0, wxString* LIST = NULL,
               int majorDimension = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& validator = wxPyDefaultValidator,
               char* name = "radioBox");

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

    %name(EnableBox)void Enable(bool enable);
    void Enable(int n, bool enable);
    int FindString(const wxString& string);
#ifdef __WXMSW__
    %name(GetBoxLabel)wxString GetLabel();
#endif
    wxString GetLabel(int n);
    int GetSelection();
    wxString GetString(int n);
    wxString GetStringSelection();
    int Number();
    %name(SetBoxLabel)void SetLabel(const wxString& label);
    void SetLabel(int n, const wxString& label);
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

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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

    %pragma(python) addtomethod = "__init__:wxp._StdWindowCallbacks(self)"

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


/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.6  1998/11/15 23:03:43  RD
// Removing some ifdef's for wxGTK
//
// Revision 1.5  1998/10/07 07:34:32  RD
// Version 0.4.1 for wxGTK
//
// Revision 1.4  1998/10/02 06:40:35  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.3  1998/08/18 19:48:14  RD
// more wxGTK compatibility things.
//
// It builds now but there are serious runtime problems...
//
// Revision 1.2  1998/08/15 07:36:28  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.1  1998/08/09 08:25:49  RD
// Initial version
//
//

