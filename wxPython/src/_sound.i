/////////////////////////////////////////////////////////////////////////////
// Name:        _sound.i
// Purpose:     SWIG interface stuff for wxSound
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
#include <wx/sound.h>
%}    

//---------------------------------------------------------------------------

enum wxSoundFlags
{
    wxSOUND_SYNC   = 0,
    wxSOUND_ASYNC  = 1,
    wxSOUND_LOOP   = 2
};



%{
#if !wxUSE_SOUND
// A C++ stub class for wxWave for platforms that don't have it.
class wxSound : public wxObject
{
public:
    wxSound() {
        bool blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxSound is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    wxSound(const wxString&/*, bool*/) {
        bool blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxSound is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    wxSound(int, const wxByte*) {
        bool blocked = wxPyBeginBlockThreads();
        PyErr_SetString(PyExc_NotImplementedError,
                        "wxSound is not available on this platform.");
        wxPyEndBlockThreads(blocked);
    }
    
    ~wxSound() {};

    bool Create(const wxString&/*, bool*/) { return false; }
    bool Create(int, const wxByte*) { return false; };
    bool IsOk() { return false; };    
    bool Play(unsigned) const { return false; }
    static bool Play(const wxString&, unsigned) { return false; }
    static void Stop() {}
};

#endif
%}



class wxSound /*: public wxObject*/
{
public:
    %extend {
        wxSound(const wxString& fileName = wxPyEmptyString /*, bool isResource = false*/) {
            if (fileName.Length() == 0)
                return new wxSound;
            else
                return new wxSound(fileName);
        }
        %name(SoundFromData) wxSound(const wxMemoryBuffer& data) {
            return new wxSound((int)data.GetDataLen(), (wxByte*)data.GetData());
        }
    }
    
    ~wxSound();

    %nokwargs Create;
    %nokwargs Play;
    
    // Create from resource or file
    bool Create(const wxString& fileName/*, bool isResource = false*/);

    %extend {
        bool CreateFromData(const wxMemoryBuffer& data) {
            %#ifndef __WXMAC__
                 return self->Create((int)data.GetDataLen(), (wxByte*)data.GetData());
            %#else
                 bool blocked = wxPyBeginBlockThreads();
                 PyErr_SetString(PyExc_NotImplementedError,
                                 "Create from data is not available on this platform.");
                 wxPyEndBlockThreads(blocked);
                 return False;
            %#endif
        }
    }
    
    bool  IsOk();
    
    // Play the sound:
    bool Play(unsigned flags = wxSOUND_ASYNC) const;

    // Plays sound from filename:
    %name(PlaySound) static bool Play(const wxString& filename, unsigned flags = wxSOUND_ASYNC);

#ifndef __WXMAC__
    static void Stop();
#else
    %extend {
        static void Stop()
            { wxPyRaiseNotImplemented(); }
    }
#endif

    %pythoncode { def __nonzero__(self): return self.IsOk() }
};




//---------------------------------------------------------------------------
