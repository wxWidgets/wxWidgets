/////////////////////////////////////////////////////////////////////////////
// Name:        metafile.h
// Purpose:     wxMetaFile, wxMetaFileDC classes.
//              This probably should be restricted to Windows platforms,
//              but if there is an equivalent on your platform, great.
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_METAFIILE_H_
#define _WX_METAFIILE_H_

#include "wx/dc.h"
#include "wx/gdiobj.h"

#if wxUSE_DATAOBJ
#include "wx/dataobj.h"
#endif

/*
 * Metafile and metafile device context classes
 *
 */

#define wxMetaFile wxMetafile
#define wxMetaFileDC wxMetafileDC

class WXDLLEXPORT wxMetafile;
class wxMetafileRefData ;

#define M_METAFILEDATA ((wxMetafileRefData *)m_refData)

class WXDLLEXPORT wxMetafile: public wxGDIObject
{
    DECLARE_DYNAMIC_CLASS(wxMetafile)
public:
    wxMetafile(const wxString& file = wxEmptyString);
    virtual ~wxMetafile(void);

    // After this is called, the metafile cannot be used for anything
    // since it is now owned by the clipboard.
    virtual bool SetClipboard(int width = 0, int height = 0);

    virtual bool Play(wxDC *dc);
    bool Ok() const ;

    wxSize GetSize() const;
    int GetWidth() const { return GetSize().x; }
    int GetHeight() const { return GetSize().y; }

    // Implementation
    WXHMETAFILE GetHMETAFILE() const ;
    void SetHMETAFILE(WXHMETAFILE mf) ;

    // Operators
    inline bool operator == (const wxMetafile& metafile) const { return m_refData == metafile.m_refData; }
    inline bool operator != (const wxMetafile& metafile) const { return m_refData != metafile.m_refData; }

protected:
};

class WXDLLEXPORT wxMetafileDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxMetafileDC)

 public:
    // the ctor parameters specify the filename (empty for memory metafiles),
    // the metafile picture size and the optional description/comment
    wxMetafileDC(const wxString& filename = wxEmptyString,
                    int width = 0, int height = 0,
                    const wxString& description = wxEmptyString);

  virtual ~wxMetafileDC(void);

  // Should be called at end of drawing
  virtual wxMetafile *Close(void);

  // Implementation
  inline wxMetafile *GetMetaFile(void) const { return m_metaFile; }
  inline void SetMetaFile(wxMetafile *mf) { m_metaFile = mf; }

protected:
    virtual void DoGetSize(int *width, int *height) const;

  wxMetafile*   m_metaFile;
};

/*
 * Pass filename of existing non-placeable metafile, and bounding box.
 * Adds a placeable metafile header, sets the mapping mode to anisotropic,
 * and sets the window origin and extent to mimic the wxMM_TEXT mapping mode.
 *
 */

// No origin or extent
#define wxMakeMetaFilePlaceable wxMakeMetafilePlaceable
bool WXDLLEXPORT wxMakeMetafilePlaceable(const wxString& filename, float scale = 1.0);

// Optional origin and extent
bool WXDLLEXPORT wxMakeMetaFilePlaceable(const wxString& filename, int x1, int y1, int x2, int y2, float scale = 1.0, bool useOriginAndExtent = TRUE);

// ----------------------------------------------------------------------------
// wxMetafileDataObject is a specialization of wxDataObject for metafile data
// ----------------------------------------------------------------------------

#if wxUSE_DATAOBJ
class WXDLLEXPORT wxMetafileDataObject : public wxDataObjectSimple
{
public:
  // ctors
  wxMetafileDataObject() 
    : wxDataObjectSimple(wxDF_METAFILE) {  };
  wxMetafileDataObject(const wxMetafile& metafile)
    : wxDataObjectSimple(wxDF_METAFILE), m_metafile(metafile) { }

    // virtual functions which you may override if you want to provide data on
    // demand only - otherwise, the trivial default versions will be used
    virtual void SetMetafile(const wxMetafile& metafile)
        { m_metafile = metafile; }
    virtual wxMetafile GetMetafile() const
        { return m_metafile; }

    // implement base class pure virtuals
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *buf) const;
    virtual bool SetData(size_t len, const void *buf);

    virtual size_t GetDataSize(const wxDataFormat& WXUNUSED(format)) const
        { return GetDataSize(); }
    virtual bool GetDataHere(const wxDataFormat& WXUNUSED(format),
                             void *buf) const
        { return GetDataHere(buf); }
    virtual bool SetData(const wxDataFormat& WXUNUSED(format),
                         size_t len, const void *buf)
        { return SetData(len, buf); }
protected:
  wxMetafile   m_metafile;
};
#endif

#endif
    // _WX_METAFIILE_H_
