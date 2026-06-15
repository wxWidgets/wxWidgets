
// beginner-friendly image sample

#include <wx/wx.h>
#include <wx/dcbuffer.h>

class ImagePanel : public wxPanel
{
public:
    ImagePanel(wxWindow* parent, const wxString& file)
        : wxPanel(parent)
    {

        if (!m_image.LoadFile(file))
        {
            wxLogError("Could not load image: %s", file);
        }
        else
        {
            m_bitmap = wxBitmap(m_image);
        }

        SetBackgroundStyle(wxBG_STYLE_PAINT);

        Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this);
    }

private:
    void OnPaint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        if (m_bitmap.IsOk())
        {

            dc.DrawBitmap(m_bitmap, 0, 0, false);
        }
    }

    wxImage m_image;
    wxBitmap m_bitmap;
};

class MyFrame : public wxFrame
{
public:
    MyFrame()
        : wxFrame(nullptr, wxID_ANY, "Mini Image Sample", wxDefaultPosition, wxSize(400, 300))
    {

        new ImagePanel(this, "horse.png");
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {

        wxInitAllImageHandlers();

        auto* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
