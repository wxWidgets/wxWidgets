///////////////////////////////////////////////////////////////////////////////
// Name:        ole/dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _OLEDATAOBJ_H
#define   _OLEDATAOBJ_H

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
struct IDataObject;

// ----------------------------------------------------------------------------
// wxDataObject is a "smart" and polymorphic piece of data.
//
// @@@ it's currently "read-only" from COM point of view, i.e. we don't support
//     SetData. We don't support all advise functions neither (but it's easy to
//     do if we really want them)
// ----------------------------------------------------------------------------

class wxDataObject
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

  #ifdef  __DEBUG__
    // function to return symbolic name of clipboard format (debug messages)
    static const char *GetFormatName(wxDataFormat format);
  #else // not used in release mode
    inline const char* GetFormatName(wxDataFormat format) { return ""; }
  #endif

  // ctor & dtor
  wxDataObject();
  ~wxDataObject();

  // pure virtuals to override
    // get the best suited format for our data
  virtual wxDataFormat GetPreferredFormat() const = 0;
    // decide if we support this format (should be one of values of
    // StdFormat enumerations or a user-defined format)
  virtual bool IsSupportedFormat(wxDataFormat format) const = 0;
    // get the (total) size of data
  virtual uint GetDataSize() const = 0;
    // copy raw data to provided pointer
  virtual void GetDataHere(void *pBuf) const = 0;

  // accessors
    // retrieve IDataObject interface (for other OLE related classes)
  IDataObject *GetInterface() const { return m_pIDataObject; }

private:
  IDataObject *m_pIDataObject; // pointer to the COM interface
};

// ----------------------------------------------------------------------------
// wxTextDataObject is a specialization of wxDataObject for text data
// ----------------------------------------------------------------------------
class wxTextDataObject : public wxDataObject
{
public:
  // ctors
  wxTextDataObject();
  wxTextDataObject(const wxString& strText) : m_strText(strText) { }
  void Init(const wxString& strText) { m_strText = strText; }

  // implement base class pure virtuals
  virtual wxDataFormat GetPreferredFormat() const
    { return wxDataObject::Text; }
  virtual bool IsSupportedFormat(wxDataFormat format) const
    { return format == wxDataObject::Text || format == wxDataObject::Locale; }
  virtual uint GetDataSize() const
    { return m_strText.Len() + 1; } // +1 for trailing '\0'of course
  virtual void GetDataHere(void *pBuf) const
    { memcpy(pBuf, m_strText.c_str(), GetDataSize()); }

private:
  wxString  m_strText;
};

// ----------------------------------------------------------------------------
// @@@ TODO: wx{Bitmap|Metafile|...}DataObject
// ----------------------------------------------------------------------------

#endif  //_OLEDATAOBJ_H
