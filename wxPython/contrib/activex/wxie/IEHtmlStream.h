/*
                wxActiveX Library Licence, Version 3
                ====================================

  Copyright (C) 2003 Lindsay Mathieson [, ...]

  Everyone is permitted to copy and distribute verbatim copies
  of this licence document, but changing it is not allowed.

                       wxActiveX LIBRARY LICENCE
     TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
  
  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public Licence as published by
  the Free Software Foundation; either version 2 of the Licence, or (at
  your option) any later version.
  
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
  General Public Licence for more details.

  You should have received a copy of the GNU Library General Public Licence
  along with this software, usually in a file named COPYING.LIB.  If not,
  write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA 02111-1307 USA.

  EXCEPTION NOTICE

  1. As a special exception, the copyright holders of this library give
  permission for additional uses of the text contained in this release of
  the library as licenced under the wxActiveX Library Licence, applying
  either version 3 of the Licence, or (at your option) any later version of
  the Licence as published by the copyright holders of version 3 of the
  Licence document.

  2. The exception is that you may use, copy, link, modify and distribute
  under the user's own terms, binary object code versions of works based
  on the Library.

  3. If you copy code from files distributed under the terms of the GNU
  General Public Licence or the GNU Library General Public Licence into a
  copy of this library, as this licence permits, the exception does not
  apply to the code that you add in this way.  To avoid misleading anyone as
  to the status of such modified files, you must delete this exception
  notice from such code and/or adjust the licensing conditions notice
  accordingly.

  4. If you write modifications of your own for this library, it is your
  choice whether to permit this exception to apply to your modifications. 
  If you do not wish that, you must delete the exception notice from such
  code and/or adjust the licensing conditions notice accordingly.
*/

// This module contains the declarations of the stream adapters and such that
// used to be in IEHtmlWin.cpp, so that they can be used independently in the
// wxPython wrappers...


#ifndef _IEHTMLSTREAM_H_
#define _IEHTMLSTREAM_H_


class IStreamAdaptorBase : public IStream
{
private:
    DECLARE_OLE_UNKNOWN(IStreamAdaptorBase);

public:
    string prepend;

    IStreamAdaptorBase() {}
    virtual ~IStreamAdaptorBase() {}

    virtual int Read(char *buf, int cb) = 0;

    // ISequentialStream
    HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead);

    HRESULT STDMETHODCALLTYPE Write(const void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbWritten) {return E_NOTIMPL;}

    // IStream
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER __RPC_FAR *plibNewPosition) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CopyTo(IStream __RPC_FAR *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER __RPC_FAR *pcbRead, ULARGE_INTEGER __RPC_FAR *pcbWritten) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Revert(void) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Stat(STATSTG __RPC_FAR *pstatstg, DWORD grfStatFlag) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Clone(IStream __RPC_FAR *__RPC_FAR *ppstm) {return E_NOTIMPL;}
};    
    


class IStreamAdaptor : public IStreamAdaptorBase
{
private:
    istream *m_is;

public:
    IStreamAdaptor(istream *is);
    ~IStreamAdaptor();
    int Read(char *buf, int cb);
};



class IwxStreamAdaptor : public IStreamAdaptorBase
{
private:
    wxInputStream *m_is;

public:
    IwxStreamAdaptor(wxInputStream *is);
    ~IwxStreamAdaptor();
    int Read(char *buf, int cb);
};


class wxOwnedMemInputStream : public wxMemoryInputStream
{
public:
    char *m_data;

    wxOwnedMemInputStream(char *data, size_t len);
    ~wxOwnedMemInputStream();
};


wxAutoOleInterface<IHTMLTxtRange> wxieGetSelRange(IOleObject *oleObject);

#endif
