///////////////////////////////////////////////////////////////////////////////
// Name:        dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Julian Smart
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DATAOBJ_H_
#define _WX_DATAOBJ_H_

#ifdef __GNUG__
#pragma interface "dataobj.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/bitmap.h"

//-------------------------------------------------------------------------
// classes
//-------------------------------------------------------------------------

class wxDataFormat;
class wxDataObject;
class wxTextDataObject;

//-------------------------------------------------------------------------
// wxDataFormat
//-------------------------------------------------------------------------

class wxDataFormat : public wxObject
{
    DECLARE_CLASS( wxDataFormat )

public:
    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( const wxChar *id );
    wxDataFormat( const wxDataFormat &format );
    wxDataFormat( const Atom atom );

    void SetType( wxDataFormatId type );
    wxDataFormatId GetType() const;

    /* the string Id identifies the format of clipboard or DnD data. a word
     * processor would e.g. add a wxTextDataObject and a wxPrivateDataObject
     * to the clipboard - the latter with the Id "application/wxword", an
     * image manipulation program would put a wxBitmapDataObject and a
     * wxPrivateDataObject to the clipboard - the latter with "image/png". */

    wxString GetId() const;
    void SetId( const wxChar *id );

    Atom GetAtom();
    void SetAtom(Atom atom) { m_hasAtom = TRUE; m_atom = atom; }

    // implicit conversion to wxDataFormatId
    operator wxDataFormatId() const { return m_type; }

    bool operator==(wxDataFormatId type) const { return m_type == type; }
    bool operator!=(wxDataFormatId type) const { return m_type != type; }

private:
    wxDataFormatId  m_type;
    wxString    m_id;
    bool        m_hasAtom;
    Atom        m_atom;
};

//----------------------------------------------------------------------------
// wxDataObject to be placed in wxDataBroker
//----------------------------------------------------------------------------

class wxDataObject : public wxObject
{
  DECLARE_DYNAMIC_CLASS( wxDataObject )

public:

  /* constructor */
  wxDataObject();

  /* destructor */
  ~wxDataObject();

  /* write data to dest */
  virtual void WriteData( void *dest ) const = 0;

  /* get size of data */
  virtual size_t GetSize() const = 0;

  /* implementation */

  wxDataFormat &GetFormat();

  wxDataFormatId GetFormatType() const;
  wxString   GetFormatId() const;
  Atom    GetFormatAtom() const;

  wxDataFormat m_format;
};

//----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
//----------------------------------------------------------------------------

class wxTextDataObject : public wxDataObject
{
  DECLARE_DYNAMIC_CLASS( wxTextDataObject )

public:

  /* default constructor. call SetText() later or override
     WriteData() and GetSize() for working on-demand */
  wxTextDataObject();

  /* constructor */
  wxTextDataObject( const wxString& data );

  /* set current text data */
  void SetText( const wxString& data );

  /* get current text data */
  wxString GetText() const;

  /* by default calls WriteString() with string set by constructor or
     by SetText(). can be overridden for working on-demand */
  virtual void WriteData( void *dest ) const;

  /* by default, returns length of string as set by constructor or
     by SetText(). can be overridden for working on-demand */
  virtual size_t GetSize() const;

  /* write string to dest */
  void WriteString( const wxString &str, void *dest ) const;

  /* implementation */

  wxString  m_data;
};


#endif  
       //_WX_DATAOBJ_H_

