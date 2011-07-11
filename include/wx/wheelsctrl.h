/////////////////////////////////////////////////////////////////////////
// Name:        wx/wheelsctrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WHEELSCTRL_H_BASE_
#define _WX_WHEELSCTRL_H_BASE_

#include "wx/bitmap.h"
#include "wx/control.h"
#include "wx/dynarray.h"
#include "wx/vlbox.h"

extern WXDLLEXPORT_DATA(const wxChar) wxWheelsCtrlNameStr[];

class WXDLLEXPORT wxWheelsCtrl;
class WXDLLEXPORT wxWheelsDataSource;
class WXDLLEXPORT wxWheelsListBox;


#pragma mark wxWheelComponent

class wxWheelComponent: public wxEvtHandler
{
    DECLARE_CLASS(wxWheelComponent)
public:

    // Constructor.
    wxWheelComponent(wxWheelsCtrl* wheelsCtrl,
                     wxWheelsListBox* listBox,
                     int n)
    {
        Init();
        m_wheelsCtrl = wheelsCtrl;
        m_listBox = listBox;
        m_componentNumber = n;
    }

    wxWheelsListBox* GetListBox() const { return m_listBox; }
    void SetListBox(wxWheelsListBox* listBox) { m_listBox = listBox; }
    int GetComponentNumber() const { return m_componentNumber; }
    const wxRect& GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { m_rect = rect; }
    wxWheelsCtrl* GetWheelsCtrl() const { return m_wheelsCtrl; }
    
protected:
    void Init()
    {
        m_listBox = NULL;
        m_componentNumber = 0;
        m_wheelsCtrl = NULL;
    }
    
private:
    wxWheelsListBox*  m_listBox;
    int                 m_componentNumber;
    wxRect              m_rect;
    wxWheelsCtrl*     m_wheelsCtrl;
};

WX_DEFINE_ARRAY_PTR(wxWheelComponent*, wxWheelComponentArray);


#pragma mark wxWheelsCtrl

/**
    @class wxWheelsCtrl

    A control consisting of a number of components that can be spun.
    The siulation uses a scrolling list without scrollbars instead of a wheel.

    Pass an instance of a class derived from wxWheelsDataSource to
    wxWheelsCtrl::SetDataSource.

    @category{wxbile}
*/

class WXDLLEXPORT wxWheelsCtrlBase: public wxControl
{
public:
    /// Default constructor.
    wxWheelsCtrlBase() { }

    /// Constructor.
    wxWheelsCtrlBase(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxWheelsCtrlNameStr) { }

    virtual ~wxWheelsCtrlBase() { }

    virtual void ClearComponents() = 0;

    /// Reloads and displays all components.
    virtual bool ReloadAllComponents() = 0;

    /// Reloads and displays the specified component.
    virtual bool ReloadComponent(int component) = 0;

    /// Sets the selection (row) in the given component.
    virtual void SetSelection(int component, int selection) = 0;

    /// Sets the selection (row) for the only component in a 1-component control.
    virtual void SetSelection(int selection) { SetSelection(0, selection); }

    /// Gets the selection for the given component.
    virtual int GetSelection(int component) const = 0;

    /// Returns the number of rows in the given component.
    virtual int GetComponentRowCount(int component) const = 0;

    /// Returns the number of components.
    virtual int GetComponentCount() const = 0;

    /// Returns the size required to display the largest view in the given component.
    virtual wxSize GetRowSizeForComponent(int component) const = 0;

    /// Sets the row selection indicator flag
    virtual void SetRowSelectionIndicator(bool showRowSelection) { m_showRowSelection = showRowSelection; }

    /// Gets the row selection indicator flag
    virtual bool GetRowSelectionIndicator() const { return m_showRowSelection; }

    /// Returns the data source object.
    virtual wxWheelsDataSource* GetDataSource() const { return m_dataSource; }

    /// Sets the data source object, recreating the components.
    virtual void SetDataSource(wxWheelsDataSource* dataSource, bool ownsDataSource = false) = 0;

    /// Initializes the components from the current data source. You do not usually
    /// need to call this function since it is called from SetDataSource.
    virtual bool InitializeComponents() = 0;

    // Returns the array of components.
    virtual const wxWheelComponentArray& GetComponents() const { return m_components; }

    // Returns the given component. Implementation only.
    virtual wxWheelComponent* GetComponent(int component) const { return m_components[component]; }

    /*
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
    */

private:
    
    wxDECLARE_NO_COPY_CLASS(wxWheelsCtrlBase);

    bool                    m_showRowSelection;
    wxWheelsDataSource*     m_dataSource;
    bool                    m_ownDataSource;
    wxWheelComponentArray   m_components;

    int                     m_leftMargin;
    int                     m_rightMargin;
    int                     m_topMargin;
    int                     m_bottomMargin;
    int                     m_interWheelSpacing;
};


#pragma mark wxWheelsCtrlEvent

/**
    @class wxWheelsCtrlEvent

    @category{wxbile}
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
    wxWheelsCtrlEvent(wxEventType commandType,
                      int id,
                      int component,
                      int row) : wxNotifyEvent(commandType, id)
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


#pragma mark wxWheelsDataSource

/**
    @class wxWheelsDataSource

    Derive from this class and override virtual member functions to
    create a data source for a wxWheelsCtrl.

    @category{wxbile}
*/

class WXDLLEXPORT wxWheelsDataSource: public wxEvtHandler
{
public:
    /// Default constructor.
    wxWheelsDataSource() { Init(); }

    void Init() {}

    /// Override to provide the number of components.
    virtual int GetNumberOfComponents(wxWheelsCtrl* ctrl) = 0;

    /// Override to provide the number of rows in a component.
    virtual int GetNumberOfRowsForComponent(wxWheelsCtrl* ctrl,
                                            int component) = 0;

    /// Override to provide the row height for a component. Should returns a height in pixels.
    virtual int GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                         int component) = 0;

    /// Override to provide the row width for a component. Should returns a width in pixels.
    virtual int GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                        int component) = 0;

    /// Override to provide the title for a component's row. If you override GetRowWindow instead,
    /// this method is optional.
    virtual wxString GetRowTitle(wxWheelsCtrl* WXUNUSED(ctrl),
                                 int WXUNUSED(component),
                                 int WXUNUSED(row))
    {
        return wxEmptyString;
    }

    /// Override to provide a control for a component's row. This is optional.
    virtual wxWindow* GetRowWindow(wxWheelsCtrl* WXUNUSED(ctrl),
                                   int WXUNUSED(component),
                                   int WXUNUSED(row),
                                   wxWindow* WXUNUSED(reusingWindow))
    {
        return NULL;
    }

    /// Override to intercept row click; by default, sends an event to itself and then to control.
    virtual bool OnSelectRow(wxWheelsCtrl* ctrl,
                             int component,
                             int row);

protected:

    DECLARE_CLASS(wxWheelsDataSource)
};


#pragma mark wxWheelsTextDataSource

/**
    @class wxWheelsTextDataSource

    A class for providing data from one or more string arrays,
    one for each component.

    @category{wxbile}
*/

WX_DECLARE_OBJARRAY(wxArrayString, wxArrayOfStringArrays);
WX_DECLARE_OBJARRAY(wxSize, wxSizeArray);

class WXDLLEXPORT wxWheelsTextDataSource: public wxWheelsDataSource
{
public:
    /// Constructor.
    wxWheelsTextDataSource(int componentCount = 1,
                             const wxSize& rowSize = wxDefaultSize);

    void Init();

    /// Override to provide the number of components.
    virtual int GetNumberOfComponents(wxWheelsCtrl* WXUNUSED(ctrl)) { return m_componentCount; }

    /// Override to provide the number of rows in a component.
    virtual int GetNumberOfRowsForComponent(wxWheelsCtrl* ctrl,
                                            int component);

    /// Override to provide the row height for a component. Should returns a height in pixels.
    virtual int GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                         int component);

    /// Override to provide the row width for a component. Should returns a width in pixels.
    virtual int GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                        int component);

    /// Override to provide the title for a component's row. If you override GetRowWindow instead,
    /// this method is optional.
    virtual wxString GetRowTitle(wxWheelsCtrl* ctrl,
                                 int component,
                                 int row);

// New methods

    wxArrayString GetStrings(int component = 0) const;
    void SetStrings(const wxArrayString& strings, int component = 0);

    wxSize GetRowSize(int component = 0) const;
    void SetRowSize(const wxSize& sz, int component = 0);

    void SetComponentCount(int count);
    int GetComponentCount() const { return m_componentCount; }

protected:

    wxArrayOfStringArrays m_stringArrays;
    wxSizeArray           m_rowSizes;
    int                     m_componentCount;

    DECLARE_CLASS(wxWheelsDataSource)
};


#pragma mark wxWheelsListBox

/**
    @class wxWheelsListBox

    A listbox to implement a single component in a wheels control.
    Currently this works for text strings only.

    @category{wxbileImplementation}
  */

class WXDLLEXPORT wxWheelsListBox: public wxVListBox
{
public:
    /// Default constructor.
    wxWheelsListBox();

    /// Constructor.
    wxWheelsListBox(wxWindow *parent,
                      wxWindowID id = wxID_ANY,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxLB_SINGLE,
                      const wxString& name = wxVListBoxNameStr);

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLB_SINGLE,
                const wxString& name = wxVListBoxNameStr);

    virtual ~wxWheelsListBox();

    /// Sets the wheels control implementing this control.
    void SetWheelsCtrl(wxWheelsCtrl* ctrl) { m_wheelsCtrl = ctrl; }

    /// Gets the wheels control implementing this control.
    wxWheelsCtrl* GetWheelsCtrl() const { return m_wheelsCtrl; }

    /// Sets the component number that this list represents.
    void SetComponentNumber(int n) { m_componentNumber = n; }

    /// Gets the component number that this list represents.
    int GetComponentNumber() const { return m_componentNumber; }

    /// Gets the component for this listbox.
    wxWheelComponent* GetComponent() const;

protected:
    virtual void OnDrawBackground(wxDC& dc,
                                  const wxRect& rect,
                                  size_t n) const;
    virtual void DrawBackground(wxDC& dc);

    virtual wxSize DoGetBestSize() const;

    void OnSelect(wxCommandEvent& event);

    // the derived class must implement this function to actually draw the item
    // with the given index on the provided DC
    virtual void OnDrawItem(wxDC& dc,
                            const wxRect& rect,
                            size_t n) const;

    // the derived class must implement this method to return the height of the
    // specified item
    virtual wxCoord OnMeasureItem(size_t n) const ;

    // common part of all ctors
    void Init();

    wxWheelsCtrl* m_wheelsCtrl;
    int             m_componentNumber;

    DECLARE_CLASS(wxWheelsListBox)
};


// ----------------------------------------------------------------------------
// wxWheelsCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/wheelsctrl.h"
#endif


#endif
    // _WX_WHEELSCTRL_H_BASE_
