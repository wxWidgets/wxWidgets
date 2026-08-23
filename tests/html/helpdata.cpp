///////////////////////////////////////////////////////////////////////////////
// Name:        tests/html/helpdata.cpp
// Purpose:     wxHtmlHelpData tests
// Author:      dxbjavid
// Copyright:   (c) 2026 wxWidgets dev team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_HTML

#include "wx/html/helpdata.h"
#include "wx/mstream.h"

#include <cstring>

namespace
{

// helpers writing the .cached binary help book format exactly the way
// wxHtmlHelpData::SaveCachedBook() does, so the stream we build is what
// LoadCachedBook() expects to read

void CacheWriteInt32(wxMemoryOutputStream& s, wxInt32 value)
{
    wxInt32 x = wxINT32_SWAP_ON_BE(value);
    s.Write(&x, sizeof(x));
}

void CacheWriteString(wxMemoryOutputStream& s, const char* str)
{
    const size_t len = strlen(str) + 1;
    CacheWriteInt32(s, (wxInt32)len);
    s.Write(str, len);
}

// expose the protected loader
class TestHelpData : public wxHtmlHelpData
{
public:
    bool DoLoadCachedBook(wxHtmlBookRecord* book, wxInputStream& f)
    {
        return LoadCachedBook(book, &f);
    }
};

} // anonymous namespace

// A crafted .cached file whose single index entry carries a parent
// back-reference pointing outside the index must be rejected and must not
// read out of bounds (it previously did, see helpdata.cpp:LoadCachedBook).
TEST_CASE("wxHtmlHelpData::BadCachedParent", "[html][help][error]")
{
    wxMemoryOutputStream os;
    CacheWriteInt32(os, 5); // CURRENT_CACHED_BOOK_VERSION
    CacheWriteInt32(os, 1); // CACHED_BOOK_FORMAT_FLAGS

    CacheWriteInt32(os, 0); // contents count

    CacheWriteInt32(os, 1); // index count
    CacheWriteString(os, "name");
    CacheWriteString(os, "page.htm");
    CacheWriteInt32(os, 1);       // level
    CacheWriteInt32(os, 1000000); // parentShift, way past the index size

    wxStreamBuffer* buf = os.GetOutputStreamBuffer();
    wxMemoryInputStream is(buf->GetBufferStart(), buf->GetIntPosition());

    wxHtmlBookRecord book("test.hhp", "", "Test", "page.htm");
    TestHelpData data;

    // before the fix this indexes m_index out of bounds; with it the file is
    // simply rejected
    CHECK( !data.DoLoadCachedBook(&book, is) );
}

namespace
{

// Write a string field whose declared length is decided by the caller instead
// of being derived from the payload, which is what CacheWriteString() does and
// so what a hostile file can do.
void CacheWriteRawString(wxMemoryOutputStream& s, wxInt32 declaredLen,
                         const char* payload, size_t payloadLen)
{
    CacheWriteInt32(s, declaredLen);
    if ( payloadLen )
        s.Write(payload, payloadLen);
}

// A .cached file carrying a single contents item, whose name field is written
// by the caller.
bool LoadWithContentsName(wxInt32 declaredLen,
                          const char* payload, size_t payloadLen)
{
    wxMemoryOutputStream os;
    CacheWriteInt32(os, 5); // CURRENT_CACHED_BOOK_VERSION
    CacheWriteInt32(os, 1); // CACHED_BOOK_FORMAT_FLAGS

    CacheWriteInt32(os, 1); // contents count
    CacheWriteInt32(os, 0); // level
    CacheWriteInt32(os, 0); // id
    CacheWriteRawString(os, declaredLen, payload, payloadLen);
    CacheWriteString(os, "page.htm");

    CacheWriteInt32(os, 0); // index count

    wxStreamBuffer* buf = os.GetOutputStreamBuffer();
    wxMemoryInputStream is(buf->GetBufferStart(), buf->GetIntPosition());

    wxHtmlBookRecord book("test.hhp", "", "Test", "page.htm");
    TestHelpData data;
    return data.DoLoadCachedBook(&book, is);
}

} // anonymous namespace

// CacheWriteString() counts the trailing NUL, so a length of 0 cannot occur in
// a file it produced. Reading one used to compute a buffer size of len - 1,
// which underflows to SIZE_MAX, and wxCharBuffer then allocated (SIZE_MAX + 1)
// bytes, i.e. none, before writing a NUL one byte in front of the block.
TEST_CASE("wxHtmlHelpData::CachedStringZeroLength", "[html][help][error]")
{
    CHECK( !LoadWithContentsName(0, nullptr, 0) );
}

// A length that is not followed by a terminator used to leave the wxString
// constructor scanning past the end of the buffer.
TEST_CASE("wxHtmlHelpData::CachedStringUnterminated", "[html][help][error]")
{
    CHECK( !LoadWithContentsName(8, "AAAAAAAA", 8) );
}

// A string cannot be longer than the file holding it.
TEST_CASE("wxHtmlHelpData::CachedStringTooLong", "[html][help][error]")
{
    CHECK( !LoadWithContentsName(0x7FFFFFFF, nullptr, 0) );
}

// Nor can a negative length, which used to be cast to a huge size_t.
TEST_CASE("wxHtmlHelpData::CachedStringNegativeLength", "[html][help][error]")
{
    CHECK( !LoadWithContentsName(-1, nullptr, 0) );
}

// The item counts are used to reserve memory before anything is read, so they
// have to be bounded by the file as well.
TEST_CASE("wxHtmlHelpData::CachedCountTooLarge", "[html][help][error]")
{
    wxMemoryOutputStream os;
    CacheWriteInt32(os, 5);
    CacheWriteInt32(os, 1);
    CacheWriteInt32(os, 0x7FFFFFF0); // contents count, far beyond the file

    wxStreamBuffer* buf = os.GetOutputStreamBuffer();
    wxMemoryInputStream is(buf->GetBufferStart(), buf->GetIntPosition());

    wxHtmlBookRecord book("test.hhp", "", "Test", "page.htm");
    TestHelpData data;

    CHECK( !data.DoLoadCachedBook(&book, is) );
}

#endif // wxUSE_HTML
