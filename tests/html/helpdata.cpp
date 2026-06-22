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

#endif // wxUSE_HTML
