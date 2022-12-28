/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     interface of wxMemoryDC
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxMemoryDC

    A memory device context provides a means to draw graphics onto a bitmap.
    When drawing in to a mono-bitmap, using @c wxWHITE, @c wxWHITE_PEN and
    @c wxWHITE_BRUSH will draw the background colour (i.e. 0) whereas all other
    colours will draw the foreground colour (i.e. 1).

    A bitmap must be selected into the new memory DC before it may be used for
    anything. Typical usage is as follows:

    @code
    // Create a memory DC
    wxMemoryDC temp_dc;
    temp_dc.SelectObject(test_bitmap);

    // We can now draw into the memory DC...
    // Copy from this DC to another DC.
    old_dc.Blit(250, 50, BITMAP_WIDTH, BITMAP_HEIGHT, temp_dc, 0, 0);
    @endcode

    Note that the memory DC must be deleted (or the bitmap selected out of it)
    before a bitmap can be reselected into another memory DC.

    And, before performing any other operations on the bitmap data, the bitmap
    must be selected out of the memory DC:

    @code
    temp_dc.SelectObject(wxNullBitmap);
    @endcode

    This happens automatically when wxMemoryDC object goes out of scope.

    Note that the scaling factor of the bitmap determines the scaling factor
    used by this device context, so when using a memory device context as a
    back buffer for a window, you should typically create the bitmap using the
    same scale factor as used by the window, e.g.
    @code
    void MyWindow::OnPaint(wxPaintEvent&)
    {
        wxBitmap bmp;
        bmp.CreateWithDIPSize(GetClientSize(), GetDPIScaleFactor());
        {
            wxMemoryDC memdc(bmp);
            ... use memdc to draw on the bitmap ...
        }

        wxPaintDC dc(this);
        dc.DrawBitmap(bmp, wxPoint(0, 0));
    }
    @endcode

    @library{wxcore}
    @category{dc}

    @see wxBitmap, wxDC
*/
class wxMemoryDC : public wxDC
{
public:
    /**
        Constructs a new memory device context.

        Use the wxDC::IsOk() member to test whether the constructor was
        successful in creating a usable device context. Don't forget to select
        a bitmap into the DC before drawing on it.
    */
    wxMemoryDC();

    /**
        Constructs a new memory device context having the same characteristics
        as the given existing device context.

        This constructor creates a memory device context @e compatible with @a
        dc in wxMSW, the argument is ignored in the other ports. If @a dc is
        @NULL, a device context compatible with the screen is created, just as
        with the default constructor.
     */
    wxMemoryDC(wxDC *dc);

    /**
        Constructs a new memory device context and calls SelectObject() with
        the given bitmap.

        Use the wxDC::IsOk() member to test whether the constructor was
        successful in creating a usable device context.
    */
    wxMemoryDC(wxBitmap& bitmap);

    /**
        Allow using this device context object to modify the given bitmap
        contents.

        Note that if you need to only use the existing bitmap contents instead
        of modifying it, you should use SelectObjectAsSource() instead.

        Before using the updated bitmap data, make sure to select it out of
        context first either by selecting ::wxNullBitmap into the device
        context or destroying the device context entirely.

        If the bitmap is already selected in this device context, nothing is
        done. If it is selected in another context, the function asserts and
        drawing on the bitmap won't work correctly.
    */
    void SelectObject(wxBitmap& bitmap);

    /**
        Selects the given bitmap into the device context, to use as the memory
        bitmap.

        Selecting the bitmap as source into a memory DC allows you to copy its
        contents to another device context using wxDC::Blit(). Note that using
        wxDC::DrawBitmap() or wxDC::DrawIcon() is a simpler way to do the same
        thing.

        @note Modifying a bitmap selected only as a source may not work
        correctly and can notably modify the other bitmaps sharing the same
        data due to the use of reference counting (see @ref overview_refcount).

        If the argument is ::wxNullBitmap (or some other uninitialised wxBitmap)
        the current bitmap is selected out of the device context, allowing the
        current bitmap to be destroyed safely.
    */
    void SelectObjectAsSource(const wxBitmap& bitmap);

    /**
       Get the selected bitmap.
    */
    const wxBitmap& GetSelectedBitmap() const;
    wxBitmap& GetSelectedBitmap();

};

