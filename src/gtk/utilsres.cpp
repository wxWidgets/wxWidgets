/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////


//#ifdef __GNUG__
//#pragma implementation "utils.h"
//#endif

#include "wx/utils.h"
#include "wx/string.h"
#include "wx/list.h"
#include "wx/log.h"
#include "wx/gdicmn.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>
//#ifdef __SVR4__
//#include <sys/systeminfo.h>
//#endif

#include "gdk/gdkx.h"        // GDK_DISPLAY
#include "gdk/gdkprivate.h"  // gdk_progclass

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

// Yuck this is really BOTH site and platform dependent
// so we should use some other strategy!
#ifdef __SUN__
    #define DEFAULT_XRESOURCE_DIR _T("/usr/openwin/lib/app-defaults")
#else
    #define DEFAULT_XRESOURCE_DIR _T("/usr/lib/X11/app-defaults")
#endif

//-----------------------------------------------------------------------------
// glabal data (data.cpp)
//-----------------------------------------------------------------------------

extern wxResourceCache *wxTheResourceCache;
extern XrmDatabase wxResourceDatabase;

//-----------------------------------------------------------------------------
// utility functions for get/write resources
//-----------------------------------------------------------------------------

static wxChar *GetResourcePath(wxChar *buf, wxChar *name, bool create)
{
    if (create && FileExists(name)) 
    {
        wxStrcpy(buf, name);
        return buf; // Exists so ...
    }
    if (*name == _T('/'))
        wxStrcpy(buf, name);
    else 
    {
        // Put in standard place for resource files if not absolute
        wxStrcpy(buf, DEFAULT_XRESOURCE_DIR);
        wxStrcat(buf, _T("/"));
        wxStrcat(buf, FileNameFromPath(name));
    }
    if (create) 
    {
        // Touch the file to create it
        FILE *fd = fopen(wxConvCurrent->cWX2MB(buf), "w");
        if (fd) fclose(fd);
    }
    return buf;
}

// Read $HOME for what it says is home, if not
// read $USER or $LOGNAME for user name else determine
// the Real User, then determine the Real home dir.
static wxChar *GetIniFile(wxChar *dest, const wxChar *filename)
{
    const wxChar *home = (const wxChar *) NULL;
    if (filename && wxIsAbsolutePath(filename))
    {
      wxStrcpy(dest, filename);
    }
    else
    {
      if ((home = wxGetUserHome(wxString())) != NULL)
      {
        wxStrcpy(dest, home);
        if (dest[wxStrlen(dest) - 1] != _T('/')) wxStrcat(dest, _T("/"));
        if (filename == NULL)
        {
          if ((filename = wxGetenv(_T("XENVIRONMENT"))) == NULL) filename = _T(".Xdefaults");
        }
        else
          if (*filename != _T('.')) wxStrcat(dest, _T("."));
        wxStrcat(dest, filename);
      }
      else
      {
        dest[0] = _T('\0');
      }
    }
    return dest;
}

static void wxXMergeDatabases()
{
    XrmDatabase homeDB, serverDB, applicationDB;
    wxChar filenamebuf[1024];

    wxChar *filename = &filenamebuf[0];
    wxChar *environment;
    char *classname = gdk_progclass;               // Robert Roebling ??
    char name[256];
    (void)strcpy(name, "/usr/lib/X11/app-defaults/");
    (void)strcat(name, classname ? classname : "wxWindows");

    // Get application defaults file, if any
    if ((applicationDB = XrmGetFileDatabase(name)))
        (void)XrmMergeDatabases(applicationDB, &wxResourceDatabase);

    // Merge server defaults, created by xrdb, loaded as a property of the root
    // window when the server initializes and loaded into the display
    // structure on XOpenDisplay;
    // if not defined, use .Xdefaults
    if (XResourceManagerString(GDK_DISPLAY()) != NULL) 
    {
        serverDB = XrmGetStringDatabase(XResourceManagerString(GDK_DISPLAY()));
    } 
    else 
    {
        (void)GetIniFile(filename, (wxChar *) NULL);
        serverDB = XrmGetFileDatabase(wxConvCurrent->cWX2MB(filename));
    }
    if (serverDB)
        XrmMergeDatabases(serverDB, &wxResourceDatabase);

    // Open XENVIRONMENT file, or if not defined, the .Xdefaults,
    // and merge into existing database

    if ((environment = wxGetenv(_T("XENVIRONMENT"))) == NULL) 
    {
        size_t len;
#if wxUSE_UNICODE
	char hostbuf[1024];
#endif
        environment = GetIniFile(filename, (const wxChar *) NULL);
        len = wxStrlen(environment);
#if !defined(SVR4) || defined(__sgi)
#if wxUSE_UNICODE
        (void)gethostname(hostbuf, 1024 - len);
#else
        (void)gethostname(environment + len, 1024 - len);
#endif
#else
#if wxUSE_UNICODE
        (void)sysinfo(SI_HOSTNAME, hostbuf, 1024 - len);
#else
        (void)sysinfo(SI_HOSTNAME, environment + len, 1024 - len);
#endif
#endif
#if wxUSE_UNICODE
	wxStrcat(environment, wxConvCurrent->cMB2WX(hostbuf));
#endif
    }
    if ((homeDB = XrmGetFileDatabase(wxConvCurrent->cWX2MB(environment))))
        XrmMergeDatabases(homeDB, &wxResourceDatabase);
}

//-----------------------------------------------------------------------------
// called on application exit
//-----------------------------------------------------------------------------

void wxFlushResources()
{
    wxChar nameBuffer[512];

    wxNode *node = wxTheResourceCache->First();
    while (node) {
        wxString str = node->GetKeyString();
        wxChar *file = WXSTRINGCAST str;
        // If file doesn't exist, create it first.
        (void)GetResourcePath(nameBuffer, file, TRUE);

        XrmDatabase database = (XrmDatabase)node->Data();
        XrmPutFileDatabase(database, wxConvCurrent->cWX2MB(nameBuffer));
        XrmDestroyDatabase(database);
        wxNode *next = node->Next();
//        delete node;
        node = next;
    }
}

void wxDeleteResources(const wxChar *file)
{
    wxLogTrace(wxTraceResAlloc, _T("Delete: Number = %d"), wxTheResourceCache->Number());
    wxChar buffer[500];
    (void)GetIniFile(buffer, file);

    wxNode *node = wxTheResourceCache->Find(buffer);
    if (node) {
        XrmDatabase database = (XrmDatabase)node->Data();
        XrmDestroyDatabase(database);
//        delete node;
    }
}

//-----------------------------------------------------------------------------
// resource functions
//-----------------------------------------------------------------------------

bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file )
{
    wxChar buffer[500];

    if (!entry) return FALSE;

    (void)GetIniFile(buffer, file);

    XrmDatabase database;
    wxNode *node = wxTheResourceCache->Find(buffer);
    if (node)
        database = (XrmDatabase)node->Data();
    else {
        database = XrmGetFileDatabase(wxConvCurrent->cWX2MB(buffer));
        wxLogTrace(wxTraceResAlloc, _T("Write: Number = %d"), wxTheResourceCache->Number());
        wxTheResourceCache->Append(buffer, (wxObject *)database);
    }
    char resName[300];
    strcpy(resName, !section.IsNull() ? MBSTRINGCAST section.mb_str() : "wxWindows");
    strcat(resName, ".");
    strcat(resName, entry.mb_str());
    XrmPutStringResource(&database, resName, value.mb_str());
    return TRUE;
};

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%.4f", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%ld", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%d", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file )
{
    if (!wxResourceDatabase)
        wxXMergeDatabases();

    XrmDatabase database;
    if (!file.IsEmpty()) 
    {
        wxChar buffer[500];
        // Is this right? Trying to get it to look in the user's
        // home directory instead of current directory -- JACS
        (void)GetIniFile(buffer, file);

        wxNode *node = (wxNode*) NULL;  /* suppress egcs warning */
        node = wxTheResourceCache->Find(buffer);
        if (node)
        {
            database = (XrmDatabase)node->Data();
        }
        else
        {
            database = XrmGetFileDatabase(wxConvCurrent->cWX2MB(buffer));
            wxLogTrace(wxTraceResAlloc, _T("Get: Number = %d"), wxTheResourceCache->Number());
            wxTheResourceCache->Append(buffer, (wxObject *)database);
        }
    } else
        database = wxResourceDatabase;

    XrmValue xvalue;
    char *str_type[20];
    char buf[150];
    strcpy(buf, section.mb_str());
    strcat(buf, ".");
    strcat(buf, entry.mb_str());

    bool success = XrmGetResource(database, buf, "*", str_type, &xvalue);
    // Try different combinations of upper/lower case, just in case...
    if (!success) 
    {
        buf[0] = (isupper(buf[0]) ? tolower(buf[0]) : toupper(buf[0]));
        success = XrmGetResource(database, buf, "*", str_type,        &xvalue);
    }
    if (success) 
    {
        if (*value)
            delete[] *value;
        *value = new char[xvalue.size + 1];
        strncpy(*value, xvalue.addr, (int)xvalue.size);
        return TRUE;
    }
    return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) 
    {
        *value = (float)strtod(s, (char **) NULL);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) 
    {
        *value = strtol(s, (char **) NULL, 10);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) 
    {
        // Handle True, False here
        // True, Yes, Enables, Set or  Activated
        if (*s == 'T' || *s == 'Y' || *s == 'E' || *s == 'S' || *s == 'A')
            *value = TRUE;
        // False, No, Disabled, Reset, Cleared, Deactivated
        else if (*s == 'F' || *s == 'N' || *s == 'D' || *s == 'R' || *s == 'C')
            *value = FALSE;
        // Handle as Integer
        else
            *value = (int)strtol(s, (char **) NULL, 10);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

