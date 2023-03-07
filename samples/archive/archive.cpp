/////////////////////////////////////////////////////////////////////////////
// Name:        samples/archive/archive.cpp
// Purpose:     A sample application to manipulate archives
// Author:      Tobias Taschner
// Created:     2018-02-07
// Copyright:   (c) 2018 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/archive.h"
#include "wx/app.h"
#include "wx/cmdline.h"
#include "wx/filename.h"
#include "wx/vector.h"
#include "wx/wfstream.h"
#include "wx/zipstrm.h"

#include <memory>

class ArchiveApp: public wxAppConsole
{
public:
    ArchiveApp()
    {
        m_forceZip64 = false;
        m_archiveClassFactory = nullptr;
        m_filterClassFactory = nullptr;
    }

    virtual void OnInitCmdLine(wxCmdLineParser& parser) override;

    virtual bool OnCmdLineParsed(wxCmdLineParser& parser) override;

    virtual int OnRun() override;

private:
    enum ArchiveCommandType
    {
        CMD_CREATE,
        CMD_LIST,
        CMD_EXTRACT,
        CMD_NONE
    };

    struct ArchiveCommandDesc
    {
        ArchiveCommandType type;
        const char* id;
        const char* desc;
    };

    static const ArchiveCommandDesc s_cmdDesc[];

    ArchiveCommandType m_command;
    wxString m_archiveFileName;
    wxVector<wxString> m_fileNames;
    bool m_forceZip64;

    // At most one of these pointers is non-null.
    const wxArchiveClassFactory* m_archiveClassFactory;
    const wxFilterClassFactory* m_filterClassFactory;

    int DoCreate();

    int DoList();

    int DoExtract();

    bool CopyStreamData(wxInputStream& inputStream, wxOutputStream& outputStream, wxFileOffset size);
};

const ArchiveApp::ArchiveCommandDesc ArchiveApp::s_cmdDesc[] =
{
    {CMD_CREATE, "c", "create"},
    {CMD_LIST, "l", "list"},
    {CMD_EXTRACT, "x", "extract"},

    {CMD_NONE, 0, 0}
};

// Helper function iterating over all factories of the given type (assumed to
// derive from wxFilterClassFactoryBase) and returning a string containing all
// comma-separated values returned from their GetProtocols() called with the
// given protocol type.
template <typename T>
void AppendAllSupportedOfType(wxString& all, wxStreamProtocolType type)
{
    for (const T* factory = T::GetFirst(); factory; factory = factory->GetNext())
    {
        const wxChar* const* exts = factory->GetProtocols(type);
        while (*exts)
        {
            if (!all.empty())
                all+= ", ";
            all += *exts++;
        }
    }
}

// Returns all supported protocols or extensions (depending on the type
// parameter value) for both archive and filter factories.
wxString GetAllSupported(wxStreamProtocolType type)
{
    wxString all;
    AppendAllSupportedOfType<wxArchiveClassFactory>(all, type);
    AppendAllSupportedOfType<wxFilterClassFactory>(all, type);
    return all;
}

void ArchiveApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxAppConsole::OnInitCmdLine(parser);

    parser.AddParam("command");
    parser.AddParam("archive_name");
    parser.AddParam("file_names", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
    parser.AddSwitch("", "force-zip64", "Force ZIP64 format when creating ZIP archives");

    parser.AddUsageText("\ncommand:");
    for (const ArchiveCommandDesc* cmdDesc = s_cmdDesc; cmdDesc->type != CMD_NONE; cmdDesc++)
        parser.AddUsageText(wxString::Format("  %s: %s", cmdDesc->id, cmdDesc->desc));

    parser.AddUsageText("\nsupported formats: " + GetAllSupported(wxSTREAM_PROTOCOL));
}

bool ArchiveApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_command = CMD_NONE;
    wxString command = parser.GetParam();
    for (const ArchiveCommandDesc* cmdDesc = s_cmdDesc; cmdDesc->type != CMD_NONE; cmdDesc++)
    {
        if (cmdDesc->id == command)
            m_command = cmdDesc->type;
    }
    if(m_command == CMD_NONE)
    {
        wxLogError("Invalid command: %s", command);
        return false;
    }
    m_archiveFileName = parser.GetParam(1);
    for (size_t i = 2; i < parser.GetParamCount(); i++)
        m_fileNames.push_back(parser.GetParam(i));
    m_forceZip64 = parser.FoundSwitch("force-zip64") == wxCMD_SWITCH_ON;

    return wxAppConsole::OnCmdLineParsed(parser);
}

bool ArchiveApp::CopyStreamData(wxInputStream& inputStream, wxOutputStream& outputStream, wxFileOffset size)
{
    wxChar buf[128 * 1024];
    int readSize = 128 * 1024;
    wxFileOffset copiedData = 0;
    for (;;)
    {
        if (size != -1 && copiedData + readSize > size)
            readSize = size - copiedData;
        inputStream.Read(buf, readSize);

        size_t actuallyRead = inputStream.LastRead();
        outputStream.Write(buf, actuallyRead);
        if (outputStream.LastWrite() != actuallyRead)
        {
            wxLogError("Failed to output data");
            return false;
        }

        if (size == -1)
        {
            if (inputStream.Eof())
                break;
        }
        else
        {
            copiedData += actuallyRead;
            if (copiedData >= size)
                break;
        }
    }

    return true;
}

int ArchiveApp::DoCreate()
{
    if (m_fileNames.empty())
    {
        wxLogError("Need at least one file to add to archive");
        return -1;
    }

    wxTempFileOutputStream fileOutputStream(m_archiveFileName);
    if (m_archiveClassFactory)
    {
        std::unique_ptr<wxArchiveOutputStream> archiveOutputStream(m_archiveClassFactory->NewStream(fileOutputStream));
        if (m_archiveClassFactory->GetProtocol().IsSameAs("zip", false) && m_forceZip64)
            reinterpret_cast<wxZipOutputStream*>(archiveOutputStream.get())->SetFormat(wxZIP_FORMAT_ZIP64);

        for(wxVector<wxString>::iterator fileName = m_fileNames.begin(); fileName != m_fileNames.end(); fileName++)
        {
            wxFileName inputFileName(*fileName);
            wxPrintf("Adding %s...\n", inputFileName.GetFullName());
            wxFileInputStream inputFileStream(*fileName);
            if (!inputFileStream.IsOk())
            {
                wxLogError("Could not open file");
                return 1;
            }
            if (!archiveOutputStream->PutNextEntry(inputFileName.GetFullName(), wxDateTime::Now(), inputFileStream.GetLength()))
                break;
            if (!CopyStreamData(inputFileStream, *archiveOutputStream, inputFileStream.GetLength()))
                return 1;
        }

        if (!archiveOutputStream->Close())
            return 1;
    }
    else // use m_filterClassFactory
    {
        if (m_fileNames.size() != 1)
        {
            wxLogError("Filter-based formats only support archives consisting of a single file");
            return -1;
        }

        std::unique_ptr<wxFilterOutputStream> filterOutputStream(m_filterClassFactory->NewStream(fileOutputStream));
        wxFileInputStream inputFileStream(*m_fileNames.begin());
        if (!inputFileStream.IsOk())
        {
            wxLogError("Could not open file");
            return 1;
        }
        if (!CopyStreamData(inputFileStream, *filterOutputStream, inputFileStream.GetLength()))
            return 1;
    }

    fileOutputStream.Commit();
    wxPrintf("Created archive\n");
    return 0;
}

int ArchiveApp::DoList()
{
    wxFileInputStream fileInputStream(m_archiveFileName);
    if (!fileInputStream.IsOk())
        return 1;

    if (m_archiveClassFactory)
    {
        std::unique_ptr<wxArchiveInputStream> archiveStream(m_archiveClassFactory->NewStream(fileInputStream));
        wxPrintf("Archive: %s\n", m_archiveFileName);
        wxPrintf("Length     Date       Time     Name\n");
        wxPrintf("---------- ---------- -------- ----\n");

        wxFileOffset combinedSize = 0;
        int entryCount = 0;
        for (wxArchiveEntry* entry = archiveStream->GetNextEntry(); entry;
             entry = archiveStream->GetNextEntry())
        {
            combinedSize += entry->GetSize();
            entryCount++;
            wxPrintf("%10lld %s %s %s\n",
                     entry->GetSize(),
                     entry->GetDateTime().FormatISODate(),
                     entry->GetDateTime().FormatISOTime(),
                     entry->GetName());
        }
        wxPrintf("----------                     -------\n");
        wxPrintf("%10lld                     %d files\n", combinedSize, entryCount);
    }
    else
    {
        wxPrintf("Archive \"%s\" contains a single file named \"%s\"\n",
                 m_archiveFileName, wxFileName(m_archiveFileName).GetName());
    }

    return 0;
}

int ArchiveApp::DoExtract()
{
    wxFileInputStream fileInputStream(m_archiveFileName);
    if (!fileInputStream.IsOk())
        return 1;

    if (m_archiveClassFactory)
    {
        std::unique_ptr<wxArchiveInputStream> archiveStream(m_archiveClassFactory->NewStream(fileInputStream));
        wxPrintf("Extracting from: %s\n", m_archiveFileName);
        for (wxArchiveEntry* entry = archiveStream->GetNextEntry(); entry;
             entry = archiveStream->GetNextEntry())
        {
            wxPrintf("Extracting: %s...\n", entry->GetName());
            wxTempFileOutputStream outputFileStream(entry->GetName());
            if (!CopyStreamData(*archiveStream, outputFileStream, entry->GetSize()))
                return 1;
            outputFileStream.Commit();
        }
        wxPrintf("Extracted all files\n");
    }
    else
    {
        std::unique_ptr<wxFilterInputStream> filterStream(m_filterClassFactory->NewStream(fileInputStream));
        wxPrintf("Extracting single file from: %s\n", m_archiveFileName);
        wxTempFileOutputStream outputFileStream(wxFileName(m_archiveFileName).GetName());
        if (!CopyStreamData(*filterStream, outputFileStream, -1))
            return 1;
        outputFileStream.Commit();
        wxPrintf("Extracted successfully\n");
    }

    return 0;
}

int ArchiveApp::OnRun()
{
    m_archiveClassFactory = wxArchiveClassFactory::Find(m_archiveFileName, wxSTREAM_FILEEXT);
    if (!m_archiveClassFactory)
    {
        m_filterClassFactory = wxFilterClassFactory::Find(m_archiveFileName, wxSTREAM_FILEEXT);
        if (!m_filterClassFactory)
        {
            wxLogError("File \"%s\" has unsupported extension, supported ones are: %s",
                       m_archiveFileName, GetAllSupported(wxSTREAM_FILEEXT));
            return -1;
        }
    }

    int result = -1;
    switch (m_command) {
        case CMD_CREATE:
            result = DoCreate();
            break;
        case CMD_LIST:
            result = DoList();
            break;
        case CMD_EXTRACT:
            result = DoExtract();
            break;
        default:
            break;
    }

    return result;
}

wxIMPLEMENT_APP(ArchiveApp);
