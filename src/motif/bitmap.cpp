/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.cpp
// Purpose:     wxBitmap
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bitmap.h"
#endif

#include "wx/setup.h"
#include "wx/utils.h"
#include "wx/palette.h"
#include "wx/bitmap.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/control.h"
#include "wx/dcmemory.h"

#include <Xm/Xm.h>

#include "wx/motif/private.h"

// TODO: correct symbol, path?
#if wxUSE_XPM
#include <X11/xpm.h>
#endif

#if !USE_SHARED_LIBRARIES
IMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxGDIObject)
IMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject)
#endif

wxBitmapRefData::wxBitmapRefData()
{
    m_ok = FALSE;
    m_width = 0;
    m_height = 0;
    m_depth = 0;
    m_quality = 0;
    m_numColors = 0;
    m_bitmapMask = NULL;

    m_pixmap = (WXPixmap) 0;
    m_display = (WXDisplay*) 0;

    m_freePixmap = TRUE; //TODO: necessary?
    m_freeColors = (unsigned long*) 0;
    m_freeColorsCount = 0;

    // These 5 variables are for wxControl
    m_insensPixmap = (WXPixmap) 0;
    m_labelPixmap = (WXPixmap) 0;
    m_armPixmap = (WXPixmap) 0;
    m_image = (WXImage*) 0;
    m_insensImage = (WXImage*) 0;
}

wxBitmapRefData::~wxBitmapRefData()
{
    if (m_labelPixmap)
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) m_display), (Pixmap) m_labelPixmap);

    if (m_armPixmap)
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) m_display), (Pixmap) m_armPixmap);

    if (m_insensPixmap)
        XmDestroyPixmap (DefaultScreenOfDisplay ((Display*) m_display), (Pixmap) m_insensPixmap);

    if (m_image)
    {
        XmUninstallImage ((XImage*) m_image);
        XtFree ((char *) (XImage*) m_image);
    }

    if (m_insensImage)
    {
        XmUninstallImage ((XImage*) m_insensImage);
        delete[] ((XImage*) m_insensImage)->data;
        XtFree ((char *) (XImage*) m_insensImage);
    }
    if (m_pixmap && m_freePixmap)
        XFreePixmap ((Display*) m_display, (Pixmap) m_pixmap);

    if (m_freeColors)
    {
       int screen = DefaultScreen((Display*) m_display);
       Colormap cmp = DefaultColormap((Display*) m_display,screen);
       long llp;
       for(llp = 0;llp < m_freeColorsCount;llp++)
          XFreeColors((Display*) m_display, cmp, &m_freeColors[llp], 1, 0L);
       delete m_freeColors;
    };

    if (m_bitmapMask)
        delete m_bitmapMask;
    m_bitmapMask = NULL;
}

wxList wxBitmap::sm_handlers;

wxBitmap::wxBitmap()
{
    m_refData = NULL;

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::~wxBitmap()
{
    if (wxTheBitmapList)
        wxTheBitmapList->DeleteObject(this);
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth)
{
    m_refData = new wxBitmapRefData;

    (void) Create((void*) bits, wxBITMAP_TYPE_XBM_DATA, width, height, depth);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(int w, int h, int d)
{
    (void)Create(w, h, d);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(void *data, long type, int width, int height, int depth)
{
    (void) Create(data, type, width, height, depth);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

wxBitmap::wxBitmap(const wxString& filename, long type)
{
    LoadFile(filename, (int)type);

    if ( wxTheBitmapList )
        wxTheBitmapList->AddBitmap(this);
}

// Create from XPM data
static wxControl* sg_Control = NULL;
wxBitmap::wxBitmap(char **data, wxControl* control)
{
    // Pass the control to the Create function using a global
    sg_Control = control;

    (void) Create((void *)data, wxBITMAP_TYPE_XPM_DATA, 0, 0, 0);

    sg_Control = (wxControl*) NULL;
}

bool wxBitmap::Create(int w, int h, int d)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    if (d < 1)
      d = wxDisplayDepth();

    M_BITMAPDATA->m_width = w;
    M_BITMAPDATA->m_height = h;
    M_BITMAPDATA->m_depth = d;
    M_BITMAPDATA->m_freePixmap = TRUE;

    Display *dpy = (Display*) wxGetDisplay();

    M_BITMAPDATA->m_display = dpy; /* MATTHEW: [4] Remember the display */

    M_BITMAPDATA->m_pixmap = (WXPixmap) XCreatePixmap (dpy, RootWindow (dpy, DefaultScreen (dpy)),
			        w, h, d);

    M_BITMAPDATA->m_ok = (M_BITMAPDATA->m_pixmap != (WXPixmap) 0) ;
    return M_BITMAPDATA->m_ok;
}

bool wxBitmap::LoadFile(const wxString& filename, long type)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->LoadFile(this, filename, type, -1, -1);
}

bool wxBitmap::Create(void *data, long type, int width, int height, int depth)
{
    UnRef();

    m_refData = new wxBitmapRefData;

    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
    }

    return handler->Create(this, data, type, width, height, depth);
}

bool wxBitmap::SaveFile(const wxString& filename, int type, const wxPalette *palette)
{
    wxBitmapHandler *handler = FindHandler(type);

    if ( handler == NULL ) {
        wxLogWarning("no bitmap handler for type %d defined.", type);

        return FALSE;
  }

  return handler->SaveFile(this, filename, type, palette);
}

void wxBitmap::SetWidth(int w)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_width = w;
}

void wxBitmap::SetHeight(int h)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_height = h;
}

void wxBitmap::SetDepth(int d)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_depth = d;
}

void wxBitmap::SetQuality(int q)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_quality = q;
}

void wxBitmap::SetOk(bool isOk)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_ok = isOk;
}

void wxBitmap::SetPalette(const wxPalette& palette)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapPalette = palette ;
}

void wxBitmap::SetMask(wxMask *mask)
{
    if (!M_BITMAPDATA)
        m_refData = new wxBitmapRefData;

    M_BITMAPDATA->m_bitmapMask = mask ;
}

void wxBitmap::AddHandler(wxBitmapHandler *handler)
{
    sm_handlers.Append(handler);
}

void wxBitmap::InsertHandler(wxBitmapHandler *handler)
{
    sm_handlers.Insert(handler);
}

bool wxBitmap::RemoveHandler(const wxString& name)
{
    wxBitmapHandler *handler = FindHandler(name);
    if ( handler )
    {
        sm_handlers.DeleteObject(handler);
        return TRUE;
    }
    else
        return FALSE;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& name)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetName() == name )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(const wxString& extension, long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if ( handler->GetExtension() == extension &&
                    (bitmapType == -1 || handler->GetType() == bitmapType) )
            return handler;
        node = node->Next();
    }
    return NULL;
}

wxBitmapHandler *wxBitmap::FindHandler(long bitmapType)
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        if (handler->GetType() == bitmapType)
            return handler;
        node = node->Next();
    }
    return NULL;
}

/*
 * wxMask
 */

wxMask::wxMask()
{
    m_pixmap = (WXPixmap) 0;
}

// Construct a mask from a bitmap and a colour indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap, colour);
}

// Construct a mask from a bitmap and a palette index indicating
// the transparent area
wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap, paletteIndex);
}

// Construct a mask from a mono bitmap (copies the bitmap).
wxMask::wxMask(const wxBitmap& bitmap)
{
    m_pixmap = (WXPixmap) 0;

    Create(bitmap);
}

wxMask::~wxMask()
{
    // TODO: this may be the wrong display
    if ( m_pixmap )
        XFreePixmap ((Display*) wxGetDisplay(), (Pixmap) m_pixmap);
}

// Create a mask from a mono bitmap (copies the bitmap).
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap))
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a palette index indicating
// the transparent area
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap), int WXUNUSED(paletteIndex))
{
// TODO
    return FALSE;
}

// Create a mask from a bitmap and a colour indicating
// the transparent area
bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap), const wxColour& WXUNUSED(colour))
{
// TODO
    return FALSE;
}

/*
 * wxBitmapHandler
 */

IMPLEMENT_DYNAMIC_CLASS(wxBitmapHandler, wxObject)

bool wxBitmapHandler::Create(wxBitmap *WXUNUSED(bitmap), void *WXUNUSED(data), long WXUNUSED(type), 
   int WXUNUSED(width), int WXUNUSED(height), int WXUNUSED(depth))
{
    return FALSE;
}

bool wxBitmapHandler::LoadFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), long WXUNUSED(type),
        int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
    return FALSE;
}

bool wxBitmapHandler::SaveFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name), int WXUNUSED(type), 
   const wxPalette *WXUNUSED(palette))
{
    return FALSE;
}

/*
 * Standard handlers
 */

class WXDLLEXPORT wxXBMFileHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXBMFileHandler)
public:
    inline wxXBMFileHandler()
    {
        m_name = "XBM file";
        m_extension = "xbm";
        m_type = wxBITMAP_TYPE_XBM;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
};
IMPLEMENT_DYNAMIC_CLASS(wxXBMFileHandler, wxBitmapHandler)

bool wxXBMFileHandler::LoadFile(wxBitmap *bitmap, const wxString& name, long WXUNUSED(flags),
          int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight))
{
    M_BITMAPHANDLERDATA->m_freePixmap = TRUE;

    int hotX, hotY;
    unsigned int w, h;
    Pixmap pixmap;

    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPDATA->m_display = (WXDisplay*) dpy;

    int value = XReadBitmapFile (dpy, RootWindow (dpy, DefaultScreen (dpy)),
                        (char*) (const char*) name, &w, &h, &pixmap, &hotX, &hotY);
    M_BITMAPHANDLERDATA->m_width = w;
    M_BITMAPHANDLERDATA->m_height = h;
    M_BITMAPHANDLERDATA->m_depth = 1;
    M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) pixmap;

    if ((value == BitmapFileInvalid) ||
        (value == BitmapOpenFailed) ||
        (value == BitmapNoMemory))
    {
        M_BITMAPHANDLERDATA->m_ok = FALSE;
        M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) 0;
    }
    else
        M_BITMAPHANDLERDATA->m_ok = TRUE;

    return M_BITMAPHANDLERDATA->m_ok ;
}

class WXDLLEXPORT wxXBMDataHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXBMDataHandler)
public:
    inline wxXBMDataHandler()
    {
        m_name = "XBM data";
        m_extension = "xbm";
        m_type = wxBITMAP_TYPE_XBM_DATA;
    };

    virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
};
IMPLEMENT_DYNAMIC_CLASS(wxXBMDataHandler, wxBitmapHandler)

bool wxXBMDataHandler::Create( wxBitmap *bitmap, void *data, long WXUNUSED(flags), 
                               int width, int height, int WXUNUSED(depth))
{
    M_BITMAPHANDLERDATA->m_width = width;
    M_BITMAPHANDLERDATA->m_height = height;
    M_BITMAPHANDLERDATA->m_depth = 1;
    M_BITMAPHANDLERDATA->m_freePixmap = TRUE;

    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPHANDLERDATA->m_display = (WXDisplay*) dpy;

    M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) XCreateBitmapFromData (dpy, RootWindow (dpy, DefaultScreen (dpy)), (char*) data, width, height);
    M_BITMAPHANDLERDATA->m_ok = (M_BITMAPHANDLERDATA->m_pixmap != (WXPixmap) 0) ;

    // code for wxControl. TODO: can we avoid doing this until we need it?
    // E.g. have CreateButtonPixmaps which is called on demand.
    XImage* image = (XImage *) XtMalloc (sizeof (XImage));
    image->width = width;
    image->height = height;
    image->data = (char*) data;
    image->depth = 1;
    image->xoffset = 0;
    image->format = XYBitmap;
    image->byte_order = LSBFirst;
    image->bitmap_unit = 8;
    image->bitmap_bit_order = LSBFirst;
    image->bitmap_pad = 8;
    image->bytes_per_line = (width + 7) >> 3;

    char tmp[128];
    sprintf (tmp, "Im%x", (unsigned int) image);
    XmInstallImage (image, tmp);

    // Build our manually stipped pixmap.

    int bpl = (width + 7) / 8;
    char *data1 = new char[height * bpl];
    char* bits = (char*) data;
    int i;
    for (i = 0; i < height; i++)
    {
        int mask = i % 2 ? 0x55 : 0xaa;
        int j;
        for (j = 0; j < bpl; j++)
            data1[i * bpl + j] = bits[i * bpl + j] & mask;
    }
    XImage* insensImage = (XImage *) XtMalloc (sizeof (XImage));
    insensImage->width = width;
    insensImage->height = height;
    insensImage->data = data1;
    insensImage->depth = 1;
    insensImage->xoffset = 0;
    insensImage->format = XYBitmap;
    insensImage->byte_order = LSBFirst;
    insensImage->bitmap_unit = 8;
    insensImage->bitmap_bit_order = LSBFirst;
    insensImage->bitmap_pad = 8;
    insensImage->bytes_per_line = bpl;

    sprintf (tmp, "Not%x", (unsigned int)insensImage);
    XmInstallImage (insensImage, tmp);

    M_BITMAPHANDLERDATA->m_image = (WXImage*) image;
    M_BITMAPHANDLERDATA->m_insensImage = (WXImage*) insensImage;

    return TRUE;
}

#if wxUSE_XPM
class WXDLLEXPORT wxXPMFileHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXPMFileHandler)
public:
    inline wxXPMFileHandler()
    {
        m_name = "XPM file";
        m_extension = "xpm";
        m_type = wxBITMAP_TYPE_XPM;
    };

    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
          int desiredWidth, int desiredHeight);
    virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);
};

IMPLEMENT_DYNAMIC_CLASS(wxXPMFileHandler, wxBitmapHandler)

bool wxXPMFileHandler::LoadFile( wxBitmap *bitmap, const wxString& name, long WXUNUSED(flags),
                                 int WXUNUSED(desiredWidth), int WXUNUSED(desiredHeight) )
{
    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPHANDLERDATA->m_display = (WXDisplay*) dpy;

    XpmAttributes xpmAttr;
    Pixmap pixmap;
    Pixmap mask = 0;

    M_BITMAPHANDLERDATA->m_ok = FALSE;
    xpmAttr.valuemask = XpmReturnInfos | XpmCloseness;
    xpmAttr.closeness = 40000;
    int errorStatus = XpmReadFileToPixmap(dpy,
        RootWindow(dpy, DefaultScreen(dpy)), (char*) (const char*) name,
        &pixmap, &mask, &xpmAttr);

    if (errorStatus == XpmSuccess)
    {
        M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) pixmap;
        if ( mask )
        {
            M_BITMAPHANDLERDATA->m_bitmapMask = new wxMask;
            M_BITMAPHANDLERDATA->m_bitmapMask->SetPixmap((WXPixmap) mask);
        }

        unsigned int depthRet;
        int xRet, yRet;
        unsigned int widthRet, heightRet, borderWidthRet;
        Window rootWindowRet;
        XGetGeometry(dpy, pixmap, &rootWindowRet, &xRet, &yRet,
            &widthRet, &heightRet, &borderWidthRet, &depthRet);

        M_BITMAPHANDLERDATA->m_width = xpmAttr.width;
        M_BITMAPHANDLERDATA->m_height = xpmAttr.height;

	/*
        if ( xpmAttr.npixels > 2 )
        {
            M_BITMAPHANDLERDATA->m_depth = 8;	// TODO: next time not just a guess :-) ...
        } else
        {
            M_BITMAPHANDLERDATA->m_depth = 1;	// mono
        }
	*/

        M_BITMAPHANDLERDATA->m_depth = depthRet;

    	M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;

	    XpmFreeAttributes(&xpmAttr);

        M_BITMAPHANDLERDATA->m_ok = TRUE;
        return TRUE;
    } else
    {
//      XpmDebugError(errorStatus, name);
        M_BITMAPHANDLERDATA->m_ok = FALSE;
        return FALSE;
    }
}

bool wxXPMFileHandler::SaveFile( wxBitmap *bitmap, const wxString& name, int WXUNUSED(type), 
                                 const wxPalette *WXUNUSED(palette))
{
    if (M_BITMAPHANDLERDATA->m_ok && M_BITMAPHANDLERDATA->m_pixmap)
    {
        Display *dpy =  (Display*) M_BITMAPHANDLERDATA->m_display;
        int errorStatus = XpmWriteFileFromPixmap(dpy, (char*) (const char*) name,
          (Pixmap) M_BITMAPHANDLERDATA->m_pixmap,
          (M_BITMAPHANDLERDATA->m_bitmapMask ? (Pixmap) M_BITMAPHANDLERDATA->m_bitmapMask->GetPixmap() : (Pixmap) 0),
          (XpmAttributes *) NULL);
        if (errorStatus == XpmSuccess)
          return TRUE;
        else
          return FALSE;
    }
    else
        return FALSE;
}

class WXDLLEXPORT wxXPMDataHandler: public wxBitmapHandler
{
    DECLARE_DYNAMIC_CLASS(wxXPMDataHandler)
public:
    inline wxXPMDataHandler()
    {
        m_name = "XPM data";
        m_extension = "xpm";
        m_type = wxBITMAP_TYPE_XPM_DATA;
    };

    virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
};
IMPLEMENT_DYNAMIC_CLASS(wxXPMDataHandler, wxBitmapHandler)

bool wxXPMDataHandler::Create( wxBitmap *bitmap, void *data, long WXUNUSED(flags), 
                               int width, int height, int WXUNUSED(depth))
{
    M_BITMAPHANDLERDATA->m_width = width;
    M_BITMAPHANDLERDATA->m_height = height;
    M_BITMAPHANDLERDATA->m_depth = 1;
    M_BITMAPHANDLERDATA->m_freePixmap = TRUE;

    Display *dpy = (Display*) wxGetDisplay();
    M_BITMAPHANDLERDATA->m_display = (WXDisplay*) dpy;

    XpmAttributes xpmAttr;

    xpmAttr.valuemask = XpmReturnInfos;    /* nothing yet, but get infos back */

    XpmColorSymbol symbolicColors[4];
    if (sg_Control && sg_Control->GetMainWidget())
    {
        symbolicColors[0].name = "foreground";
        symbolicColors[0].value = NULL;
        symbolicColors[1].name = "background";
        symbolicColors[1].value = NULL;
        XtVaGetValues((Widget) sg_Control->GetMainWidget(),
            XmNforeground,  &symbolicColors[0].pixel,
            XmNbackground,  &symbolicColors[1].pixel,NULL);
        xpmAttr.numsymbols = 2;
        xpmAttr.colorsymbols = symbolicColors;
        xpmAttr.valuemask |= XpmColorSymbols;    // add flag
    }

    Pixmap pixmap;
    Pixmap mask = 0;
    int ErrorStatus = XpmCreatePixmapFromData(dpy, RootWindow(dpy, DefaultScreen(dpy)),
               (char**) data, &pixmap, &mask, &xpmAttr);
    if (ErrorStatus == XpmSuccess)
    {
        // Set attributes
        M_BITMAPHANDLERDATA->m_width = xpmAttr.width;
        M_BITMAPHANDLERDATA->m_height = xpmAttr.height;

        unsigned int depthRet;
        int xRet, yRet;
        unsigned int widthRet, heightRet, borderWidthRet;
        Window rootWindowRet;
        XGetGeometry(dpy, pixmap, &rootWindowRet, &xRet, &yRet,
            &widthRet, &heightRet, &borderWidthRet, &depthRet);

	/*
        if ( xpmAttr.npixels > 2 )
        {
            M_BITMAPHANDLERDATA->m_depth = 8;    // next time not just a guess :-) ...
        } else
        {
            M_BITMAPHANDLERDATA->m_depth = 1;    // mono
        }
	*/

        M_BITMAPHANDLERDATA->m_depth = depthRet;

        M_BITMAPHANDLERDATA->m_numColors = xpmAttr.npixels;
        XpmFreeAttributes(&xpmAttr);
        M_BITMAPHANDLERDATA->m_ok = TRUE;
        M_BITMAPHANDLERDATA->m_pixmap = (WXPixmap) pixmap;
        if ( mask )
        {
            M_BITMAPHANDLERDATA->m_bitmapMask = new wxMask;
            M_BITMAPHANDLERDATA->m_bitmapMask->SetPixmap((WXPixmap) mask);
        }
    }
    else
    {
//      XpmDebugError(ErrorStatus, NULL);
        M_BITMAPHANDLERDATA->m_ok = FALSE;
    }
    return M_BITMAPHANDLERDATA->m_ok ;
}

#endif

void wxBitmap::CleanUpHandlers()
{
    wxNode *node = sm_handlers.First();
    while ( node )
    {
        wxBitmapHandler *handler = (wxBitmapHandler *)node->Data();
        wxNode *next = node->Next();
        delete handler;
        delete node;
        node = next;
    }
}

void wxBitmap::InitStandardHandlers()
{
    // Initialize all standard bitmap or derived class handlers here.
    AddHandler(new wxXBMFileHandler);
    AddHandler(new wxXBMDataHandler);

    // XPM is considered standard for Moif, although it can be omitted if absolutely
    // necessary.
#if wxUSE_XPM
    AddHandler(new wxXPMFileHandler);
    AddHandler(new wxXPMDataHandler);
#endif
}

WXPixmap wxBitmap::GetLabelPixmap (WXWidget w)
{
  if (M_BITMAPDATA->m_image == (WXPixmap) 0)
    return M_BITMAPDATA->m_pixmap;

  Display *dpy = (Display*) M_BITMAPDATA->m_display;

#ifdef FOO
/*
   If we do:
   if (labelPixmap) return labelPixmap;
   things can be wrong, because colors can have been changed.

   If we do:
   if (labelPixmap)
   XmDestroyPixmap(DefaultScreenOfDisplay(dpy),labelPixmap) ;
   we got BadDrawable if the pixmap is referenced by multiples widgets

   this is a catch22!!

   So, before doing thing really clean, I just do nothing; if the pixmap is
   referenced by many widgets, Motif performs caching functions.
   And if pixmap is referenced with multiples colors, we just have some
   memory leaks... I hope we can deal with them...
 */
  // Must be destroyed, because colours can have been changed!
  if (M_BITMAPDATA->m_labelPixmap)
    XmDestroyPixmap (DefaultScreenOfDisplay (dpy), M_BITMAPDATA->m_labelPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int) M_BITMAPDATA->m_image);

  Pixel fg, bg;
  Widget widget = (Widget) w;

  while (XmIsGadget ( widget ))
    widget = XtParent (widget);
  XtVaGetValues (widget, XmNbackground, &bg, XmNforeground, &fg, NULL);

  M_BITMAPDATA->m_labelPixmap = (WXPixmap) XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return M_BITMAPDATA->m_labelPixmap;
}

WXPixmap wxBitmap::GetArmPixmap (WXWidget w)
{
  if (M_BITMAPDATA->m_image == (WXPixmap) 0)
    return M_BITMAPDATA->m_pixmap;

  Display *dpy = (Display*) M_BITMAPDATA->m_display;
#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (M_BITMAPDATA->m_armPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), M_BITMAPDATA->m_armPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Im%x", (unsigned int) M_BITMAPDATA->m_image);

  Pixel fg, bg;
  Widget widget = (Widget) w;

  XtVaGetValues (widget, XmNarmColor, &bg, NULL);
  while (XmIsGadget (widget))
    widget = XtParent (widget);
  XtVaGetValues (widget, XmNforeground, &fg, NULL);

  M_BITMAPDATA->m_armPixmap = (WXPixmap) XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return M_BITMAPDATA->m_armPixmap;
}

WXPixmap wxBitmap::GetInsensPixmap (WXWidget w)
{
  Display *dpy = (Display*) M_BITMAPDATA->m_display;

  if (M_BITMAPDATA->m_insensPixmap)
    return M_BITMAPDATA->m_insensPixmap;

  if (!w)
  {
    M_BITMAPDATA->m_insensPixmap = (WXPixmap) XCreateInsensitivePixmap(dpy, (Pixmap) M_BITMAPDATA->m_pixmap);
    if (M_BITMAPDATA->m_insensPixmap)
      return M_BITMAPDATA->m_insensPixmap;
    else
      return M_BITMAPDATA->m_pixmap;
  }

  if (M_BITMAPDATA->m_insensImage == (WXPixmap) 0)
    return M_BITMAPDATA->m_pixmap;

#ifdef FOO
  See GetLabelPixmap () comment
  // Must be destroyed, because colours can have been changed!
  if (M_BITMAPDATA->m_insensPixmap)
      XmDestroyPixmap (DefaultScreenOfDisplay (dpy), (Pixmap) M_BITMAPDATA->m_insensPixmap);
#endif

  char tmp[128];
  sprintf (tmp, "Not%x", (unsigned int) M_BITMAPDATA->m_insensImage);

  Pixel fg, bg;
  Widget widget = (Widget) w;

  while (XmIsGadget (widget))
    widget = XtParent (widget);
  XtVaGetValues (widget, XmNbackground, &bg, XmNforeground, &fg, NULL);

  M_BITMAPDATA->m_insensPixmap = (WXPixmap) XmGetPixmap (DefaultScreenOfDisplay (dpy), tmp, fg, bg);

  return M_BITMAPDATA->m_insensPixmap;
}

// We may need this sometime...

/****************************************************************************

NAME
    XCreateInsensitivePixmap - create a grayed-out copy of a pixmap

SYNOPSIS
    Pixmap XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

DESCRIPTION
    This function creates a grayed-out copy of the argument pixmap, suitable
    for use as a XmLabel's XmNlabelInsensitivePixmap resource.

RETURN VALUES
    The return value is the new Pixmap id or zero on error.  Errors include
    a NULL display argument or an invalid Pixmap argument.

ERRORS
    If one of the XLib functions fail, it will produce a X error.  The
    default X error handler prints a diagnostic and calls exit().

SEE ALSO
    XCopyArea(3), XCreateBitmapFromData(3), XCreateGC(3), XCreatePixmap(3),
    XFillRectangle(3), exit(2)

AUTHOR
    John R Veregge - john@puente.jpl.nasa.gov
    Advanced Engineering and Prototyping Group (AEG)
    Information Systems Technology Section (395)
    Jet Propulsion Lab - Calif Institute of Technology

*****************************************************************************/

Pixmap
XCreateInsensitivePixmap( Display *display, Pixmap pixmap )

{
static
    char    stipple_data[] = 
        {
            0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
            0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
            0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA,
            0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA
        };
    GC        gc;
    Pixmap    ipixmap, stipple;
    unsigned    width, height, depth;

    Window    window;    /* These return values */
    unsigned    border;    /* from XGetGeometry() */
    int        x, y;    /* are not needed.     */

    ipixmap = 0;

    if ( NULL == display || 0 == pixmap )
    return ipixmap;

    if ( 0 == XGetGeometry( display, pixmap, &window, &x, &y,
                &width, &height, &border, &depth )
       )
    return ipixmap; /* BadDrawable: probably an invalid pixmap */

    /* Get the stipple pixmap to be used to 'gray-out' the argument pixmap.
    */
    stipple = XCreateBitmapFromData( display, pixmap, stipple_data, 16, 16 );
    if ( 0 != stipple )
    {
    gc = XCreateGC( display, pixmap, (XtGCMask)0, (XGCValues*)NULL );
    if ( NULL != gc )
    {
        /* Create an identical copy of the argument pixmap.
        */
        ipixmap = XCreatePixmap( display, pixmap, width, height, depth );
        if ( 0 != ipixmap )
        {
        /* Copy the argument pixmap into the new pixmap.
        */
        XCopyArea( display, pixmap, ipixmap,
               gc, 0, 0, width, height, 0, 0 );

        /* Refill the new pixmap using the stipple algorithm/pixmap.
        */
        XSetStipple( display, gc, stipple );
        XSetFillStyle( display, gc, FillStippled );
        XFillRectangle( display, ipixmap, gc, 0, 0, width, height );
        }
        XFreeGC( display, gc );
    }
    XFreePixmap( display, stipple );
    }
    return ipixmap;
}

// Creates a bitmap with transparent areas drawn in
// the given colour.
wxBitmap wxCreateMaskedBitmap(wxBitmap& bitmap, wxColour& colour)
{
    wxBitmap newBitmap(bitmap.GetWidth(),
                       bitmap.GetHeight(),
                       bitmap.GetDepth());
    wxMemoryDC destDC;
    wxMemoryDC srcDC;
    srcDC.SelectObject(bitmap);
    destDC.SelectObject(newBitmap);

    wxBrush brush(colour, wxSOLID);
    destDC.SetOptimization(FALSE);
    destDC.SetBackground(brush);
    destDC.Clear();
    destDC.Blit(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), & srcDC, 0, 0, wxCOPY, TRUE);

    return newBitmap;
}
