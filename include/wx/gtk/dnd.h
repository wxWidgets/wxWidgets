///////////////////////////////////////////////////////////////////////////////
// Name:        dnd.h
// Purpose:     declaration of the wxDropTarget class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin, Robert Roebling
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDNDH__
#define __GTKDNDH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/cursor.h"

//-------------------------------------------------------------------------
// conditional compilation
//-------------------------------------------------------------------------

#if (GTK_MINOR_VERSION == 1)
#if (GTK_MICRO_VERSION >= 3)
#define NEW_GTK_DND_CODE
#endif
#endif

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxWindow;

class wxDataObject;
class wxTextDataObject;
class wxFileDataObject;

class wxDropTarget;
class wxTextDropTarget;
class wxFileDropTarget;

class wxDropSource;

//-------------------------------------------------------------------------
// wxDataObject
//-------------------------------------------------------------------------

class wxDataObject: public wxObject
{
public:

  wxDataObject() {};
  ~wxDataObject() {};

  virtual wxDataFormat GetPreferredFormat() const = 0;
  virtual bool IsSupportedFormat( wxDataFormat format ) const = 0;
  virtual size_t GetDataSize() const = 0;
  virtual void GetDataHere( void *data ) const = 0;

};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
public:

  wxTextDataObject() { }
  wxTextDataObject(const wxString& strText) : m_strText(strText) { }
  void Init(const wxString& strText) { m_strText = strText; }

  virtual wxDataFormat GetPreferredFormat() const
    { return wxDF_TEXT; }
    
  virtual bool IsSupportedFormat(wxDataFormat format) const
    { return format == wxDF_TEXT; }

  virtual size_t GetDataSize() const
    { return m_strText.Len() + 1; } // +1 for trailing '\0'
    
  virtual void GetDataHere( void *data ) const
    { memcpy(data, m_strText.c_str(), GetDataSize()); }

private:
  wxString  m_strText;
  
};

// ----------------------------------------------------------------------------
// wxFileDataObject is a specialization of wxDataObject for file names
// ----------------------------------------------------------------------------

class wxFileDataObject : public wxDataObject
{
public:

  wxFileDataObject(void) { }
  void AddFile( const wxString &file )
    { m_files += file; m_files += '\0'; }

  virtual wxDataFormat GetPreferredFormat() const
    { return wxDF_FILENAME; }
    
  virtual bool IsSupportedFormat( wxDataFormat format ) const
    { return format == wxDF_FILENAME; }
    
  virtual size_t GetDataSize() const
    { return m_files.Len(); } // no trailing '\0'
    
  virtual void GetDataHere( void *data ) const
    { memcpy(data, m_files.c_str(), GetDataSize()); }

private:
  wxString  m_files;
  
};
//-------------------------------------------------------------------------
// wxDropTarget
//-------------------------------------------------------------------------

class wxDropTarget: public wxObject
{
  public:

    wxDropTarget();
    ~wxDropTarget();
    
    virtual void OnEnter() { }
    virtual void OnLeave() { }
    virtual bool OnDrop( long x, long y, const void *data, size_t size ) = 0;

    // Override these to indicate what kind of data you support: 
  
    virtual size_t GetFormatCount() const = 0;
    virtual wxDataFormat GetFormat(size_t n) const = 0;
  
  // implementation
  
    void RegisterWidget( GtkWidget *widget );
    void UnregisterWidget( GtkWidget *widget );
};

//-------------------------------------------------------------------------
// wxTextDropTarget
//-------------------------------------------------------------------------

class wxTextDropTarget: public wxDropTarget
{
  public:

    wxTextDropTarget() {};
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropText( long x, long y, const char *psz );
    
  protected:
  
    virtual size_t GetFormatCount() const;
    virtual wxDataFormat GetFormat(size_t n) const;
};

// ----------------------------------------------------------------------------
// A drop target which accepts files (dragged from File Manager or Explorer)
// ----------------------------------------------------------------------------

class wxFileDropTarget: public wxDropTarget
{
  public:
    
    wxFileDropTarget() {};
    
    virtual bool OnDrop( long x, long y, const void *data, size_t size );
    virtual bool OnDropFiles( long x, long y, 
                              size_t nFiles, const char * const aszFiles[] );

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

class wxDropSource: public wxObject
{
  public:

    wxDropSource( wxWindow *win );
    wxDropSource( wxDataObject &data, wxWindow *win );
    
    ~wxDropSource(void);
    
    void SetData( wxDataObject &data  );
    wxDragResult DoDragDrop( bool bAllowMove = FALSE );
    
    virtual bool GiveFeedback( wxDragResult WXUNUSED(effect), bool WXUNUSED(bScrolling) ) { return TRUE; };
  
  // implementation
      
    void RegisterWindow(void);
    void UnregisterWindow(void);
  
    GtkWidget     *m_widget;
    wxWindow      *m_window;
    wxDragResult   m_retValue;
    wxDataObject  *m_data;
    
    wxCursor      m_defaultCursor;
    wxCursor      m_goaheadCursor;
};

#endif  
       //__GTKDNDH__

