/*
 * Program: image
 *
 * Author: Robert Roebling
 *
 * Copyright: (C) 1998, Robert Roebling
 *
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/file.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"
#include "wx/quantize.h"

#include "smile.xbm"

#if !defined(__WINDOWS__) || wxUSE_XPM_IN_MSW
    #include "smile.xpm"
#endif


// derived classes

class MyFrame;
class MyApp;

// MyCanvas

class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas() {}
    MyCanvas( wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size );
    ~MyCanvas();
    void OnPaint( wxPaintEvent &event );
    void CreateAntiAliasedBitmap();

    wxBitmap  *my_horse_png;
    wxBitmap  *my_horse_jpeg;
    wxBitmap  *my_horse_gif;
    wxBitmap  *my_horse_bmp;
    wxBitmap  *my_horse_bmp2;
    wxBitmap  *my_horse_pcx;
    wxBitmap  *my_horse_pnm;
    wxBitmap  *my_horse_tiff;
    wxBitmap  *my_horse_xpm;
    wxBitmap  *my_horse_ico32;
    wxBitmap  *my_horse_ico16;
    wxBitmap  *my_horse_ico;
    wxBitmap  *my_horse_cur;
    wxBitmap  *my_horse_ani;

    wxBitmap  *my_smile_xbm;
    wxBitmap  *my_square;
    wxBitmap  *my_anti;

    int xH, yH ;
    int m_ani_images ;

protected:
    wxBitmap m_bmpSmileXpm;
    wxIcon   m_iconSmileXpm;

private:
    DECLARE_DYNAMIC_CLASS(MyCanvas)
    DECLARE_EVENT_TABLE()
};


const int nChoices = 8 ;
static const wxString bppchoices[nChoices] =
{
    _T("1 bpp color"),
    _T("1 bpp B&W"),
    _T("4 bpp color"),
    _T("8 bpp color"),
    _T("8 bpp greyscale"),
    _T("8 bpp red"),
    _T("8 bpp own palette"),
    _T("24 bpp")
};

static const int bppvalues[nChoices] =
{
    wxBMP_1BPP,
    wxBMP_1BPP_BW,
    wxBMP_4BPP,
    wxBMP_8BPP,
    wxBMP_8BPP_GREY,
    wxBMP_8BPP_RED,
    wxBMP_8BPP_PALETTE,
    wxBMP_24BPP
};

// MyFrame


class MyFrame: public wxFrame
{
public:
    MyFrame();

    void OnAbout( wxCommandEvent &event );
    void OnNewFrame( wxCommandEvent &event );
    void OnQuit( wxCommandEvent &event );

    MyCanvas         *m_canvas;

private:
    DECLARE_DYNAMIC_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};

class MyImageFrame : public wxFrame
{
public:
    MyImageFrame(wxFrame *parent, const wxBitmap& bitmap)
        : wxFrame(parent, -1, _T("Double click to save"),
                  wxDefaultPosition, wxDefaultSize,
                  wxCAPTION | wxSYSTEM_MENU),
                  m_bitmap(bitmap)
    {
        SetClientSize(bitmap.GetWidth(), bitmap.GetHeight());
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event))
    {
        wxPaintDC dc( this );
        //TRUE for masked images
        dc.DrawBitmap( m_bitmap, 0, 0, TRUE );
    }

    void OnSave(wxCommandEvent& WXUNUSED(event))
    {
        wxImage image = m_bitmap.ConvertToImage();

        int bppselection = wxGetSingleChoiceIndex(_T("Set BMP BPP"),
                                                  _T("Set BMP BPP"),
                                                  nChoices,
                                                  bppchoices,
                                                  this);
        if ( bppselection == -1 )
        {
            // cancelled
            return;
        }

        image.SetOption(wxIMAGE_OPTION_BMP_FORMAT, bppvalues[bppselection]);

        wxString deffilename = bppchoices[bppselection];
        deffilename.Replace(wxT(" "), wxT("_"));
        deffilename += wxT(".bmp");
        wxString savefilename = wxFileSelector( wxT("Save Image"),
                                                wxT(""),
                                                deffilename,
                                                (const wxChar *)NULL,
                                            wxT("BMP files (*.bmp)|*.bmp|")
                                            wxT("PNG files (*.png)|*.png|")
                                            wxT("JPEG files (*.jpg)|*.jpg|")
                                            wxT("GIF files (*.gif)|*.gif|")
                                            wxT("TIFF files (*.tif)|*.tif|")
                                            wxT("PCX files (*.pcx)|*.pcx|")
                                            wxT("ICO files (*.ico)|*.ico|")
                                            wxT("CUR files (*.cur)|*.cur"),
                                                wxSAVE);

        if ( savefilename.empty() )
            return;

        if ( image.GetOptionInt(wxIMAGE_OPTION_BMP_FORMAT) == wxBMP_8BPP_PALETTE )
        {
            unsigned char *cmap = new unsigned char [256];
            for ( int i = 0; i < 256; i++ )
                cmap[i] = i;
            image.SetPalette(wxPalette(256, cmap, cmap, cmap));

            delete cmap;
        }

        bool loaded;
        wxString extension = savefilename.AfterLast('.').Lower();

        if (extension == _T("cur"))
        {
            image.Rescale(32,32);
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
            image.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
            // This shows how you can save an image with explicitly
            // specified image format:
            loaded = image.SaveFile(savefilename, wxBITMAP_TYPE_CUR);
        }
        else
        {
            // This one guesses image format from filename extension
            // (it may fail if the extension is not recognized):
            loaded = image.SaveFile(savefilename);
        }

        if ( !loaded )
            wxMessageBox(_T("No handler for this file type."),
                         _T("File was not saved"),
                         wxOK|wxCENTRE, this);
    }

private:
    wxBitmap m_bitmap;

    DECLARE_EVENT_TABLE()
};

// MyApp

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

// main program

IMPLEMENT_APP(MyApp)

// MyCanvas

IMPLEMENT_DYNAMIC_CLASS(MyCanvas, wxScrolledWindow)

BEGIN_EVENT_TABLE(MyImageFrame, wxFrame)
  EVT_PAINT(MyImageFrame::OnPaint)
  EVT_LEFT_DCLICK(MyImageFrame::OnSave)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
  EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id,
                    const wxPoint &pos, const wxSize &size )
        : wxScrolledWindow( parent, id, pos, size, wxSUNKEN_BORDER )
#if !defined(__WINDOWS__) || wxUSE_XPM_IN_MSW
          , m_bmpSmileXpm((const char **) smile_xpm)
          , m_iconSmileXpm((const char **) smile_xpm)
#endif
{
    my_horse_png = (wxBitmap*) NULL;
    my_horse_jpeg = (wxBitmap*) NULL;
    my_horse_gif = (wxBitmap*) NULL;
    my_horse_bmp = (wxBitmap*) NULL;
    my_horse_bmp2 = (wxBitmap*) NULL;
    my_horse_pcx = (wxBitmap*) NULL;
    my_horse_pnm = (wxBitmap*) NULL;
    my_horse_tiff = (wxBitmap*) NULL;
    my_horse_xpm = (wxBitmap*) NULL;
    my_horse_ico32 = (wxBitmap*) NULL;
    my_horse_ico16 = (wxBitmap*) NULL;
    my_horse_ico = (wxBitmap*) NULL;
    my_horse_cur = (wxBitmap*) NULL;
    my_horse_ani = (wxBitmap*) NULL;

    my_smile_xbm = (wxBitmap*) NULL;
    my_square = (wxBitmap*) NULL;
    my_anti = (wxBitmap*) NULL;

    m_ani_images = 0 ;

    SetBackgroundColour(* wxWHITE);

    wxBitmap bitmap( 100, 100 );

    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetBrush( wxBrush( wxT("orange"), wxSOLID ) );
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawRectangle( 0, 0, 100, 100 );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 20, 20, 60, 60 );
    dc.SelectObject( wxNullBitmap );

    // try to find the directory with our images
    wxString dir;
    if ( wxFile::Exists(wxT("./horse.png")) )
        dir = wxT("./");
    else if ( wxFile::Exists(wxT("../horse.png")) )
        dir = wxT("../");
    else
        wxLogWarning(wxT("Can't find image files in either '.' or '..'!"));

    wxImage image = bitmap.ConvertToImage();

#if wxUSE_LIBPNG
    if ( !image.SaveFile( dir + _T("test.png"), wxBITMAP_TYPE_PNG ))
        wxLogError(wxT("Can't save file"));

    image.Destroy();

    image.LoadFile( dir + _T("test.png") );
    my_square = new wxBitmap( image );

    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.png")) )
        wxLogError(wxT("Can't load PNG image"));
    else
        my_horse_png = new wxBitmap( image );
#endif // wxUSE_LIBPNG

#if wxUSE_LIBJPEG
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.jpg")) )
        wxLogError(wxT("Can't load JPG image"));
    else
        my_horse_jpeg = new wxBitmap( image );
#endif // wxUSE_LIBJPEG

#if wxUSE_GIF
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.gif" )) )
        wxLogError(wxT("Can't load GIF image"));
    else
        my_horse_gif = new wxBitmap( image );
#endif

#if wxUSE_PCX
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.pcx"), wxBITMAP_TYPE_PCX ) )
        wxLogError(wxT("Can't load PCX image"));
    else
        my_horse_pcx = new wxBitmap( image );
#endif

    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.bmp"), wxBITMAP_TYPE_BMP ) )
        wxLogError(wxT("Can't load BMP image"));
    else
        my_horse_bmp = new wxBitmap( image );

#if wxUSE_XPM
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.xpm"), wxBITMAP_TYPE_XPM ) )
        wxLogError(wxT("Can't load XPM image"));
    else
        my_horse_xpm = new wxBitmap( image );

    if ( !image.SaveFile( dir + _T("test.xpm"), wxBITMAP_TYPE_XPM ))
        wxLogError(wxT("Can't save file"));
#endif

#if wxUSE_PNM
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.pnm"), wxBITMAP_TYPE_PNM ) )
        wxLogError(wxT("Can't load PNM image"));
    else
        my_horse_pnm = new wxBitmap( image );
#endif

#if wxUSE_LIBTIFF
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.tif"), wxBITMAP_TYPE_TIF ) )
        wxLogError(wxT("Can't load TIFF image"));
    else
        my_horse_tiff = new wxBitmap( image );
#endif

    CreateAntiAliasedBitmap();

    my_smile_xbm = new wxBitmap( (const char*)smile_bits, smile_width,
                                 smile_height, 1 );

#if !defined(__WINDOWS__) || wxUSE_XPM_IN_MSW
    // demonstrates XPM automatically using the mask when saving
    if ( m_bmpSmileXpm.Ok() )
        m_bmpSmileXpm.SaveFile(_T("saved.xpm"), wxBITMAP_TYPE_XPM);
#endif

#if wxUSE_ICO_CUR
    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.ico"), wxBITMAP_TYPE_ICO, 0 ) )
        wxLogError(wxT("Can't load first ICO image"));
    else
        my_horse_ico32 = new wxBitmap( image );

    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.ico"), wxBITMAP_TYPE_ICO, 1 ) )
        wxLogError(wxT("Can't load second ICO image"));
    else
        my_horse_ico16 = new wxBitmap( image );

    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.ico") ) )
        wxLogError(wxT("Can't load best ICO image"));
    else
        my_horse_ico = new wxBitmap( image );

    image.Destroy();

    if ( !image.LoadFile( dir + _T("horse.cur"), wxBITMAP_TYPE_CUR ) )
        wxLogError(wxT("Can't load best ICO image"));
    else
    {
        my_horse_cur = new wxBitmap( image );
        xH = 30 + image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X) ;
        yH = 2420 + image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y) ;
    }

    m_ani_images = wxImage::GetImageCount ( dir + _T("horse3.ani"), wxBITMAP_TYPE_ANI );
    if (m_ani_images==0)
        wxLogError(wxT("No ANI-format images found"));
    else
        my_horse_ani = new wxBitmap [m_ani_images];
    int i ;
    for (i=0; i < m_ani_images; i++)
    {
        image.Destroy();
        if (!image.LoadFile( dir + _T("horse3.ani"), wxBITMAP_TYPE_ANI, i ))
        {
            wxString tmp = wxT("Can't load image number ");
            tmp << i ;
            wxLogError(tmp);
        }
        else    
            my_horse_ani [i] = wxBitmap( image );
    }
    
    
#endif

    image.Destroy();

    // test image loading from stream
    wxFile file(dir + _T("horse.bmp"));
    off_t len = file.Length();
    void *data = malloc(len);
    if ( file.Read(data, len) != len )
        wxLogError(_T("Reading bitmap file failed"));
    else
    {
        wxMemoryInputStream mis(data, len);
        if ( !image.LoadFile(mis) )
            wxLogError(wxT("Can't load BMP image from stream"));
        else
            my_horse_bmp2 = new wxBitmap( image );
    }

    free(data);
}

MyCanvas::~MyCanvas()
{
    delete my_horse_pnm;
    delete my_horse_png;
    delete my_horse_jpeg;
    delete my_horse_gif;
    delete my_horse_bmp;
    delete my_horse_bmp2;
    delete my_horse_pcx;
    delete my_horse_tiff;
    delete my_horse_xpm;
    delete my_horse_ico32;
    delete my_horse_ico16;
    delete my_horse_ico;
    delete my_horse_cur;
    delete [] my_horse_ani;
    delete my_smile_xbm;
    delete my_square;
    delete my_anti;
}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );

    dc.DrawText( _T("Loaded image"), 30, 10 );
    if (my_square && my_square->Ok()) dc.DrawBitmap( *my_square, 30, 30 );

    dc.DrawText( _T("Drawn directly"), 150, 10 );
    dc.SetBrush( wxBrush( wxT("orange"), wxSOLID ) );
    dc.SetPen( *wxBLACK_PEN );
    dc.DrawRectangle( 150, 30, 100, 100 );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawRectangle( 170, 50, 60, 60 );

    if (my_anti && my_anti->Ok())
        dc.DrawBitmap( *my_anti, 280, 30 );

    dc.DrawText( _T("PNG handler"), 30, 135 );
    if (my_horse_png && my_horse_png->Ok())
    {
        dc.DrawBitmap( *my_horse_png, 30, 150 );
        wxRect rect(0,0,100,100);
        wxBitmap sub( my_horse_png->GetSubBitmap(rect) );
        dc.DrawText( _T("GetSubBitmap()"), 280, 190 );
        dc.DrawBitmap( sub, 280, 210 );
    }

    dc.DrawText( _T("JPEG handler"), 30, 365 );
    if (my_horse_jpeg && my_horse_jpeg->Ok())
        dc.DrawBitmap( *my_horse_jpeg, 30, 380 );

    dc.DrawText( _T("GIF handler"), 30, 595 );
    if (my_horse_gif && my_horse_gif->Ok())
        dc.DrawBitmap( *my_horse_gif, 30, 610 );

    dc.DrawText( _T("PCX handler"), 30, 825 );
    if (my_horse_pcx && my_horse_pcx->Ok())
        dc.DrawBitmap( *my_horse_pcx, 30, 840 );

    dc.DrawText( _T("BMP handler"), 30, 1055 );
    if (my_horse_bmp && my_horse_bmp->Ok())
        dc.DrawBitmap( *my_horse_bmp, 30, 1070 );

    dc.DrawText( _T("BMP read from memory"), 280, 1055 );
    if (my_horse_bmp2 && my_horse_bmp2->Ok())
        dc.DrawBitmap( *my_horse_bmp2, 280, 1070 );

    dc.DrawText( _T("PNM handler"), 30, 1285 );
    if (my_horse_pnm && my_horse_pnm->Ok())
        dc.DrawBitmap( *my_horse_pnm, 30, 1300 );

    dc.DrawText( _T("TIFF handler"), 30, 1515 );
    if (my_horse_tiff && my_horse_tiff->Ok())
        dc.DrawBitmap( *my_horse_tiff, 30, 1530 );

    dc.DrawText( _T("XPM handler"), 30, 1745 );
    if (my_horse_xpm && my_horse_xpm->Ok())
        dc.DrawBitmap( *my_horse_xpm, 30, 1760 );


    if (my_smile_xbm && my_smile_xbm->Ok())
    {
        dc.DrawText( _T("XBM bitmap"), 30, 1975 );
        dc.DrawText( _T("(green on red)"), 30, 1990 );
        dc.SetTextForeground( _T("GREEN") );
        dc.SetTextBackground( _T("RED") );
        dc.DrawBitmap( *my_smile_xbm, 30, 2010 );

        dc.SetTextForeground( wxT("BLACK") );
        dc.DrawText( _T("After wxImage conversion"), 150, 1975 );
        dc.DrawText( _T("(red on white)"), 150, 1990 );
        dc.SetTextForeground( wxT("RED") );
        wxImage i = my_smile_xbm->ConvertToImage();
        i.SetMaskColour( 255, 255, 255 );
        i.Replace( 0, 0, 0,
               wxRED_PEN->GetColour().Red(),
               wxRED_PEN->GetColour().Green(),
               wxRED_PEN->GetColour().Blue() );
        dc.DrawBitmap( wxBitmap(i), 150, 2010, TRUE );
        dc.SetTextForeground( wxT("BLACK") );
    }


    wxBitmap mono( 60,50,1 );
    wxMemoryDC memdc;
    memdc.SelectObject( mono );
    memdc.SetPen( *wxBLACK_PEN );
    memdc.SetBrush( *wxWHITE_BRUSH );
    memdc.DrawRectangle( 0,0,60,50 );
    memdc.SetTextForeground( *wxBLACK );
    memdc.DrawText( _T("Hi!"), 5, 5 );
    memdc.SetBrush( *wxBLACK_BRUSH );
    memdc.DrawRectangle( 33,5,20,20 );
    memdc.SetPen( *wxRED_PEN );
    memdc.DrawLine( 5, 42, 50, 42 );
    memdc.SelectObject( wxNullBitmap );

    if (mono.Ok())
    {
        dc.DrawText( _T("Mono bitmap"), 30, 2095 );
        dc.DrawText( _T("(red on green)"), 30, 2110 );
        dc.SetTextForeground( wxT("RED") );
        dc.SetTextBackground( wxT("GREEN") );
        dc.DrawBitmap( mono, 30, 2130 );

        dc.SetTextForeground( wxT("BLACK") );
        dc.DrawText( _T("After wxImage conversion"), 150, 2095 );
        dc.DrawText( _T("(red on white)"), 150, 2110 );
        dc.SetTextForeground( wxT("RED") );
        wxImage i = mono.ConvertToImage();
        i.SetMaskColour( 255,255,255 );
        i.Replace( 0,0,0,
               wxRED_PEN->GetColour().Red(),
               wxRED_PEN->GetColour().Green(),
               wxRED_PEN->GetColour().Blue() );
        dc.DrawBitmap( wxBitmap(i), 150, 2130, TRUE );
        dc.SetTextForeground( wxT("BLACK") );
    }

    dc.DrawText(_T("XPM bitmap"), 30, 2230);
    if ( m_bmpSmileXpm.Ok() )
    {
        dc.DrawBitmap(m_bmpSmileXpm, 30, 2250, TRUE);
    }

    dc.DrawText(_T("XPM icon"), 150, 2230);
    if ( m_iconSmileXpm.Ok() )
    {
        dc.DrawIcon(m_iconSmileXpm, 150, 2250);
    }

    dc.DrawText( _T("ICO handler (1st image)"), 30, 2290 );
    if (my_horse_ico32 && my_horse_ico32->Ok())
        dc.DrawBitmap( *my_horse_ico32, 30, 2330, TRUE );

    dc.DrawText( _T("ICO handler (2nd image)"), 230, 2290 );
    if (my_horse_ico16 && my_horse_ico16->Ok())
        dc.DrawBitmap( *my_horse_ico16, 230, 2330, TRUE );

    dc.DrawText( _T("ICO handler (best image)"), 430, 2290 );
    if (my_horse_ico && my_horse_ico->Ok())
        dc.DrawBitmap( *my_horse_ico, 430, 2330, TRUE );

    dc.DrawText( _T("CUR handler"), 30, 2390 );
    if (my_horse_cur && my_horse_cur->Ok())
    {
        dc.DrawBitmap( *my_horse_cur, 30, 2420, TRUE );
        dc.SetPen (*wxRED_PEN);
        dc.DrawLine (xH-10,yH,xH+10,yH);
        dc.DrawLine (xH,yH-10,xH,yH+10);
    }
    dc.DrawText( _T("ANI handler"), 230, 2390 );
    int i ;
    for (i=0; i < m_ani_images; i ++)
        if (my_horse_ani[i].Ok())
        {
            dc.DrawBitmap( my_horse_ani[i], 230 + i * 2 * my_horse_ani[i].GetWidth() , 2420, TRUE );
        }
}

void MyCanvas::CreateAntiAliasedBitmap()
{
  wxBitmap bitmap( 300, 300 );

  wxMemoryDC dc;

  dc.SelectObject( bitmap );

  dc.Clear();

  dc.SetFont( wxFont( 24, wxDECORATIVE, wxNORMAL, wxNORMAL) );
  dc.SetTextForeground( wxT("RED") );
  dc.DrawText( _T("This is anti-aliased Text."), 20, 20 );
  dc.DrawText( _T("And a Rectangle."), 20, 60 );

  dc.SetBrush( *wxRED_BRUSH );
  dc.SetPen( *wxTRANSPARENT_PEN );
  dc.DrawRoundedRectangle( 20, 100, 200, 180, 20 );

  wxImage original= bitmap.ConvertToImage();
  wxImage anti( 150, 150 );

  /* This is quite slow, but safe. Use wxImage::GetData() for speed instead. */

  for (int y = 1; y < 149; y++)
    for (int x = 1; x < 149; x++)
    {
       int red = original.GetRed( x*2, y*2 ) +
                 original.GetRed( x*2-1, y*2 ) +
                 original.GetRed( x*2, y*2+1 ) +
                 original.GetRed( x*2+1, y*2+1 );
       red = red/4;

       int green = original.GetGreen( x*2, y*2 ) +
                   original.GetGreen( x*2-1, y*2 ) +
                   original.GetGreen( x*2, y*2+1 ) +
                   original.GetGreen( x*2+1, y*2+1 );
       green = green/4;

       int blue = original.GetBlue( x*2, y*2 ) +
                  original.GetBlue( x*2-1, y*2 ) +
                  original.GetBlue( x*2, y*2+1 ) +
                  original.GetBlue( x*2+1, y*2+1 );
       blue = blue/4;
       anti.SetRGB( x, y, red, green, blue );
    }
  my_anti = new wxBitmap(anti);
}

// MyFrame

const int ID_QUIT  = 108;
const int ID_ABOUT = 109;
const int ID_NEW = 110;

IMPLEMENT_DYNAMIC_CLASS( MyFrame, wxFrame )

BEGIN_EVENT_TABLE(MyFrame,wxFrame)
  EVT_MENU    (ID_ABOUT, MyFrame::OnAbout)
  EVT_MENU    (ID_QUIT,  MyFrame::OnQuit)
  EVT_MENU    (ID_NEW,  MyFrame::OnNewFrame)
END_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame( (wxFrame *)NULL, -1, _T("wxImage sample"),
                  wxPoint(20,20), wxSize(470,360) )
{
  wxMenu *file_menu = new wxMenu();
  file_menu->Append( ID_NEW, _T("&Show image..."));
  file_menu->AppendSeparator();
  file_menu->Append( ID_ABOUT, _T("&About..."));
  file_menu->AppendSeparator();
  file_menu->Append( ID_QUIT, _T("E&xit"));

  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, _T("&File"));

  SetMenuBar( menu_bar );

  CreateStatusBar(2);
  int widths[] = { -1, 100 };
  SetStatusWidths( 2, widths );

  m_canvas = new MyCanvas( this, -1, wxPoint(0,0), wxSize(10,10) );

  // 500 width * 2500 height
  m_canvas->SetScrollbars( 10, 10, 50, 250 );
}

void MyFrame::OnQuit( wxCommandEvent &WXUNUSED(event) )
{
  Close( TRUE );
}

void MyFrame::OnAbout( wxCommandEvent &WXUNUSED(event) )
{
  (void)wxMessageBox( _T("wxImage demo\n")
                      _T("Robert Roebling (c) 1998,2000"),
                      _T("About wxImage Demo"), wxICON_INFORMATION | wxOK );
}

void MyFrame::OnNewFrame( wxCommandEvent &WXUNUSED(event) )
{
    wxString filename = wxFileSelector(_T("Select image file"));
    if ( !filename )
        return;

    wxImage image;
    if ( !image.LoadFile(filename) )
    {
        wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());

        return;
    }

    (new MyImageFrame(this, wxBitmap(image)))->Show();
}

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

bool MyApp::OnInit()
{
#if wxUSE_LIBPNG
  wxImage::AddHandler( new wxPNGHandler );
#endif

#if wxUSE_LIBJPEG
  wxImage::AddHandler( new wxJPEGHandler );
#endif

#if wxUSE_LIBTIFF
  wxImage::AddHandler( new wxTIFFHandler );
#endif

#if wxUSE_GIF
  wxImage::AddHandler( new wxGIFHandler );
#endif

#if wxUSE_PCX
  wxImage::AddHandler( new wxPCXHandler );
#endif

#if wxUSE_PNM
  wxImage::AddHandler( new wxPNMHandler );
#endif

#if wxUSE_XPM
  wxImage::AddHandler( new wxXPMHandler );
#endif

#if wxUSE_ICO_CUR
  wxImage::AddHandler( new wxICOHandler );
  wxImage::AddHandler( new wxCURHandler );
  wxImage::AddHandler( new wxANIHandler );
#endif

  wxFrame *frame = new MyFrame();
  frame->Show( TRUE );

  return TRUE;
}

