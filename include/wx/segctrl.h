/////////////////////////////////////////////////////////////////////////////
// Name:        wx/segctrl.h
// Purpose:     Segmented control (NSSegmentedControl - OS X, UISegmentedControl - iOS)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SEGCTRL_H_BASE_
#define _WX_SEGCTRL_H_BASE_

class WXDLLIMPEXP_CORE wxImageList;

#include "wx/tabctrl.h"

/**
 @class wxSegmentedCtrl
 
 A class with the same API as wxTabCtrl, but displaying a space-efficient row of buttons.
 
 @category{wxbile}
 */

class WXDLLEXPORT wxSegmentedCtrlBase: public wxTabCtrl
{    
public:
    /// Default constructor.
    wxSegmentedCtrlBase() { }
    
    /// Constructor.
    wxSegmentedCtrlBase(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxTAB_TEXT,
                      const wxString& name = wxT("tabCtrl")) { }
    
    // wxTabCtrl overrides
    
    /// Add an item, passing an optional index into the image list.
    virtual bool AddItem(const wxString& text, int imageId = -1) = 0;
    
    /// Add an item, passing a bitmap.
    virtual bool AddItem(const wxString& text, const wxBitmap& bitmap) = 0;

    /// Set the selection, generating events
    virtual int SetSelection(int item) = 0;
    
protected:
        
    wxDECLARE_NO_COPY_CLASS(wxSegmentedCtrlBase);
    
};


// ----------------------------------------------------------------------------
// wxSegmentedCtrl class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/segctrl.h"
#endif


#endif
    // _WX_SEGCTRL_H_BASE_
