/////////////////////////////////////////////////////////////////////////////
// Name:        stattool.i
// Purpose:     SWIG definitions for StatusBar and ToolBar classes
//
// Author:      Robin Dunn
//
// Created:     08/24/1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module stattool

%{
#include "helpers.h"
#include <wx/toolbar.h>
#include <wx/tbarsmpl.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import windows.i
%import controls.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

class wxStatusBar : public wxWindow {
public:
    wxStatusBar(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = wxST_SIZEGRIP,
                char* name = "statusBar");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    %addmethods {
        %new wxRect* GetFieldRect(long item) {
            wxRect* rect= new wxRect;
            self->GetFieldRect(item, *rect);
            return rect;
        }
    }
    int GetFieldsCount(void);
    wxString GetStatusText(int ir = 0);
    int GetBorderX();
    int GetBorderY();

//      void DrawField(wxDC& dc, int i);
//      void DrawFieldText(wxDC& dc, int i);
//      void InitColours(void);

    void SetFieldsCount(int number = 1);
    void SetStatusText(const wxString& text, int i = 0);
    void SetStatusWidths(int LCOUNT, int* choices);
    void SetMinHeight(int height);
};


//---------------------------------------------------------------------------

class wxToolBarBase;

enum wxToolBarToolStyle
{
    wxTOOL_STYLE_BUTTON    = 1,
    wxTOOL_STYLE_SEPARATOR = 2,
    wxTOOL_STYLE_CONTROL
};



class wxToolBarToolBase {
public:
//      wxToolBarToolBase(wxToolBarBase *tbar = (wxToolBarBase *)NULL,
//                        int id = wxID_SEPARATOR,
//                        const wxBitmap& bitmap1 = wxNullBitmap,
//                        const wxBitmap& bitmap2 = wxNullBitmap,
//                        bool toggle = FALSE,
//                        wxObject *clientData = (wxObject *) NULL,
//                        const wxString& shortHelpString = wxEmptyString,
//                        const wxString& longHelpString = wxEmptyString);
//      wxToolBarToolBase(wxToolBarBase *tbar, wxControl *control);
//      ~wxToolBarToolBase();

    %addmethods { void Destroy() { delete self; } }

    int GetId();
    wxControl *GetControl();
    wxToolBarBase *GetToolBar();
    int IsButton();
    int IsControl();
    int IsSeparator();
    int GetStyle();
    bool IsEnabled();
    bool IsToggled();
    bool CanBeToggled();
    const wxBitmap& GetBitmap1();
    const wxBitmap& GetBitmap2();
    const wxBitmap& GetBitmap();
    wxString GetShortHelp();
    wxString GetLongHelp();
    bool Enable(bool enable);
    bool Toggle(bool toggle);
    bool SetToggle(bool toggle);
    bool SetShortHelp(const wxString& help);
    bool SetLongHelp(const wxString& help);
    void SetBitmap1(const wxBitmap& bmp);
    void SetBitmap2(const wxBitmap& bmp);
    void Detach();
    void Attach(wxToolBarBase *tbar);

    //wxObject *GetClientData();
    %addmethods {
        // convert the ClientData back to a PyObject
        PyObject* GetClientData() {
            wxPyUserData* udata = (wxPyUserData*)self->GetClientData();
            if (udata) {
                Py_INCREF(udata->m_obj);
                return udata->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        void SetClientData(PyObject* clientData) {
            self->SetClientData(new wxPyUserData(clientData));
        }
    }
};



class wxToolBarBase : public wxControl {
public:

    // This is an Abstract Base Class

    %addmethods {
        // wrap ClientData in a class that knows about PyObjects
        wxToolBarToolBase *AddTool(int id,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& pushedBitmap = wxNullBitmap,
                                   int isToggle = FALSE,
                                   PyObject *clientData = NULL,
                                   const wxString& shortHelpString = wxPyEmptyStr,
                                   const wxString& longHelpString = wxPyEmptyStr) {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->AddTool(id, bitmap, pushedBitmap, (bool)isToggle,
                                 udata, shortHelpString, longHelpString);
        }

        // This one is easier to use...
        wxToolBarToolBase *AddSimpleTool(int id,
                                         const wxBitmap& bitmap,
                                         const wxString& shortHelpString = wxPyEmptyStr,
                                         const wxString& longHelpString = wxPyEmptyStr,
                                         int isToggle = FALSE) {
            return self->AddTool(id, bitmap, wxNullBitmap, isToggle, NULL,
                                 shortHelpString, longHelpString);
        }


        // wrap ClientData in a class that knows about PyObjects
        wxToolBarToolBase *InsertTool(size_t pos,
                                      int id,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& pushedBitmap = wxNullBitmap,
                                      int isToggle = FALSE,
                                      PyObject *clientData = NULL,
                                      const wxString& shortHelpString = wxPyEmptyStr,
                                      const wxString& longHelpString = wxPyEmptyStr) {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->InsertTool(pos, id, bitmap, pushedBitmap, (bool)isToggle,
                                    udata, shortHelpString, longHelpString);
        }

        // This one is easier to use...
        wxToolBarToolBase *InsertSimpleTool(size_t pos,
                                            int id,
                                            const wxBitmap& bitmap,
                                            const wxString& shortHelpString = wxPyEmptyStr,
                                            const wxString& longHelpString = wxPyEmptyStr,
                                            int isToggle = FALSE) {
            return self->InsertTool(pos, id, bitmap, wxNullBitmap, isToggle, NULL,
                                    shortHelpString, longHelpString);
        }
    }


    wxToolBarToolBase *AddControl(wxControl *control);
    wxToolBarToolBase *InsertControl(size_t pos, wxControl *control);

    wxToolBarToolBase *AddSeparator();
    wxToolBarToolBase *InsertSeparator(size_t pos);

    wxToolBarToolBase *RemoveTool(int id);

    bool DeleteToolByPos(size_t pos);
    bool DeleteTool(int id);
    void ClearTools();
    bool Realize();

    void EnableTool(int id, bool enable);
    void ToggleTool(int id, bool toggle);
    void SetToggle(int id, bool toggle);


    %addmethods {
        // convert the ClientData back to a PyObject
        PyObject* GetToolClientData(int index) {
            wxPyUserData* udata = (wxPyUserData*)self->GetToolClientData(index);
            if (udata) {
                Py_INCREF(udata->m_obj);
                return udata->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        void SetToolClientData(int index, PyObject* clientData) {
            self->SetToolClientData(index, new wxPyUserData(clientData));
        }
    }


    bool GetToolState(int id);
    bool GetToolEnabled(int id);
    void SetToolShortHelp(int id, const wxString& helpString);
    wxString GetToolShortHelp(int id);
    void SetToolLongHelp(int id, const wxString& helpString);
    wxString GetToolLongHelp(int id);

    %name(SetMarginsXY) void SetMargins(int x, int y);
    void SetMargins(const wxSize& size);
    void SetToolPacking(int packing);
    void SetToolSeparation(int separation);
    wxSize GetToolMargins();
    int GetToolPacking();
    int GetToolSeparation();

    void SetRows(int nRows);
    void SetMaxRowsCols(int rows, int cols);
    int GetMaxRows();
    int GetMaxCols();

    void SetToolBitmapSize(const wxSize& size);
    wxSize GetToolBitmapSize();
    wxSize GetToolSize();

};




class wxToolBar : public wxToolBarBase {
public:
    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const char* name = wxToolBarNameStr);

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);
};




class wxToolBarSimple : public wxToolBarBase {
public:
    wxToolBarSimple(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxPyDefaultPosition,
                    const wxSize& size = wxPyDefaultSize,
                    long style = wxNO_BORDER | wxTB_HORIZONTAL,
                    const char* name = wxToolBarNameStr);

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);
};

//---------------------------------------------------------------------------



#ifdef THE_OLD_ONE

class wxToolBarTool {
public:
    wxToolBarTool();
    ~wxToolBarTool();
    void SetSize( long w, long h )
    long GetWidth ();
    long GetHeight ();
    wxControl *GetControl();

public:
    int                   m_toolStyle;
    wxObject *            m_clientData;
    int                   m_index;
    long                  m_x;
    long                  m_y;
    long                  m_width;
    long                  m_height;
    bool                  m_toggleState;
    bool                  m_isToggle;
    bool                  m_deleteSecondBitmap;
    bool                  m_enabled;
    wxBitmap              m_bitmap1;
    wxBitmap              m_bitmap2;
    bool                  m_isMenuCommand;
    wxString              m_shortHelpString;
    wxString              m_longHelpString;
};



//  class wxToolBarBase : public wxControl {
//  public:

class wxToolBar : public wxControl {
public:
    wxToolBar(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxPyDefaultPosition,
              const wxSize& size = wxPyDefaultSize,
              long style = wxTB_HORIZONTAL | wxNO_BORDER,
              char* name = "toolBar");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"


    bool AddControl(wxControl * control);
    void AddSeparator();
    void ClearTools();

    // Ignoge the clientData for now...
    %addmethods {
        wxToolBarTool* AddTool(int toolIndex,
                               const wxBitmap& bitmap1,
                               const wxBitmap& bitmap2 = wxNullBitmap,
                               int  isToggle = FALSE,
                               long xPos = -1,
                               long yPos = -1,
                               //wxObject* clientData = NULL,
                               const wxString& shortHelpString = wxPyEmptyStr,
                               const wxString& longHelpString = wxPyEmptyStr) {
            return self->AddTool(toolIndex, bitmap1, bitmap2,
                                 isToggle, xPos, yPos, NULL,
                                 shortHelpString, longHelpString);
        }

        wxToolBarTool* AddSimpleTool(int toolIndex,
                               const wxBitmap& bitmap,
                               const wxString& shortHelpString = wxPyEmptyStr,
                               const wxString& longHelpString = wxPyEmptyStr) {
            return self->AddTool(toolIndex, bitmap, wxNullBitmap,
                                 FALSE, -1, -1, NULL,
                                 shortHelpString, longHelpString);
        }
    }


    void EnableTool(int toolIndex, bool enable);
#ifdef __WXMSW__
    wxToolBarTool* FindToolForPosition(long x, long y);
    wxSize GetToolSize();
    wxSize GetToolBitmapSize();
    void SetToolBitmapSize(const wxSize& size);
    wxSize GetMaxSize();
#endif
    wxSize GetToolMargins();
//    wxObject* GetToolClientData(int toolIndex);
    bool GetToolEnabled(int toolIndex);
    wxString GetToolLongHelp(int toolIndex);
    int GetToolPacking();
    int GetToolSeparation();
    wxString GetToolShortHelp(int toolIndex);
    bool GetToolState(int toolIndex);


    bool Realize();

    void SetToolLongHelp(int toolIndex, const wxString& helpString);
    void SetToolShortHelp(int toolIndex, const wxString& helpString);
    void SetMargins(const wxSize& size);
    void SetToolPacking(int packing);
    void SetToolSeparation(int separation);
    void ToggleTool(int toolIndex, const bool toggle);
    void SetToggle(int toolIndex, bool toggle);
    void SetMaxRowsCols(int rows, int cols);
    int GetMaxRows();
    int GetMaxCols();
};


#endif

//---------------------------------------------------------------------------

