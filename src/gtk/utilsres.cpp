/////////////////////////////////////////////////////////////////////////////
// Name:        utilres.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//#ifdef __GNUG__
//#pragma implementation "utils.h"
//#endif

#include "wx/utils.h"
#include "wx/string.h"
#include "wx/list.h"
#include "wx/log.h"
#include "wx/config.h"
#include "wx/app.h"

//-----------------------------------------------------------------------------
// resource functions
//-----------------------------------------------------------------------------

bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file )
{
    wxString filename( file );
    if (filename.IsEmpty()) filename = wxT(".wxWindows");
    
    wxFileConfig conf( wxTheApp->GetAppName(), wxTheApp->GetVendorName(), filename );
    
    conf.SetPath( section );
    
    return conf.Write( entry, value );
}

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file )
{
    wxString buf;
    buf.Printf(wxT("%.4f"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file )
{
    wxString buf;
    buf.Printf(wxT("%ld"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file )
{
    wxString buf;
    buf.Printf(wxT("%d"), value);

    return wxWriteResource(section, entry, buf, file);
}

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file )
{
    wxString filename( file );
    if (filename.IsEmpty()) filename = wxT(".wxWindows");
    
    wxFileConfig conf( wxTheApp->GetAppName(), wxTheApp->GetVendorName(), filename );
    
    conf.SetPath( section );
    
    wxString result;
    if (conf.Write( entry, &result ))
    {
        if (!result.IsEmpty())
	{
            char *s = new char[result.Len()+1];
	    wxStrcpy( s, result.c_str() );
	    *value = s;
	    return TRUE;
	}
    }
    
    return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file )
{
    wxChar *s = NULL;
    bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
    if (succ)
    {
        *value = (float)wxStrtod(s, NULL);
        delete[] s;
        return TRUE;
    }
    else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file )
{
    wxChar *s = NULL;
    bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
    if (succ)
    {
        *value = wxStrtol(s, NULL, 10);
        delete[] s;
        return TRUE;
    }
    else return FALSE;
}

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file )
{
    wxChar *s = NULL;
    bool succ = wxGetResource(section, entry, (wxChar **)&s, file);
    if (succ)
    {
        *value = (int)wxStrtol(s, NULL, 10);
        delete[] s;
        return TRUE;
    }
    else return FALSE;
}

