///////////////////////////////////////////////////////////////////////////////
// Name:        tests/uris/ftp.cpp
// Purpose:     wxFTP unit test
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-05-23
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_PROTOCOL_FTP

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <wx/protocol/ftp.h>
#include "wx/thread.h"

#include <memory>

// For this to run, the following environment variables need to be defined:
//
//  - WX_FTP_TEST_HOST: the host to use for testing (e.g. ftp.example.com)
//  - WX_FTP_TEST_DIR: the directory in which to perform most of the tests
//  - WX_FTP_TEST_FILE: name of an existing file in this directory
//
// Optionally, WX_FTP_TEST_USER and WX_FTP_TEST_PASS may also be defined,
// otherwise anonymous FTP is used.
TEST_CASE("FTP", "[net][.]")
{
    wxString hostname,
             directory,
             valid_filename;
    if ( !wxGetEnv("WX_FTP_TEST_HOST", &hostname) ||
            !wxGetEnv("WX_FTP_TEST_DIR", &directory) ||
                !wxGetEnv("WX_FTP_TEST_FILE", &valid_filename) )
    {
        WARN("Skipping FTPTestCase because required WX_FTP_TEST_XXX "
             "environment variables are not defined.");
        return;
    }

    const wxString user = wxGetenv("WX_FTP_TEST_USER");
    const wxString password = wxGetenv("WX_FTP_TEST_PASS");

    wxSocketInitializer socketInit;

    // wxFTP cannot be a static variable as its ctor needs to access
    // wxWidgets internals after it has been initialized
    wxFTP ftp;

    if ( !user.empty() )
    {
        ftp.SetUser(user);
        ftp.SetPassword(password);
    }

    REQUIRE( ftp.Connect(hostname) );

    SECTION("List")
    {
        // test CWD
        REQUIRE( ftp.ChDir(directory) );

        // test NLIST and LIST
        wxArrayString files;
        CHECK( ftp.GetFilesList(files) );

        wxArrayString dirs;
        CHECK( ftp.GetDirList(dirs) );

        CHECK( files.size() == dirs.size() );
    }

    SECTION("Download")
    {
        REQUIRE( ftp.ChDir(directory) );

        // test RETR
        std::unique_ptr<wxInputStream> in1(ftp.GetInputStream("bloordyblop"));
        CHECK( !in1 );

        std::unique_ptr<wxInputStream> in2(ftp.GetInputStream(valid_filename));
        CHECK( in2 );

        size_t size = in2->GetSize();
        std::vector<unsigned char> data(size);
        CHECK( in2->Read(&data[0], size).GetLastError() == wxSTREAM_NO_ERROR );
    }

    SECTION("FileSize")
    {
        REQUIRE( ftp.ChDir(directory) );

        REQUIRE( ftp.FileExists(valid_filename) );

        int size = ftp.GetFileSize(valid_filename);
        CHECK( size != -1 );
    }

    SECTION("Pwd")
    {
        CHECK( ftp.Pwd()  == "/" );

        REQUIRE( ftp.ChDir(directory) );
        CHECK( ftp.Pwd()  == directory );
    }

    SECTION("Misc")
    {
        CHECK( ftp.SendCommand(wxT("STAT")) == '2' );
        CHECK( ftp.SendCommand(wxT("HELP SITE")) == '2' );
    }

    SECTION("Upload")
    {
        if ( user.empty() )
        {
            WARN("Skipping upload test when using anonymous FTP.");
            return;
        }

        // upload a file
        static const wxChar *file1 = wxT("test1");
        std::unique_ptr<wxOutputStream> out(ftp.GetOutputStream(file1));
        REQUIRE( out );
        CHECK( out->Write("First hello", 11).GetLastError() == wxSTREAM_NO_ERROR );

        // send a command to check the remote file
        REQUIRE( ftp.SendCommand(wxString(wxT("STAT ")) + file1) == '2' );
        CHECK( ftp.GetLastResult() == "11" );
    }
}

#if wxUSE_THREADS

// Minimal local FTP server whose PASV reply advertises an address different
// from its own: the client must still connect to the server it is talking to.
namespace
{

class FakePASVServer : public wxThread
{
public:
    FakePASVServer()
        : wxThread(wxTHREAD_JOINABLE)
    {
        wxIPV4address addr;
        addr.Hostname("127.0.0.1");
        addr.Service(0);

        m_ctrl.reset(new wxSocketServer(addr, wxSOCKET_BLOCK));
        m_data.reset(new wxSocketServer(addr, wxSOCKET_BLOCK));

        // Don't use the default timeout of 10 minutes: if the client doesn't
        // connect or stops talking to us, the test should fail quickly instead
        // of hanging.
        m_ctrl->SetTimeout(TIMEOUT);
        m_data->SetTimeout(TIMEOUT);
    }

    bool IsOk() const { return m_ctrl->IsOk() && m_data->IsOk(); }

    unsigned short GetPort() const
    {
        wxIPV4address addr;
        m_ctrl->GetLocal(addr);
        return addr.Service();
    }

    unsigned short GetDataPort() const
    {
        wxIPV4address addr;
        m_data->GetLocal(addr);
        return addr.Service();
    }

    // Return the port of the data connection as "p1,p2" string as used in
    // PASV reply.
    wxString GetDataPortBytes() const
    {
        const unsigned short port = GetDataPort();
        return wxString::Format("%d,%d", port >> 8, port & 0xff);
    }

    // Set the contents of the parentheses in the reply to PASV command.
    //
    // By default, an address which can't be connected to, but a correct port,
    // is used. This must be called before calling Run().
    void SetPASVReply(const wxString& reply) { m_pasvReply = reply; }

    virtual void* Entry() override
    {
        std::unique_ptr<wxSocketBase> ctrl(m_ctrl->Accept(true));
        if ( !ctrl )
            return nullptr;

        ctrl->SetTimeout(TIMEOUT);

        Send(*ctrl, "220 ready");

        for ( ;; )
        {
            const wxString cmd = ReadLine(*ctrl);
            if ( cmd.empty() || cmd.StartsWith("QUIT") )
            {
                Send(*ctrl, "221 bye");
                break;
            }

            if ( cmd.StartsWith("USER") )
            {
                Send(*ctrl, "230 logged in");
            }
            else if ( cmd.StartsWith("PASV") )
            {
                wxString reply = m_pasvReply;
                if ( reply.empty() )
                    reply = "255,255,255,255," + GetDataPortBytes();

                Send(*ctrl, "227 Entering Passive Mode (" + reply + ")");
            }
            else if ( cmd.StartsWith("NLST") )
            {
                Send(*ctrl, "150 here it comes");

                std::unique_ptr<wxSocketBase> data(m_data->Accept(true));
                if ( data )
                {
                    data->SetTimeout(TIMEOUT);

                    const char* const listing = "file.txt\r\n";
                    data->Write(listing, strlen(listing));
                }
                data.reset();

                Send(*ctrl, "226 done");
            }
            else
            {
                Send(*ctrl, "200 ok");
            }
        }

        return nullptr;
    }

private:
    // Timeout for all socket operations, in seconds.
    static constexpr long TIMEOUT = 10;

    static void Send(wxSocketBase& sock, const wxString& line)
    {
        const wxString full = line + "\r\n";
        const wxScopedCharBuffer buf = full.utf8_str();
        sock.Write(buf.data(), buf.length());
    }

    static wxString ReadLine(wxSocketBase& sock)
    {
        wxString line;
        for ( ;; )
        {
            char ch;
            sock.Read(&ch, 1);
            if ( sock.Error() || sock.LastCount() != 1 || ch == '\n' )
                break;
            if ( ch != '\r' )
                line += ch;
        }
        return line;
    }

    std::unique_ptr<wxSocketServer> m_ctrl,
                                    m_data;

    wxString m_pasvReply;
};

} // anonymous namespace

TEST_CASE("FTP::PASVAddress", "[net]")
{
    wxSocketInitializer socketInit;

    FakePASVServer server;
    REQUIRE( server.IsOk() );
    REQUIRE( server.Run() == wxTHREAD_NO_ERROR );

    {
        wxFTP ftp;
        REQUIRE( ftp.Connect("127.0.0.1", server.GetPort()) );

        wxArrayString files;
        CHECK( ftp.GetFilesList(files) );
        CHECK( wxJoin(files, ',') == "file.txt" );
    }

    CHECK( server.Wait() == nullptr );
}

TEST_CASE("FTP::PASVInvalid", "[net]")
{
    wxSocketInitializer socketInit;

    FakePASVServer server;
    REQUIRE( server.IsOk() );

    // Note that invalid port values below are chosen so that they would still
    // result in the correct port if they were truncated to 16 bits, to check
    // that they're really rejected and not just fail to connect.
    const wxString port = server.GetDataPortBytes();
    const int p1 = server.GetDataPort() >> 8,
              p2 = server.GetDataPort() & 0xff;

    SECTION("Too few values")
    {
        server.SetPASVReply("127,0,0,1,1");
    }

    SECTION("Not a number")
    {
        server.SetPASVReply("127,0,0,1,x,1");
    }

    SECTION("Address out of range")
    {
        server.SetPASVReply("127,0,0,256," + port);
    }

    SECTION("Negative address")
    {
        server.SetPASVReply("127,0,-1,1," + port);
    }

    SECTION("Port out of range")
    {
        server.SetPASVReply(wxString::Format("127,0,0,1,%d,%d", p1 + 256, p2));
    }

    SECTION("Negative port")
    {
        server.SetPASVReply(wxString::Format("127,0,0,1,%d,%d", p1 - 256, p2));
    }

    SECTION("Zero port")
    {
        server.SetPASVReply("127,0,0,1,0,0");
    }

    REQUIRE( server.Run() == wxTHREAD_NO_ERROR );

    {
        wxFTP ftp;
        REQUIRE( ftp.Connect("127.0.0.1", server.GetPort()) );

        wxArrayString files;
        CHECK_FALSE( ftp.GetFilesList(files) );
    }

    CHECK( server.Wait() == nullptr );
}

#endif // wxUSE_THREADS

#endif // wxUSE_PROTOCOL_FTP
