/////////////////////////////////////////////////////////////////////////////
// Name:        metafile.h
// Purpose:     wxMetaFile, wxMetaFileDC classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////


#ifndef __METAFIILEH__
#define __METAFIILEH__

#ifdef __GNUG__
#pragma interface "metafile.h"
#endif

#include "wx/setup.h"

#if USE_METAFILE
#include "wx/dc.h"

/*
 * Metafile and metafile device context classes - work in Windows 3.1 only
 *
 */

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxMetaFile: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxMetaFile)
 public:
  wxMetaFile(const wxString& file = "");
  ~wxMetaFile(void);

  // After this is called, the metafile cannot be used for anything
  // since it is now owned by the clipboard.
  virtual bool SetClipboard(int width = 0, int height = 0);

  virtual bool Play(wxDC *dc);
  inline bool Ok(void) { return m_metaFile != 0; };

  // Implementation
  inline WXHANDLE GetHMETAFILE(void) { return m_metaFile; }
  inline void SetHMETAFILE(WXHANDLE mf) { m_metaFile = mf; }
  inline int GetWindowsMappingMode(void) { return m_windowsMappingMode; }
  inline void SetWindowsMappingMode(int mm) { m_windowsMappingMode = mm; }

protected:
  WXHANDLE m_metaFile;
  int m_windowsMappingMode;
};

class WXDLLEXPORT wxMetaFileDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxMetaFileDC)

 public:
  // Don't supply origin and extent
  // Supply them to wxMakeMetaFilePlaceable instead.
  wxMetaFileDC(const wxString& file = "");

  // Supply origin and extent (recommended).
  // Then don't need to supply them to wxMakeMetaFilePlaceable.
  wxMetaFileDC(const wxString& file, int xext, int yext, int xorg, int yorg);

  ~wxMetaFileDC(void);

  // Should be called at end of drawing
  virtual wxMetaFile *Close(void);
  virtual void SetMapMode(int mode);
  virtual void GetTextExtent(const wxString& string, float *x, float *y,
                     float *descent = NULL, float *externalLeading = NULL,
                     wxFont *theFont = NULL, bool use16bit = FALSE);

  // Implementation
  inline wxMetaFile *GetMetaFile(void) { return m_metaFile; }
  inline void SetMetaFile(wxMetaFile *mf) { m_metaFile = mf; }
  inline int GetWindowsMappingMode(void) { return m_windowsMappingMode; }
  inline void SetWindowsMappingMode(int mm) { m_windowsMappingMode = mm; }

protected:
  int m_windowsMappingMode;
  wxMetaFile *m_metaFile;
};

/*
 * Pass filename of existing non-placeable metafile, and bounding box.
 * Adds a placeable metafile header, sets the mapping mode to anisotropic,
 * and sets the window origin and extent to mimic the MM_TEXT mapping mode.
 *
 */

// No origin or extent
bool WXDLLEXPORT wxMakeMetaFilePlaceable(const wxString& filename, float scale = 1.0);

// Optional origin and extent
bool WXDLLEXPORT wxMakeMetaFilePlaceable(const wxString& filename, int x1, int y1, int x2, int y2, float scale = 1.0, bool useOriginAndExtent = TRUE);

#endif // USE_METAFILE
#endif
    // __METAFIILEH__
