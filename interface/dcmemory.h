/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     documentation for wxMemoryDC class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryDC
    @wxheader{dcmemory.h}

    A memory device context provides a means to draw graphics onto a bitmap. When
    drawing in to a mono-bitmap, using @c wxWHITE, @c wxWHITE_PEN and
    @c wxWHITE_BRUSH
    will draw the background colour (i.e. 0) whereas all other colours will draw the
    foreground colour (i.e. 1).

    @library{wxcore}
    @category{dc}

    @seealso
    wxBitmap, wxDC
*/
class wxMemoryDC : public wxDC
{
public:
    //@{
    /**
        Constructs a new memory device context and calls SelectObject()
        with the given bitmap.
        Use the wxDC::IsOk member to test whether the constructor was successful
        in creating a usable device context.
    */
    wxMemoryDC();
    wxMemoryDC(wxBitmap& bitmap);
    //@}

    /**
        Works exactly like SelectObjectAsSource() but
        this is the function you should use when you select a bitmap because you want
        to modify
        it, e.g. drawing on this DC.
        Using SelectObjectAsSource() when modifying
        the bitmap may incurr some problems related to wxBitmap being a reference
        counted object
        (see @ref overview_trefcount "reference counting overview").
        Also, before using the updated bitmap data, make sure to select it out of
        context first
        (for example by selecting wxNullBitmap into the device context).
        
        @see wxDC::DrawBitmap
    */
    void SelectObject(wxBitmap& bitmap);

    /**
        Selects the given bitmap into the device context, to use as the memory
        bitmap. Selecting the bitmap into a memory DC allows you to draw into
        the DC (and therefore the bitmap) and also to use wxDC::Blit to copy
        the bitmap to a window. For this purpose, you may find wxDC::DrawIcon
        easier to use instead.
        If the argument is wxNullBitmap (or some other uninitialised wxBitmap) the
        current bitmap is
        selected out of the device context, and the original bitmap restored, allowing
        the current bitmap to
        be destroyed safely.
    */
    void SelectObjectAsSource(const wxBitmap& bitmap);
};
