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


//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(StatusLineNameStr);
    DECLARE_DEF_STRING(ToolBarNameStr);
    static const wxString wxPyEmptyString(wxT(""));
%}

//---------------------------------------------------------------------------

class wxStatusBar : public wxWindow {
public:
    wxStatusBar(wxWindow* parent, wxWindowID id = -1,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxPyStatusLineNameStr);
    %name(wxPreStatusBar)wxStatusBar();

    bool Create(wxWindow* parent, wxWindowID id,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxPyStatusLineNameStr);

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
//                        const wxString& label = wxEmptyString,
//                        const wxBitmap& bmpNormal = wxNullBitmap,
//                        const wxBitmap& bmpDisabled = wxNullBitmap,
//                        wxItemKind kind = wxITEM_NORMAL,
//                        wxObject *clientData = (wxObject *) NULL,
//                        const wxString& shortHelpString = wxEmptyString,
//                        const wxString& longHelpString = wxEmptyString)
//      ~wxToolBarToolBase();

    %addmethods { void Destroy() { delete self; } }

    int GetId();
    wxControl *GetControl();
    wxToolBarBase *GetToolBar();
    int IsButton();
    int IsControl();
    int IsSeparator();
    int GetStyle();
    wxItemKind GetKind();
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

        // the full AddTool() function
        //
        // If bmpDisabled is wxNullBitmap, a shadowed version of the normal bitmap
        // is created and used as the disabled image.
        wxToolBarToolBase *AddTool(int id,
                                   const wxString& label,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& bmpDisabled,
                                   wxItemKind kind = wxITEM_NORMAL,
                                   const wxString& shortHelp = wxPyEmptyString,
                                   const wxString& longHelp = wxPyEmptyString,
                                   PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->AddTool(id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, udata);
        }

        // The most common version of AddTool
        wxToolBarToolBase *AddSimpleTool(int id,
                                         const wxString& label,
                                         const wxBitmap& bitmap,
                                         const wxString& shortHelp = wxPyEmptyString,
                                         const wxString& longHelp = wxPyEmptyString,
                                         wxItemKind kind = wxITEM_NORMAL)
        {
            return self->AddTool(id, label, bitmap, wxNullBitmap, kind,
                                 shortHelp, longHelp, NULL);
        }

        // add a check tool, i.e. a tool which can be toggled
        wxToolBarToolBase *AddCheckTool(int id,
                                        const wxString& label,
                                        const wxBitmap& bitmap,
                                        const wxBitmap& bmpDisabled = wxNullBitmap,
                                        const wxString& shortHelp = wxEmptyString,
                                        const wxString& longHelp = wxEmptyString,
                                        PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->AddCheckTool(id, label, bitmap, bmpDisabled,
                                      shortHelp, longHelp, udata);
        }

        // add a radio tool, i.e. a tool which can be toggled and releases any
        // other toggled radio tools in the same group when it happens
        wxToolBarToolBase *AddRadioTool(int id,
                                    const wxString& label,
                                    const wxBitmap& bitmap,
                                    const wxBitmap& bmpDisabled = wxNullBitmap,
                                    const wxString& shortHelp = wxEmptyString,
                                    const wxString& longHelp = wxEmptyString,
                                    PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->AddRadioTool(id, label, bitmap, bmpDisabled,
                                      shortHelp, longHelp, udata);
        }

        // insert the new tool at the given position, if pos == GetToolsCount(), it
        // is equivalent to AddTool()
        wxToolBarToolBase *InsertTool(size_t pos,
                                      int id,
                                      const wxString& label,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& bmpDisabled = wxNullBitmap,
                                      wxItemKind kind = wxITEM_NORMAL,
                                      const wxString& shortHelp = wxEmptyString,
                                      const wxString& longHelp = wxEmptyString,
                                      PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData)
                udata = new wxPyUserData(clientData);
            return self->InsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                    shortHelp, longHelp, udata);
        }

        // A simpler InsertTool
        wxToolBarToolBase *InsertSimpleTool(size_t pos,
                                      int id,
                                      const wxString& label,
                                      const wxBitmap& bitmap,
                                      wxItemKind kind = wxITEM_NORMAL,
                                      const wxString& shortHelp = wxEmptyString,
                                      const wxString& longHelp = wxEmptyString)
        {
            return self->InsertTool(pos, id, label, bitmap, wxNullBitmap, kind,
                                    shortHelp, longHelp);
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
              const wxString& name = wxPyToolBarNameStr);
    %name(wxPreToolBar)wxToolBar();

    bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const wxString& name = wxPyToolBarNameStr);

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
                    const wxString& name = wxPyToolBarNameStr);
    %name(wxPreToolBarSimple)wxToolBarSimple();

    bool Create(wxWindow *parent,
                    wxWindowID id,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxNO_BORDER | wxTB_HORIZONTAL,
                    const wxString& name = wxPyToolBarNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreToolBarSimple:val._setOORInfo(val)"

    wxToolBarToolBase *FindToolForPosition(wxCoord x, wxCoord y);
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

