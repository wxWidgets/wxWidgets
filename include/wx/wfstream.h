/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wfstream.h
// Purpose:     File stream classes
// Author:      Guilhem Lavaux
// Modified by:
// Created:     11/07/98
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXFSTREAM_H__
#define _WX_WXFSTREAM_H__

#include "wx/defs.h"

#if wxUSE_STREAMS

#include "wx/object.h"
#include "wx/string.h"
#include "wx/stream.h"
#include "wx/file.h"
#include "wx/ffile.h"

#if wxUSE_FILE

// ----------------------------------------------------------------------------
// wxFileStream using wxFile
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxFileInputStream : public wxInputStream
{
public:
    wxFileInputStream(const wxString& ifileName);
    wxFileInputStream(wxFile& file);
    wxFileInputStream(int fd);
    virtual ~wxFileInputStream();

    virtual wxFileOffset GetLength() const override;

    bool Ok() const { return IsOk(); }
    virtual bool IsOk() const override;
    virtual bool IsSeekable() const override { return m_file->GetKind() == wxFILE_KIND_DISK; }

    wxFile* GetFile() const { return m_file; }

protected:
    wxFileInputStream();

    virtual size_t OnSysRead(void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
    virtual wxFileOffset OnSysTell() const override;

protected:
    wxFile *m_file;
    bool m_file_destroy;

    wxDECLARE_NO_COPY_CLASS(wxFileInputStream);
};

class WXDLLIMPEXP_BASE wxFileOutputStream : public wxOutputStream
{
public:
    wxFileOutputStream(const wxString& fileName);
    wxFileOutputStream(wxFile& file);
    wxFileOutputStream(int fd);
    virtual ~wxFileOutputStream();

    void Sync() override;
    bool Close() override { return m_file_destroy ? m_file->Close() : true; }
    virtual wxFileOffset GetLength() const override;

    bool Ok() const { return IsOk(); }
    virtual bool IsOk() const override;
    virtual bool IsSeekable() const override { return m_file->GetKind() == wxFILE_KIND_DISK; }

    wxFile* GetFile() const { return m_file; }

protected:
    wxFileOutputStream();

    virtual size_t OnSysWrite(const void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
    virtual wxFileOffset OnSysTell() const override;

protected:
    wxFile *m_file;
    bool m_file_destroy;

    wxDECLARE_NO_COPY_CLASS(wxFileOutputStream);
};

class WXDLLIMPEXP_BASE wxTempFileOutputStream : public wxOutputStream
{
public:
    wxTempFileOutputStream(const wxString& fileName);
    virtual ~wxTempFileOutputStream();

    bool Close() override { return Commit(); }
    WXDLLIMPEXP_INLINE_BASE virtual bool Commit() { return m_file->Commit(); }
    WXDLLIMPEXP_INLINE_BASE virtual void Discard() { m_file->Discard(); }

    virtual wxFileOffset GetLength() const override { return m_file->Length(); }
    virtual bool IsSeekable() const override { return true; }

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
        { return m_file->Seek(pos, mode); }
    virtual wxFileOffset OnSysTell() const override { return m_file->Tell(); }

private:
    wxTempFile *m_file;

    wxDECLARE_NO_COPY_CLASS(wxTempFileOutputStream);
};

class WXDLLIMPEXP_BASE wxTempFFileOutputStream : public wxOutputStream
{
public:
    wxTempFFileOutputStream(const wxString& fileName);
    virtual ~wxTempFFileOutputStream();

    bool Close() override { return Commit(); }
    WXDLLIMPEXP_INLINE_BASE virtual bool Commit() { return m_file->Commit(); }
    WXDLLIMPEXP_INLINE_BASE virtual void Discard() { m_file->Discard(); }

    virtual wxFileOffset GetLength() const override { return m_file->Length(); }
    virtual bool IsSeekable() const override { return true; }

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
        { return m_file->Seek(pos, mode); }
    virtual wxFileOffset OnSysTell() const override { return m_file->Tell(); }

private:
    wxTempFFile *m_file;

    wxDECLARE_NO_COPY_CLASS(wxTempFFileOutputStream);
};

class WXDLLIMPEXP_BASE wxFileStream : public wxFileInputStream,
                                      public wxFileOutputStream
{
public:
    wxFileStream(const wxString& fileName);
    virtual bool IsOk() const override;

    // override (some) virtual functions inherited from both classes to resolve
    // ambiguities (this wouldn't be necessary if wxStreamBase were a virtual
    // base class but it isn't)

    virtual bool IsSeekable() const override
    {
        return wxFileInputStream::IsSeekable();
    }

    virtual wxFileOffset GetLength() const override
    {
        return wxFileInputStream::GetLength();
    }

protected:
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
    {
        return wxFileInputStream::OnSysSeek(pos, mode);
    }

    virtual wxFileOffset OnSysTell() const override
    {
        return wxFileInputStream::OnSysTell();
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxFileStream);
};

#endif //wxUSE_FILE

#if wxUSE_FFILE

// ----------------------------------------------------------------------------
// wxFFileStream using wxFFile
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxFFileInputStream : public wxInputStream
{
public:
    wxFFileInputStream(const wxString& fileName, const wxString& mode = wxASCII_STR("rb"));
    wxFFileInputStream(wxFFile& file);
    wxFFileInputStream(FILE *file);
    virtual ~wxFFileInputStream();

    virtual wxFileOffset GetLength() const override;

    bool Ok() const { return IsOk(); }
    virtual bool IsOk() const override;
    virtual bool IsSeekable() const override { return m_file->GetKind() == wxFILE_KIND_DISK; }

    wxFFile* GetFile() const { return m_file; }

protected:
    wxFFileInputStream();

    virtual size_t OnSysRead(void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
    virtual wxFileOffset OnSysTell() const override;

protected:
    wxFFile *m_file;
    bool m_file_destroy;

    wxDECLARE_NO_COPY_CLASS(wxFFileInputStream);
};

class WXDLLIMPEXP_BASE wxFFileOutputStream : public wxOutputStream
{
public:
    wxFFileOutputStream(const wxString& fileName, const wxString& mode = wxASCII_STR("wb"));
    wxFFileOutputStream(wxFFile& file);
    wxFFileOutputStream(FILE *file);
    virtual ~wxFFileOutputStream();

    void Sync() override;
    bool Close() override { return m_file_destroy ? m_file->Close() : true; }
    virtual wxFileOffset GetLength() const override;

    bool Ok() const { return IsOk(); }
    virtual bool IsOk() const override;
    virtual bool IsSeekable() const override { return m_file->GetKind() == wxFILE_KIND_DISK; }

    wxFFile* GetFile() const { return m_file; }

protected:
    wxFFileOutputStream();

    virtual size_t OnSysWrite(const void *buffer, size_t size) override;
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
    virtual wxFileOffset OnSysTell() const override;

protected:
    wxFFile *m_file;
    bool m_file_destroy;

    wxDECLARE_NO_COPY_CLASS(wxFFileOutputStream);
};

class WXDLLIMPEXP_BASE wxFFileStream : public wxFFileInputStream,
                                       public wxFFileOutputStream
{
public:
    wxFFileStream(const wxString& fileName, const wxString& mode = wxASCII_STR("w+b"));

    // override some virtual functions to resolve ambiguities, just as in
    // wxFileStream

    virtual bool IsOk() const override;

    virtual bool IsSeekable() const override
    {
        return wxFFileInputStream::IsSeekable();
    }

    virtual wxFileOffset GetLength() const override
    {
        return wxFFileInputStream::GetLength();
    }

protected:
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
    {
        return wxFFileInputStream::OnSysSeek(pos, mode);
    }

    virtual wxFileOffset OnSysTell() const override
    {
        return wxFFileInputStream::OnSysTell();
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxFFileStream);
};

#endif //wxUSE_FFILE

#endif // wxUSE_STREAMS

#endif // _WX_WXFSTREAM_H__
