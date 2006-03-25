///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     Declaration of the wxDropTarget, wxDropSource class etc.
// Author:      AUTHOR
// RCS-ID:      $Id$
// Copyright:   (c) 1998 AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DND_H_
#define _WX_DND_H_

#ifdef __GNUG__
#pragma interface "dnd.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxDataObject;
class WXDLLEXPORT wxTextDataObject;
class WXDLLEXPORT wxFileDataObject;

class WXDLLEXPORT wxDropTarget;
class WXDLLEXPORT wxTextDropTarget;
class WXDLLEXPORT wxFileDropTarget;

class WXDLLEXPORT wxDropSource;

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject: public wxObject
{
public:
  // all data formats (values are the same as in windows.h, do not change!)
  enum StdFormat
  {
    Invalid,
    Text,
    Bitmap,
    MetafilePict,
    Sylk,
    Dif,
    Tiff,
    OemText,
    Dib,
    Palette,
    Pendata,
    Riff,
    Wave,
    UnicodeText,
    EnhMetafile,
    Hdrop,
    Locale,
    Max
  };

  // function to return symbolic name of clipboard format (debug messages)
  static const char *GetFormatName(wxDataFormat format);

  // ctor & dtor
  wxDataObject() {};
  ~wxDataObject() {};

  // pure virtuals to override
    // get the best suited format for our data
  virtual wxDataFormat GetPreferredFormat() const = 0;
    // decide if we support this format (should be one of values of
    // StdFormat enumerations or a user-defined format)
  virtual bool IsSupportedFormat(wxDataFormat format) const = 0;
    // get the (total) size of data
  virtual size_t GetDataSize() const = 0;
    // copy raw data to provided pointer
  virtual void GetDataHere(void *pBuf) const = 0;

};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTextDataObject : public wxDataObject
{
public:
  // ctors
  wxTextDataObject() { }
  wxTextDataObject(const wxString& strText) : m_strText(strText) { }
  void Init(const wxString& strText) { m_strText = strText; }

  // implement base class pure virtuals
  virtual wxDataFormat GetPreferredFormat() const
    { return wxDF_TEXT; }
  virtual bool IsSupportedFormat(wxDataFormat format) const
    { return format == wxDF_TEXT; }
  virtual size_t GetDataSize() const
    { return m_strText.Len() + 1; } // +1 for trailing '\0'of course
  virtual void GetDataHere(void *pBuf) const
    { memcpy(pBuf, m_strText.c_str(), GetDataSize()); }

private:
  wxString  m_strText;
  
};

// ----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileDataObject : public wxDataObject
{
public:

  wxFileDataObject(void) { }
  void AddFile( const wxString &file )
    { m_files += file; m_files += ";"; }

  // implement base class pure virtuals
  virtual wxDataFormat GetPreferredFormat() const
    { return wxDF_FILENAME; }
  virtual bool IsSupportedFormat(wxDataFormat format) const
    { return format == wxDF_FILENAME; }
  virtual size_t GetDataSize() const
    { return m_files.Len() + 1; } // +1 for trailing '\0'of course
  virtual void GetDataHere(void *pBuf) const
    { memcpy(pBuf, m_files.c_str(), GetDataSize()); }

private:
  wxString  m_files;
  
};
//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxDropTarget: public wxObject
{
  public:

    wxDropTarget();
    ~wxDropTarget();
    
    virtual void OnEnter() { }
    virtual void OnLeave() { }
    virtual bool OnDrop( long x, long y, const void *pData ) = 0;

//  protected:
      
    friend wxWindow;
    
    // Override these to indicate what kind of data you support: 
  
    virtual size_t GetFormatCount() const = 0;
    virtual wxDataFormat GetFormat(size_t n) const = 0;
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class WXDLLEXPORT wxTextDropTarget: public wxDropTarget
{
  public:

    wxTextDropTarget() {};
    virtual bool OnDrop( long x, long y, const void *pData );
    virtual bool OnDropText( long x, long y, const char *psz );
    
  protected:
  
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFileDropTarget: public wxDropTarget
{
  public:
    
    wxFileDropTarget() {};
    
    virtual bool OnDrop(long x, long y, const void *pData);
    virtual bool OnDropFiles( long x, long y, 
                              size_t nFiles, const char * const aszFiles[]);

  protected:
  
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

enum wxDragResult
  {
    wxDragError,    // error prevented the d&d operation from completing
    wxDragNone,     // drag target didn't accept the data
    wxDragCopy,     // the data was successfully copied
    wxDragMove,     // the data was successfully moved
    wxDragCancel    // the operation was cancelled by user (not an error)
  };

class WXDLLEXPORT wxDropSource: public wxObject
{
  public:

    wxDropSource( wxWindow *win );
    wxDropSource( wxDataObject &data, wxWindow *win );
    
    ~wxDropSource(void);
    
    void SetData( wxDataObject &data  );
    wxDragResult DoDragDrop( bool bAllowMove = FALSE );
    
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; };

  protected:
  
    wxDataObject  *m_data;
};

#endif  
       //_WX_DND_H_

