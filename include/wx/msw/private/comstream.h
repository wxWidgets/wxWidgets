///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/comstream.h
// Purpose:     COM Stream adapter to wxStreamBase based streams
// Created:     2021-01-15
// Copyright:   (c) 2021 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_COMSTREAM_H
#define WX_COMSTREAM_H

#include "wx/stream.h"
#include "wx/msw/ole/comimpl.h"

class wxCOMBaseStreamAdapter : public IStream
{
public:
    wxCOMBaseStreamAdapter(wxStreamBase* stream):
        m_stream(stream)
    { }

    virtual ~wxCOMBaseStreamAdapter() { }

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (riid == IID_IUnknown ||
            riid == IID_ISequentialStream ||
            riid == IID_IStream)
        {
            *ppv = this;
            AddRef();
            return S_OK;
        }

        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return ++m_cRef;
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        if (--m_cRef == wxAutoULong(0))
        {
                delete this;
                return 0;
        }
        else
            return m_cRef;
    }

    // IStream
    virtual HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER WXUNUSED(dlibMove), DWORD WXUNUSED(dwOrigin),
        ULARGE_INTEGER *WXUNUSED(plibNewPosition)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER WXUNUSED(libNewSize)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream *WXUNUSED(pstm),
        ULARGE_INTEGER WXUNUSED(cb), ULARGE_INTEGER *WXUNUSED(pcbRead),
        ULARGE_INTEGER *WXUNUSED(pcbWritten)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE Commit(DWORD WXUNUSED(grfCommitFlags)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE Revert(void) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER WXUNUSED(libOffset),
        ULARGE_INTEGER WXUNUSED(cb), DWORD WXUNUSED(dwLockType)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER WXUNUSED(libOffset),
        ULARGE_INTEGER WXUNUSED(cb), DWORD WXUNUSED(dwLockType)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD WXUNUSED(grfStatFlag)) override
    {
        pstatstg->type = STGTY_STREAM;
        pstatstg->cbSize.QuadPart = m_stream->GetSize();

        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Clone(IStream **WXUNUSED(ppstm)) override
    {
        return E_NOTIMPL;
    }

    // ISequentialStream

    virtual HRESULT STDMETHODCALLTYPE Read(void *WXUNUSED(pv),
        ULONG WXUNUSED(cb), ULONG *WXUNUSED(pcbRead)) override
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE Write(const void *WXUNUSED(pv),
        ULONG WXUNUSED(cb), ULONG *WXUNUSED(pcbWritten)) override
    {
        return E_NOTIMPL;
    }


protected:
    wxStreamBase* m_stream;

    static wxSeekMode OriginToSeekMode(DWORD origin)
    {
        switch (origin)
        {
        case STREAM_SEEK_SET:
            return wxFromStart;
        case STREAM_SEEK_CUR:
            return wxFromCurrent;
        case STREAM_SEEK_END:
            return wxFromEnd;
        }

        return wxFromStart;
    }
private:
    wxAutoULong m_cRef;
};

class wxCOMInputStreamAdapter : public wxCOMBaseStreamAdapter
{
public:
    wxCOMInputStreamAdapter(wxInputStream* stream):
        wxCOMBaseStreamAdapter(stream)
    {

    }

    virtual HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER dlibMove, DWORD dwOrigin,
        ULARGE_INTEGER *plibNewPosition) override
    {
        wxFileOffset newOffset = reinterpret_cast<wxInputStream*>(m_stream)->SeekI(dlibMove.QuadPart, OriginToSeekMode(dwOrigin));
        if (plibNewPosition)
            plibNewPosition->QuadPart = newOffset;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead) override
    {
        wxInputStream* inputStr = reinterpret_cast<wxInputStream*>(m_stream);
        inputStr->Read(pv, cb);
        *pcbRead = inputStr->LastRead();
        return S_OK;
    }

};

class wxCOMOutputStreamAdapter : public wxCOMBaseStreamAdapter
{
public:
    wxCOMOutputStreamAdapter(wxOutputStream* stream) :
        wxCOMBaseStreamAdapter(stream)
    { }

    virtual HRESULT STDMETHODCALLTYPE Seek(
        LARGE_INTEGER dlibMove, DWORD dwOrigin,
        ULARGE_INTEGER *plibNewPosition) override
    {
        wxFileOffset newOffset = reinterpret_cast<wxOutputStream*>(m_stream)->SeekO(dlibMove.QuadPart, OriginToSeekMode(dwOrigin));
        if (plibNewPosition)
            plibNewPosition->QuadPart = newOffset;
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten) override
    {
        wxOutputStream* outputStr = reinterpret_cast<wxOutputStream*>(m_stream);
        outputStr->Write(pv, cb);
        *pcbWritten = outputStr->LastWrite();
        return S_OK;
    }

};

#endif // WX_COMSTREAM_H
