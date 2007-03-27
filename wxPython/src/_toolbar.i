/////////////////////////////////////////////////////////////////////////////
// Name:        _toolbar.i
// Purpose:     SWIG interface defs for wxStatusBar
//
// Author:      Robin Dunn
//
// Created:     24-Aug-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING_NOSWIG(ToolBarNameStr);


//---------------------------------------------------------------------------
%newgroup;


class wxToolBarBase;

enum wxToolBarToolStyle
{
    wxTOOL_STYLE_BUTTON    = 1,
    wxTOOL_STYLE_SEPARATOR = 2,
    wxTOOL_STYLE_CONTROL
};

enum {
    wxTB_HORIZONTAL,
    wxTB_VERTICAL,
    wxTB_TOP,
    wxTB_LEFT,
    wxTB_BOTTOM,
    wxTB_RIGHT,

    wxTB_3DBUTTONS,
    wxTB_FLAT,
    wxTB_DOCKABLE,
    wxTB_NOICONS,
    wxTB_TEXT,
    wxTB_NODIVIDER,
    wxTB_NOALIGN,
    wxTB_HORZ_LAYOUT,
    wxTB_HORZ_TEXT,
    wxTB_NO_TOOLTIPS
};



// wxToolBarTool is a toolbar element.
//
// It has a unique id (except for the separators which always have id -1), the
// style (telling whether it is a normal button, separator or a control), the
// state (toggled or not, enabled or not) and short and long help strings. The
// default implementations use the short help string for the tooltip text which
// is popped up when the mouse pointer enters the tool and the long help string
// for the applications status bar.
class wxToolBarToolBase : public wxObject {
public:
//      wxToolBarToolBase(wxToolBarBase *tbar = (wxToolBarBase *)NULL,
//                        int id = wxID_SEPARATOR,
//                        const wxString& label = wxPyEmptyString,
//                        const wxBitmap& bmpNormal = wxNullBitmap,
//                        const wxBitmap& bmpDisabled = wxNullBitmap,
//                        wxItemKind kind = wxITEM_NORMAL,
//                        wxObject *clientData = (wxObject *) NULL,
//                        const wxString& shortHelpString = wxPyEmptyString,
//                        const wxString& longHelpString = wxPyEmptyString)
//      ~wxToolBarToolBase();

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
    %extend {
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

    %pythoncode {
    GetBitmap1 = GetNormalBitmap
    GetBitmap2 = GetDisabledBitmap
    SetBitmap1 = SetNormalBitmap
    SetBitmap2 = SetDisabledBitmap
    }
    
    %property(Bitmap, GetBitmap, doc="See `GetBitmap`");
    %property(ClientData, GetClientData, SetClientData, doc="See `GetClientData` and `SetClientData`");
    %property(Control, GetControl, doc="See `GetControl`");
    %property(DisabledBitmap, GetDisabledBitmap, SetDisabledBitmap, doc="See `GetDisabledBitmap` and `SetDisabledBitmap`");
    %property(Id, GetId, doc="See `GetId`");
    %property(Kind, GetKind, doc="See `GetKind`");
    %property(Label, GetLabel, SetLabel, doc="See `GetLabel` and `SetLabel`");
    %property(LongHelp, GetLongHelp, SetLongHelp, doc="See `GetLongHelp` and `SetLongHelp`");
    %property(NormalBitmap, GetNormalBitmap, SetNormalBitmap, doc="See `GetNormalBitmap` and `SetNormalBitmap`");
    %property(ShortHelp, GetShortHelp, SetShortHelp, doc="See `GetShortHelp` and `SetShortHelp`");
    %property(Style, GetStyle, doc="See `GetStyle`");
    %property(ToolBar, GetToolBar, doc="See `GetToolBar`");
};




class wxToolBarBase : public wxControl {
public:

    // This is an Abstract Base Class

    %extend {

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


    %pythoncode {
    %# These match the original Add methods for this class, kept for
    %# backwards compatibility with versions < 2.3.3.


    def AddTool(self, id, bitmap,
                pushedBitmap = wx.NullBitmap,
                isToggle = 0,
                clientData = None,
                shortHelpString = '',
                longHelpString = '') :
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoAddTool(id, '', bitmap, pushedBitmap, kind,
                              shortHelpString, longHelpString, clientData)

    def AddSimpleTool(self, id, bitmap,
                      shortHelpString = '',
                      longHelpString = '',
                      isToggle = 0):
        '''Old style method to add a tool to the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoAddTool(id, '', bitmap, wx.NullBitmap, kind,
                              shortHelpString, longHelpString, None)

    def InsertTool(self, pos, id, bitmap,
                   pushedBitmap = wx.NullBitmap,
                   isToggle = 0,
                   clientData = None,
                   shortHelpString = '',
                   longHelpString = ''):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, pushedBitmap, kind,
                                 shortHelpString, longHelpString, clientData)

    def InsertSimpleTool(self, pos, id, bitmap,
                         shortHelpString = '',
                         longHelpString = '',
                         isToggle = 0):
        '''Old style method to insert a tool in the toolbar.'''
        kind = wx.ITEM_NORMAL
        if isToggle: kind = wx.ITEM_CHECK
        return self.DoInsertTool(pos, id, '', bitmap, wx.NullBitmap, kind,
                                 shortHelpString, longHelpString, None)


    %# The following are the new toolbar Add methods starting with
    %# 2.3.3.  They are renamed to have 'Label' in the name so as to be
    %# able to keep backwards compatibility with using the above
    %# methods.  Eventually these should migrate to be the methods used
    %# primarily and lose the 'Label' in the name...

    def AddLabelTool(self, id, label, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     kind = wx.ITEM_NORMAL,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        The full AddTool() function.

        If bmpDisabled is wx.NullBitmap, a shadowed version of the normal bitmap
        is created and used as the disabled image.
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, kind,
                              shortHelp, longHelp, clientData)


    def InsertLabelTool(self, pos, id, label, bitmap,
                        bmpDisabled = wx.NullBitmap,
                        kind = wx.ITEM_NORMAL,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''
        Insert the new tool at the given position, if pos == GetToolsCount(), it
        is equivalent to AddTool()
        '''
        return self.DoInsertTool(pos, id, label, bitmap, bmpDisabled, kind,
                                 shortHelp, longHelp, clientData)

    def AddCheckLabelTool(self, id, label, bitmap,
                        bmpDisabled = wx.NullBitmap,
                        shortHelp = '', longHelp = '',
                        clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.ITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioLabelTool(self, id, label, bitmap,
                          bmpDisabled = wx.NullBitmap,
                          shortHelp = '', longHelp = '',
                          clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, label, bitmap, bmpDisabled, wx.ITEM_RADIO,
                              shortHelp, longHelp, clientData)


    %# For consistency with the backwards compatible methods above, here are
    %# some non-'Label' versions of the Check and Radio methods

    def AddCheckTool(self, id, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''Add a check tool, i.e. a tool which can be toggled'''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.ITEM_CHECK,
                              shortHelp, longHelp, clientData)

    def AddRadioTool(self, id, bitmap,
                     bmpDisabled = wx.NullBitmap,
                     shortHelp = '', longHelp = '',
                     clientData = None):
        '''
        Add a radio tool, i.e. a tool which can be toggled and releases any
        other toggled radio tools in the same group when it happens
        '''
        return self.DoAddTool(id, '', bitmap, bmpDisabled, wx.ITEM_RADIO,
                              shortHelp, longHelp, clientData)
    }

    %Rename(AddToolItem,  wxToolBarToolBase*, AddTool (wxToolBarToolBase *tool));
    %Rename(InsertToolItem,  wxToolBarToolBase*, InsertTool (size_t pos, wxToolBarToolBase *tool));

    wxToolBarToolBase *AddControl(wxControl *control,
                                  const wxString& label = wxEmptyString);
    wxToolBarToolBase *InsertControl(size_t pos, wxControl *control,
                                     const wxString& label = wxEmptyString);
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


    %extend {
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

    // returns tool pos, or wxNOT_FOUND if tool isn't found
    int GetToolPos(int id) const;

    bool GetToolState(int id);
    bool GetToolEnabled(int id);
    void SetToolShortHelp(int id, const wxString& helpString);
    wxString GetToolShortHelp(int id);
    void SetToolLongHelp(int id, const wxString& helpString);
    wxString GetToolLongHelp(int id);

    %Rename(SetMarginsXY,  void, SetMargins(int x, int y));
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

    // find the tool by id
    wxToolBarToolBase *FindById(int toolid) const;

    // return True if this is a vertical toolbar, otherwise False
    bool IsVertical();

    size_t GetToolsCount() const;

    %property(Margins, GetMargins, SetMargins, doc="See `GetMargins` and `SetMargins`");
    %property(MaxCols, GetMaxCols, doc="See `GetMaxCols`");
    %property(MaxRows, GetMaxRows, doc="See `GetMaxRows`");
    %property(ToolBitmapSize, GetToolBitmapSize, SetToolBitmapSize, doc="See `GetToolBitmapSize` and `SetToolBitmapSize`");
    %property(ToolMargins, GetToolMargins, doc="See `GetToolMargins`");
    %property(ToolPacking, GetToolPacking, SetToolPacking, doc="See `GetToolPacking` and `SetToolPacking`");
    %property(ToolSeparation, GetToolSeparation, SetToolSeparation, doc="See `GetToolSeparation` and `SetToolSeparation`");
    %property(ToolSize, GetToolSize, doc="See `GetToolSize`");
    %property(ToolsCount, GetToolsCount, doc="See `GetToolsCount`");
};




MustHaveApp(wxToolBar);

class wxToolBar : public wxToolBarBase {
public:
    %pythonAppend wxToolBar         "self._setOORInfo(self)"
    %pythonAppend wxToolBar()       ""
    %typemap(out) wxToolBar*;    // turn off this typemap
 
    wxToolBar(wxWindow *parent,
              wxWindowID id=-1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const wxString& name = wxPyToolBarNameStr);
    %RenameCtor(PreToolBar, wxToolBar());

    // Turn it back on again
    %typemap(out) wxToolBar* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow *parent,
              wxWindowID id=-1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxNO_BORDER | wxTB_HORIZONTAL,
              const wxString& name = wxPyToolBarNameStr);

    // TODO: In 2.9 move these to the base class...
    void SetToolNormalBitmap(int id, const wxBitmap& bitmap);
    void SetToolDisabledBitmap(int id, const wxBitmap& bitmap);
    
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------

#if 0
%{
#include <wx/generic/buttonbar.h>
%}

MustHaveApp(wxToolBar);
class  wxButtonToolBar : public wxToolBarBase
{
public:
    %pythonAppend wxButtonToolBar         "self._setOORInfo(self)"
    %pythonAppend wxButtonToolBar()       ""

    wxButtonToolBar(wxWindow *parent,
                    wxWindowID id=-1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyToolBarNameStr);
    %RenameCtor(PreButtonToolBar, wxButtonToolBar());


    bool Create(wxWindow *parent,
              wxWindowID id=-1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxPyToolBarNameStr);
};

#endif
//---------------------------------------------------------------------------
