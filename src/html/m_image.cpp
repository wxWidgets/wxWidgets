/////////////////////////////////////////////////////////////////////////////
// Name:        mod_image.cpp
// Purpose:     wxHtml module for displaying images
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik, Joel Lucsy
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"


#include "wx/image.h"
#include "wx/dynarray.h"

#include <math.h>
#include <float.h>

FORCE_LINK_ME(mod_image)




WX_DECLARE_OBJARRAY(int, CoordArray);
#include "wx/arrimpl.cpp" // this is a magic incantation which must be done!
WX_DEFINE_OBJARRAY(CoordArray);


//--------------------------------------------------------------------------------
// wxHtmlImageMapAreaCell
//                  0-width, 0-height cell that represents single area in imagemap
//                  (it's GetLink is called from wxHtmlImageCell's)
//--------------------------------------------------------------------------------

class wxHtmlImageMapAreaCell : public wxHtmlCell
{
    public:
        enum celltype { CIRCLE, RECT, POLY };
    protected:
        CoordArray coords;
        celltype type;
        int	radius;
    public:
        wxHtmlImageMapAreaCell( celltype t, wxString &coords );
        virtual wxString GetLink( int x = 0, int y = 0 ) const;
};





wxHtmlImageMapAreaCell::wxHtmlImageMapAreaCell( wxHtmlImageMapAreaCell::celltype t, wxString &incoords )
{
    int	i;
    wxString x = incoords, y;

    type = t;
    while ((i = x.Find( ',' )) != -1) {
        coords.Add( wxAtoi( x.Left( i ).c_str() ) );
        x = x.Mid( i + 1 );
    }
    coords.Add( wxAtoi( x.c_str() ) );
}

wxString wxHtmlImageMapAreaCell::GetLink( int x, int y ) const
{
    switch (type) {
        case RECT:
            {
                int	l, t, r, b;

                l = coords[ 0 ];
                t = coords[ 1 ];
                r = coords[ 2 ];
                b = coords[ 3 ];
                if (x >= l && x <= r && y >= t && y <= b) {
                    return m_Link;
                }
                break;
            }
        case CIRCLE:
            {
                int	l, t, r;
                double	d;

                l = coords[ 0 ];
                t = coords[ 1 ];
                r = coords[ 2 ];
                d = sqrt( (double) (((x - l) * (x - l)) + ((y - t) * (y - t))) );
                if (d < (double)r) {
                    return m_Link;
                }
            }
            break;
        case POLY:
            {
                if (coords.GetCount() >= 6) {
                    int	intersects = 0;
                    int	wherex = x;
                    int	wherey = y;
                    int	totalv = coords.GetCount() / 2;
                    int	totalc = totalv * 2;
                    int	xval = coords[totalc - 2];
                    int	yval = coords[totalc - 1];
                    int	end = totalc;
                    int	pointer = 1;

                    if ((yval >= wherey) != (coords[pointer] >= wherey)) {
                        if ((xval >= wherex) == (coords[0] >= wherex)) {
                            intersects += (xval >= wherex) ? 1 : 0;
                        } else {
                            intersects += ((xval - (yval - wherey) *
                                            (coords[0] - xval) /
                                            (coords[pointer] - yval)) >= wherex) ? 1 : 0;
                        }
                    }

                    while (pointer < end) {
                        yval = coords[pointer];
                        pointer += 2;
                        if (yval >= wherey) {
                            while ((pointer < end) && (coords[pointer] >= wherey)) {
                                pointer += 2;
                            }
                            if (pointer >= end) {
                                break;
                            }
                            if ((coords[pointer - 3] >= wherex) ==
                                    (coords[pointer - 1] >= wherex)) {
                                intersects += (coords[pointer - 3] >= wherex) ? 1 : 0;
                            } else {
                                intersects +=
                                    ((coords[pointer - 3] - (coords[pointer - 2] - wherey) *
                                      (coords[pointer - 1] - coords[pointer - 3]) /
                                      (coords[pointer] - coords[pointer - 2])) >= wherex) ? 1 : 0;
                            }
                        } else {
                            while ((pointer < end) && (coords[pointer] < wherey)) {
                                pointer += 2;
                            }
                            if (pointer >= end) {
                                break;
                            }
                            if ((coords[pointer - 3] >= wherex) ==
                                    (coords[pointer - 1] >= wherex)) {
                                intersects += (coords[pointer - 3] >= wherex) ? 1 : 0;
                            } else {
                                intersects +=
                                    ((coords[pointer - 3] - (coords[pointer - 2] - wherey) *
                                      (coords[pointer - 1] - coords[pointer - 3]) /
                                      (coords[pointer] - coords[pointer - 2])) >= wherex) ? 1 : 0;
                            }
                        }
                    }
                    if ((intersects & 1) != 0) {
                        return m_Link;
                    }
                }
            }
            break;
    }
    if (m_Next) {
        wxHtmlImageMapAreaCell	*a = (wxHtmlImageMapAreaCell*)m_Next;
        return a->GetLink( x, y );
    }
    return wxEmptyString;
}








//--------------------------------------------------------------------------------
// wxHtmlImageMapCell
//                  0-width, 0-height cell that represents map from imagemaps
//                  it is always placed before wxHtmlImageMapAreaCells
//                  It responds to Find(wxHTML_COND_ISIMAGEMAP)
//--------------------------------------------------------------------------------


class wxHtmlImageMapCell : public wxHtmlCell
{
    public:
        wxHtmlImageMapCell( wxString &name );
    protected:
        wxString m_Name;
    public:
        virtual wxString GetLink( int x = 0, int y = 0 ) const;
        virtual const wxHtmlCell *Find( int cond, const void *param ) const;
};


wxHtmlImageMapCell::wxHtmlImageMapCell( wxString &name )
{
    m_Name = name ;
}

wxString wxHtmlImageMapCell::GetLink( int x, int y ) const
{
    wxHtmlImageMapAreaCell	*a = (wxHtmlImageMapAreaCell*)m_Next;
    if (a)
        return a->GetLink( x, y );
    return wxHtmlCell::GetLink( x, y );
}

const wxHtmlCell *wxHtmlImageMapCell::Find( int cond, const void *param ) const
{
    if (cond == wxHTML_COND_ISIMAGEMAP) {
        if (m_Name == *((wxString*)(param)))
            return this;
    }
    return wxHtmlCell::Find(cond, param);
}





//--------------------------------------------------------------------------------
// wxHtmlImageCell
//                  Image/bitmap
//--------------------------------------------------------------------------------

class wxHtmlImageCell : public wxHtmlCell
{
    public:
        wxBitmap *m_Image;
        wxHtmlImageMapCell *m_ImageMap;
        wxString m_MapName;

        wxHtmlImageCell(wxFSFile *input, int w = -1, int h = -1, int align = wxHTML_ALIGN_BOTTOM, wxString mapname = wxEmptyString);
        ~wxHtmlImageCell() {if (m_Image) delete m_Image; }
        void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
        virtual wxString GetLink( int x = 0, int y = 0 ) const;
};




//--------------------------------------------------------------------------------
// wxHtmlImageCell
//--------------------------------------------------------------------------------

wxHtmlImageCell::wxHtmlImageCell(wxFSFile *input, int w, int h, int align, wxString mapname) : wxHtmlCell()
{
    wxImage *img;
    int ww, hh;
    wxInputStream *s = input -> GetStream();

    img = new wxImage(*s, wxBITMAP_TYPE_ANY);

    m_Image = NULL;
    if (img && (img -> Ok())) {
        ww = img -> GetWidth();
        hh = img -> GetHeight();
        if (w != -1) m_Width = w; else m_Width = ww;
        if (h != -1) m_Height = h; else m_Height = hh;
        if ((m_Width != ww) || (m_Height != hh)) {
            wxImage img2 = img -> Scale(m_Width, m_Height);
            m_Image = new wxBitmap(img2.ConvertToBitmap());
        } else
            m_Image = new wxBitmap(img -> ConvertToBitmap());
        delete img;
    }
    switch (align) {
        case wxHTML_ALIGN_TOP :
            m_Descent = m_Height;
            break;
        case wxHTML_ALIGN_CENTER :
            m_Descent = m_Height / 2;
            break;
        case wxHTML_ALIGN_BOTTOM :
        default :
            m_Descent = 0;
            break;
    }

    m_ImageMap = NULL;
    m_MapName = mapname;
    SetCanLiveOnPagebreak(FALSE);
}



void wxHtmlImageCell::Draw(wxDC& dc, int x, int y, int view_y1, int view_y2)
{
    if (m_Image)
        dc.DrawBitmap(*m_Image, x + m_PosX, y + m_PosY, TRUE);
    wxHtmlCell::Draw(dc, x, y, view_y1, view_y2);
}

wxString wxHtmlImageCell::GetLink( int x, int y ) const
{
    if (m_MapName.IsEmpty())
        return wxHtmlCell::GetLink( x, y );
    if (!m_ImageMap) {
        wxHtmlContainerCell	*p, *op;
        op = p = GetParent();
        while (p) {
            op = p;
            p = p->GetParent();
        }
        p = op;
        wxHtmlCell *cell = (wxHtmlCell*)p->Find( wxHTML_COND_ISIMAGEMAP, (const void*)(&m_MapName));
        if (!cell) {
            ((wxString&)m_MapName).Clear();
            return wxHtmlCell::GetLink( x, y );
        }
        { // dirty hack, ask Joel why he fills m_ImageMap in this place
            // THE problem is that we're in const method and we can't modify m_ImageMap
            wxHtmlImageMapCell **cx = (wxHtmlImageMapCell**)(&m_ImageMap);
            *cx = (wxHtmlImageMapCell*)cell;
        }
    }
    return m_ImageMap->GetLink( x, y );
}



//--------------------------------------------------------------------------------
// tag handler
//--------------------------------------------------------------------------------

TAG_HANDLER_BEGIN(IMG, "IMG,MAP,AREA")

    TAG_HANDLER_PROC(tag)
    {
        if (tag.GetName() == "IMG") {
            if (tag.HasParam("SRC")) {
                int w = -1, h = -1;
                int al;
                wxFSFile *str;
                wxString tmp = tag.GetParam("SRC");
                wxString mn = wxEmptyString;

                str = m_WParser -> GetFS() -> OpenFile(tmp);
                if (tag.HasParam(wxT("WIDTH"))) tag.ScanParam(wxT("WIDTH"), wxT("%i"), &w);
                if (tag.HasParam(wxT("HEIGHT"))) tag.ScanParam(wxT("HEIGHT"), wxT("%i"), &h);
                al = wxHTML_ALIGN_BOTTOM;
                if (tag.HasParam("ALIGN")) {
                    wxString alstr = tag.GetParam("ALIGN");
                    alstr.MakeUpper();  // for the case alignment was in ".."
                    if (alstr == "TEXTTOP") al = wxHTML_ALIGN_TOP;
                    else if ((alstr == "CENTER") || (alstr == "ABSCENTER")) al = wxHTML_ALIGN_CENTER;
                }
                if (tag.HasParam("USEMAP")) {
                    mn = tag.GetParam( "USEMAP" );
                    if (mn[ 0 ] == '#') {
                        mn = mn.Mid( 1 );
                    }
                }
                wxHtmlImageCell *cel = NULL;
                if (str) {
                    cel = new wxHtmlImageCell(str, w, h, al, mn);
                    cel -> SetLink(m_WParser -> GetLink());
                    m_WParser -> GetContainer() -> InsertCell(cel);
                    delete str;
                }
            }
        }
        if (tag.GetName() == "MAP") {
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
            if (tag.HasParam("NAME")) {
                wxString tmp = tag.GetParam("NAME");
                wxHtmlImageMapCell *cel = new wxHtmlImageMapCell( tmp );
                m_WParser->GetContainer()->InsertCell( cel );
            }
            ParseInner( tag );
            m_WParser->CloseContainer();
            m_WParser->OpenContainer();
        }
        if (tag.GetName() == "AREA") {
            if (tag.HasParam("SHAPE")) {
                wxString tmp = tag.GetParam("SHAPE");
                wxString coords;
                tmp.MakeUpper();
                wxHtmlImageMapAreaCell *cel = NULL;
                if (tag.HasParam("COORDS")) {
                    coords = tag.GetParam("COORDS");
                }
                if (tmp == "POLY") {
                    cel = new wxHtmlImageMapAreaCell( wxHtmlImageMapAreaCell::POLY, coords );
                } else if (tmp == "CIRCLE") {
                    cel = new wxHtmlImageMapAreaCell( wxHtmlImageMapAreaCell::CIRCLE, coords );
                } else if (tmp == "RECT") {
                    cel = new wxHtmlImageMapAreaCell( wxHtmlImageMapAreaCell::RECT, coords );
                }
                if (cel != NULL && tag.HasParam("HREF")) {
                    wxString tmp = tag.GetParam("HREF");
                    cel->SetLink( tmp );
                }
                if (cel != NULL) m_WParser->GetContainer()->InsertCell( cel );
            }
        }

        return FALSE;
    }

TAG_HANDLER_END(IMG)



TAGS_MODULE_BEGIN(Image)

    TAGS_MODULE_ADD(IMG)

TAGS_MODULE_END(Image)


#endif
