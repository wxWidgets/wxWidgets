/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/wheelsctrl.h
// Purpose:     wxMoWheelsCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_WHEELSCTRL_H_
#define _WX_MOBILE_GENERIC_WHEELSCTRL_H_

#include "wx/bitmap.h"
#include "wx/control.h"
#include "wx/dynarray.h"
#include "wx/mobile/utils.h"
#include "wx/mobile/vlbox.h"

extern WXDLLEXPORT_DATA(const wxChar) wxWheelsCtrlNameStr[];

class WXDLLEXPORT wxMoWheelsCtrl;
class WXDLLEXPORT wxMoWheelsDataSource;
class WXDLLEXPORT wxMoWheelsListBox;

class wxMoWheelComponent: public wxEvtHandler
{
    DECLARE_CLASS(wxMoWheelComponent)
public:

    // Constructor.
    wxMoWheelComponent(wxMoWheelsCtrl* wheelsCtrl,
        wxMoWheelsListBox* listBox, int n)
    { Init(); m_wheelsCtrl = wheelsCtrl; m_listBox = listBox; m_componentNumber = n; }

    void Init();

    wxMoWheelsListBox* GetListBox() const { return m_listBox; }
    void SetListBox(wxMoWheelsListBox* listBox) { m_listBox = listBox; }
    int GetComponentNumber() const { return m_componentNumber; }
    const wxRect& GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { m_rect = rect; }
    wxMoWheelsCtrl* GetWheelsCtrl() const { return m_wheelsCtrl; }

    wxMoWheelsListBox*  m_listBox;
    int                 m_componentNumber;
    wxRect              m_rect;
    wxMoWheelsCtrl*     m_wheelsCtrl;
};

WX_DEFINE_ARRAY_PTR(wxMoWheelComponent*, wxMoWheelComponentArray);

/**
    @class wxMoWheelsCtrl

    A control consisting of a number of components that can be spun.
    The siulation uses a scrolling list without scrollbars instead of a wheel.

    Pass an instance of a class derived from wxMoWheelsDataSource to
    wxMoWheelsCtrl::SetDataSource.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoWheelsCtrl: public wxControl
{
public:
    /// Default constructor.
    wxMoWheelsCtrl() { Init(); }

    /// Constructor.
    wxMoWheelsCtrl(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxWheelsCtrlNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWheelsCtrlNameStr);

    virtual ~wxMoWheelsCtrl();

    void Init();

    virtual void ClearComponents();

    /// Reloads and displays all components.
    virtual bool ReloadAllComponents();

    /// Reloads and displays the specified component.
    virtual bool ReloadComponent(int component);

    /// Sets the selection (row) in the given component.
    virtual void SetSelection(int component, int selection);

    /// Sets the selection (row) for the only component in a 1-component control.
    virtual void SetSelection(int selection) { SetSelection(0, selection); }

    /// Gets the selection for the given component.
    virtual int GetSelection(int component) const;

    /// Returns the number of rows in the given component.
    virtual int GetComponentRowCount(int component) const;

    /// Returns the number of components.
    virtual int GetComponentCount() const;

    /// Returns the size required to display the largest view in the given component.
    virtual wxSize GetRowSizeForComponent(int component) const;

    /// Sets the row selection indicator flag
    void SetRowSelectionIndicator(bool showRowSelection) { m_showRowSelection = showRowSelection; }

    /// Gets the row selection indicator flag
    bool GetRowSelectionIndicator() const { return m_showRowSelection; }

    /// Returns the data source object.
    wxMoWheelsDataSource* GetDataSource() const { return m_dataSource; }

    /// Sets the data source object, recreating the components.
    void SetDataSource(wxMoWheelsDataSource* dataSource, bool ownsDataSource = false);

    /// Initializes the components from the current data source. You do not usually
    /// need to call this function since it is called from SetDataSource.
    virtual bool InitializeComponents();

    // Returns the array of components.
    const wxMoWheelComponentArray& GetComponents() const { return m_components; }

    // Returns the given component. Implementation only.
    wxMoWheelComponent* GetComponent(int component) const { return m_components[component]; }

    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);

protected:
    // send a notification event, return true if processed
    bool SendClickEvent();

    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    void ResizeComponents();

    // usually overridden base class virtuals
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoWheelsCtrl)
    DECLARE_EVENT_TABLE()

    bool                    m_showRowSelection;
    wxMoWheelsDataSource*   m_dataSource;
    bool                    m_ownDataSource;
    wxMoWheelComponentArray m_components;

    int                     m_leftMargin;
    int                     m_rightMargin;
    int                     m_topMargin;
    int                     m_bottomMargin;
    int                     m_interWheelSpacing;
};

/**
    @class wxMoWheelsCtrlEvent

    @category{wxMobile}
*/

class WXDLLEXPORT wxWheelsCtrlEvent: public wxNotifyEvent
{
public:
    /// Default constructor.
    wxWheelsCtrlEvent()
    {
        m_component = 0;
    }
    /// Copy constructor.
    wxWheelsCtrlEvent(const wxWheelsCtrlEvent& event): wxNotifyEvent(event)
    {
        m_component = event.m_component;
    }
    /// Constructor.
    wxWheelsCtrlEvent(wxEventType commandType, int id,
                    int component, int row)
        : wxNotifyEvent(commandType, id)
        {
            m_commandInt = row;
            m_component = component;
        }

    /// Set the component
    void SetComponent(int component) { m_component = component; }

    /// Get the component
    int GetComponent() const { return m_component; }

    virtual wxEvent *Clone() const { return new wxWheelsCtrlEvent(*this); }

private:
    int m_component;

    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxWheelsCtrlEvent)
};

typedef void (wxEvtHandler::*wxWheelsCtrlEventFunction)(wxWheelsCtrlEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_WHEEL_SELECTED,     860)
END_DECLARE_EVENT_TYPES()

#define wxWheelsCtrlEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxWheelsCtrlEventFunction, &func)

#define EVT_WHEEL_SELECTED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_WHEEL_SELECTED, \
  id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxWheelsCtrlEventFunction, & fn ), NULL),

/**
    @class wxMoWheelsDataSource

    Derive from this class and override virtual member functions to
    create a data source for a wxMoWheelsCtrl.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoWheelsDataSource: public wxEvtHandler
{
public:
    /// Default constructor.
    wxMoWheelsDataSource() { Init(); }

    void Init() {}

    /// Override to provide the number of components.
    virtual int GetNumberOfComponents(wxMoWheelsCtrl* ctrl) = 0;

    /// Override to provide the number of rows in a component.
    virtual int GetNumberOfRowsForComponent(wxMoWheelsCtrl* ctrl, int component) = 0;

    /// Override to provide the row height for a component. Should returns a height in pixels.
    virtual int GetRowHeightForComponent(wxMoWheelsCtrl* ctrl, int component) = 0;

    /// Override to provide the row width for a component. Should returns a width in pixels.
    virtual int GetRowWidthForComponent(wxMoWheelsCtrl* ctrl, int component) = 0;

    /// Override to provide the title for a component's row. If you override GetRowWindow instead,
    /// this method is optional.
    virtual wxString GetRowTitle(wxMoWheelsCtrl* WXUNUSED(ctrl), int WXUNUSED(component), int WXUNUSED(row)) { return wxEmptyString; }

    /// Override to provide a control for a component's row. This is optional.
    virtual wxWindow* GetRowWindow(wxMoWheelsCtrl* WXUNUSED(ctrl), int WXUNUSED(component), int WXUNUSED(row), wxWindow* WXUNUSED(reusingWindow)) { return NULL; }

    /// Override to intercept row click; by default, sends an event to itself and then to control.
    virtual bool OnSelectRow(wxMoWheelsCtrl* ctrl, int component, int row);

protected:

    DECLARE_CLASS(wxMoWheelsDataSource)
};

/**
    @class wxMoWheelsTextDataSource

    A class for providing data from one or more string arrays,
    one for each component.

    @category{wxMobile}
*/

WX_DECLARE_OBJARRAY(wxArrayString, wxMoArrayOfStringArrays);
WX_DECLARE_OBJARRAY(wxSize, wxMoSizeArray);

class WXDLLEXPORT wxMoWheelsTextDataSource: public wxMoWheelsDataSource
{
public:
    /// Constructor.
    wxMoWheelsTextDataSource(int componentCount = 1, const wxSize& rowSize = wxDefaultSize);

    void Init();

    /// Override to provide the number of components.
    virtual int GetNumberOfComponents(wxMoWheelsCtrl* WXUNUSED(ctrl)) { return m_componentCount; }

    /// Override to provide the number of rows in a component.
    virtual int GetNumberOfRowsForComponent(wxMoWheelsCtrl* ctrl, int component);

    /// Override to provide the row height for a component. Should returns a height in pixels.
    virtual int GetRowHeightForComponent(wxMoWheelsCtrl* ctrl, int component);

    /// Override to provide the row width for a component. Should returns a width in pixels.
    virtual int GetRowWidthForComponent(wxMoWheelsCtrl* ctrl, int component);

    /// Override to provide the title for a component's row. If you override GetRowWindow instead,
    /// this method is optional.
    virtual wxString GetRowTitle(wxMoWheelsCtrl* ctrl, int component, int row);

// New methods

    wxArrayString GetStrings(int component = 0) const;
    void SetStrings(const wxArrayString& strings, int component = 0);

    wxSize GetRowSize(int component = 0) const;
    void SetRowSize(const wxSize& sz, int component = 0);

    void SetComponentCount(int count);
    int GetComponentCount() const { return m_componentCount; }

protected:

    wxMoArrayOfStringArrays m_stringArrays;
    wxMoSizeArray           m_rowSizes;
    int                     m_componentCount;

    DECLARE_CLASS(wxMoWheelsDataSource)
};

/**
    @class wxMoWheelsListBox

    A listbox to implement a single component in a wheels control.
    Currently this works for text strings only.

    @category{wxMobileImplementation}
  */

class WXDLLEXPORT wxMoWheelsListBox: public wxMoVListBox
{
public:
    /// Default constructor.
    wxMoWheelsListBox() { Init(); }

    /// Constructor.
    wxMoWheelsListBox(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxLB_SINGLE,
               const wxString& name = wxMoVListBoxNameStr)
    {
        Init();

        (void)Create(parent, id, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLB_SINGLE,
                const wxString& name = wxMoVListBoxNameStr);

    virtual ~wxMoWheelsListBox() {}

    /// Sets the wheels control implementing this control.
    void SetWheelsCtrl(wxMoWheelsCtrl* ctrl) { m_wheelsCtrl = ctrl; }

    /// Gets the wheels control implementing this control.
    wxMoWheelsCtrl* GetWheelsCtrl() const { return m_wheelsCtrl; }

    /// Sets the component number that this list represents.
    void SetComponentNumber(int n) { m_componentNumber = n; }

    /// Gets the component number that this list represents.
    int GetComponentNumber() const { return m_componentNumber; }

    /// Gets the component for this listbox.
    wxMoWheelComponent* GetComponent() const;

protected:
    virtual void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;
    virtual void DrawBackground(wxDC& dc);

    virtual wxSize DoGetBestSize() const;

    void OnSelect(wxCommandEvent& event);

    // the derived class must implement this function to actually draw the item
    // with the given index on the provided DC
    virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;

    // the derived class must implement this method to return the height of the
    // specified item
    virtual wxCoord OnMeasureItem(size_t n) const ;

    // common part of all ctors
    void Init();

    wxMoWheelsCtrl* m_wheelsCtrl;
    int             m_componentNumber;

    DECLARE_CLASS(wxMoWheelsListBox)
};

#endif
    // _WX_MOBILE_GENERIC_WHEELSCTRL_H_
