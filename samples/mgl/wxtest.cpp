#include <wx/wxprec.h>


#include "wx/dcmemory.h"
#include "wx/app.h"
#include "wx/image.h"

#include "tests.h"

#ifdef TEST_DC

void TestDC()
{
    wxMemoryDC dc;
    wxBitmap bmp(SIZEX, SIZEY);
    dc.SelectObject(bmp);
    RenderTest1(dc);
    wxImage img(bmp);
#ifdef __WXMSW__
    img.SaveFile("wxmsw_test1.png", wxBITMAP_TYPE_PNG);
#else
    img.SaveFile("wxgtk_test1.png", wxBITMAP_TYPE_PNG);
#endif
}

#endif

class MyApp : public wxApp
{
    public:
        virtual bool OnInit()
        {
	    wxImage::AddHandler(new wxPNGHandler);
#ifdef TEST_DC
            TestDC();
#endif
#ifdef TEST_BITMAP
            TestBitmap();
#endif
            return FALSE;
        }
};

IMPLEMENT_APP(MyApp)
