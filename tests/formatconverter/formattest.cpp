/*
 * Tester for wxFormatConverter, by M. J. Wetherell 
 *
 * Reads stdin, expects two column input (as produced by formats.pl or
 * formats2.pl), the first column contains the test pattern, the second
 * the expected result.
 *
 * The output is any patterns that wxFormatConveter doesn't transform to the
 * expected value, in three columns: input, expected, output.
 *
 *  ./formats.pl | ./formattest
 *  ./formats2.pl | ./formattest
 */

#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <iostream>

/* This function is in wxchar.cpp to give access to wxFormatConverter,
 * but only in debug mode.
 */

#ifdef __WXDEBUG__ 
extern wxString wxConvertFormat(const wxChar *format);
#endif

class TestApp : public wxAppConsole
{
public:
    int OnRun();
};

IMPLEMENT_APP_CONSOLE(TestApp)

int TestApp::OnRun()
{
#ifdef __WXDEBUG__ 
    wxFFileInputStream in(stdin);
    wxTextInputStream txt(in, _T("\t"));

    for (;;) {
        wxString format = txt.ReadWord();
        wxString expected = txt.ReadWord();
        if (!in) break;
        wxString converted = wxConvertFormat(format);

        if (converted != expected)
            std::cout << "'" << format.mb_str() << "'\t"
                      << "'" << expected.mb_str() << "'\t"
                      << "'" << converted.mb_str() << "'\n";
    }
#else
    std::cout << "Please compile this test program in debug mode.\n";
#endif
    return 0;
}

