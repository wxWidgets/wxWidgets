///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/image.h
// Author:      Paul Cornett
// Copyright:   (c) 2020 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/bmpbndl.h"
#include "wx/math.h"

// Class that can be used in place of GtkImage, to allow drawing of alternate
// bitmaps, such as HiDPI or disabled.

class wxGtkImage: GtkImage
{
public:
    struct BitmapProvider
    {
        virtual ~BitmapProvider() { }

        virtual double GetScale() const = 0;
        virtual wxBitmap Get() const = 0;
        virtual void Set(const wxBitmapBundle&) { }

        // Simple helpers used in implementation.
        bool IsScaled() const { return !wxIsSameDouble(GetScale(), 1); }
        wxBitmap GetAtScale(const wxBitmapBundle& b) const
        {
            return b.GetBitmap(b.GetDefaultSize()*GetScale());
        }
    };

    static GType Type();
    static GtkWidget* New(BitmapProvider* provider);
    static GtkWidget* New(wxWindow* win = NULL);

    // Use bitmaps from the given bundle, the logical bitmap size is the
    // default size of the bundle.
    void Set(const wxBitmapBundle& bitmapBundle);

    // This pointer is never null and is owned by this class.
    BitmapProvider* m_provider;

    wxDECLARE_NO_COPY_CLASS(wxGtkImage);

    // This class is constructed by New() and destroyed by its GObject
    // finalizer, so neither its ctor nor dtor can ever be used.
    wxGtkImage() wxMEMBER_DELETE;
    ~wxGtkImage() wxMEMBER_DELETE;
};

#define WX_GTK_IMAGE(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, wxGtkImage::Type(), wxGtkImage)
#define WX_GTK_IS_IMAGE(obj) G_TYPE_CHECK_INSTANCE_TYPE(obj, wxGtkImage::Type())
