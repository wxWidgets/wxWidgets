///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/textwrap.cpp
// Purpose:     wxTextWrapper unit test
// Author:      Vadim Zeitlin
// Created:     2025-01-19
// Copyright:   (c) 2025 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // !PCH

#include "wx/textwrapper.h"

// ----------------------------------------------------------------------------
// wrapper implementation for testing
// ----------------------------------------------------------------------------

namespace
{

class HardBreakWrapper : public wxTextWrapper
{
public:
    HardBreakWrapper() { }

    // Return the window used for wrapping: for now, use the main frame because
    // it doesn't really matter which one we use.
    wxWindow* GetWindow() const
    {
        return wxTheApp->GetTopWindow();
    }

    // Helper function returning the width of the given text.
    int GetExtent(const wxString& text) const
    {
        return GetWindow()->GetTextExtent(text).x;
    }

    // Wrap and return the number of lines in the wrapped text.
    size_t Do(const wxString& text, int width)
    {
        Wrap(GetWindow(), text, width);

        return m_lines.size();
    }

    const wxString& GetLine(size_t n) const
    {
        REQUIRE( n < m_lines.size() );

        return m_lines[n];
    }

    wxString GetResult() const { return wxJoin(m_lines, '\n'); }

protected:
    void OnOutputLine(const wxString& line) wxOVERRIDE { m_lines.push_back(line); }

private:
    wxArrayString m_lines;
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// the tests
// ----------------------------------------------------------------------------

TEST_CASE("wxTextWrapper::Wrap", "[text]")
{
    // Note that this text shouldn't contain line breaks.
    const wxString text =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua."
        ;

    HardBreakWrapper w;

    // Check that not wrapping the text works.
    SECTION("None")
    {
        REQUIRE( w.Do(text, -1) == 1 );
        CHECK( w.GetLine(0) == text );
    }

    SECTION("Wide")
    {
        REQUIRE( w.Do(text, 200*w.GetExtent("x")) == 1 );
        CHECK( w.GetLine(0) == text );
    }

    // Check that wrapping the text using reasonable width works.
    SECTION("Normal")
    {
        const size_t n = w.Do(text, 40*w.GetExtent("x"));
        INFO("Wrapped text:\n" << w.GetResult() << "\n");
        CHECK( n >= 3 );
    }

    SECTION("Narrow")
    {
        const size_t n = w.Do(text, w.GetExtent("Lorum ipsum dolor"));
        INFO("Wrapped text:\n" << w.GetResult() << "\n");
        CHECK( n >= 8 );
        CHECK( w.GetLine(0) == "Lorem ipsum dolor" );
    }

    // Make the window too narrow to fit the word "consectetur" and check that
    // the text is still wrapped reasonableness well.
    SECTION("Thin")
    {
        const size_t n = w.Do(text, w.GetExtent("Lorum"));
        INFO("Wrapped text:\n" << w.GetResult() << "\n");
        CHECK( n > 10 );
        CHECK( w.GetLine(0) == "Lorem" );

        // There should be just one line with the word "consectetur", longer
        // than the wrap width.
        bool found = false;
        for ( size_t i = 1; i < n; ++i )
        {
            if ( w.GetLine(i) == "consectetur" )
            {
                found = true;
                break;
            }
        }

        CHECK( found );
    }

    // Make the window too narrow to fit even a single character so that
    // wrapped text has one word per line.
    SECTION("Narrowest")
    {
        const size_t n = w.Do(text, 1);
        INFO("Wrapped text:\n" << w.GetResult() << "\n");
        REQUIRE( n == static_cast<size_t>(text.Freq(' ')) + 1 );
        CHECK( w.GetLine(0) == "Lorem" );
        CHECK( w.GetLine(5) == "consectetur" );
    }
}
