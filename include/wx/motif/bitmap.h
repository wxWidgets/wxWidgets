/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:     wxBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_
#define _WX_BITMAP_H_

#ifdef __GNUG__
#pragma interface "bitmap.h"
#endif

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"
#include "wx/palette.h"

// Bitmap
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxControl;
class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBitmapHandler;
class WXDLLEXPORT wxIcon;
class WXDLLEXPORT wxCursor;

// A mask is a mono bitmap used for drawing bitmaps
// transparently.
class WXDLLEXPORT wxMask: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxMask)

public:
  wxMask();

  // Construct a mask from a bitmap and a colour indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, const wxColour& colour);

  // Construct a mask from a bitmap and a palette index indicating
  // the transparent area
  wxMask(const wxBitmap& bitmap, int paletteIndex);

  // Construct a mask from a mono bitmap (copies the bitmap).
  wxMask(const wxBitmap& bitmap);

  ~wxMask();

  bool Create(const wxBitmap& bitmap, const wxColour& colour);
  bool Create(const wxBitmap& bitmap, int paletteIndex);
  bool Create(const wxBitmap& bitmap);

  WXPixmap GetPixmap() const { return m_pixmap; }
  void SetPixmap(WXPixmap pixmap) { m_pixmap = pixmap; }

protected:
  WXPixmap      m_pixmap;
};

class WXDLLEXPORT wxBitmapRefData: public wxGDIRefData
{
    friend class WXDLLEXPORT wxBitmap;
    friend class WXDLLEXPORT wxIcon;
    friend class WXDLLEXPORT wxCursor;
public:
    wxBitmapRefData();
    ~wxBitmapRefData();

public:
  int               m_width;
  int               m_height;
  int               m_depth;
  bool              m_ok;
  int               m_numColors;
  wxPalette         m_bitmapPalette;
  int               m_quality;

  wxMask *          m_bitmapMask; // Optional mask

  // Motif implementation
public:
  WXPixmap          m_pixmap;
  WXDisplay*        m_display;
  bool              m_freePixmap;
  unsigned long*    m_freeColors;
  long              m_freeColorsCount;

  // These 5 variables are for wxControl
  WXPixmap          m_insensPixmap ;
  WXPixmap          m_labelPixmap ;
  WXPixmap          m_armPixmap ;
  WXImage*          m_image ;
  WXImage*          m_insensImage ;
};

#define M_BITMAPDATA ((wxBitmapRefData *)m_refData)

class WXDLLEXPORT wxBitmapHandler: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
public:
  wxBitmapHandler() { m_name = ""; m_extension = ""; m_type = 0; };

  virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
  virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
      int desiredWidth, int desiredHeight);
  virtual bool SaveFile(wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);

  void SetName(const wxString& name) { m_name = name; }
  void SetExtension(const wxString& ext) { m_extension = ext; }
  void SetType(long type) { m_type = type; }
  wxString GetName() const { return m_name; }
  wxString GetExtension() const { return m_extension; }
  long GetType() const { return m_type; }
protected:
  wxString  m_name;
  wxString  m_extension;
  long      m_type;
};

#define M_BITMAPHANDLERDATA ((wxBitmapRefData *)bitmap->GetRefData())

class WXDLLEXPORT wxBitmap: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxBitmap)

  friend class WXDLLEXPORT wxBitmapHandler;

public:
  wxBitmap(); // Platform-specific

  // Copy constructors
  wxBitmap(const wxBitmap& bitmap)
  { Ref(bitmap); if ( wxTheBitmapList ) wxTheBitmapList->AddBitmap(this); }

  // Initialize with raw XBM data
  wxBitmap(const char bits[], int width, int height, int depth = 1);

  // from XPM
  wxBitmap(const char **data) { (void)CreateFromXpm(data); }
  wxBitmap(char **data) { (void)CreateFromXpm((const char **)data); }

  // Initialize with XPM data -- deprecated
  wxBitmap(char **data, wxControl* control);

  // Load a file or resource
  wxBitmap(const wxString& name, long type = wxBITMAP_TYPE_XPM);

  // Constructor for generalised creation from data
  wxBitmap(void *data, long type, int width, int height, int depth = 1);

  // If depth is omitted, will create a bitmap compatible with the display
  wxBitmap(int width, int height, int depth = -1);
  ~wxBitmap();

  virtual bool Create(int width, int height, int depth = -1);
  virtual bool Create(void *data, long type, int width, int height, int depth = 1);
   
   wxBitmap GetSubBitmap( const wxRect& rect ) const;
   
  virtual bool LoadFile(const wxString& name, long type = wxBITMAP_TYPE_XPM);
  virtual bool SaveFile(const wxString& name, int type, const wxPalette *cmap = NULL);

  bool Ok() const { return (M_BITMAPDATA && M_BITMAPDATA->m_ok); }
  int GetWidth() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_width : 0); }
  int GetHeight() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_height : 0); }
  int GetDepth() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_depth : 0); }
  int GetQuality() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_quality : 0); }
  void SetWidth(int w);
  void SetHeight(int h);
  void SetDepth(int d);
  void SetQuality(int q);
  void SetOk(bool isOk);

  wxPalette* GetPalette() const { return (M_BITMAPDATA ? (& M_BITMAPDATA->m_bitmapPalette) : (wxPalette*) NULL); }
  void SetPalette(const wxPalette& palette);

  wxMask *GetMask() const { return (M_BITMAPDATA ? M_BITMAPDATA->m_bitmapMask : (wxMask*) NULL); }
  void SetMask(wxMask *mask) ;

  wxBitmap& operator = (const wxBitmap& bitmap) { if (*this == bitmap) return (*this); Ref(bitmap); return *this; }
  bool operator == (const wxBitmap& bitmap) { return m_refData == bitmap.m_refData; }
  bool operator != (const wxBitmap& bitmap) { return m_refData != bitmap.m_refData; }

  // Format handling
  static wxList& GetHandlers() { return sm_handlers; }
  static void AddHandler(wxBitmapHandler *handler);
  static void InsertHandler(wxBitmapHandler *handler);
  static bool RemoveHandler(const wxString& name);
  static wxBitmapHandler *FindHandler(const wxString& name);
  static wxBitmapHandler *FindHandler(const wxString& extension, long bitmapType);
  static wxBitmapHandler *FindHandler(long bitmapType);

  static void InitStandardHandlers();
  static void CleanUpHandlers();

  // Motif implementation
public:
  WXDisplay* GetDisplay() const { return M_BITMAPDATA->m_display; }
  WXPixmap GetPixmap() const { return (WXPixmap) M_BITMAPDATA->m_pixmap; }
  virtual WXPixmap GetLabelPixmap(WXWidget w) ;
  virtual WXPixmap GetArmPixmap(WXWidget w) ;
  virtual WXPixmap GetInsensPixmap(WXWidget w = (WXWidget) 0) ;
  void SetPixmapNull() { M_BITMAPDATA->m_pixmap = 0; }

protected:
  static wxList sm_handlers;

protected:
    bool CreateFromXpm(const char **bits);
};

// Creates a bitmap with transparent areas drawn in
// the given colour.
wxBitmap wxCreateMaskedBitmap(const wxBitmap& bitmap, wxColour& colour);

#endif
  // _WX_BITMAP_H_
