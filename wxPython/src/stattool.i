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
%pragma(python) code = "wxITEM_NORMAL = 0 # predeclare this since wx isn't fully imported yet"

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
    void PushStatusText(const wxString& text, int number = 0);
    void PopStatusText(int number = 0);

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

        // The full AddTool() function.  Call it DoAddTool in wxPython and
        // implement the other Add methods by calling it.
        //
        // If bmpDisabled is wxNullBitmap, a shadowed version of the normal bitmap
        // is created and used as the disabled image.
        wxToolBarToolBase *DoAddTool(int id,
                                     const wxString& label,
                                     const wxBitmap& bitmap,
                                     const wxBitmap& bmpDisabled = wxNullBitmap,
                                     wxItemKind kind = wxITEM_NORMAL,
                                     const wxString& shortHelp = wxPyEmptyString,
                                     const wxString& longHelp = wxPyEmptyString,
                                     PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData && clientData != Py_None)
                udata = new wxPyUserData(clientData);
            return self->AddTool(id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, udata);
        }


        // Insert the new tool at the given position, if pos == GetToolsCount(), it
        // is equivalent to DoAddTool()
        wxToolBarToolBase *DoInsertTool(size_t pos,
                                      int id,
                                      const wxString& label,
                                      const wxBitmap& bitmap,
                                      const wxBitmap& bmpDisabled = wxNullBitmap,
                                      wxItemKind kind = wxITEM_NORMAL,
                                      const wxString& shortHelp = wxPyEmptyString,
                                      const wxString& longHelp = wxPyEmptyString,
                                      PyObject *clientData = NULL)
        {
            wxPyUserData* udata = NULL;
            if (clientData && clientData != Py_None)
                udata = new wxPyUserData(clientData);
            return self->InsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                    shortHelp, longHelp, udata);
        }

    }


    %pragma(python) addtoclass = "
    # These match the original Add methods for this class, kept for
    # backwards compatibility with versions < 2.3.3.


    def AddTool(self, id, bitmap,
                pushedBitmap = wxNullBitmap,
                isToggle = 0,
                clientData = None,
                shortHelpString = '',
                longHelpString = '') :
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoAddTool(id, '', bitmap, pushedBitmap, kind,
                              shortHelpString, longHelpString, clientData)

    def AddSimpleTool(self, id, bitmap,
                      shortHelpString = '',
                      longHelpString = '',
                      isToggle = 0):
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoAddTool(id, '', bitmap, wxNullBitmap, kind,
                              shortHelpString, longHelpString, None)

    def InsertTool(self, pos, id, bitmap,
                   pushedBitmap = wxNullBitmap,
                   isToggle = 0,
                   clientData = None,
                   shortHelpString = '',
                   longHelpString = ''):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, pushedBitmap, kind,
                                 shortHelpString, longHelpString, clientData)

    def InsertSimpleTool(self, pos, id, bitmap,
                         shortHelpString = '',
                         longHelpString = '',
                         isToggle = 0):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.wxITEM_NORMAL
        if isToggle: kind = wx.wxITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, wxNullBitmap, kind,
                                 shortHelpString, longHelpString, None)


    # The following are the new toolbar Add methods starting with
    # 2.3.3.  They are renamed to have 'Label' in the name so as to be
    # able to keep backwards compatibility with using the above
    # methods.  Eventually these should migrate to be the methods used
    # primarily and lose the 'Label' in the name...

    def AddLabelTool(self, id, label, bitmap,
                     bmpDisabled = wxNullBitmap,
                     kind = wxITEM_NORMAL,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        The full AddTool() function.

        If bmpDisabled is wxNullBitmap, a shadowed version of the normal bitmap
        is created and used as the disabled image.
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, kind,
                              shortHelp, longHelp, clientData)


    def InsertLabelTool(self, pos, id, label, bitmap,
                        bmpDisabled = wxNullBitmap,
                        kind = wxITEM_NORMAL,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''
        Insert the new tool at the given position, if pos == GetToolsCount(), it
        is equivalent to AddTool()
        '''
        return self.DoInsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, clientData)

    def AddCheckLabelTool(self, id, label, bitmap,
                        bmpDisabled = wxNullBitmap,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.wxITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioLabelTool(self, id, label, bitmap,
                          bmpDisabled = wxNullBitmap,
                          shortHelp = '', longHelp = '',
                          clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.wxITEM_RADIO,
                              shortHelp, longHelp, clientData)


    # For consistency with the backwards compatible methods above, here are
    # some non-'Label' versions of the Check and Radio methods
    def AddCheckTool(self, id, bitmap,
                     bmpDisabled = wxNullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.wxITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioTool(self, id, bitmap,
                     bmpDisabled = wxNullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.wxITEM_RADIO,
                              shortHelp, longHelp, clientData)
    "

    // TODO?
    //wxToolBarToolBase *AddTool (wxToolBarToolBase *tool);
    //wxToolBarToolBase *InsertTool (size_t pos, wxToolBarToolBase *tool);

    wxToolBarToolBase *AddControl(wxControl *control);
    wxToolBarToolBase *InsertControl(size_t pos, wxControl *control);
    wxControl *FindControl( int id );

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

