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


#pragma mark wxWheelComponent

class wxWheelComponent: public wxEvtHandler
{
    DECLARE_CLASS(wxWheelComponent)
public:

    // Constructor.
    wxWheelComponent(wxWheelsCtrl* wheelsCtrl,
                     int n)
    {
        Init();
        
        m_wheelsCtrl = wheelsCtrl;
        m_componentNumber = n;
    }

    int GetComponentNumber() const { return m_componentNumber; }
    const wxRect& GetRect() const { return m_rect; }
    void SetRect(const wxRect& rect) { m_rect = rect; }
    wxWheelsCtrl* GetWheelsCtrl() const { return m_wheelsCtrl; }
    
protected:
    void Init()
    {
        m_componentNumber = 0;
        m_wheelsCtrl = NULL;
    }
    
private:
    int               m_componentNumber;
    wxRect            m_rect;
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

    /// Sets the row selection indicator flag
    virtual void SetRowSelectionIndicator(bool showRowSelection) { m_showRowSelection = showRowSelection; }

    /// Gets the row selection indicator flag
    virtual bool GetRowSelectionIndicator() const { return m_showRowSelection; }

    /// Returns the data source object.
    virtual wxWheelsDataSource* GetDataSource() const { return m_dataSource; }

    /// Sets the data source object, recreating the components.
    virtual void SetDataSource(wxWheelsDataSource* dataSource, bool ownsDataSource = false) = 0;

    // Returns the array of components.
    virtual const wxWheelComponentArray& GetComponents() const { return m_components; }

    // Returns the given component. Implementation only.
    virtual wxWheelComponent* GetComponent(int component) const { return m_components[component]; }

protected:
    
    bool                    m_showRowSelection;
    wxWheelsDataSource*     m_dataSource;
    bool                    m_ownDataSource;
    wxWheelComponentArray   m_components;

    int                     m_leftMargin;
    int                     m_rightMargin;
    int                     m_topMargin;
    int                     m_bottomMargin;
    int                     m_interWheelSpacing;
    
    wxDECLARE_NO_COPY_CLASS(wxWheelsCtrlBase);
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
    virtual int GetNumberOfComponents(wxWheelsCtrl* ctrl) { return 0; }

    /// Override to provide the number of rows in a component.
    virtual int GetNumberOfRowsForComponent(wxWheelsCtrl* ctrl,
                                            int component) { return 0; }

    /// Override to provide the row height for a component. Should returns a height in pixels.
    virtual int GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                         int component) { return 0; }

    /// Override to provide the row width for a component. Should returns a width in pixels.
    virtual int GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                        int component) { return 0; }

    /// Override to provide the title for a component's row. If you override GetRowWindow instead,
    /// this method is optional.
    virtual wxString GetRowTitle(wxWheelsCtrl* WXUNUSED(ctrl),
                                 int WXUNUSED(component),
                                 int WXUNUSED(row)) { return wxEmptyString; }

    /// Override to provide a control for a component's row. This is optional.
    virtual wxWindow* GetRowWindow(wxWheelsCtrl* WXUNUSED(ctrl),
                                   int WXUNUSED(component),
                                   int WXUNUSED(row),
                                   wxWindow* WXUNUSED(reusingWindow)) { return NULL; }

    /// Override to intercept row click; by default, sends an event to itself and then to control.
    virtual bool OnSelectRow(wxWheelsCtrl* ctrl,
                             int component,
                             int row) { }

protected:

    DECLARE_CLASS(wxWheelsDataSource)
};

WX_DECLARE_OBJARRAY(wxArrayString, wxArrayOfStringArrays);
WX_DECLARE_OBJARRAY(wxSize, wxSizeArray);


#pragma mark wxWheelsTextDataSource

/**
    @class wxWheelsTextDataSource

    A class for providing data from one or more string arrays,
    one for each component.

    @category{wxbile}
*/

class WXDLLEXPORT wxWheelsTextDataSource: public wxWheelsDataSource
{
public:
    /// Constructor.
    wxWheelsTextDataSource(int componentCount = 1,
                           const wxSize& rowSize = wxDefaultSize);

    void Init();

    /// Override to provide the number of components.
    virtual int GetNumberOfComponents(wxWheelsCtrl* WXUNUSED(ctrl));

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
    int GetComponentCount() const;

protected:

    wxArrayOfStringArrays m_stringArrays;
    wxSizeArray           m_rowSizes;
    int                   m_componentCount;

    DECLARE_CLASS(wxWheelsDataSource)
};


#pragma mark wxWheelsDateTimeDataSource

// wxWheelsDateTimeDataSource modes
enum {
    wxWHEELS_DT_TIME =      0x00000000,
    wxWHEELS_DT_DATE =      0x00010000,
    wxWHEELS_DT_DATE_TIME = 0x00020000,
    wxWHEELS_DT_COUNTDOWN = 0x00040000,
};


/**
    @class wxWheelsDateTimeDataSource

    A class for creating a wheel control for choosing date and/or time.

    @category{wxbile}
*/

class WXDLLEXPORT wxWheelsDateTimeDataSource: public wxWheelsDataSource
{
public:
    /// Constructor.
    wxWheelsDateTimeDataSource()
    {
        Init();
    }

    void Init()
    {
        m_pickerMode = wxWHEELS_DT_DATE_TIME;
        m_initialDate = NULL;
        m_minimumDate = NULL;
        m_maximumDate = NULL;
        m_minuteInterval = 1;
        m_countDownDuration = 0.0;
    }

    // Not used.
    virtual int GetNumberOfComponents(wxWheelsCtrl* WXUNUSED(ctrl)) { return 0; }

    // Not used.
    virtual int GetNumberOfRowsForComponent(wxWheelsCtrl* ctrl,
                                            int component) { return 0; }

    // Not used.
    virtual int GetRowHeightForComponent(wxWheelsCtrl* ctrl,
                                         int component) { return 0; }

    // Not used.
    virtual int GetRowWidthForComponent(wxWheelsCtrl* ctrl,
                                        int component) { return 0; }

    // Not used.
    virtual wxString GetRowTitle(wxWheelsCtrl* ctrl,
                                 int component,
                                 int row) { return wxEmptyString; }

    // New methods
    
    /// Get date picker mode.
    int GetPickerMode() { return m_pickerMode; }
    
    /// Set date picker mode.
    void SetPickerMode(int pickerMode) { m_pickerMode = pickerMode; }
    
    /// Get initial picker date.
    wxDateTime* GetInitialDate() { return m_initialDate; }
    
    /// Set initial picker date.
    void SetInitialDate(wxDateTime* initialDate) { m_initialDate = initialDate; }
    
    /// Get minumum picker date.
    wxDateTime* GetMinimumPickerDate() { return m_minimumDate; }
    
    /// Set minumum picker date.
    void SetMinimumPickerDate(wxDateTime* minimumDate) { m_minimumDate = minimumDate; }
    
    /// Get maximum picker date.
    wxDateTime* GetMaximumPickerDate() { return m_maximumDate; }
    
    /// Set maximum picker date.
    void SetMaximumPickerDate(wxDateTime* maximumDate) { m_maximumDate = maximumDate; }
    
    /// Get minute interval.
    int GetMinuteInterval() { return m_minuteInterval; }
    
    /// Set minute interval.
    void SetMinuteInterval(int minuteInterval) { m_minuteInterval = minuteInterval; }
    
    /// Get count down duration (timer mode only).
    double GetCountDownDuration() { return m_countDownDuration; }
    
    /// Set count down duration (timer mode only).
    void SetCountDownDuration(double countDownDuration ) { m_countDownDuration = countDownDuration;  }
    
    /// Override to intercept date select; by default, sends an event to itself and then to control.
    virtual bool OnSelectDate(wxWheelsCtrl *ctrl, wxDateTime *date) { }

protected:
    
    DECLARE_CLASS(wxWheelsDataSource)
    
private:
    int         m_pickerMode;
    wxDateTime* m_initialDate;
    wxDateTime* m_minimumDate;
    wxDateTime* m_maximumDate;
    int         m_minuteInterval;
    double      m_countDownDuration;
};



// ----------------------------------------------------------------------------
// wxWheelsCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/wheelsctrl.h"
#endif


#endif
    // _WX_WHEELSCTRL_H_BASE_
