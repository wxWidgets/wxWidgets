/////////////////////////////////////////////////////////////////////////////
// Name:        _joystick.i
// Purpose:     SWIG interface stuff for wxWave
//
// Author:      Robin Dunn
//
// Created:     18-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

%{
#include <wx/wave.h>
%}    

//---------------------------------------------------------------------------

%{
#if !wxUSE_WAVE
// A C++ stub class for wxWave for platforms that don't have it.
class wxWave : public wxObject
{
public:
    wxWave(const wxString& fileName, bool isResource = False) {
        wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError, "wxWave is not available on this platform.");
        wxPyEndBlockThreads();
    }
    wxWave(int size, const wxByte* data) {
        wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError, "wxWave is not available on this platform.");
        wxPyEndBlockThreads();
    }

    ~wxWave() {}

    bool  IsOk() const { return False; }
    bool  Play(bool async = True, bool looped = False) const { return False; }
};

#endif
%}



class wxWave /*: public wxObject*/
{
public:
    wxWave(const wxString& fileName, bool isResource = False);
    %extend {
        %name(WaveData) wxWave(const wxString& data) {
            return new wxWave(data.Len(), (wxByte*)data.c_str());
        }
    }
        
    ~wxWave();

    bool  IsOk() const;
    bool  Play(bool async = True, bool looped = False);

    %pythoncode { def __nonzero__(self): return self.IsOk() }
};




//---------------------------------------------------------------------------
