/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/clipbrd.cpp
// Purpose:     Clipboard functionality
// Author:      Stefan Csomor;
//              Generalized clipboard implementation by Matthew Flatt
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/frame.h"
    #include "wx/bitmap.h"
#endif

#include "wx/metafile.h"

#include "wx/mac/uma.h"

#define wxUSE_DATAOBJ 1

#include <string.h>


// the trace mask we use with wxLogTrace() - call
// wxLog::AddTraceMask(TRACE_CLIPBOARD) to enable the trace messages from here
// (there will be a *lot* of them!)
static const wxChar *TRACE_CLIPBOARD = wxT("clipboard");

IMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject)

// in order to keep the binary interface the same this class
// serves just to have a few additional member variables inside
// the clipboard class

class wxMacBinaryCompatHelper : public wxDataObject
{
public :
    wxMacBinaryCompatHelper() 
    {
        m_trueData = NULL;
    }
    
    ~wxMacBinaryCompatHelper() 
    {
        if (m_trueData != NULL)
        {
            delete m_trueData;
            m_trueData = NULL;
        }
    }
    
    virtual wxDataFormat GetPreferredFormat(Direction dir = Get) const 
    {
        return wxDataFormat();
    }

    virtual size_t GetFormatCount(Direction dir = Get) const 
    {
        return 0;
    }

    virtual void GetAllFormats(wxDataFormat *formats,
                               Direction dir = Get) const
    {
    }

    virtual size_t GetDataSize(const wxDataFormat& format) const
    {
        return 0;
    }

    virtual bool GetDataHere(const wxDataFormat& format, void *buf) const
    {
        return false;
    }
    
    // only relevant from here on
    
    wxDataObject* m_trueData;
    wxCFRef<PasteboardRef> m_pasteboard;
};

#define M_CLIPBOARD ((wxMacBinaryCompatHelper*)m_data)

wxClipboard::wxClipboard()
{
    m_open = false;
    m_data = new wxMacBinaryCompatHelper() ;
    PasteboardRef clipboard = 0;
    OSStatus err = PasteboardCreate( kPasteboardClipboard, &clipboard );
    if (err != noErr)
    {
        wxLogSysError( wxT("Failed to create the clipboard.") );
    }
    M_CLIPBOARD->m_pasteboard.reset(clipboard);
}

wxClipboard::~wxClipboard()
{
    M_CLIPBOARD->m_pasteboard.reset((PasteboardRef)0);
    delete m_data;
}

void wxClipboard::Clear()
{
    if (M_CLIPBOARD->m_trueData != NULL)
    {
        delete M_CLIPBOARD->m_trueData;
        M_CLIPBOARD->m_trueData = NULL;
    }
    
    OSStatus err = PasteboardClear( M_CLIPBOARD->m_pasteboard );
    if (err != noErr)
    {
        wxLogSysError( wxT("Failed to empty the clipboard.") );
    }
}

bool wxClipboard::Flush()
{
    return false;
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    return true;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );
    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    // as we can only store one wxDataObject,
    // this is the same in this implementation
    return AddData( data );
}

bool wxClipboard::AddData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );
    wxCHECK_MSG( data, false, wxT("data is invalid") );

    // we can only store one wxDataObject
    Clear();

    PasteboardSyncFlags syncFlags = PasteboardSynchronize( M_CLIPBOARD->m_pasteboard );
    wxCHECK_MSG( !(syncFlags&kPasteboardModified), false, wxT("clipboard modified after clear") );
    wxCHECK_MSG( (syncFlags&kPasteboardClientIsOwner), false, wxT("client couldn't own clipboard") );

    M_CLIPBOARD->m_trueData = data;

    data->AddToPasteboard( M_CLIPBOARD->m_pasteboard, 1 );
 
    return true;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;

    // Get rid of cached object.
    // If this is not done, copying data from
    // another application will only work once
    if (M_CLIPBOARD->m_trueData)
    {
        delete M_CLIPBOARD->m_trueData;
        M_CLIPBOARD->m_trueData = (wxDataObject*) NULL;
    }
}

bool wxClipboard::IsSupported( const wxDataFormat &dataFormat )
{
    if ( M_CLIPBOARD->m_trueData )
        return M_CLIPBOARD->m_trueData->IsSupported( dataFormat );
    return wxDataObject::IsFormatInPasteboard( M_CLIPBOARD->m_pasteboard, dataFormat );
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    size_t formatcount = data.GetFormatCount() + 1;
    wxDataFormat *array = new wxDataFormat[ formatcount ];
    array[0] = data.GetPreferredFormat();
    data.GetAllFormats( &array[1] );

    bool transferred = false;

    if ( M_CLIPBOARD->m_trueData )
    {
        for (size_t i = 0; !transferred && i < formatcount; i++)
        {
            wxDataFormat format = array[ i ];
            if ( M_CLIPBOARD->m_trueData->IsSupported( format ) )
            {
                int dataSize = M_CLIPBOARD->m_trueData->GetDataSize( format );
                transferred = true;

                if (dataSize == 0)
                {
                    data.SetData( format, 0, 0 );
                }
                else
                {
                    char *d = new char[ dataSize ];
                    M_CLIPBOARD->m_trueData->GetDataHere( format, (void*)d );
                    data.SetData( format, dataSize, d );
                    delete [] d;
                }
            }
        }
    }

    // get formats from wxDataObjects
    if ( !transferred )
    {
        transferred = data.GetFromPasteboard( M_CLIPBOARD->m_pasteboard ) ;
    }

    delete [] array;

    return transferred;
}

#endif
