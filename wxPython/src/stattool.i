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
    wxStatusBar(wxWindow* parent, wxWindowID id = -1,
                long style = wxST_SIZEGRIP,
                char* name = "statusBar");
    %name(wxPreStatusBar)wxStatusBar();

    bool Create(wxWindow* parent, wxWindowID id,
                long style = wxST_SIZEGRIP,
                char* name = "statusBar");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreStatusBar:val._setOORInfo(val)"

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



class wxToolBarToolBase : public wxObject {
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
    const wxBitmap& GetNormalBitmap();
    const wxBitmap& GetDisabledBitmap();
    wxBitmap GetBitmap();
    wxString GetLabel();
    wxString GetShortHelp();
    wxString GetLongHelp();
    bool Enable(bool enable);
    void Toggle();
    bool SetToggle(bool toggle);
    bool SetShortHelp(const wxString& help);
    bool SetLongHelp(const wxString& help);
    void SetNormalBitmap(const wxBitmap& bmp);
    void SetDisabledBitmap(const wxBitmap& bmp);
    void SetLabel(const wxString& label);
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

    %pragma(python) addtoclass="
    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap
    "
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
                                   const wxString& shortHelpString = wxEmptyString,
                                   const wxString& longHelpString = wxEmptyString) {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->AddTool(id, bitmap, pushedBitmap, (bool)isToggle,
                                 udata, shortHelpString, longHelpString);
        }

        // This one is easier to use...
        wxToolBarToolBase *AddSimpleTool(int id,
                                         const wxBitmap& bitmap,
                                         const wxString& shortHelpString = wxEmptyString,
                                         const wxString& longHelpString = wxEmptyString,
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
                                      const wxString& shortHelpString = wxEmptyString,
                                      const wxString& longHelpString = wxEmptyString) {
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
                                            const wxString& shortHelpString = wxEmptyString,
                                            const wxString& longHelpString = wxEmptyString,
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
        PyObject* GetToolClientData(int id) {
            wxPyUserData* udata = (wxPyUserData*)self->GetToolClientData(id);
            if (udata) {
                Py_INCREF(udata->m_obj);
                return udata->m_obj;
            } else {
                Py_INCREF(Py_None);
                return Py_None;
            }
        }

        void SetToolClientData(int id, PyObject* clientData) {
            self->SetToolClientData(id, new wxPyUserData(clientData));
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
    wxSize GetMargins();
    int GetToolPacking();
    int GetToolSeparation();

    void SetRows(int nRows);
    void SetMaxRowsCols(int rows, int cols);
    int GetMaxRows();
    int GetMaxCols();

    void SetToolBitmapSize(const wxSize& size);
    wxSize GetToolBitmapSize();
    wxSize GetToolSize();

    // returns a (non separator) tool containing the point (x, y) or NULL if
    // there is no tool at this point (corrdinates are client)
    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);

    // return TRUE if this is a vertical toolbar, otherwise FALSE
    bool IsVertical();
};




class wxToolBar : public wxToolBarBase {
public:
    wxToolBar(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const char* name = wxToolBarNameStr);
    %name(wxPreToolBar)wxToolBar();

    bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const char* name = wxToolBarNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreToolBar:val._setOORInfo(val)"

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);
};




class wxToolBarSimple : public wxToolBarBase {
public:
    wxToolBarSimple(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxNO_BORDER | wxTB_HORIZONTAL,
                    const char* name = wxToolBarNameStr);
    %name(wxPreToolBarSimple)wxToolBarSimple();

    bool Create(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxNO_BORDER | wxTB_HORIZONTAL,
                    const char* name = wxToolBarNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreToolBarSimple:val._setOORInfo(val)"

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

