/////////////////////////////////////////////////////////////////////////////
// Name:        _overlay.i
// Purpose:     wxOverlay classes
//
// Author:      Robin Dunn
//
// Created:     10-Oct-2006
// RCS-ID:      $Id$
// Copyright:   (c) 2006 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module



//---------------------------------------------------------------------------

class wxOverlay
{
public:
    wxOverlay();
    ~wxOverlay();

    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    void Reset();
};

class wxDCOverlay
{
public:

    %nokwargs wxDCOverlay;
    
    // connects this overlay to the corresponding drawing dc, if the overlay is not initialized yet
    // this call will do so
    wxDCOverlay(wxOverlay &overlay, wxWindowDC *dc, int x , int y , int width , int height);

    // convenience wrapper that behaves the same using the entire area of the dc
    wxDCOverlay(wxOverlay &overlay, wxWindowDC *dc);

    // removes the connection between the overlay and the dc
    virtual ~wxDCOverlay();

    // clears the layer, restoring the state at the last init
    void Clear();
};


//---------------------------------------------------------------------------
