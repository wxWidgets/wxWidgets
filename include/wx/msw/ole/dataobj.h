///////////////////////////////////////////////////////////////////////////////
// Name:        ole/dataobj.h
// Purpose:     declaration of the wxDataObject class
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     10.05.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_OLEDATAOBJ_H
#define   _WX_OLEDATAOBJ_H

// ----------------------------------------------------------------------------
// wxDataFormat identifies the single format of data
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataFormat
{
public:
    // the clipboard formats under Win32 are UINTs
    typedef unsigned int NativeFormat;

    wxDataFormat(NativeFormat format = wxDF_INVALID) { m_format = format; }
    wxDataFormat& operator=(NativeFormat format)
        { m_format = format; return *this; }

    // defautl copy ctor/assignment operators ok

    // comparison (must have both versions)
    bool operator==(wxDataFormatId format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(wxDataFormatId format) const
        { return m_format != (NativeFormat)format; }
    bool operator==(const wxDataFormat& format) const
        { return m_format == format.m_format; }
    bool operator!=(const wxDataFormat& format) const
        { return m_format != format.m_format; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }

    // this only works with standard ids
    void SetId(wxDataFormatId format) { m_format = format; }

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId(const wxChar *format);

private:
    // returns TRUE if the format is one of those defined in wxDataFormatId
    bool IsStandard() const { return m_format > 0 && m_format < wxDF_MAX; }

    NativeFormat m_format;
};

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------
struct IDataObject;

// ----------------------------------------------------------------------------
// wxDataObject is a "smart" and polymorphic piece of data.
//
// TODO it's currently "read-only" from COM point of view, i.e. we don't support
//      SetData. We don't support all advise functions neither (but it's easy to
//      do if we really want them)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDataObject
{
public:
  // function to return symbolic name of clipboard format (debug messages)
  static const char *GetFormatName(wxDataFormat format);

  // ctor & dtor
  wxDataObject();
  virtual ~wxDataObject();

  // pure virtuals to override
    // get the best suited format for our data
  virtual wxDataFormat GetPreferredFormat() const = 0;
    // decide if we support this format (should be one of values of
    // wxDataFormatId enumerations or a user-defined format)
  virtual bool IsSupportedFormat(wxDataFormat format) const = 0;
    // get the (total) size of data
  virtual size_t GetDataSize() const = 0;
    // copy raw data to provided pointer
  virtual void GetDataHere(void *pBuf) const = 0;

  // accessors
    // retrieve IDataObject interface (for other OLE related classes)
  IDataObject *GetInterface() const { return m_pIDataObject; }

  ////// wxGTK compatibility: hopefully to become the preferred API.
  virtual wxDataFormat GetFormat() const { return GetPreferredFormat(); }

private:
  IDataObject *m_pIDataObject; // pointer to the COM interface
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
    { return format == wxDF_TEXT || format == wxDF_LOCALE; }
  virtual size_t GetDataSize() const
    { return m_strText.Len() + 1; } // +1 for trailing '\0'of course
  virtual void GetDataHere(void *pBuf) const
    { memcpy(pBuf, m_strText.c_str(), GetDataSize()); }

  ////// wxGTK compatibility: hopefully to become the preferred API.
  void SetText(const wxString& strText) { m_strText = strText; }
  wxString GetText() const { return m_strText; }
  virtual wxDataFormat GetFormat() const { return wxDF_TEXT; }

private:
  wxString  m_strText;
};

// ----------------------------------------------------------------------------
// @@@ TODO: wx{Bitmap|Metafile|...}DataObject
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxBitmapDataObject is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

// TODO: implement OLE side of things. At present, it's just for clipboard
// use.
class WXDLLEXPORT wxBitmapDataObject : public wxDataObject
{
public:
  // ctors
  wxBitmapDataObject() {};
  wxBitmapDataObject(const wxBitmap& bitmap): m_bitmap(bitmap) {}
  void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
  wxBitmap GetBitmap() const { return m_bitmap; }

  virtual wxDataFormat GetFormat() const { return wxDF_BITMAP; }

  // implement base class pure virtuals
  virtual wxDataFormat GetPreferredFormat() const
    { return wxDF_BITMAP; }
  virtual bool IsSupportedFormat(wxDataFormat format) const
    { return format == wxDF_BITMAP; }
  virtual size_t GetDataSize() const
    { wxASSERT(FALSE); return 0; } // BEMIMP
  virtual void GetDataHere(void *pBuf) const
    { wxASSERT(FALSE); } // BEMIMP

private:
  wxBitmap  m_bitmap;
};

// ----------------------------------------------------------------------------
// wxMetaFileDataObject: see metafile.h is a specialization of wxDataObject for bitmap data
// ----------------------------------------------------------------------------

// TODO: wxFileDataObject.

#endif  //_WX_OLEDATAOBJ_H

