///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/markup.cpp
// Purpose:     wxMarkupParser and related classes unit tests
// Author:      Vadim Zeitlin
// Created:     2011-02-17
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/private/markupparser.h"

class MarkupTestCase : public CppUnit::TestCase
{
public:
    MarkupTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MarkupTestCase );
        CPPUNIT_TEST( RoundTrip );
        CPPUNIT_TEST( Quote );
        CPPUNIT_TEST( Strip );
    CPPUNIT_TEST_SUITE_END();

    void RoundTrip();
    void Quote();
    void Strip();

    wxDECLARE_NO_COPY_CLASS(MarkupTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MarkupTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MarkupTestCase, "MarkupTestCase" );

void MarkupTestCase::RoundTrip()
{
    // Define a wxMarkupParserOutput object which produces the same markup
    // string on output. This is, of course, perfectly useless, but allows us
    // to test that parsing works as expected.
    class RoundTripOutput : public wxMarkupParserOutput
    {
    public:
        RoundTripOutput() { }

        void Reset() { m_text.clear(); }

        const wxString& GetText() const { return m_text; }


        virtual void OnText(const wxString& text) wxOVERRIDE { m_text += text; }

        virtual void OnBoldStart() wxOVERRIDE { m_text += "<b>"; }
        virtual void OnBoldEnd() wxOVERRIDE { m_text += "</b>"; }

        virtual void OnItalicStart() wxOVERRIDE { m_text += "<i>"; }
        virtual void OnItalicEnd() wxOVERRIDE { m_text += "</i>"; }

        virtual void OnUnderlinedStart() wxOVERRIDE { m_text += "<u>"; }
        virtual void OnUnderlinedEnd() wxOVERRIDE { m_text += "</u>"; }

        virtual void OnStrikethroughStart() wxOVERRIDE { m_text += "<s>"; }
        virtual void OnStrikethroughEnd() wxOVERRIDE { m_text += "</s>"; }

        virtual void OnBigStart() wxOVERRIDE { m_text += "<big>"; }
        virtual void OnBigEnd() wxOVERRIDE { m_text += "</big>"; }

        virtual void OnSmallStart() wxOVERRIDE { m_text += "<small>"; }
        virtual void OnSmallEnd() wxOVERRIDE { m_text += "</small>"; }

        virtual void OnTeletypeStart() wxOVERRIDE { m_text += "<tt>"; }
        virtual void OnTeletypeEnd() wxOVERRIDE { m_text += "</tt>"; }

        virtual void OnSpanStart(const wxMarkupSpanAttributes& attrs) wxOVERRIDE
        {
            m_text << "<span";

            if ( !attrs.m_fgCol.empty() )
                m_text << " foreground='" << attrs.m_fgCol << "'";

            if ( !attrs.m_bgCol.empty() )
                m_text << " background='" << attrs.m_bgCol << "'";

            if ( !attrs.m_fontFace.empty() )
                m_text << " face='" << attrs.m_fontFace << "'";

            wxString size;
            switch ( attrs.m_sizeKind )
            {
                case wxMarkupSpanAttributes::Size_Unspecified:
                    break;

                case wxMarkupSpanAttributes::Size_Relative:
                    size << (attrs.m_fontSize > 0 ? "larger" : "smaller");
                    break;

                case wxMarkupSpanAttributes::Size_Symbolic:
                    {
                        CPPUNIT_ASSERT( attrs.m_fontSize >= -3 );
                        CPPUNIT_ASSERT( attrs.m_fontSize <= 3 );
                        static const char *cssSizes[] =
                        {
                            "xx-small", "x-small", "small",
                            "medium",
                            "large", "x-large", "xx-large",
                        };

                        size << cssSizes[attrs.m_fontSize + 3];
                    }
                    break;

                case wxMarkupSpanAttributes::Size_PointParts:
                    size.Printf("%u", attrs.m_fontSize);
                    break;
            }

            if ( !size.empty() )
                m_text << " size='" << size << '\'';

            // TODO: Handle the rest of attributes.

            m_text << ">";
        }

        virtual void OnSpanEnd(const wxMarkupSpanAttributes& WXUNUSED(attrs)) wxOVERRIDE
        {
            m_text += "</span>";
        }

    private:
        wxString m_text;
    };


    RoundTripOutput output;
    wxMarkupParser parser(output);

    #define CHECK_PARSES_OK(text) \
        output.Reset(); \
        CPPUNIT_ASSERT( parser.Parse(text) ); \
        CPPUNIT_ASSERT_EQUAL( text, output.GetText() )

    #define CHECK_PARSES_AS(text, result) \
        output.Reset(); \
        CPPUNIT_ASSERT( parser.Parse(text) ); \
        CPPUNIT_ASSERT_EQUAL( result, output.GetText() )

    #define CHECK_DOESNT_PARSE(text) \
        CPPUNIT_ASSERT( !parser.Parse(text) )

    CHECK_PARSES_OK( "" );
    CHECK_PARSES_OK( "foo" );
    CHECK_PARSES_OK( "foo<b>bar</b>" );
    CHECK_PARSES_OK( "1<big>2<small>3</small>4<big>5</big></big>6" );
    CHECK_PARSES_OK( "first <span foreground='red'>second</span> last" );
    CHECK_PARSES_OK( "first <span foreground='red' "
                                 "background='#ffffff'>second </span> last" );
    CHECK_PARSES_OK( "<span size='10240'>10pt</span>" );
    CHECK_PARSES_OK( "<span size='x-small'>much smaller</span>" );
    CHECK_PARSES_OK( "<span size='larger'>larger</span>" );
    CHECK_PARSES_OK
    (
        "<u>Please</u> notice: <i><b>any</b></i> <span foreground='grey'>"
        "<s><tt>bugs</tt></s></span> in this code are <span foreground='red' "
        "size='xx-large'>NOT</span> allowed."
    );

    CHECK_PARSES_OK( "foo&bar" );
    CHECK_PARSES_AS( "foo&amp;bar", "foo&bar" );
    CHECK_PARSES_AS( "&lt;O&apos;Reilly&gt;", "<O'Reilly>" );

    CHECK_DOESNT_PARSE( "<" );
    CHECK_DOESNT_PARSE( "<b" );
    CHECK_DOESNT_PARSE( "<b>" );
    CHECK_DOESNT_PARSE( "<b></i>" );
    CHECK_DOESNT_PARSE( "<b><i></b></i>" );
    CHECK_DOESNT_PARSE( "<foo></foo>" );

    #undef CHECK_PARSES_OK
    #undef CHECK_DOESNT_PARSE
}

void MarkupTestCase::Quote()
{
    CPPUNIT_ASSERT_EQUAL( "", wxMarkupParser::Quote("") );
    CPPUNIT_ASSERT_EQUAL( "foo", wxMarkupParser::Quote("foo") );
    CPPUNIT_ASSERT_EQUAL( "&lt;foo&gt;", wxMarkupParser::Quote("<foo>") );
    CPPUNIT_ASSERT_EQUAL( "B&amp;B", wxMarkupParser::Quote("B&B") );
    CPPUNIT_ASSERT_EQUAL( "&quot;&quot;", wxMarkupParser::Quote("\"\"") );
}

void MarkupTestCase::Strip()
{
    #define CHECK_STRIP( text, stripped ) \
        CPPUNIT_ASSERT_EQUAL( stripped, wxMarkupParser::Strip(text) )

    CHECK_STRIP( "", "" );
    CHECK_STRIP( "foo", "foo" );
    CHECK_STRIP( "&lt;foo&gt;", "<foo>" );
    CHECK_STRIP( "<b>Big</b> problem", "Big problem" );
    CHECK_STRIP( "<span foreground='red'>c</span>"
                 "<span background='green'>o</span>"
                 "<span background='blue'>l</span>"
                 "<span background='green'>o</span>"
                 "<span foreground='yellow'>u</span>"
                 "<span background='green'>r</span>",
                 "colour" );

    #undef CHECK_STRIP
}
