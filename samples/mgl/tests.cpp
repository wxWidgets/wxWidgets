
#include "tests.h"

#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/image.h"
#include "wx/log.h"
#include "wx/dcmemory.h"

#if defined(__WXMSW__)
#define FULLNAME(x) (x "_msw.png")
#elif defined(__WXGTK__)
#define FULLNAME(x) (x "_gtk.png")
#else
#define FULLNAME(x) (x ".png")
#endif



#ifdef TEST_DC

void RenderTest1(wxDC& dc)
{
    dc.SetBackground(*wxLIGHT_GREY_BRUSH);
    dc.Clear();

    wxPen pen;
    dc.SetPen(*wxRED_PEN);
    dc.DrawLine(10, 10, SIZEX-11, 10);
    dc.SetPen(*wxBLACK_DASHED_PEN);
    dc.DrawLine(10, 20, SIZEX-11, 20);
    dc.DrawLine(0, 39, SIZEX-1, 39);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawLine(10, 30, SIZEX-11, 30);
    pen = *wxRED_PEN;
    pen.SetWidth(10);
    dc.SetPen(pen);
    dc.DrawLine(10, 40, SIZEX-11, 40);
    pen = *wxBLACK_DASHED_PEN;
    pen.SetWidth(5);
    dc.SetPen(pen);
    dc.DrawLine(10, 50, SIZEX-11, 50);
    dc.SetPen(wxPen("blue", 1, wxDOT_DASH));
    dc.CrossHair(300,200);

    // ---------
    
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.DrawRectangle(10, 100, 100, 100);
    dc.DrawRectangle(10, 200, 100, 100);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(100, 100, 100, 100);
    dc.DrawRectangle(100, 200, 100, 100);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(*wxGREY_BRUSH);
    dc.DrawRectangle(300, 100, 100, 100);
    dc.DrawRectangle(300, 100, 100, 100);
    
    dc.SetPen(wxPen("red", 5, wxSOLID));
    dc.SetBrush(*wxBLUE_BRUSH);
    dc.SetLogicalFunction(wxXOR);
    dc.DrawRectangle(150, 150, 100, 100);
    dc.SetLogicalFunction(wxCOPY);

    // ---------

    dc.SetDeviceOrigin(400, 400);
    dc.SetPen(wxPen("green", 1, wxLONG_DASH));
    dc.DrawRoundedRectangle(0, 0, 200, 100, 25);
    dc.SetPen(wxPen("green", 5, wxSOLID));
    dc.DrawArc(240, 40, 320, 120, 280, 80);
    dc.DrawRoundedRectangle(0, 0, 200, 100, 25);
    dc.DrawCheckMark(-20, -20, 15, 15);
    dc.SetPen(wxPen("green", 5, wxCROSSDIAG_HATCH));
    dc.DrawEllipse(0, 250, 300, 100);
    dc.DrawEllipticArc(320, 250, 300, 100, 13, 260);

    // ---------
    
    dc.SetPen(wxPen("black", 3, wxSOLID));
    wxPoint pts[] = {wxPoint(20, 20), wxPoint(100,200), wxPoint(10,167)}; 
    dc.DrawPolygon(3, pts);
    dc.DrawLines(3, pts, 100, 100);
    dc.SetUserScale(2.0, 2.0);
    dc.DrawLines(3, pts, 100, 100);
    dc.SetUserScale(1.0, 1.0);


    // ---------

    dc.SetDeviceOrigin(500, 0);
    dc.SetPen(*wxCYAN_PEN);
    dc.SetBrush(wxBrush(*wxBLACK, wxBDIAGONAL_HATCH));
    dc.DrawRectangle(0, 0, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxCROSSDIAG_HATCH));
    dc.DrawRectangle(0, 50, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxFDIAGONAL_HATCH));
    dc.DrawRectangle(0, 100, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxCROSS_HATCH));
    dc.DrawRectangle(0, 150, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxHORIZONTAL_HATCH));
    dc.DrawRectangle(0, 200, 100, 50);
    dc.SetBrush(wxBrush(*wxBLACK, wxVERTICAL_HATCH));
    dc.DrawRectangle(0, 250, 100, 50);

    dc.SetDeviceOrigin(0, 0);
}


void RenderTest2(wxDC& dc)
{
    wxRegion clip;
    clip.Union(256,0,512,1024);
    clip.Union(0,256,900,512);
    
    dc.SetClippingRegion(clip);
    RenderTest1(dc);
    
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    for (wxRegionIterator it(clip); it.HaveRects(); it++)
        dc.DrawRectangle(it.GetRect());
    
}

#endif


#ifdef TEST_BITMAP



unsigned char g_XBM[] = {0x55,0x55,0xF0,0xF0,0xCC,0xCC,0x39,0x39,0xFF,0xFF,
                         0x55,0x55,0xF0,0xF0,0xCC,0xCC,0x39,0x39,0xFF,0xFF};
                                               /* - - - - - - - -*/;
                                               /*    ----    ----*/;
                                               /*--  --  --  --  */;
                                               /*  ---  -  ---  -*/;        
                                               /*----------------*/;        
                                               /* - - - - - - - -*/;
                                               /*    ----    ----*/;
                                               /*--  --  --  --  */;
                                               /*  ---  -  ---  -*/;        
                                               /*----------------*/;        

#include "garfield.xpm"

void TestBitmap()
{
    wxBitmap myBmp(800, 600);

    wxBitmap garfield("garfield.png", wxBITMAP_TYPE_PNG);
    wxBitmap bmask1("mask1.png", wxBITMAP_TYPE_PNG);
    wxBitmap bmask2("mask2.png", wxBITMAP_TYPE_PNG);
    wxBitmap blender("blender.png", wxBITMAP_TYPE_PNG);
    wxIcon blender_icon;
    blender_icon.CopyFromBitmap(blender);
    
    wxMemoryDC dc;
    dc.SelectObject(myBmp);
    
    dc.SetUserScale(2.0, 3.1);
    dc.DrawBitmap(garfield, 0, 0);
    dc.SetUserScale(1.0, 1.0);
    garfield.SetMask(new wxMask(bmask2, wxColour(0,0,0)));
    dc.DrawBitmap(garfield, 50, 50, TRUE);
    garfield.SaveFile(FULLNAME("test_garfield"), wxBITMAP_TYPE_PNG);
    garfield.SetMask(NULL);
    dc.DrawIcon(blender_icon, 10, 10);
    dc.DrawBitmap(blender, 80, 10);
    dc.DrawBitmap(blender, 160, 10, TRUE);
    dc.SetUserScale(3.0, 3.0);
    dc.DrawIcon(blender_icon, 10, 30);
    dc.SetUserScale(1.0, 1.0);
    
    const int ROPs[] = {wxAND_REVERSE, wxNOR, wxNAND, wxCLEAR, wxXOR, wxINVERT, 
                        wxOR_REVERSE, wxCOPY, wxAND, wxAND_INVERT, wxNO_OP, 
                        wxEQUIV, wxSRC_INVERT, wxOR_INVERT, wxOR, wxSET};
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {        
            dc.DrawBitmap(garfield, 200+130*x, 130*y);
            dc.SetLogicalFunction(ROPs[y*4+x]);
            dc.DrawBitmap(blender, 200+130*x+50, 130*y+50, TRUE);
            dc.SetLogicalFunction(wxCOPY);
        }
    }

    wxBitmap stipple1("stipple1.png", wxBITMAP_TYPE_PNG);
    wxBitmap stipple2("stipple2.png", wxBITMAP_TYPE_PNG);
    
    dc.DrawBitmap(stipple1, 10, 10, TRUE);
    dc.DrawBitmap(stipple2, 20, 10, TRUE);
    
#ifndef __WXGTK__
    dc.SetPen(wxPen(stipple2, 10));
    dc.SetBrush(*wxTRANSPARENT_BRUSH);    
    dc.DrawRectangle(50, 50, 100, 100);
#endif    
    dc.SetPen(wxPen("black", 10, wxSOLID));
    dc.SetTextForeground("red");
    dc.SetTextBackground("blue");
    dc.SetBrush(wxBrush(stipple2));
    dc.DrawRectangle(50, 200, 100, 100);
    dc.SetBrush(wxBrush(stipple1));
    dc.DrawRectangle(50, 350, 100, 100);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    
    wxBitmap xbm((const char*)g_XBM, 16, 10);
    dc.DrawBitmap(xbm, 700, 500);
    dc.SetUserScale(5.0, 5.0);
    dc.SetTextForeground("green");
    dc.SetTextBackground("red");
    dc.DrawBitmap(xbm, 720/5, 520/5);
    dc.SetUserScale(1.0, 1.0);

    dc.DrawBitmap(garfield.GetSubBitmap(wxRect(32, 32, 64, 64)), 720, 10);

    dc.DrawBitmap(wxBitmap(garfield_xpm), 720, 120);
    
#ifdef __WXMGL__
    dc.DrawIcon(wxICON(folder1), 10, 80);
#endif

    myBmp.SaveFile(FULLNAME("test_bmp1"), wxBITMAP_TYPE_PNG);
}


#endif





#ifdef TEST_FONT

#include "wx/fontenum.h"

void Enumerate(wxArrayString &all)
{
    wxArrayString *a;

    {    
        wxFontEnumerator e;
        e.EnumerateFacenames();
        a = e.GetFacenames();   
        wxPrintf("FACENAMES:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
        all = *a;
    }
    {
        wxFontEnumerator e;
        e.EnumerateFacenames(wxFONTENCODING_SYSTEM, TRUE);
        a = e.GetFacenames();   
        wxPrintf("FIXED-WIDTH FACENAMES:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
    {    
        wxFontEnumerator e;
        e.EnumerateFacenames(wxFONTENCODING_ISO8859_2);
        a = e.GetFacenames();   
        wxPrintf("FACENAMES for iso-8859-2:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
    {    
        wxFontEnumerator e;
        e.EnumerateFacenames(wxFONTENCODING_CP1252);
        a = e.GetFacenames();   
        wxPrintf("FACENAMES for cp1252:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
    {    
        wxFontEnumerator e;
        e.EnumerateEncodings();
        a = e.GetEncodings();   
        wxPrintf("ENCODINGS:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
    {    
        wxFontEnumerator e;
        e.EnumerateEncodings("Verdana");
        a = e.GetEncodings();   
        wxPrintf("ENCODINGS for Verdana:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
            wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
    {    
        wxFontEnumerator e;
        e.EnumerateEncodings("Helvetica");
        a = e.GetEncodings();   
        wxPrintf("ENCODINGS for Helvetica:\n");
        for (size_t i = 0; i < a->GetCount(); i++)
             wxPrintf("    '%s'\n", (*a)[i].c_str());
    }
}


void DoFont(wxDC& dc, wxCoord &x, wxCoord &y, const wxFont& font)
{
    static wxCoord max_w = 0;

    wxCoord w, h;
    wxString text(font.GetFaceName());
    if (!text) text = "noname :)";

    dc.SetFont(font);
    dc.GetTextExtent(text, &w, &h);
    
    if (y + h > 600)
    {
        y = 20;
        x += max_w;
        max_w = 0;
    }
    if (w > max_w)
        max_w = w;
    
    dc.DrawRectangle(x, y, w, h);
    dc.DrawText(text, x, y);
    y += dc.GetCharHeight();
}

void TestFont1()
{
    wxArrayString all;
    Enumerate(all);

    wxBitmap myBmp(800, 600);
    wxMemoryDC dc;
    dc.SelectObject(myBmp);
    wxCoord y = 10, x = 10;

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground(*wxWHITE);
    dc.SetTextBackground(wxColour("blue"));

#ifdef __WXMGL__
    // this is only to test ref counting
    wxFont verdana(22, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE, "Verdana");
    (void)verdana.GetMGLfont_t(1.0, FALSE);
#endif

    DoFont(dc, x,y, wxFont(9, wxDEFAULT, wxNORMAL, wxNORMAL));
    DoFont(dc, x,y, wxFont(12, wxDECORATIVE, wxNORMAL, wxNORMAL));
    DoFont(dc, x,y, wxFont(18, wxROMAN, wxITALIC, wxLIGHT));
    DoFont(dc, x,y, wxFont(18, wxSCRIPT, wxNORMAL, wxLIGHT));
    DoFont(dc, x,y, wxFont(18, wxSWISS, wxNORMAL, wxLIGHT));
    dc.SetBackgroundMode(wxSOLID);
    DoFont(dc, x,y, wxFont(18, wxMODERN, wxNORMAL, wxLIGHT));
    dc.SetBackgroundMode(wxTRANSPARENT);

    for (size_t i = 0; i < all.GetCount(); i++)
    {
        DoFont(dc, x,y, wxFont(12, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE, all[i]));
        DoFont(dc, x,y, wxFont(12, wxDEFAULT, wxITALIC, wxNORMAL, FALSE, all[i]));
        DoFont(dc, x,y, wxFont(12, wxDEFAULT, wxITALIC, wxBOLD, FALSE, all[i]));
        DoFont(dc, x,y, wxFont(18, wxDEFAULT, wxNORMAL, wxBOLD, FALSE, all[i]));
        y += 40;
    }
  
    wxFont verdanaCZ(13, wxNORMAL, wxNORMAL, wxNORMAL, FALSE, 
                     "Verdana", wxFONTENCODING_ISO8859_2);
    // FIXME_MGL -- try with verdanaCZ,sz=12pt after Kendall fixes related 
    // bug in MGL
    dc.SetFont(verdanaCZ);
    dc.DrawText("®lu»ouèký kùò...", x, y);

    myBmp.SaveFile(FULLNAME("test_font1"), wxBITMAP_TYPE_PNG);
}


void TestFont2()
{
    wxBitmap myBmp(800, 600);
    wxMemoryDC dc;
    dc.SelectObject(myBmp);
    wxCoord y = 30, x = 30;

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetTextForeground(*wxWHITE);
    dc.SetTextBackground(wxColour("blue"));

    wxFont verdana(12, wxDEFAULT, wxNORMAL, wxNORMAL, TRUE, "Verdana");

    DoFont(dc, x,y, verdana);
    dc.SetUserScale(2.8, 2.8);
    y = (int)(y / 2.8);
    DoFont(dc, x,y, verdana);  
    dc.SetUserScale(1.0, 1.0);
    y = (int)(y * 2.8);
    DoFont(dc, x,y, verdana); 

    dc.DrawLine(400, 0, 400, 600);    
    dc.DrawLine(0, 300, 800, 300);    
    dc.DrawRotatedText("rotated text 1", 400, 300, 0);
    dc.DrawRotatedText("rotated text 2", 400, 300, 90);
    dc.DrawRotatedText("rotated text 3", 400, 300, 180);
    dc.DrawRotatedText("rotated text 4", 400, 300, 270);

    myBmp.SaveFile(FULLNAME("test_font2"), wxBITMAP_TYPE_PNG);
}

void TestFont()
{
#ifdef __WXMGL__
    wxLog::AddTraceMask("mgl_font");
    TestFont1();
#endif
    TestFont2();
}

#endif
