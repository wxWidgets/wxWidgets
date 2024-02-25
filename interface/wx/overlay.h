/////////////////////////////////////////////////////////////////////////////
// Name:        overlay.h
// Purpose:     interface of wxOverlay
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
   @class wxOverlay

   Creates an overlay over an existing window, allowing for manipulations like
   rubberbanding, etc.  On wxOSX the overlay is implemented with native
   platform APIs, on the other platforms it is simulated using wxMemoryDC.

   @library{wxcore}

   @see wxDCOverlay, wxDC
 */
class wxOverlay
{
public:
    wxOverlay();
    ~wxOverlay();

    /**
       Clears the overlay without restoring the former state.  To be done, for
       example, when the window content has been changed and repainted.
    */
    void Reset();

    /**
        Sets or unsets constant opacity of the overlay window.

        If @a alpha is `-1`, use per-pixel alpha blending, otherwise use the
        given alpha value for all pixels.

        @note Drawing on the overlay window is opaque by default under wxMSW.
        You have to call SetOpacity(-1) before initializing the overlay and use
        a graphics context to do the actual drawing if you want per-pixel alpha
        blending. Or, assuming that SetOpacity(-1) wasn't called, explicitly set
        the alpha value (at any time) to change the overlay window's opacity.

        @note This is currently only implemented for wxMSW and does nothing in
        the other ports.

        @since 3.3.0
    */
    void SetOpacity(int alpha);
};



/**
   @class wxDCOverlay

   Connects an overlay with a drawing DC.

   @library{wxcore}

   @see wxOverlay, wxDC

 */
class wxDCOverlay
{
public:
    /**
       Connects this overlay to the corresponding drawing dc, if the overlay is
       not initialized yet this call will do so.
    */
    wxDCOverlay(wxOverlay &overlay, wxDC *dc, int x , int y , int width , int height);

    /**
       Convenience wrapper that behaves the same using the entire area of the dc.
    */
    wxDCOverlay(wxOverlay &overlay, wxDC *dc);

    /**
       Removes the connection between the overlay and the dc.
    */
    virtual ~wxDCOverlay();

    /**
       Clears the layer, restoring the state at the last init.
    */
    void Clear();
};
