/////////////////////////////////////////////////////////////////////////////
// Name:        overlay.h
// Purpose:     interface of wxOverlay
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
   @class wxOverlay

   Creates an overlay over an existing window, allowing for manipulations like
   rubber-banding, etc.

   Overlay is implemented internally as a native window shown on top of the
   window it is used with and it is more efficient to keep the wxOverlay object
   around instead of recreating it every time it's needed, i.e. typically you
   would have a member variable of type wxOverlay in your window class.

   The overlay is initialized automatically when it is used but has to be
   cleared manually, using its Reset() function, when its contents shouldn't be
   shown any longer.

   Use of this class is shown in the @ref page_samples_drawing where pressing
   the left mouse button and dragging the mouse creates a rubber band effect
   using wxOverlay.

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
