/////////////////////////////////////////////////////////////////////////////
// Name:        pyistream.h
// Purpose:     Classes for managing wxInputStream <--> Python streams
//
// Author:      Robin Dunn
//
// Created:     25-Sept-2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __PYISTREAM__
#define __PYISTREAM__


//----------------------------------------------------------------------
// Handling of wxInputStreams by Joerg Baumann
// See stream.i for implementations


// C++ class wxPyInputStream to act as base for python class wxInputStream
// You can use it in python like a python file object.
class wxPyInputStream {
public:
    // underlying wxInputStream
    wxInputStream* m_wxis;

public:
    wxPyInputStream(wxInputStream* wxis) : m_wxis(wxis) {}
    ~wxPyInputStream();

    // python file object interface for input files (most of it)
    void close();
    void flush();
    bool eof();
    PyObject* read(int size=-1);
    PyObject* readline(int size=-1);
    PyObject* readlines(int sizehint=-1);
    void seek(int offset, int whence=0);
    int tell();

    /*   do these later?
      bool isatty();
      int fileno();
      void truncate(int size=-1);
      void write(wxString data);
      void writelines(wxStringPtrList);
    */

    // wxInputStream methods that may come in handy...

    char Peek()           { if (m_wxis) return m_wxis->Peek(); else return -1; }
    char GetC()           { if (m_wxis) return m_wxis->GetC(); else return -1; }
    size_t LastRead()     { if (m_wxis) return m_wxis->LastRead(); else return 0; }
    bool CanRead()        { if (m_wxis) return m_wxis->CanRead(); else return false; }
    bool Eof()            { if (m_wxis) return m_wxis->Eof(); else return false; }
    bool Ungetch(char c)  { if (m_wxis) return m_wxis->Ungetch(c); else return false; }

    unsigned long SeekI(unsigned long pos, wxSeekMode mode)
          { if (m_wxis) return m_wxis->SeekI(pos, mode); else return 0; }
    unsigned long TellI() { if (m_wxis) return m_wxis->TellI(); else return 0; }
};



// This is a wxInputStream that wraps a Python file-like
// object and calls the Python methods as needed.
class wxPyCBInputStream : public wxInputStream {
public:
    ~wxPyCBInputStream();
    virtual size_t GetSize() const;

    // factory function
    static wxPyCBInputStream* create(PyObject *py, bool block=true);

protected:
    // can only be created via the factory
    wxPyCBInputStream(PyObject *r, PyObject *s, PyObject *t, bool block);

    // wxStreamBase methods
    virtual size_t OnSysRead(void *buffer, size_t bufsize);
    virtual size_t OnSysWrite(const void *buffer, size_t bufsize);
    virtual wxFileOffset OnSysSeek(wxFileOffset off, wxSeekMode mode);
    virtual wxFileOffset OnSysTell() const;

    // helper
    static PyObject* getMethod(PyObject* py, char* name);

    PyObject* m_read;
    PyObject* m_seek;
    PyObject* m_tell;
    bool      m_block;
};

//----------------------------------------------------------------------
#endif
