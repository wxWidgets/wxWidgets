/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/segctrl.h
// Purpose:     wxSegmentedCtrl
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEGCTRL_H_
#define _WX_SEGCTRL_H_


/**
 @class wxSegmentedCtrl
 
 @category{wxbile}
 */

class WXDLLEXPORT wxSegmentedCtrl: public wxSegmentedCtrlBase
{
    DECLARE_DYNAMIC_CLASS(wxSegmentedCtrl)
public:
    /// Default constructor.
    wxSegmentedCtrl();
    
    /// Constructor.
    wxSegmentedCtrl(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxTAB_TEXT,
                      const wxString& name = wxT("tabCtrl"));
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TEXT,
                const wxString& name = wxT("segmentedCtrl"));
            
    // wxTabCtrl overrides
    
    /// Add an item, passing an optional index into the image list.
    bool AddItem(const wxString& text, int imageId = -1);
    
    /// Add an item, passing a bitmap.
    bool AddItem(const wxString& text, const wxBitmap& bitmap);

    /// Set the selection, generating events
    int SetSelection(int item);
    
protected:
    
    void Init();
    
    DECLARE_NO_COPY_CLASS(wxSegmentedCtrl)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_SEGCTRL_H_
