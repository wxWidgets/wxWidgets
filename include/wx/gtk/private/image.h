///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/image.h
// Author:      Paul Cornett
// Copyright:   (c) 2020 Paul Cornett
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// Class that can be used in place of GtkImage, to allow drawing of alternate
// bitmaps, such as HiDPI or disabled.

class wxGtkImage: GtkImage
{
public:
    struct BitmapProvider
    {
        virtual ~BitmapProvider() { }
        virtual wxBitmap Get() const = 0;
        virtual void Set(const wxBitmap&) { }
    };

    static GType Type();
    static GtkWidget* New(BitmapProvider* provider);
    static GtkWidget* New(wxWindow* win = NULL);
    void Set(const wxBitmap& bitmap);

    BitmapProvider* m_provider;

    wxDECLARE_NO_COPY_CLASS(wxGtkImage);
    wxGtkImage() wxMEMBER_DELETE;
    ~wxGtkImage() wxMEMBER_DELETE;
};

#define WX_GTK_IMAGE(obj) G_TYPE_CHECK_INSTANCE_CAST(obj, wxGtkImage::Type(), wxGtkImage)
