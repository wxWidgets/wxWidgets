// RCS-ID:      $Id$

//---------------------------------------------------------------------------
//-- all #includes that the whole Project needs. ----------------------------
//---------------------------------------------------------------------------
#include <wx/config.h>      // to let wxWindows choose a wxConfig class for your platform
#include <wx/confbase.h>    // base config class
// #include <wx/event.h>
// #include <wx/date.h>
#include <wx/dbtable.h>
#include <wx/generic/grid.h>
#include <wx/file.h>
#include <wx/wxhtml.h>
#include <wx/image.h>
#include <wx/imaglist.h>
#include <wx/intl.h>
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include <wx/resource.h>       // wxResourceParsedata/File/String
#include <wx/splitter.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/treectrl.h>
#ifdef __WXMSW__
    #include <wx/xpmhand.h>
#endif
//---------------------------
#include "tabpgwin.h"         // Original name : "controlarea.h"
//---------------------------
#include "dlguser.h"
#include "browsedb.h"
#include "dbtree.h"
#include "dbgrid.h"
#include "pgmctrl.h"
#include "doc.h"
#include "dbbrowse.h"
//---------------------------------------------------------------------------
