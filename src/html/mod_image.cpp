/////////////////////////////////////////////////////////////////////////////
// Name:        mod_image.cpp
// Purpose:     wxHtml module for displaying images
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/defs.h"
#if wxUSE_HTML

#include <wx/html/forcelink.h>
#include <wx/html/mod_templ.h>

#include <wx/wxhtml.h>
#include <wx/image.h>

FORCE_LINK_ME(mod_image)


//--------------------------------------------------------------------------------
// wxHtmlImageCell
//                  Image/bitmap
//--------------------------------------------------------------------------------

class wxHtmlImageCell : public wxHtmlCell
{
    public:
        wxBitmap *m_Image;

        wxHtmlImageCell(wxFSFile *input, int w = -1, int h = -1, int align = HTML_ALIGN_BOTTOM);
        ~wxHtmlImageCell() {if (m_Image) delete m_Image;}
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
};



//--------------------------------------------------------------------------------
// wxHtmlImageCell
//--------------------------------------------------------------------------------

wxHtmlImageCell::wxHtmlImageCell(wxFSFile *input, int w, int h, int align) : wxHtmlCell()
{
    wxImage *img;
    int ww, hh;
    wxString m = input -> GetMimeType();
    wxInputStream *s = input -> GetStream();

#if wxVERSION_NUMBER < 2100
/* NOTE : use this *old* code only if you have old 2.0.1 wxWindows distribution
   and don't want to upgrade it with stuffs from add-on/wxwin201 */
         if (wxMimeTypesManager::IsOfType(m, "image/png"))  img = new wxImage(*s, wxBITMAP_TYPE_PNG);
    else if (wxMimeTypesManager::IsOfType(m, "image/jpeg")) img = new wxImage(*s, wxBITMAP_TYPE_JPEG);
    else if (wxMimeTypesManager::IsOfType(m, "image/bmp"))  img = new wxImage(*s, wxBITMAP_TYPE_BMP);
    else if (wxMimeTypesManager::IsOfType(m, "image/gif"))  img = new wxImage(*s, wxBITMAP_TYPE_GIF);
    else if (wxMimeTypesManager::IsOfType(m, "image/tiff")) img = new wxImage(*s, wxBITMAP_TYPE_TIF);
    else if (wxMimeTypesManager::IsOfType(m, "image/xpm"))  img = new wxImage(*s, wxBITMAP_TYPE_XPM);
    else if (wxMimeTypesManager::IsOfType(m, "image/xbm"))  img = new wxImage(*s, wxBITMAP_TYPE_XBM);
    else img = NULL;
#else
    img = new wxImage(*s, m);
#endif

    m_Image = NULL;
    if (img && (img -> Ok())) {
        ww = img -> GetWidth();
        hh = img -> GetHeight();
        if (w != -1) m_Width = w; else m_Width = ww;
        if (h != -1) m_Height = h; else m_Height = hh;
        if ((m_Width != ww) || (m_Height != hh)) {
            wxImage img2 = img -> Scale(m_Width, m_Height);
            m_Image = new wxBitmap(img2.ConvertToBitmap());
        }
        else
            m_Image = new wxBitmap(img -> ConvertToBitmap());
        delete img;
    }
    switch (align) {
        case HTML_ALIGN_TOP :
                        m_Descent = m_Height; break;
        case HTML_ALIGN_CENTER :
                        m_Descent = m_Height / 2; break;
        case HTML_ALIGN_BOTTOM : default :
                        m_Descent = 0; break;
    }
}



void wxHtmlImageCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
{
    if (m_Image)
        dc.DrawBitmap(*m_Image, x + m_PosX, y + m_PosY, TRUE);
    wxHtmlCell::Draw(dc, x, y, view_y1, view_y2);
}





//--------------------------------------------------------------------------------
// tag handler
//--------------------------------------------------------------------------------

TAG_HANDLER_BEGIN(IMG, "IMG")

    TAG_HANDLER_PROC(tag)
    {
        if (tag.HasParam("SRC")) {
            int w = -1, h = -1;
            int al;
            wxFSFile *str;
            wxString tmp = tag.GetParam("SRC");

            str = m_WParser -> GetFS() -> OpenFile(tmp);
            if (tag.HasParam("WIDTH")) tag.ScanParam("WIDTH", "%i", &w);
            if (tag.HasParam("HEIGHT")) tag.ScanParam("HEIGHT", "%i", &h);
            al = HTML_ALIGN_BOTTOM;
            if (tag.HasParam("ALIGN")) {
                wxString alstr = tag.GetParam("ALIGN");
                alstr.MakeUpper(); // for the case alignment was in ".."
                if (alstr == "TEXTTOP") al = HTML_ALIGN_TOP;
                else if ((alstr == "CENTER") || (alstr == "ABSCENTER")) al = HTML_ALIGN_CENTER;
            }
            if (str) {
                wxHtmlCell *cel = new wxHtmlImageCell(str, w, h, al);
                cel -> SetLink(m_WParser -> GetLink());
                m_WParser -> GetContainer() -> InsertCell(cel);
                delete str;
            }
        }

        return FALSE;
    }

TAG_HANDLER_END(IMAGE)



TAGS_MODULE_BEGIN(Image)

    TAGS_MODULE_ADD(IMG)

TAGS_MODULE_END(Image)


#endif