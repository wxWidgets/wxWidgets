/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/zipstrm.cpp
// Purpose:     Streams for Zip files
// Author:      Mike Wetherell
// Copyright:   (c) Mike Wetherell
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_ZIPSTREAM

#include "wx/zipstrm.h"

#ifndef WX_PRECOMP
    #include "wx/hashmap.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
#endif

#include "wx/datstrm.h"
#include "wx/zstream.h"
#include "wx/mstream.h"
#include "wx/scopedptr.h"
#include "wx/wfstream.h"
#include "zlib.h"

// value for the 'version needed to extract' field (20 means 2.0)
enum {
    VERSION_NEEDED_TO_EXTRACT = 20,
    Z64_VERSION_NEEDED_TO_EXTRACT = 45 // File uses ZIP64 format extensions
};

// signatures for the various records (PKxx)
enum {
    CENTRAL_MAGIC = 0x02014b50,     // central directory record
    LOCAL_MAGIC   = 0x04034b50,     // local header
    END_MAGIC     = 0x06054b50,     // end of central directory record
    SUMS_MAGIC    = 0x08074b50,     // data descriptor (info-zip)
    Z64_LOC_MAGIC = 0x07064b50,     // zip64 end of central directory locator
    Z64_END_MAGIC = 0x06064b50      // zip64 end of central directory record
};

// unix file attributes. zip stores them in the high 16 bits of the
// 'external attributes' field, hence the extra zeros.
enum {
    wxZIP_S_IFMT  = 0xF0000000,
    wxZIP_S_IFDIR = 0x40000000,
    wxZIP_S_IFREG = 0x80000000
};

// minimum sizes for the various records
enum {
    CENTRAL_SIZE  = 46,
    LOCAL_SIZE    = 30,
    END_SIZE      = 22,
    SUMS_SIZE     = 12,
    Z64_LOC_SIZE  = 20,
    Z64_END_SIZE  = 56
};

// The number of bytes that must be written to an wxZipOutputStream before
// a zip entry is created. The purpose of this latency is so that
// OpenCompressor() can see a little data before deciding which compressor
// it should use.
enum {
    OUTPUT_LATENCY = 4096
};

// Some offsets into the local header
enum {
    SUMS_OFFSET  = 14
};

wxIMPLEMENT_DYNAMIC_CLASS(wxZipEntry, wxArchiveEntry);
wxIMPLEMENT_DYNAMIC_CLASS(wxZipClassFactory, wxArchiveClassFactory);


/////////////////////////////////////////////////////////////////////////////
// Helpers

// read a string of a given length
//
static wxString ReadString(wxInputStream& stream, wxUint16 len, wxMBConv& conv)
{
    if (len == 0)
        return wxEmptyString;

#if wxUSE_UNICODE
    wxCharBuffer buf(len);
    stream.Read(buf.data(), len);
    wxString str(buf, conv);
#else
    wxString str;
    (void)conv;
    {
        wxStringBuffer buf(str, len);
        stream.Read(buf, len);
    }
#endif

    return str;
}

static inline wxUint16 LimitUint16(wxUint64 value)
{
    if (value > 0xffff)
        return 0xffff;
    else
        return wx_truncate_cast(wxUint16, value);
}

static inline wxUint32 LimitUint32(wxUint64 value)
{
    if (value > 0xffffffff)
        return 0xffffffff;
    else
        return wx_truncate_cast(wxUint32,value);
}

// Decode a little endian wxUint64 number from a character array
//
static inline wxUint64 CrackUint64(const char *m)
{
    const unsigned char *n = (const unsigned char*)m;
    return (static_cast<wxUint64>(n[7]) << 56) |
        (static_cast<wxUint64>(n[6]) << 48) |
        (static_cast<wxUint64>(n[5]) << 40) |
        (static_cast<wxUint64>(n[4]) << 32) |
        (n[3] << 24) | (n[2] << 16) | (n[1] << 8) | n[0];
}

// Decode a little endian wxUint32 number from a character array
//
static inline wxUint32 CrackUint32(const char *m)
{
    const unsigned char *n = (const unsigned char*)m;
    return (n[3] << 24) | (n[2] << 16) | (n[1] << 8) | n[0];
}

// Decode a little endian wxUint16 number from a character array
//
static inline wxUint16 CrackUint16(const char *m)
{
    const unsigned char *n = (const unsigned char*)m;
    return (n[1] << 8) | n[0];
}

// Temporarily lower the logging level in debug mode to avoid a warning
// from SeekI about seeking on a stream with data written back to it.
//
static wxFileOffset QuietSeek(wxInputStream& stream, wxFileOffset pos)
{
#if wxUSE_LOG
    wxLogLevel level = wxLog::GetLogLevel();
    wxLog::SetLogLevel(wxLOG_Debug - 1);
    wxFileOffset result = stream.SeekI(pos);
    wxLog::SetLogLevel(level);
    return result;
#else
    return stream.SeekI(pos);
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Class factory

static wxZipClassFactory g_wxZipClassFactory;

wxZipClassFactory::wxZipClassFactory()
{
    if (this == &g_wxZipClassFactory)
        PushFront();
}

const wxChar * const *
wxZipClassFactory::GetProtocols(wxStreamProtocolType type) const
{
    static const wxChar *protocols[] = { wxT("zip"), NULL };
    static const wxChar *mimetypes[] = { wxT("application/zip"), NULL };
    static const wxChar *fileexts[]  = { wxT(".zip"), wxT(".htb"), NULL };
    static const wxChar *empty[]     = { NULL };

    switch (type) {
        case wxSTREAM_PROTOCOL: return protocols;
        case wxSTREAM_MIMETYPE: return mimetypes;
        case wxSTREAM_FILEEXT:  return fileexts;
        default:                return empty;
    }
}


/////////////////////////////////////////////////////////////////////////////
// Read a zip header

class wxZipHeader
{
public:
    wxZipHeader(wxInputStream& stream, size_t size);
    wxZipHeader(const char* data, size_t size);

    inline wxUint8 Read8();
    inline wxUint16 Read16();
    inline wxUint32 Read32();
    inline wxUint64 Read64();

    const char *GetData() const             { return m_data; }
    size_t GetSize() const                  { return m_size; }
    operator bool() const                   { return m_ok; }

    size_t Seek(size_t pos)                 { m_pos = pos; return m_pos; }
    size_t Skip(size_t size)                { m_pos += size; return m_pos; }

    wxZipHeader& operator>>(wxUint8& n)     { n = Read8();  return *this; }
    wxZipHeader& operator>>(wxUint16& n)    { n = Read16(); return *this; }
    wxZipHeader& operator>>(wxUint32& n)    { n = Read32(); return *this; }
    wxZipHeader& operator>>(wxUint64& n)    { n = Read64(); return *this; }

private:
    char m_data[64];
    size_t m_size;
    size_t m_pos;
    bool m_ok;
};

wxZipHeader::wxZipHeader(wxInputStream& stream, size_t size)
  : m_size(0),
    m_pos(0),
    m_ok(false)
{
    wxCHECK_RET(size <= sizeof(m_data), wxT("buffer too small"));
    m_size = stream.Read(m_data, size).LastRead();
    m_ok = m_size == size;
}

wxZipHeader::wxZipHeader(const char* data, size_t size)
  : m_size(size),
    m_pos(0),
    m_ok(true)
{
    wxCHECK_RET(size <= sizeof(m_data), wxT("buffer too small"));
    memcpy(m_data, data, size);
}

inline wxUint8 wxZipHeader::Read8()
{
    wxASSERT(m_pos < m_size);
    return m_data[m_pos++];
}

inline wxUint16 wxZipHeader::Read16()
{
    wxASSERT(m_pos + 2 <= m_size);
    wxUint16 n = CrackUint16(m_data + m_pos);
    m_pos += 2;
    return n;
}

inline wxUint32 wxZipHeader::Read32()
{
    wxASSERT(m_pos + 4 <= m_size);
    wxUint32 n = CrackUint32(m_data + m_pos);
    m_pos += 4;
    return n;
}

inline wxUint64 wxZipHeader::Read64()
{
    wxASSERT(m_pos + 8 <= m_size);
    wxUint64 n = CrackUint64(m_data + m_pos);
    m_pos += 8;
    return n;
}

/////////////////////////////////////////////////////////////////////////////
// Stored input stream
// Trival decompressor for files which are 'stored' in the zip file.

class wxStoredInputStream : public wxFilterInputStream
{
public:
    wxStoredInputStream(wxInputStream& stream);

    void Open(wxFileOffset len) { Close(); m_len = len; }
    void Close() { m_pos = 0; m_lasterror = wxSTREAM_NO_ERROR; }

    virtual char Peek() wxOVERRIDE { return wxInputStream::Peek(); }
    virtual wxFileOffset GetLength() const wxOVERRIDE { return m_len; }

protected:
    virtual size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;
    virtual wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    wxFileOffset m_pos;
    wxFileOffset m_len;

    wxDECLARE_NO_COPY_CLASS(wxStoredInputStream);
};

wxStoredInputStream::wxStoredInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream),
    m_pos(0),
    m_len(0)
{
}

size_t wxStoredInputStream::OnSysRead(void *buffer, size_t size)
{
    size_t count = wx_truncate_cast(size_t,
                wxMin(size + wxFileOffset(0), m_len - m_pos + size_t(0)));
    count = m_parent_i_stream->Read(buffer, count).LastRead();
    m_pos += count;

    if (count < size)
        m_lasterror = m_pos == m_len ? wxSTREAM_EOF : wxSTREAM_READ_ERROR;

    return count;
}


/////////////////////////////////////////////////////////////////////////////
// Stored output stream
// Trival compressor for files which are 'stored' in the zip file.

class wxStoredOutputStream : public wxFilterOutputStream
{
public:
    wxStoredOutputStream(wxOutputStream& stream) :
        wxFilterOutputStream(stream), m_pos(0) { }

    bool Close() wxOVERRIDE {
        m_pos = 0;
        m_lasterror = wxSTREAM_NO_ERROR;
        return true;
    }

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size) wxOVERRIDE;
    virtual wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    wxFileOffset m_pos;
    wxDECLARE_NO_COPY_CLASS(wxStoredOutputStream);
};

size_t wxStoredOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    if (!IsOk() || !size)
        return 0;
    size_t count = m_parent_o_stream->Write(buffer, size).LastWrite();
    if (count != size)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    m_pos += count;
    return count;
}


/////////////////////////////////////////////////////////////////////////////
// wxRawInputStream
//
// Used to handle the unusal case of raw copying an entry of unknown
// length. This can only happen when the zip being copied from is being
// read from a non-seekable stream, and also was original written to a
// non-seekable stream.
//
// In this case there's no option but to decompress the stream to find
// its length, but we can still write the raw compressed data to avoid the
// compression overhead (which is the greater one).
//
// Usage is like this:
//  m_rawin = new wxRawInputStream(*m_parent_i_stream);
//  m_decomp = m_rawin->Open(OpenDecompressor(m_rawin->GetTee()));
//
// The wxRawInputStream owns a wxTeeInputStream object, the role of which
// is something like the unix 'tee' command; it is a transparent filter, but
// allows the data read to be read a second time via an extra method 'GetData'.
//
// The wxRawInputStream then draws data through the tee using a decompressor
// then instead of returning the decompressed data, retuns the raw data
// from wxTeeInputStream::GetData().

class wxTeeInputStream : public wxFilterInputStream
{
public:
    wxTeeInputStream(wxInputStream& stream);

    size_t GetCount() const { return m_end - m_start; }
    size_t GetData(char *buffer, size_t size);

    void Open();
    bool Final();

    wxInputStream& Read(void *buffer, size_t size) wxOVERRIDE;

protected:
    virtual size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;
    virtual wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    wxFileOffset m_pos;
    wxMemoryBuffer m_buf;
    size_t m_start;
    size_t m_end;

    wxDECLARE_NO_COPY_CLASS(wxTeeInputStream);
};

wxTeeInputStream::wxTeeInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream),
    m_pos(0), m_buf(8192), m_start(0), m_end(0)
{
}

void wxTeeInputStream::Open()
{
    m_pos = m_start = m_end = 0;
    m_lasterror = wxSTREAM_NO_ERROR;
}

bool wxTeeInputStream::Final()
{
    bool final = m_end == m_buf.GetDataLen();
    m_end = m_buf.GetDataLen();
    return final;
}

wxInputStream& wxTeeInputStream::Read(void *buffer, size_t size)
{
    size_t count = wxInputStream::Read(buffer, size).LastRead();
    m_end = m_buf.GetDataLen();
    m_buf.AppendData(buffer, count);
    return *this;
}

size_t wxTeeInputStream::OnSysRead(void *buffer, size_t size)
{
    size_t count = m_parent_i_stream->Read(buffer, size).LastRead();
    if (count < size)
        m_lasterror = m_parent_i_stream->GetLastError();
    return count;
}

size_t wxTeeInputStream::GetData(char *buffer, size_t size)
{
    if (m_wbacksize) {
        size_t len = m_buf.GetDataLen();
        len = len > m_wbacksize ? len - m_wbacksize : 0;
        m_buf.SetDataLen(len);
        if (m_end > len) {
            wxFAIL; // we've already returned data that's now being ungot
            m_end = len;
        }
        m_parent_i_stream->Reset();
        m_parent_i_stream->Ungetch(m_wback, m_wbacksize);
        free(m_wback);
        m_wback = NULL;
        m_wbacksize = 0;
        m_wbackcur = 0;
    }

    if (size > GetCount())
        size = GetCount();
    if (size) {
        memcpy(buffer, m_buf + m_start, size);
        m_start += size;
        wxASSERT(m_start <= m_end);
    }

    if (m_start == m_end && m_start > 0 && m_buf.GetDataLen() > 0) {
        size_t len = m_buf.GetDataLen();
        char *buf = (char*)m_buf.GetWriteBuf(len);
        len -= m_end;
        memmove(buf, buf + m_end, len);
        m_buf.UngetWriteBuf(len);
        m_start = m_end = 0;
    }

    return size;
}

class wxRawInputStream : public wxFilterInputStream
{
public:
    wxRawInputStream(wxInputStream& stream);
    virtual ~wxRawInputStream() { delete m_tee; }

    wxInputStream* Open(wxInputStream *decomp);
    wxInputStream& GetTee() const { return *m_tee; }

protected:
    virtual size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;
    virtual wxFileOffset OnSysTell() const wxOVERRIDE { return m_pos; }

private:
    wxFileOffset m_pos;
    wxTeeInputStream *m_tee;

    enum { BUFSIZE = 8192 };
    wxCharBuffer m_dummy;

    wxDECLARE_NO_COPY_CLASS(wxRawInputStream);
};

wxRawInputStream::wxRawInputStream(wxInputStream& stream)
  : wxFilterInputStream(stream),
    m_pos(0),
    m_tee(new wxTeeInputStream(stream)),
    m_dummy(BUFSIZE)
{
}

wxInputStream *wxRawInputStream::Open(wxInputStream *decomp)
{
    if (decomp) {
        m_parent_i_stream = decomp;
        m_pos = 0;
        m_lasterror = wxSTREAM_NO_ERROR;
        m_tee->Open();
        return this;
    } else {
        return NULL;
    }
}

size_t wxRawInputStream::OnSysRead(void *buffer, size_t size)
{
    char *buf = (char*)buffer;
    size_t count = 0;

    while (count < size && IsOk())
    {
        while (m_parent_i_stream->IsOk() && m_tee->GetCount() == 0)
            m_parent_i_stream->Read(m_dummy.data(), BUFSIZE);

        size_t n = m_tee->GetData(buf + count, size - count);
        count += n;

        if (n == 0 && m_tee->Final())
            m_lasterror = m_parent_i_stream->GetLastError();
    }

    m_pos += count;
    return count;
}


/////////////////////////////////////////////////////////////////////////////
// Zlib streams than can be reused without recreating.

class wxZlibOutputStream2 : public wxZlibOutputStream
{
public:
    wxZlibOutputStream2(wxOutputStream& stream, int level) :
        wxZlibOutputStream(stream, level, wxZLIB_NO_HEADER) { }

    bool Open(wxOutputStream& stream);
    bool Close() wxOVERRIDE { DoFlush(true); m_pos = wxInvalidOffset; return IsOk(); }
};

bool wxZlibOutputStream2::Open(wxOutputStream& stream)
{
    wxCHECK(m_pos == wxInvalidOffset, false);

    m_deflate->next_out = m_z_buffer;
    m_deflate->avail_out = m_z_size;
    m_pos = 0;
    m_lasterror = wxSTREAM_NO_ERROR;
    m_parent_o_stream = &stream;

    if (deflateReset(m_deflate) != Z_OK) {
        wxLogError(_("can't re-initialize zlib deflate stream"));
        m_lasterror = wxSTREAM_WRITE_ERROR;
        return false;
    }

    return true;
}

class wxZlibInputStream2 : public wxZlibInputStream
{
public:
    wxZlibInputStream2(wxInputStream& stream) :
        wxZlibInputStream(stream, wxZLIB_NO_HEADER) { }

    bool Open(wxInputStream& stream);
};

bool wxZlibInputStream2::Open(wxInputStream& stream)
{
    m_inflate->avail_in = 0;
    m_pos = 0;
    m_lasterror = wxSTREAM_NO_ERROR;
    m_parent_i_stream = &stream;

    if (inflateReset(m_inflate) != Z_OK) {
        wxLogError(_("can't re-initialize zlib inflate stream"));
        m_lasterror = wxSTREAM_READ_ERROR;
        return false;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////
// Class to hold wxZipEntry's Extra and LocalExtra fields

class wxZipMemory
{
public:
    wxZipMemory() : m_data(NULL), m_size(0), m_capacity(0), m_ref(1) { }

    wxZipMemory *AddRef() { m_ref++; return this; }
    void Release() { if (--m_ref == 0) delete this; }

    char *GetData() const { return m_data; }
    size_t GetSize() const { return m_size; }
    size_t GetCapacity() const { return m_capacity; }

    wxZipMemory *Unique(size_t size);

private:
    ~wxZipMemory() { delete [] m_data; }

    char *m_data;
    size_t m_size;
    size_t m_capacity;
    int m_ref;

    wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING(wxZipMemory)
};

wxZipMemory *wxZipMemory::Unique(size_t size)
{
    wxZipMemory *zm;

    if (m_ref > 1) {
        --m_ref;
        zm = new wxZipMemory;
    } else {
        zm = this;
    }

    if (zm->m_capacity < size) {
        delete [] zm->m_data;
        zm->m_data = new char[size];
        zm->m_capacity = size;
    }

    zm->m_size = size;
    return zm;
}

static inline wxZipMemory *AddRef(wxZipMemory *zm)
{
    if (zm)
        zm->AddRef();
    return zm;
}

static inline void Release(wxZipMemory *zm)
{
    if (zm)
        zm->Release();
}

static void Copy(wxZipMemory*& dest, wxZipMemory *src)
{
    Release(dest);
    dest = AddRef(src);
}

static void Unique(wxZipMemory*& zm, size_t size)
{
    if (!zm && size)
        zm = new wxZipMemory;
    if (zm)
        zm = zm->Unique(size);
}


/////////////////////////////////////////////////////////////////////////////
// Collection of weak references to entries

WX_DECLARE_HASH_MAP(long, wxZipEntry*, wxIntegerHash,
                    wxIntegerEqual, wxOffsetZipEntryMap_);

class wxZipWeakLinks
{
public:
    wxZipWeakLinks() : m_ref(1) { }

    void Release(const wxZipInputStream* WXUNUSED(x))
        { if (--m_ref == 0) delete this; }
    void Release(wxFileOffset key)
        { RemoveEntry(key); if (--m_ref == 0) delete this; }

    wxZipWeakLinks *AddEntry(wxZipEntry *entry, wxFileOffset key);
    void RemoveEntry(wxFileOffset key)
        { m_entries.erase(wx_truncate_cast(key_type, key)); }
    wxZipEntry *GetEntry(wxFileOffset key) const;
    bool IsEmpty() const { return m_entries.empty(); }

private:
    ~wxZipWeakLinks() { wxASSERT(IsEmpty()); }

    typedef wxOffsetZipEntryMap_::key_type key_type;

    int m_ref;
    wxOffsetZipEntryMap_ m_entries;

    wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING(wxZipWeakLinks)
};

wxZipWeakLinks *wxZipWeakLinks::AddEntry(wxZipEntry *entry, wxFileOffset key)
{
    m_entries[wx_truncate_cast(key_type, key)] = entry;
    m_ref++;
    return this;
}

wxZipEntry *wxZipWeakLinks::GetEntry(wxFileOffset key) const
{
    wxOffsetZipEntryMap_::const_iterator it =
        m_entries.find(wx_truncate_cast(key_type, key));
    return it != m_entries.end() ?  it->second : NULL;
}


/////////////////////////////////////////////////////////////////////////////
// ZipEntry

wxZipEntry::wxZipEntry(
    const wxString& name /*=wxEmptyString*/,
    const wxDateTime& dt /*=wxDateTime::Now()*/,
    wxFileOffset size    /*=wxInvalidOffset*/)
  :
    m_SystemMadeBy(wxZIP_SYSTEM_MSDOS),
    m_VersionMadeBy(wxMAJOR_VERSION * 10 + wxMINOR_VERSION),
    m_VersionNeeded(VERSION_NEEDED_TO_EXTRACT),
    m_Flags(0),
    m_Method(wxZIP_METHOD_DEFAULT),
    m_DateTime(dt),
    m_Crc(0),
    m_CompressedSize(wxInvalidOffset),
    m_Size(size),
    m_Key(wxInvalidOffset),
    m_Offset(wxInvalidOffset),
    m_DiskStart(0),
    m_InternalAttributes(0),
    m_ExternalAttributes(0),
    m_Extra(NULL),
    m_LocalExtra(NULL),
    m_zipnotifier(NULL),
    m_backlink(NULL)
{
    if (!name.empty())
        SetName(name);
}

wxZipEntry::~wxZipEntry()
{
    if (m_backlink)
        m_backlink->Release(m_Key);
    Release(m_Extra);
    Release(m_LocalExtra);
}

wxZipEntry::wxZipEntry(const wxZipEntry& e)
  : wxArchiveEntry(e),
    m_SystemMadeBy(e.m_SystemMadeBy),
    m_VersionMadeBy(e.m_VersionMadeBy),
    m_VersionNeeded(e.m_VersionNeeded),
    m_Flags(e.m_Flags),
    m_Method(e.m_Method),
    m_DateTime(e.m_DateTime),
    m_Crc(e.m_Crc),
    m_CompressedSize(e.m_CompressedSize),
    m_Size(e.m_Size),
    m_Name(e.m_Name),
    m_Key(e.m_Key),
    m_Offset(e.m_Offset),
    m_Comment(e.m_Comment),
    m_DiskStart(e.m_DiskStart),
    m_InternalAttributes(e.m_InternalAttributes),
    m_ExternalAttributes(e.m_ExternalAttributes),
    m_Extra(AddRef(e.m_Extra)),
    m_LocalExtra(AddRef(e.m_LocalExtra)),
    m_zipnotifier(NULL),
    m_backlink(NULL)
{
}

wxZipEntry& wxZipEntry::operator=(const wxZipEntry& e)
{
    if (&e != this) {
        m_SystemMadeBy = e.m_SystemMadeBy;
        m_VersionMadeBy = e.m_VersionMadeBy;
        m_VersionNeeded = e.m_VersionNeeded;
        m_Flags = e.m_Flags;
        m_Method = e.m_Method;
        m_DateTime = e.m_DateTime;
        m_Crc = e.m_Crc;
        m_CompressedSize = e.m_CompressedSize;
        m_Size = e.m_Size;
        m_Name = e.m_Name;
        m_Key = e.m_Key;
        m_Offset = e.m_Offset;
        m_Comment = e.m_Comment;
        m_DiskStart = e.m_DiskStart;
        m_InternalAttributes = e.m_InternalAttributes;
        m_ExternalAttributes = e.m_ExternalAttributes;
        Copy(m_Extra, e.m_Extra);
        Copy(m_LocalExtra, e.m_LocalExtra);
        m_zipnotifier = NULL;
        if (m_backlink) {
            m_backlink->Release(m_Key);
            m_backlink = NULL;
        }
    }
    return *this;
}

wxString wxZipEntry::GetName(wxPathFormat format /*=wxPATH_NATIVE*/) const
{
    bool isDir = IsDir() && !m_Name.empty();

    // optimisations for common (and easy) cases
    switch (wxFileName::GetFormat(format)) {
        case wxPATH_DOS:
        {
            wxString name(m_Name);
            name.Replace(wxFILE_SEP_PATH_UNIX, wxFILE_SEP_PATH_DOS);
            if (isDir)
                name += wxFILE_SEP_PATH_DOS;
            return name;
        }

        case wxPATH_UNIX:
            return isDir ? m_Name + wxFILE_SEP_PATH_UNIX : m_Name;

        default:
            ;
    }

    wxFileName fn;

    if (isDir)
        fn.AssignDir(m_Name, wxPATH_UNIX);
    else
        fn.Assign(m_Name, wxPATH_UNIX);

    return fn.GetFullPath(format);
}

namespace
{

// Accept both slashes and backslashes for compatibility as a lot of broken
// programs create such archives even though the ZIP specification explicitly
// says to use slashes (see 4.4.17.1).
inline bool IsDOSPathSep(wxUniChar ch)
{
    return ch == wxFILE_SEP_PATH_DOS || ch == wxFILE_SEP_PATH_UNIX;
}
} // anonymous namespace

// Static - Internally tars and zips use forward slashes for the path
// separator, absolute paths aren't allowed, and directory names have a
// trailing slash.  This function converts a path into this internal format,
// but without a trailing slash for a directory.
//
wxString wxZipEntry::GetInternalName(const wxString& name,
                                     wxPathFormat format /*=wxPATH_NATIVE*/,
                                     bool *pIsDir        /*=NULL*/)
{
    wxString internal;

    if (wxFileName::GetFormat(format) != wxPATH_UNIX)
        internal = wxFileName(name, format).GetFullPath(wxPATH_UNIX);
    else
        internal = name;

    bool isDir = !internal.empty() && IsDOSPathSep(internal.Last());
    if (pIsDir)
        *pIsDir = isDir;
    if (isDir)
        internal.erase(internal.length() - 1);

    while (!internal.empty() && IsDOSPathSep(*internal.begin()))
        internal.erase(0, 1);
    while (!internal.empty() &&
            (internal[0] == wxS('.') && IsDOSPathSep(internal[1])))
        internal.erase(0, 2);
    if (internal == wxT(".") || internal == wxT(".."))
        internal.clear();

    return internal;
}

void wxZipEntry::SetSystemMadeBy(int system)
{
    int mode = GetMode();
    bool wasUnix = IsMadeByUnix();

    m_SystemMadeBy = (wxUint8)system;

    if (!wasUnix && IsMadeByUnix()) {
        SetIsDir(IsDir());
        SetMode(mode);
    } else if (wasUnix && !IsMadeByUnix()) {
        m_ExternalAttributes &= 0xffff;
    }
}

void wxZipEntry::SetIsDir(bool isDir /*=true*/)
{
    if (isDir)
        m_ExternalAttributes |= wxZIP_A_SUBDIR;
    else
        m_ExternalAttributes &= ~wxZIP_A_SUBDIR;

    if (IsMadeByUnix()) {
        m_ExternalAttributes &= ~wxZIP_S_IFMT;
        if (isDir)
            m_ExternalAttributes |= wxZIP_S_IFDIR;
        else
            m_ExternalAttributes |= wxZIP_S_IFREG;
    }
}

// Return unix style permission bits
//
int wxZipEntry::GetMode() const
{
    // return unix permissions if present
    if (IsMadeByUnix())
        return (m_ExternalAttributes >> 16) & 0777;

    // otherwise synthesize from the dos attribs
    int mode = 0644;
    if (m_ExternalAttributes & wxZIP_A_RDONLY)
        mode &= ~0200;
    if (m_ExternalAttributes & wxZIP_A_SUBDIR)
        mode |= 0111;

    return mode;
}

// Set unix permissions
//
void wxZipEntry::SetMode(int mode)
{
    // Set dos attrib bits to be compatible
    if (mode & 0222)
        m_ExternalAttributes &= ~wxZIP_A_RDONLY;
    else
        m_ExternalAttributes |= wxZIP_A_RDONLY;

    // set the actual unix permission bits if the system type allows
    if (IsMadeByUnix()) {
        m_ExternalAttributes &= ~(0777L << 16);
        m_ExternalAttributes |= (mode & 0777L) << 16;
    }
}

const char *wxZipEntry::GetExtra() const
{
    return m_Extra ? m_Extra->GetData() : NULL;
}

size_t wxZipEntry::GetExtraLen() const
{
    return m_Extra ? m_Extra->GetSize() : 0;
}

void wxZipEntry::SetExtra(const char *extra, size_t len)
{
    Unique(m_Extra, len);
    if (len)
        memcpy(m_Extra->GetData(), extra, len);
}

const char *wxZipEntry::GetLocalExtra() const
{
    return m_LocalExtra ? m_LocalExtra->GetData() : NULL;
}

size_t  wxZipEntry::GetLocalExtraLen() const
{
    return m_LocalExtra ? m_LocalExtra->GetSize() : 0;
}

void wxZipEntry::SetLocalExtra(const char *extra, size_t len)
{
    Unique(m_LocalExtra, len);
    if (len)
        memcpy(m_LocalExtra->GetData(), extra, len);
}

void wxZipEntry::SetNotifier(wxZipNotifier& notifier)
{
    wxArchiveEntry::UnsetNotifier();
    m_zipnotifier = &notifier;
    m_zipnotifier->OnEntryUpdated(*this);
}

void wxZipEntry::Notify()
{
    if (m_zipnotifier)
        m_zipnotifier->OnEntryUpdated(*this);
    else if (GetNotifier())
        GetNotifier()->OnEntryUpdated(*this);
}

void wxZipEntry::UnsetNotifier()
{
    wxArchiveEntry::UnsetNotifier();
    m_zipnotifier = NULL;
}

bool wxZipEntry::LoadExtraInfo(const char* extraData, wxUint16 extraLen, bool localInfo)
{
    // We need to iterate over all headers
    // seeking for the field with Header ID = 1.
    // (There is not stated in the documentation
    // that it should be the first one in the collection.)
    const char* const extraDataEnd = extraData + extraLen;
    while ( extraData + 4 <= extraDataEnd )
    {
        // Parse extra header
        wxZipHeader hds(extraData, 4);
        wxUint16 fieldID = hds.Read16();
        wxUint16 fieldLen = hds.Read16();
        if ( fieldID == 1 )
        {
            // Check that we don't overflow the input buffer.
            if ( extraData + 4 + fieldLen > extraDataEnd )
            {
                wxLogWarning(_("Ignoring malformed extra data record, "
                               "ZIP file may be corrupted"));
                return false;
            }

            // Data block for extra field with Header ID = 1 (ZIP64)
            // can have length up to 28 bytes.
            wxZipHeader ds(extraData+4, wxMin(fieldLen, 28));
            // A file may contain larger size, compressed size or offset
            // in a zip64 extra data block. Use the 64 bit values if available
            if ( m_Size == 0xffffffff )
                m_Size = ds.Read64();
            if ( m_CompressedSize == 0xffffffff )
                m_CompressedSize = ds.Read64();
            if ( !localInfo && m_Offset == 0xffffffff )
                m_Offset = ds.Read64();
            // extraInfo was used and parsed
            return true;
        }

        // Avoid "optimizing" the lines below by doing "fieldLen += 4" as this
        // could overflow wxUint16 range and, at worst, make fieldLen equal to
        // 0 resulting in an infinite loop. Written as it is now, everything is
        // promoted to int, which has range large enough to deal with any value
        // of the field length.
        extraData += fieldLen + 4;
        extraLen -= fieldLen + 4;
    }

    // extraInfo had unknown format
    return false;
}

size_t wxZipEntry::ReadLocal(wxInputStream& stream, wxMBConv& conv)
{
    wxUint16 nameLen, extraLen;
    wxUint32 compressedSize, size, crc;

    wxZipHeader ds(stream, LOCAL_SIZE - 4);
    if (!ds)
        return 0;

    ds >> m_VersionNeeded >> m_Flags >> m_Method;
    SetDateTime(wxDateTime().SetFromDOS(ds.Read32()));
    ds >> crc >> compressedSize >> size >> nameLen >> extraLen;

    bool sumsValid = (m_Flags & wxZIP_SUMS_FOLLOW) == 0;

    if (sumsValid || crc)
        m_Crc = crc;
    if ((sumsValid || compressedSize) || m_Method == wxZIP_METHOD_STORE)
        m_CompressedSize = compressedSize;
    if ((sumsValid || size) || m_Method == wxZIP_METHOD_STORE)
        m_Size = size;

    // Explicit cast to the base class is needed to work around apparent
    // compiler bug in MSVS 2005 (FIXME-VC8).
    wxMBConv& strConv = m_Flags & wxZIP_LANG_ENC_UTF8
                            ? static_cast<wxMBConv&>(wxConvUTF8)
                            : conv;
    SetName(ReadString(stream, nameLen, strConv), wxPATH_UNIX);
    if (stream.LastRead() != nameLen + 0u)
        return 0;

    if (extraLen || GetLocalExtraLen()) {
        Unique(m_LocalExtra, extraLen);
        if (extraLen) {
            stream.Read(m_LocalExtra->GetData(), extraLen);
            if (stream.LastRead() != extraLen + 0u)
                return 0;

            if (LoadExtraInfo(m_LocalExtra->GetData(), extraLen, true))
            {
                Release(m_LocalExtra);
                m_LocalExtra = NULL;
            }
        }
    }

    return LOCAL_SIZE + nameLen + extraLen;
}

size_t wxZipEntry::WriteLocal(wxOutputStream& stream, wxMBConv& conv) const
{
    wxString unixName = GetName(wxPATH_UNIX);
    const wxWX2MBbuf name_buf = unixName.mb_str(conv);
    const char *name = name_buf;
    if (!name) name = "";
    wxUint16 nameLen = wx_truncate_cast(wxUint16, strlen(name));

    bool z64Required = m_CompressedSize > 0xffffffff || m_Size > 0xffffffff;
    wxUint16 versionNeeded =
        z64Required ? Z64_VERSION_NEEDED_TO_EXTRACT : int(m_VersionNeeded);

    wxDataOutputStream ds(stream);

    ds << versionNeeded << GetInternalFlags(conv.IsUTF8()) << m_Method;
    ds.Write32(GetDateTime().GetAsDOS());

    ds.Write32(m_Crc);
    ds.Write32(m_CompressedSize != wxInvalidOffset ?
               LimitUint32(m_CompressedSize) : 0);
    ds.Write32(m_Size != wxInvalidOffset ?
               LimitUint32(m_Size) : 0);

    ds << nameLen;
    wxUint16 extraLen = wx_truncate_cast(wxUint16, GetLocalExtraLen());
    if (z64Required)
        extraLen += 20; // tag and 2x64bit file sizes
    ds.Write16(extraLen);

    stream.Write(name, nameLen);
    if (z64Required)
    {
        ds.Write16(1);  // id
        ds.Write16(16); // record size
        ds.Write64(static_cast<wxInt64>(m_CompressedSize));
        ds.Write64(static_cast<wxInt64>(m_Size));
    }
    if (GetLocalExtraLen())
        stream.Write(m_LocalExtra->GetData(), GetLocalExtraLen());

    return LOCAL_SIZE + nameLen + extraLen;
}

size_t wxZipEntry::ReadCentral(wxInputStream& stream, wxMBConv& conv)
{
    wxUint16 nameLen, extraLen, commentLen;

    wxZipHeader ds(stream, CENTRAL_SIZE - 4);
    if (!ds)
        return 0;

    ds >> m_VersionMadeBy >> m_SystemMadeBy;

    SetVersionNeeded(ds.Read16());
    SetFlags(ds.Read16());
    SetMethod(ds.Read16());
    SetDateTime(wxDateTime().SetFromDOS(ds.Read32()));
    SetCrc(ds.Read32());
    SetCompressedSize(ds.Read32());
    SetSize(ds.Read32());

    ds >> nameLen >> extraLen >> commentLen
       >> m_DiskStart >> m_InternalAttributes >> m_ExternalAttributes;
    SetOffset(ds.Read32());

    // Another MSVS 2005 workaround, see above (FIXME-VC8).
    wxMBConv& strConv = m_Flags & wxZIP_LANG_ENC_UTF8
                            ? static_cast<wxMBConv&>(wxConvUTF8)
                            : conv;
    SetName(ReadString(stream, nameLen, strConv), wxPATH_UNIX);
    if (stream.LastRead() != nameLen + 0u)
        return 0;

    if (extraLen || GetExtraLen()) {
        Unique(m_Extra, extraLen);
        if (extraLen) {
            stream.Read(m_Extra->GetData(), extraLen);
            if (stream.LastRead() != extraLen + 0u)
                return 0;

            if (LoadExtraInfo(m_Extra->GetData(), extraLen, false))
            {
                Release(m_Extra);
                m_Extra = NULL;
            }
        }
    }

    if (commentLen) {
        m_Comment = ReadString(stream, commentLen, strConv);
        if (stream.LastRead() != commentLen + 0u)
            return 0;
    } else {
        m_Comment.clear();
    }

    return CENTRAL_SIZE + nameLen + extraLen + commentLen;
}

size_t wxZipEntry::WriteCentral(wxOutputStream& stream, wxMBConv& conv) const
{
    wxString unixName = GetName(wxPATH_UNIX);
    const wxWX2MBbuf name_buf = unixName.mb_str(conv);
    const char *name = name_buf;
    if (!name) name = "";
    wxUint16 nameLen = wx_truncate_cast(wxUint16, strlen(name));

    const wxWX2MBbuf comment_buf = m_Comment.mb_str(conv);
    const char *comment = comment_buf;
    if (!comment) comment = "";
    wxUint16 commentLen = wx_truncate_cast(wxUint16, strlen(comment));

    wxUint16 extraLen = wx_truncate_cast(wxUint16, GetExtraLen());
    wxUint16 z64InfoLen = 0;

    bool z64Required = false;
    if ( m_CompressedSize > 0xffffffff )
    {
        z64Required = true;
        z64InfoLen += 8;
    }
    if ( m_Size > 0xffffffff )
    {
        z64Required = true;
        z64InfoLen += 8;
    }
    if ( m_Offset > 0xffffffff )
    {
        z64Required = true;
        z64InfoLen += 8;
    }
    if (z64Required)
    {
        extraLen += 4 + z64InfoLen;
    }

    wxUint16 versionNeeded =
        (z64Required) ? Z64_VERSION_NEEDED_TO_EXTRACT : GetVersionNeeded();

    wxDataOutputStream ds(stream);

    ds << CENTRAL_MAGIC << m_VersionMadeBy << m_SystemMadeBy;

    ds.Write16(versionNeeded);
    ds.Write16(wx_truncate_cast(wxUint16, GetInternalFlags(conv.IsUTF8())));
    ds.Write16(wx_truncate_cast(wxUint16, GetMethod()));
    ds.Write32(GetDateTime().GetAsDOS());
    ds.Write32(GetCrc());
    ds.Write32(LimitUint32(GetCompressedSize()));
    ds.Write32(LimitUint32(GetSize()));
    ds.Write16(nameLen);
    ds.Write16(extraLen);

    ds << commentLen << m_DiskStart << m_InternalAttributes
       << m_ExternalAttributes << LimitUint32(GetOffset());

    stream.Write(name, nameLen);
    if (z64Required)
    {
        ds.Write16(1); // tag
        ds.Write16(z64InfoLen); // record size
        if (m_CompressedSize > 0xffffffff)
            ds.Write64(static_cast<wxInt64>(m_CompressedSize));
        if (m_Size > 0xffffffff)
            ds.Write64(static_cast<wxInt64>(m_Size));
        if (m_Offset > 0xffffffff)
            ds.Write64(static_cast<wxInt64>(m_Offset));
    }
    if (GetExtraLen())
        stream.Write(GetExtra(), GetExtraLen());
    stream.Write(comment, commentLen);

    return CENTRAL_SIZE + nameLen + extraLen + commentLen;
}

// Info-zip prefixes this record with a signature, but pkzip doesn't. So if
// the 1st value is the signature then it is probably an info-zip record,
// though there is a small chance that it is in fact a pkzip record which
// happens to have the signature as its CRC.
//
size_t wxZipEntry::ReadDescriptor(wxInputStream& stream)
{
    wxZipHeader ds(stream, SUMS_SIZE);
    if (!ds)
        return 0;

    m_Crc = ds.Read32();
    m_CompressedSize = ds.Read32();
    m_Size = ds.Read32();

    // if 1st value is the signature then this is probably an info-zip record
    if (m_Crc == SUMS_MAGIC)
    {
        wxZipHeader buf(stream, 8);
        wxUint32 u1 = buf.GetSize() >= 4 ? buf.Read32() : (wxUint32)LOCAL_MAGIC;
        wxUint32 u2 = buf.GetSize() == 8 ? buf.Read32() : 0;

        // look for the signature of the following record to decide which
        if ((u1 == LOCAL_MAGIC || u1 == CENTRAL_MAGIC) &&
            (u2 != LOCAL_MAGIC && u2 != CENTRAL_MAGIC))
        {
            // it's a pkzip style record after all!
            if (buf.GetSize() > 0)
                stream.Ungetch(buf.GetData(), buf.GetSize());
        }
        else
        {
            // it's an info-zip record as expected
            if (buf.GetSize() > 4)
                stream.Ungetch(buf.GetData() + 4, buf.GetSize() - 4);
            m_Crc = wx_truncate_cast(wxUint32, m_CompressedSize);
            m_CompressedSize = m_Size;
            m_Size = u1;
            return SUMS_SIZE + 4;
        }
    }

    return SUMS_SIZE;
}

size_t wxZipEntry::WriteDescriptor(wxOutputStream& stream, wxUint32 crc,
                                   wxFileOffset compressedSize, wxFileOffset size)
{
    m_Crc = crc;
    m_CompressedSize = compressedSize;
    m_Size = size;

    wxDataOutputStream ds(stream);

    ds.Write32(crc);
    ds.Write32(wx_truncate_cast(wxUint32, compressedSize));
    ds.Write32(wx_truncate_cast(wxUint32, size));

    return SUMS_SIZE;
}

// Returns the flags as specified or including the UTF-8 flag if filename
// contains non ASCII characters
wxUint16 wxZipEntry::GetInternalFlags(bool checkForUTF8) const
{
    wxUint16 flags = m_Flags;
    if (checkForUTF8 && (!m_Name.IsAscii() || !m_Comment.IsAscii()))
        flags |= wxZIP_LANG_ENC_UTF8;

    return flags;
}

/////////////////////////////////////////////////////////////////////////////
// wxZipEndRec - holds the end of central directory record

class wxZipEndRec
{
public:
    wxZipEndRec();

    int GetDiskNumber() const                   { return m_DiskNumber; }
    int GetStartDisk() const                    { return m_StartDisk; }
    int GetEntriesHere() const                  { return m_EntriesHere; }
    int GetTotalEntries() const                 { return m_TotalEntries; }
    wxFileOffset GetSize() const                { return m_Size; }
    wxFileOffset GetOffset() const              { return m_Offset; }
    wxString GetComment() const                 { return m_Comment; }

    void SetDiskNumber(int num)
        { m_DiskNumber = wx_truncate_cast(wxUint32, num); }
    void SetStartDisk(int num)
        { m_StartDisk = wx_truncate_cast(wxUint32, num); }
    void SetEntriesHere(int num)
        { m_EntriesHere = wx_truncate_cast(wxUint32, num); }
    void SetTotalEntries(int num)
        { m_TotalEntries = wx_truncate_cast(wxUint32, num); }
    void SetSize(wxFileOffset size)
        { m_Size = wx_truncate_cast(wxUint64, size); }
    void SetOffset(wxFileOffset offset)
        { m_Offset = wx_truncate_cast(wxUint64, offset); }
    void SetComment(const wxString& comment)
        { m_Comment = comment; }

    bool Read(wxInputStream& stream, wxMBConv& conv);
    bool Write(wxOutputStream& stream, wxMBConv& conv) const;

private:
    wxUint32 m_DiskNumber;
    wxUint32 m_StartDisk;
    wxUint64 m_EntriesHere;
    wxUint64 m_TotalEntries;
    wxUint64 m_Size;
    wxUint64 m_Offset;
    wxString m_Comment;
};

wxZipEndRec::wxZipEndRec()
  : m_DiskNumber(0),
    m_StartDisk(0),
    m_EntriesHere(0),
    m_TotalEntries(0),
    m_Size(0),
    m_Offset(0)
{
}

bool wxZipEndRec::Write(wxOutputStream& stream, wxMBConv& conv) const
{
    const wxWX2MBbuf comment_buf = m_Comment.mb_str(conv);
    const char *comment = comment_buf;
    if (!comment) comment = "";
    wxUint16 commentLen = (wxUint16)strlen(comment);

    wxDataOutputStream ds(stream);

    // Check if zip64 is required
    if (m_DiskNumber > 0xffff || m_StartDisk > 0xffff ||
        m_EntriesHere > 0xffff || m_TotalEntries > 0xffff ||
        m_Size > 0xffffffff || m_Offset > 0xffffffff)
    {
        // Write zip64 end of central directory record
        wxFileOffset z64endOffset = stream.TellO();
        ds.Write32(Z64_END_MAGIC);
        ds.Write64(static_cast<wxUint64>(Z64_END_SIZE - 12)); // size of zip64 end record
        ds.Write16(Z64_VERSION_NEEDED_TO_EXTRACT);
        ds.Write16(Z64_VERSION_NEEDED_TO_EXTRACT);
        ds.Write32(m_DiskNumber);
        ds.Write32(m_StartDisk);
        ds.Write64(m_EntriesHere);
        ds.Write64(m_TotalEntries);
        ds.Write64(m_Size);
        ds.Write64(m_Offset);

        // Write zip64 end of central directory locator
        ds.Write32(Z64_LOC_MAGIC);
        ds.Write32(m_StartDisk);
        ds.Write64(static_cast<wxInt64>(z64endOffset));
        ds.Write32(1); // total number of disks
    }

    ds << END_MAGIC;

    ds.Write16(LimitUint16(m_DiskNumber));
    ds.Write16(LimitUint16(m_StartDisk));
    ds.Write16(LimitUint16(m_EntriesHere));
    ds.Write16(LimitUint16(m_TotalEntries));
    ds.Write32(LimitUint32(m_Size));
    ds.Write32(LimitUint32(m_Offset));

    ds << commentLen;

    stream.Write(comment, commentLen);

    return stream.IsOk();
}

bool wxZipEndRec::Read(wxInputStream& stream, wxMBConv& conv)
{
    wxZipHeader ds(stream, END_SIZE - 4);
    if (!ds)
        return false;

    wxUint16 commentLen;

    m_DiskNumber = ds.Read16();
    m_StartDisk = ds.Read16();
    m_EntriesHere = ds.Read16();
    m_TotalEntries = ds.Read16();
    m_Size = ds.Read32();
    m_Offset = ds.Read32();
    ds >> commentLen;

    if (commentLen) {
        m_Comment = ReadString(stream, commentLen, conv);
        if (stream.LastRead() != commentLen + 0u)
            return false;
    }

    if (m_DiskNumber != 0 || m_StartDisk != 0 ||
            m_EntriesHere != m_TotalEntries)
    {
        wxLogWarning(_("assuming this is a multi-part zip concatenated"));
    }

    // Look for zip64 end record
    stream.SeekI(-(END_SIZE+Z64_LOC_SIZE), wxFromCurrent);
    wxZipHeader dsLoc(stream, Z64_LOC_SIZE);
    if ( dsLoc && dsLoc.Read32() == Z64_LOC_MAGIC )
    {
        // Found zip64 locator, read z64 directory
        dsLoc.Read32(); // skip: disk with the start of the zip64
        wxUint64 z64EndOffset = dsLoc.Read64();

        // Read zip64 end of central directory record
        if (stream.SeekI(z64EndOffset) == wxInvalidOffset)
            return false;
        wxZipHeader dsEnd(stream, Z64_END_SIZE);
        if ( dsEnd.Read32() != Z64_END_MAGIC ||
            dsEnd.Read64() < Z64_END_SIZE - 12 ) // Check record size
            return false;

        dsEnd.Read16(); // skip: version made by
        dsEnd.Read16(); // skip: version needed to extract

        m_DiskNumber = dsEnd.Read32();
        m_StartDisk = dsEnd.Read32();
        m_EntriesHere = dsEnd.Read64();
        m_TotalEntries = dsEnd.Read64();
        m_Size = dsEnd.Read64();
        m_Offset = dsEnd.Read64();
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////
// A weak link from an input stream to an output stream

class wxZipStreamLink
{
public:
    wxZipStreamLink(wxZipOutputStream *stream) : m_ref(1), m_stream(stream) { }

    wxZipStreamLink *AddRef() { m_ref++; return this; }
    wxZipOutputStream *GetOutputStream() const { return m_stream; }

    void Release(class wxZipInputStream *WXUNUSED(s))
        { if (--m_ref == 0) delete this; }
    void Release(class wxZipOutputStream *WXUNUSED(s))
        { m_stream = NULL; if (--m_ref == 0) delete this; }

private:
    ~wxZipStreamLink() { }

    int m_ref;
    wxZipOutputStream *m_stream;

    wxSUPPRESS_GCC_PRIVATE_DTOR_WARNING(wxZipStreamLink)
};


/////////////////////////////////////////////////////////////////////////////
// Input stream

// leave the default wxZipEntryPtr free for users
wxDECLARE_SCOPED_PTR(wxZipEntry, wxZipEntryPtr_)
wxDEFINE_SCOPED_PTR (wxZipEntry, wxZipEntryPtr_)

// constructor
//
wxZipInputStream::wxZipInputStream(wxInputStream& stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv)
{
    Init();
}

wxZipInputStream::wxZipInputStream(wxInputStream *stream,
                                   wxMBConv& conv /*=wxConvLocal*/)
  : wxArchiveInputStream(stream, conv)
{
    Init();
}

void wxZipInputStream::Init()
{
    m_store = new wxStoredInputStream(*m_parent_i_stream);
    m_inflate = NULL;
    m_rawin = NULL;
    m_raw = false;
    m_headerSize = 0;
    m_decomp = NULL;
    m_parentSeekable = false;
    m_weaklinks = new wxZipWeakLinks;
    m_streamlink = NULL;
    m_offsetAdjustment = 0;
    m_position = wxInvalidOffset;
    m_signature = 0;
    m_TotalEntries = 0;
    m_lasterror = m_parent_i_stream->GetLastError();
}

wxZipInputStream::~wxZipInputStream()
{
    CloseDecompressor(m_decomp);

    delete m_store;
    delete m_inflate;
    delete m_rawin;

    m_weaklinks->Release(this);

    if (m_streamlink)
        m_streamlink->Release(this);
}

wxString wxZipInputStream::GetComment()
{
    if (m_position == wxInvalidOffset)
        if (!LoadEndRecord())
            return wxEmptyString;

    if (!m_parentSeekable && Eof() && m_signature) {
        m_lasterror = wxSTREAM_NO_ERROR;
        m_lasterror = ReadLocal(true);
    }

    return m_Comment;
}

int wxZipInputStream::GetTotalEntries()
{
    if (m_position == wxInvalidOffset)
        LoadEndRecord();
    return m_TotalEntries;
}

wxZipStreamLink *wxZipInputStream::MakeLink(wxZipOutputStream *out)
{
    wxZipStreamLink *link = NULL;

    if (!m_parentSeekable && (IsOpened() || !Eof())) {
        link = new wxZipStreamLink(out);
        if (m_streamlink)
            m_streamlink->Release(this);
        m_streamlink = link->AddRef();
    }

    return link;
}

bool wxZipInputStream::LoadEndRecord()
{
    wxCHECK(m_position == wxInvalidOffset, false);
    if (!IsOk())
        return false;

    m_position = 0;

    // First find the end-of-central-directory record.
    if (!FindEndRecord()) {
        // failed, so either this is a non-seekable stream (ok), or not a zip
        if (m_parentSeekable) {
            m_lasterror = wxSTREAM_READ_ERROR;
            wxLogError(_("invalid zip file"));
            return false;
        }
        else {
            wxLogNull nolog;
            wxFileOffset pos = m_parent_i_stream->TellI();
            if (pos != wxInvalidOffset)
                m_offsetAdjustment = m_position = pos;
            return true;
        }
    }

    wxZipEndRec endrec;

    // Read in the end record
    const wxFileOffset curPos = m_parent_i_stream->TellI();
    if ( curPos < 4 )
    {
        // Either failed to get the position (if it's negative) or can't be a
        // valid ZIP file anyhow (this probably can't happen here, but it
        // doesn't harm to check).
        return false;
    }

    const size_t endPos = curPos - 4;
    if (!endrec.Read(*m_parent_i_stream, GetConv()))
        return false;

    m_TotalEntries = endrec.GetTotalEntries();
    m_Comment = endrec.GetComment();

    wxUint32 magic = m_TotalEntries ? CENTRAL_MAGIC : END_MAGIC;

    // Now find the central-directory. we have the file offset of
    // the CD, so look there first.
    if (m_parent_i_stream->SeekI(endrec.GetOffset()) != wxInvalidOffset &&
            ReadSignature() == magic) {
        m_signature = magic;
        m_position = endrec.GetOffset();
        m_offsetAdjustment = 0;
        return true;
    }

    // If it's not there, then it could be that the zip has been appended
    // to a self extractor, so take the CD size (also in endrec), subtract
    // it from the file offset of the end-central-directory and look there.
    const wxUint64 recSize = endrec.GetSize();
    if (recSize <= endPos &&
            m_parent_i_stream->SeekI(endPos - recSize) != wxInvalidOffset &&
                ReadSignature() == magic) {
        m_signature = magic;
        m_position = endPos - recSize;
        if ( endrec.GetOffset() >= 0 && endrec.GetOffset() < m_position )
        {
            m_offsetAdjustment = m_position - endrec.GetOffset();
            return true;
        }
    }

    wxLogError(_("can't find central directory in zip"));
    m_lasterror = wxSTREAM_READ_ERROR;
    return false;
}

// Find the end-of-central-directory record.
// If found the stream will be positioned just past the 4 signature bytes.
//
bool wxZipInputStream::FindEndRecord()
{
    if (!m_parent_i_stream->IsSeekable())
        return false;

    // usually it's 22 bytes in size and the last thing in the file
    {
        wxLogNull nolog;
        if (m_parent_i_stream->SeekI(-END_SIZE, wxFromEnd) == wxInvalidOffset)
            return false;
    }

    m_parentSeekable = true;
    m_signature = 0;
    char magic[4];
    if (m_parent_i_stream->Read(magic, 4).LastRead() != 4)
        return false;
    if ((m_signature = CrackUint32(magic)) == END_MAGIC)
        return true;

    // unfortunately, the record has a comment field that can be up to 65535
    // bytes in length, so if the signature not found then search backwards.
    wxFileOffset pos = m_parent_i_stream->TellI();
    const int BUFSIZE = 1024;
    wxCharBuffer buf(BUFSIZE);

    memcpy(buf.data(), magic, 3);
    wxFileOffset minpos = wxMax(pos - 65535L, 0);

    while (pos > minpos) {
        size_t len = wx_truncate_cast(size_t,
                        pos - wxMax(pos - (BUFSIZE - 3), minpos));
        if ( len < 3 )
            break;

        memcpy(buf.data() + len, buf, 3);
        pos -= len;

        if (m_parent_i_stream->SeekI(pos, wxFromStart) == wxInvalidOffset ||
                m_parent_i_stream->Read(buf.data(), len).LastRead() != len)
            return false;

        char *p = buf.data() + len;

        while (p-- > buf.data()) {
            if ((m_signature = CrackUint32(p)) == END_MAGIC) {
                size_t remainder = buf.data() + len - p;
                if (remainder > 4)
                    m_parent_i_stream->Ungetch(p + 4, remainder - 4);
                return true;
            }
        }
    }

    return false;
}

wxZipEntry *wxZipInputStream::GetNextEntry()
{
    if (m_position == wxInvalidOffset)
        if (!LoadEndRecord())
            return NULL;

    m_lasterror = m_parentSeekable ? ReadCentral() : ReadLocal();
    if (!IsOk())
        return NULL;

    wxZipEntryPtr_ entry(new wxZipEntry(m_entry));
    entry->m_backlink = m_weaklinks->AddEntry(entry.get(), entry->GetKey());
    return entry.release();
}

wxStreamError wxZipInputStream::ReadCentral()
{
    if (!AtHeader())
        CloseEntry();

    if (m_signature == END_MAGIC || m_signature == Z64_END_MAGIC)
        return wxSTREAM_EOF;

    if (m_signature != CENTRAL_MAGIC) {
        wxLogError(_("error reading zip central directory"));
        return wxSTREAM_READ_ERROR;
    }

    if (QuietSeek(*m_parent_i_stream, m_position + 4) == wxInvalidOffset)
        return wxSTREAM_READ_ERROR;

    size_t size = m_entry.ReadCentral(*m_parent_i_stream, GetConv());
    if (!size) {
        m_signature = 0;
        return wxSTREAM_READ_ERROR;
    }

    m_position += size;
    m_signature = ReadSignature();

    if (m_offsetAdjustment)
        m_entry.SetOffset(m_entry.GetOffset() + m_offsetAdjustment);
    m_entry.SetKey(m_entry.GetOffset());

    return wxSTREAM_NO_ERROR;
}

wxStreamError wxZipInputStream::ReadLocal(bool readEndRec /*=false*/)
{
    if (!AtHeader())
        CloseEntry();

    if (!m_signature)
        m_signature = ReadSignature();

    if (m_signature == CENTRAL_MAGIC || m_signature == END_MAGIC ||
        m_signature == Z64_END_MAGIC) {
        if (m_streamlink && !m_streamlink->GetOutputStream()) {
            m_streamlink->Release(this);
            m_streamlink = NULL;
        }
    }

    while (m_signature == CENTRAL_MAGIC) {
        if (m_weaklinks->IsEmpty() && m_streamlink == NULL)
            return wxSTREAM_EOF;

        size_t size = m_entry.ReadCentral(*m_parent_i_stream, GetConv());
        m_position += size;
        m_signature = 0;
        if (!size)
            return wxSTREAM_READ_ERROR;

        wxZipEntry *entry = m_weaklinks->GetEntry(m_entry.GetOffset());
        if (entry) {
            entry->SetSystemMadeBy(m_entry.GetSystemMadeBy());
            entry->SetVersionMadeBy(m_entry.GetVersionMadeBy());
            entry->SetComment(m_entry.GetComment());
            entry->SetDiskStart(m_entry.GetDiskStart());
            entry->SetInternalAttributes(m_entry.GetInternalAttributes());
            entry->SetExternalAttributes(m_entry.GetExternalAttributes());
            Copy(entry->m_Extra, m_entry.m_Extra);
            entry->Notify();
            m_weaklinks->RemoveEntry(entry->GetOffset());
        }

        m_signature = ReadSignature();
    }

    if (m_signature == END_MAGIC || m_signature == Z64_END_MAGIC) {
        if (readEndRec || m_streamlink) {
            wxZipEndRec endrec;
            endrec.Read(*m_parent_i_stream, GetConv());
            m_Comment = endrec.GetComment();
            m_signature = 0;
            if (m_streamlink) {
                m_streamlink->GetOutputStream()->SetComment(endrec.GetComment());
                m_streamlink->Release(this);
                m_streamlink = NULL;
            }
        }
        return wxSTREAM_EOF;
    }

    if (m_signature == LOCAL_MAGIC) {
        m_headerSize = m_entry.ReadLocal(*m_parent_i_stream, GetConv());
        m_signature = 0;
        m_entry.SetOffset(m_position);
        m_entry.SetKey(m_position);

        if (m_headerSize) {
            m_TotalEntries++;
            return wxSTREAM_NO_ERROR;
        }
    }

    wxLogError(_("error reading zip local header"));
    return wxSTREAM_READ_ERROR;
}

wxUint32 wxZipInputStream::ReadSignature()
{
    char magic[4];
    m_parent_i_stream->Read(magic, 4);
    return m_parent_i_stream->LastRead() == 4 ? CrackUint32(magic) : 0;
}

bool wxZipInputStream::OpenEntry(wxArchiveEntry& entry)
{
    wxZipEntry *zipEntry = wxStaticCast(&entry, wxZipEntry);
    return zipEntry ? OpenEntry(*zipEntry) : false;
}

// Open an entry
//
bool wxZipInputStream::DoOpen(wxZipEntry *entry, bool raw)
{
    if (m_position == wxInvalidOffset)
        if (!LoadEndRecord())
            return false;
    if (m_lasterror == wxSTREAM_READ_ERROR)
        return false;
    if (IsOpened())
        CloseEntry();

    m_raw = raw;

    if (entry) {
        if (AfterHeader() && entry->GetKey() == m_entry.GetOffset())
            return true;
        // can only open the current entry on a non-seekable stream
        wxCHECK(m_parentSeekable, false);
    }

    m_lasterror = wxSTREAM_READ_ERROR;

    if (entry)
        m_entry = *entry;

    if (m_parentSeekable) {
        if (QuietSeek(*m_parent_i_stream, m_entry.GetOffset())
                == wxInvalidOffset)
            return false;
        if (ReadSignature() != LOCAL_MAGIC) {
            wxLogError(_("bad zipfile offset to entry"));
            return false;
        }
    }

    if (m_parentSeekable || AtHeader()) {
        m_headerSize = m_entry.ReadLocal(*m_parent_i_stream, GetConv());
        if (m_headerSize && m_parentSeekable) {
            wxZipEntry *ref = m_weaklinks->GetEntry(m_entry.GetKey());
            if (ref) {
                Copy(ref->m_LocalExtra, m_entry.m_LocalExtra);
                ref->Notify();
                m_weaklinks->RemoveEntry(ref->GetKey());
            }
            if (entry && entry != ref) {
                Copy(entry->m_LocalExtra, m_entry.m_LocalExtra);
                entry->Notify();
            }
        }
    }

    if (m_headerSize)
        m_lasterror = wxSTREAM_NO_ERROR;
    return IsOk();
}

bool wxZipInputStream::OpenDecompressor(bool raw /*=false*/)
{
    wxASSERT(AfterHeader());

    wxFileOffset compressedSize = m_entry.GetCompressedSize();

    if (raw)
        m_raw = true;

    if (m_raw) {
        if (compressedSize != wxInvalidOffset) {
            m_store->Open(compressedSize);
            m_decomp = m_store;
        } else {
            if (!m_rawin)
                m_rawin = new wxRawInputStream(*m_parent_i_stream);
            m_decomp = m_rawin->Open(OpenDecompressor(m_rawin->GetTee()));
        }
    } else {
        if (compressedSize != wxInvalidOffset &&
                (m_entry.GetMethod() != wxZIP_METHOD_DEFLATE ||
                 wxZlibInputStream::CanHandleGZip())) {
            m_store->Open(compressedSize);
            m_decomp = OpenDecompressor(*m_store);
        } else {
            m_decomp = OpenDecompressor(*m_parent_i_stream);
        }
    }

    m_crcAccumulator = crc32(0, Z_NULL, 0);
    m_lasterror = m_decomp ? m_decomp->GetLastError() : wxSTREAM_READ_ERROR;
    return IsOk();
}

// Can be overridden to add support for additional decompression methods
//
wxInputStream *wxZipInputStream::OpenDecompressor(wxInputStream& stream)
{
    switch (m_entry.GetMethod()) {
        case wxZIP_METHOD_STORE:
            if (m_entry.GetSize() == wxInvalidOffset) {
                wxLogError(_("stored file length not in Zip header"));
                break;
            }
            m_store->Open(m_entry.GetSize());
            return m_store;

        case wxZIP_METHOD_DEFLATE:
            if (!m_inflate)
                m_inflate = new wxZlibInputStream2(stream);
            else
                m_inflate->Open(stream);
            return m_inflate;

        default:
            wxLogError(_("unsupported Zip compression method"));
    }

    return NULL;
}

bool wxZipInputStream::CloseDecompressor(wxInputStream *decomp)
{
    if (decomp && decomp == m_rawin)
        return CloseDecompressor(m_rawin->GetFilterInputStream());
    if (decomp != m_store && decomp != m_inflate)
        delete decomp;
    return true;
}

// Closes the current entry and positions the underlying stream at the start
// of the next entry
//
bool wxZipInputStream::CloseEntry()
{
    if (AtHeader())
        return true;
    if (m_lasterror == wxSTREAM_READ_ERROR)
        return false;

    if (!m_parentSeekable) {
        if (!IsOpened() && !OpenDecompressor(true))
            return false;

        const int BUFSIZE = 8192;
        wxCharBuffer buf(BUFSIZE);
        while (IsOk())
            Read(buf.data(), BUFSIZE);

        m_position += m_headerSize + m_entry.GetCompressedSize();
    }

    if (m_lasterror == wxSTREAM_EOF)
        m_lasterror = wxSTREAM_NO_ERROR;

    CloseDecompressor(m_decomp);
    m_decomp = NULL;
    m_entry = wxZipEntry();
    m_headerSize = 0;
    m_raw = false;

    return IsOk();
}

size_t wxZipInputStream::OnSysRead(void *buffer, size_t size)
{
    if (!IsOpened())
        if ((AtHeader() && !DoOpen()) || !OpenDecompressor())
            m_lasterror = wxSTREAM_READ_ERROR;
    if (!IsOk() || !size)
        return 0;

    size_t count = m_decomp->Read(buffer, size).LastRead();
    if (!m_raw)
        m_crcAccumulator = crc32(m_crcAccumulator, (Byte*)buffer, count);
    if (count < size)
        m_lasterror = m_decomp->GetLastError();

    if (Eof()) {
        if ((m_entry.GetFlags() & wxZIP_SUMS_FOLLOW) != 0) {
            m_headerSize += m_entry.ReadDescriptor(*m_parent_i_stream);
            wxZipEntry *entry = m_weaklinks->GetEntry(m_entry.GetKey());

            if (entry) {
                entry->SetCrc(m_entry.GetCrc());
                entry->SetCompressedSize(m_entry.GetCompressedSize());
                entry->SetSize(m_entry.GetSize());
                entry->Notify();
            }
        }

        if (!m_raw) {
            m_lasterror = wxSTREAM_READ_ERROR;

            if (m_entry.GetSize() != TellI())
            {
                wxLogError(_("reading zip stream (entry %s): bad length"),
                           m_entry.GetName().c_str());
            }
            else if (m_crcAccumulator != m_entry.GetCrc())
            {
                wxLogError(_("reading zip stream (entry %s): bad crc"),
                           m_entry.GetName().c_str());
            }
            else
            {
                m_lasterror = wxSTREAM_EOF;
            }
        }
    }

    return count;
}

/////////////////////////////////////////////////////////////////////////////
// Output stream

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxZipEntryList_)

wxZipOutputStream::wxZipOutputStream(wxOutputStream& stream,
                                     int level      /*=-1*/,
                                     wxMBConv& conv /*=wxConvUTF8*/)
  : wxArchiveOutputStream(stream, conv)
{
    Init(level);
}

wxZipOutputStream::wxZipOutputStream(wxOutputStream *stream,
                                     int level      /*=-1*/,
                                     wxMBConv& conv /*=wxConvUTF8*/)
  : wxArchiveOutputStream(stream, conv)
{
    Init(level);
}

void wxZipOutputStream::Init(int level)
{
    m_store = new wxStoredOutputStream(*m_parent_o_stream);
    m_deflate = NULL;
    m_backlink = NULL;
    m_initialData = new char[OUTPUT_LATENCY];
    m_initialSize = 0;
    m_pending = NULL;
    m_raw = false;
    m_headerOffset = 0;
    m_headerSize = 0;
    m_entrySize = 0;
    m_comp = NULL;
    m_level = level;
    m_offsetAdjustment = wxInvalidOffset;
    m_endrecWritten = false;
}

wxZipOutputStream::~wxZipOutputStream()
{
    Close();
    WX_CLEAR_LIST(wxZipEntryList_, m_entries);
    delete m_store;
    delete m_deflate;
    delete m_pending;
    delete [] m_initialData;
    if (m_backlink)
        m_backlink->Release(this);
}

bool wxZipOutputStream::PutNextEntry(
    const wxString& name,
    const wxDateTime& dt /*=wxDateTime::Now()*/,
    wxFileOffset size    /*=wxInvalidOffset*/)
{
    return PutNextEntry(new wxZipEntry(name, dt, size));
}

bool wxZipOutputStream::PutNextDirEntry(
    const wxString& name,
    const wxDateTime& dt /*=wxDateTime::Now()*/)
{
    wxZipEntry *entry = new wxZipEntry(name, dt);
    entry->SetIsDir();
    return PutNextEntry(entry);
}

bool wxZipOutputStream::CopyEntry(wxZipEntry *entry,
                                  wxZipInputStream& inputStream)
{
    wxZipEntryPtr_ e(entry);

    return
        inputStream.DoOpen(e.get(), true) &&
        DoCreate(e.release(), true) &&
        Write(inputStream).IsOk() && inputStream.Eof();
}

bool wxZipOutputStream::PutNextEntry(wxArchiveEntry *entry)
{
    wxZipEntry *zipEntry = wxStaticCast(entry, wxZipEntry);
    if (!zipEntry)
        delete entry;
    return PutNextEntry(zipEntry);
}

bool wxZipOutputStream::CopyEntry(wxArchiveEntry *entry,
                                  wxArchiveInputStream& stream)
{
    wxZipEntry *zipEntry = wxStaticCast(entry, wxZipEntry);

    if (!zipEntry || !stream.OpenEntry(*zipEntry)) {
        delete entry;
        return false;
    }

    return CopyEntry(zipEntry, static_cast<wxZipInputStream&>(stream));
}

bool wxZipOutputStream::CopyArchiveMetaData(wxZipInputStream& inputStream)
{
    m_Comment = inputStream.GetComment();
    if (m_backlink)
        m_backlink->Release(this);
    m_backlink = inputStream.MakeLink(this);
    return true;
}

bool wxZipOutputStream::CopyArchiveMetaData(wxArchiveInputStream& stream)
{
    return CopyArchiveMetaData(static_cast<wxZipInputStream&>(stream));
}

void wxZipOutputStream::SetLevel(int level)
{
    if (level != m_level) {
        if (m_comp != m_deflate)
            delete m_deflate;
        m_deflate = NULL;
        m_level = level;
    }
}

bool wxZipOutputStream::DoCreate(wxZipEntry *entry, bool raw /*=false*/)
{
    CloseEntry();

    m_pending = entry;
    if (!m_pending)
        return false;

    // write the signature bytes right away
    wxDataOutputStream ds(*m_parent_o_stream);
    ds << LOCAL_MAGIC;

    // and if this is the first entry test for seekability
    if (m_headerOffset == 0 && m_parent_o_stream->IsSeekable()) {
#if wxUSE_LOG
        bool logging = wxLog::IsEnabled();
        wxLogNull nolog;
#endif // wxUSE_LOG
        wxFileOffset here = m_parent_o_stream->TellO();

        if (here != wxInvalidOffset && here >= 4) {
            if (m_parent_o_stream->SeekO(here - 4) == here - 4) {
                m_offsetAdjustment = here - 4;
#if wxUSE_LOG
                wxLog::EnableLogging(logging);
#endif // wxUSE_LOG
                m_parent_o_stream->SeekO(here);
            }
        }
    }

    m_pending->SetOffset(m_headerOffset);

    m_crcAccumulator = crc32(0, Z_NULL, 0);

    if (raw)
        m_raw = true;

    m_lasterror = wxSTREAM_NO_ERROR;
    return true;
}

// Can be overridden to add support for additional compression methods
//
wxOutputStream *wxZipOutputStream::OpenCompressor(
    wxOutputStream& stream,
    wxZipEntry& entry,
    const Buffer bufs[])
{
    if (entry.GetMethod() == wxZIP_METHOD_DEFAULT) {
        if (GetLevel() == 0
                && (IsParentSeekable()
                    || entry.GetCompressedSize() != wxInvalidOffset
                    || entry.GetSize() != wxInvalidOffset)) {
            entry.SetMethod(wxZIP_METHOD_STORE);
        } else {
            int size = 0;
            for (int i = 0; bufs[i].m_data; ++i)
                size += bufs[i].m_size;
            entry.SetMethod(size <= 6 ?
                            wxZIP_METHOD_STORE : wxZIP_METHOD_DEFLATE);
        }
    }

    switch (entry.GetMethod()) {
        case wxZIP_METHOD_STORE:
            if (entry.GetCompressedSize() == wxInvalidOffset)
                entry.SetCompressedSize(entry.GetSize());
            return m_store;

        case wxZIP_METHOD_DEFLATE:
        {
            int defbits = wxZIP_DEFLATE_NORMAL;
            switch (GetLevel()) {
                case 0: case 1:
                    defbits = wxZIP_DEFLATE_SUPERFAST;
                    break;
                case 2: case 3: case 4:
                    defbits = wxZIP_DEFLATE_FAST;
                    break;
                case 8: case 9:
                    defbits = wxZIP_DEFLATE_EXTRA;
                    break;
            }
            entry.SetFlags((entry.GetFlags() & ~wxZIP_DEFLATE_MASK) |
                            defbits | wxZIP_SUMS_FOLLOW);

            if (!m_deflate)
                m_deflate = new wxZlibOutputStream2(stream, GetLevel());
            else
                m_deflate->Open(stream);

            return m_deflate;
        }

        default:
            wxLogError(_("unsupported Zip compression method"));
    }

    return NULL;
}

bool wxZipOutputStream::CloseCompressor(wxOutputStream *comp)
{
    if (comp == m_deflate)
        m_deflate->Close();
    else if (comp != m_store)
        delete comp;
    return true;
}

// This is called when OUPUT_LATENCY bytes has been written to the
// wxZipOutputStream to actually create the zip entry.
//
void wxZipOutputStream::CreatePendingEntry(const void *buffer, size_t size)
{
    wxASSERT(IsOk() && m_pending && !m_comp);
    wxZipEntryPtr_ spPending(m_pending);
    m_pending = NULL;

    Buffer bufs[] = {
        { m_initialData, m_initialSize },
        { (const char*)buffer, size },
        { NULL, 0 }
    };

    if (m_raw)
        m_comp = m_store;
    else
        m_comp = OpenCompressor(*m_store, *spPending,
                                m_initialSize ? bufs : bufs + 1);

    if (IsParentSeekable()
        || (spPending->m_Crc
            && spPending->m_CompressedSize != wxInvalidOffset
            && spPending->m_Size != wxInvalidOffset))
        spPending->m_Flags &= ~wxZIP_SUMS_FOLLOW;
    else
        if (spPending->m_CompressedSize != wxInvalidOffset)
            spPending->m_Flags |= wxZIP_SUMS_FOLLOW;

    m_headerSize = spPending->WriteLocal(*m_parent_o_stream, GetConv());
    m_lasterror = m_parent_o_stream->GetLastError();

    if (IsOk()) {
        m_entries.push_back(spPending.release());
        OnSysWrite(m_initialData, m_initialSize);
    }

    m_initialSize = 0;
}

// This is called to write out the zip entry when Close has been called
// before OUTPUT_LATENCY bytes has been written to the wxZipOutputStream.
//
void wxZipOutputStream::CreatePendingEntry()
{
    wxASSERT(IsOk() && m_pending && !m_comp);
    wxZipEntryPtr_ spPending(m_pending);
    m_pending = NULL;
    m_lasterror = wxSTREAM_WRITE_ERROR;

    if (!m_raw) {
        // Initially compresses the data to memory, then fall back to 'store'
        // if the compressor makes the data larger rather than smaller.
        wxMemoryOutputStream mem;
        Buffer bufs[] = { { m_initialData, m_initialSize }, { NULL, 0 } };
        wxOutputStream *comp = OpenCompressor(mem, *spPending, bufs);

        if (!comp)
            return;
        if (comp != m_store) {
            bool ok = comp->Write(m_initialData, m_initialSize).IsOk();
            CloseCompressor(comp);
            if (!ok)
                return;
        }

        m_entrySize = m_initialSize;
        m_crcAccumulator = crc32(0, (Byte*)m_initialData, m_initialSize);

        if (mem.GetSize() > 0 && mem.GetSize() < m_initialSize) {
            m_initialSize = mem.GetSize();
            mem.CopyTo(m_initialData, m_initialSize);
        } else {
            spPending->SetMethod(wxZIP_METHOD_STORE);
        }

        spPending->SetSize(m_entrySize);
        spPending->SetCrc(m_crcAccumulator);
        spPending->SetCompressedSize(m_initialSize);
    }

    spPending->m_Flags &= ~wxZIP_SUMS_FOLLOW;
    m_headerSize = spPending->WriteLocal(*m_parent_o_stream, GetConv());

    if (m_parent_o_stream->IsOk()) {
        m_entries.push_back(spPending.release());
        m_comp = m_store;
        m_store->Write(m_initialData, m_initialSize);
    }

    m_initialSize = 0;
    m_lasterror = m_parent_o_stream->GetLastError();
}

// Write the 'central directory' and the 'end-central-directory' records.
//
bool wxZipOutputStream::Close()
{
    CloseEntry();

    if (m_lasterror == wxSTREAM_WRITE_ERROR
        || (m_entries.size() == 0 && m_endrecWritten))
    {
        wxFilterOutputStream::Close();
        return false;
    }

    wxZipEndRec endrec;

    endrec.SetEntriesHere(m_entries.size());
    endrec.SetTotalEntries(m_entries.size());
    endrec.SetOffset(m_headerOffset);
    endrec.SetComment(m_Comment);

    wxZipEntryList_::iterator it;
    wxFileOffset size = 0;

    for (it = m_entries.begin(); it != m_entries.end(); ++it) {
        size += (*it)->WriteCentral(*m_parent_o_stream, GetConv());
        delete *it;
    }
    m_entries.clear();

    endrec.SetSize(size);
    endrec.Write(*m_parent_o_stream, GetConv());

    m_lasterror = m_parent_o_stream->GetLastError();
    m_endrecWritten = true;

    if (!wxFilterOutputStream::Close() || !IsOk())
        return false;
    m_lasterror = wxSTREAM_EOF;
    return true;
}

// Finish writing the current entry
//
bool wxZipOutputStream::CloseEntry()
{
    if (IsOk() && m_pending)
        CreatePendingEntry();
    if (!IsOk())
        return false;
    if (!m_comp)
        return true;

    CloseCompressor(m_comp);
    m_comp = NULL;

    wxFileOffset compressedSize = m_store->TellO();

    wxZipEntry& entry = *m_entries.back();

    // When writing raw the crc and size can't be checked
    if (m_raw) {
        m_crcAccumulator = entry.GetCrc();
        m_entrySize = entry.GetSize();
    }

    // Write the sums in the trailing 'data descriptor' if necessary
    if (entry.m_Flags & wxZIP_SUMS_FOLLOW) {
        wxASSERT(!IsParentSeekable());
        m_headerOffset +=
            entry.WriteDescriptor(*m_parent_o_stream, m_crcAccumulator,
                                  compressedSize, m_entrySize);
        m_lasterror = m_parent_o_stream->GetLastError();
    }

    // If the local header didn't have the correct crc and size written to
    // it then seek back and fix it
    else if (m_crcAccumulator != entry.GetCrc()
            || m_entrySize != entry.GetSize()
            || compressedSize != entry.GetCompressedSize())
    {
        if (IsParentSeekable()) {
            wxFileOffset here = m_parent_o_stream->TellO();
            wxFileOffset headerOffset = m_headerOffset + m_offsetAdjustment;
            m_parent_o_stream->SeekO(headerOffset + SUMS_OFFSET);
            entry.WriteDescriptor(*m_parent_o_stream, m_crcAccumulator,
                                  compressedSize, m_entrySize);
            m_parent_o_stream->SeekO(here);
            m_lasterror = m_parent_o_stream->GetLastError();
        } else {
            m_lasterror = wxSTREAM_WRITE_ERROR;
        }
    }

    m_headerOffset += m_headerSize + compressedSize;
    m_headerSize = 0;
    m_entrySize = 0;
    m_store->Close();
    m_raw = false;

    if (IsOk())
        m_lasterror = m_parent_o_stream->GetLastError();
    else
        wxLogError(_("error writing zip entry '%s': bad crc or length"),
                   entry.GetName().c_str());
    return IsOk();
}

void wxZipOutputStream::Sync()
{
    if (IsOk() && m_pending)
        CreatePendingEntry(NULL, 0);
    if (!m_comp)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    if (IsOk()) {
        m_comp->Sync();
        m_lasterror = m_comp->GetLastError();
    }
}

size_t wxZipOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    if (IsOk() && m_pending) {
        if (m_initialSize + size < OUTPUT_LATENCY) {
            memcpy(m_initialData + m_initialSize, buffer, size);
            m_initialSize += size;
            return size;
        } else {
            CreatePendingEntry(buffer, size);
        }
    }

    if (!m_comp)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    if (!IsOk() || !size)
        return 0;

    if (m_comp->Write(buffer, size).LastWrite() != size)
        m_lasterror = wxSTREAM_WRITE_ERROR;
    m_crcAccumulator = crc32(m_crcAccumulator, (Byte*)buffer, size);
    m_entrySize += m_comp->LastWrite();

    return m_comp->LastWrite();
}

#endif // wxUSE_ZIPSTREAM
