/////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/wheelsctrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WHEELSCTRL_H_
#define _WX_WHEELSCTRL_H_

/**
    @class wxWheelsCtrl

    @category{wxbile}
*/

class WXDLLEXPORT wxWheelsCtrl: public wxWheelsCtrlBase
{
public:
    /// Default constructor.
    wxWheelsCtrl();

    /// Constructor.
    wxWheelsCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxWheelsCtrlNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxWheelsCtrlNameStr);

    virtual ~wxWheelsCtrl();

    /// Reloads and displays all components.
    virtual bool ReloadAllComponents();

    /// Reloads and displays the specified component.
    virtual bool ReloadComponent(int component);

    /// Sets the selection (row) in the given component.
    virtual void SetSelection(int component, int selection);

    /// Gets the selection for the given component.
    virtual int GetSelection(int component) const;

    /// Returns the number of rows in the given component.
    virtual int GetComponentRowCount(int component) const;

    /// Returns the number of components.
    virtual int GetComponentCount() const;

    /// Sets the data source object, recreating the components.
    void SetDataSource(wxWheelsDataSource* dataSource, bool ownsDataSource = false);

protected:
    
    void Init();
    
private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxWheelsCtrl)
    DECLARE_EVENT_TABLE()

};

#endif
    // _WX_WHEELSCTRL_H_
