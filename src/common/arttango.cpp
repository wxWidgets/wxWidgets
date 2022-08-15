///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/arttango.cpp
// Purpose:     art provider using embedded SVG versions of Tango icons
// Author:      Vadim Zeitlin
// Created:     2010-12-27
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_ARTPROVIDER_TANGO

#ifndef WX_PRECOMP
    #include "wx/image.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"

#include "wx/mstream.h"

// ----------------------------------------------------------------------------
// image data
// ----------------------------------------------------------------------------

// All files in art/tango in alphabetical order:
#include "../../art/tango/application_exit.h"
#include "../../art/tango/application_x_executable.h"
#include "../../art/tango/dialog_error.h"
#include "../../art/tango/dialog_information.h"
#include "../../art/tango/dialog_warning.h"
#include "../../art/tango/document_new.h"
#include "../../art/tango/document_open.h"
#include "../../art/tango/document_print.h"
#include "../../art/tango/document_save.h"
#include "../../art/tango/document_save_as.h"
#include "../../art/tango/drive_harddisk.h"
#include "../../art/tango/drive_optical.h"
#include "../../art/tango/drive_removable_media.h"
#include "../../art/tango/edit_copy.h"
#include "../../art/tango/edit_cut.h"
#include "../../art/tango/edit_delete.h"
#include "../../art/tango/edit_find.h"
#include "../../art/tango/edit_find_replace.h"
#include "../../art/tango/edit_paste.h"
#include "../../art/tango/edit_redo.h"
#include "../../art/tango/edit_undo.h"
#include "../../art/tango/folder.h"
#include "../../art/tango/folder_new.h"
#include "../../art/tango/folder_open.h"
#include "../../art/tango/fullscreen.h"
#include "../../art/tango/go_down.h"
#include "../../art/tango/go_first.h"
#include "../../art/tango/go_home.h"
#include "../../art/tango/go_last.h"
#include "../../art/tango/go_next.h"
#include "../../art/tango/go_previous.h"
#include "../../art/tango/go_up.h"
#include "../../art/tango/image_missing.h"
#include "../../art/tango/list_add.h"
#include "../../art/tango/list_remove.h"
#include "../../art/tango/process_stop.h"
#include "../../art/tango/text_x_generic.h"
#include "../../art/tango/view_refresh.h"

// ----------------------------------------------------------------------------
// art provider class
// ----------------------------------------------------------------------------

namespace
{

class wxTangoArtProvider : public wxArtProvider
{
public:
    wxTangoArtProvider()
    {
    }

protected:
    virtual wxBitmapBundle CreateBitmapBundle(const wxArtID& id,
        const wxArtClient& client,
        const wxSize& size) wxOVERRIDE;

private:

    wxDECLARE_NO_COPY_CLASS(wxTangoArtProvider);
};

} // anonymous namespace

// ============================================================================
// implementation
// ============================================================================

wxBitmapBundle
wxTangoArtProvider::CreateBitmapBundle(const wxArtID& id,
                                 const wxArtClient& client,
                                 const wxSize& size)
{
#ifdef wxHAS_SVG
    // Array indexed by the id names with pointers to svg data.
    // The order of the elements in this array is the
    // same as the definition order in wx/artprov.h. While it's not very
    // logical, this should make it simpler to add new icons later. Notice that
    // most elements without Tango equivalents are simply omitted.

    // To avoid repetition use BITMAP_DATA to only specify the image name once
    // (this is especially important if we decide to add more image sizes
    // later).
    #define BITMAP_DATA(name) \
        name ## _svg_data, sizeof(name ## _svg_data)

    static const struct BitmapEntry
    {
        wxString id;
        const unsigned char *data;
        size_t len;
    } s_allBitmaps[] =
    {
        // Tango does have bookmark-new but no matching bookmark-delete and
        // using mismatching icons would be ugly so we don't provide this one
        // either, we should add both of them if Tango ever adds the other one.
        //{ wxART_ADD_BOOKMARK,       BITMAP_DATA(bookmark_new)},
        //{ wxART_DEL_BOOKMARK,       BITMAP_DATA() },

        { wxART_GO_BACK,            BITMAP_DATA(go_previous)                },
        { wxART_GO_FORWARD,         BITMAP_DATA(go_next)                    },
        { wxART_GO_UP,              BITMAP_DATA(go_up)                      },
        { wxART_GO_DOWN,            BITMAP_DATA(go_down)                    },
        // wxART_GO_TO_PARENT doesn't seem to exist in Tango
        { wxART_GO_HOME,            BITMAP_DATA(go_home)                    },
        { wxART_GOTO_FIRST,         BITMAP_DATA(go_first)                   },
        { wxART_GOTO_LAST,          BITMAP_DATA(go_last)                    },

        { wxART_FILE_OPEN,          BITMAP_DATA(document_open)              },
        { wxART_FILE_SAVE,          BITMAP_DATA(document_save)              },
        { wxART_FILE_SAVE_AS,       BITMAP_DATA(document_save_as)           },
        { wxART_PRINT,              BITMAP_DATA(document_print)             },

        // Should we use help-browser for wxART_HELP?

        { wxART_NEW_DIR,            BITMAP_DATA(folder_new)                 },
        { wxART_HARDDISK,           BITMAP_DATA(drive_harddisk)             },
        // drive-removable-media seems to be better than media-floppy
        { wxART_FLOPPY,             BITMAP_DATA(drive_removable_media)      },
        { wxART_CDROM,              BITMAP_DATA(drive_optical)              },
        { wxART_REMOVABLE,          BITMAP_DATA(drive_removable_media)      },

        { wxART_FOLDER,             BITMAP_DATA(folder)                     },
        { wxART_FOLDER_OPEN,        BITMAP_DATA(folder_open)                },
        // wxART_GO_DIR_UP doesn't seem to exist in Tango

        { wxART_EXECUTABLE_FILE,    BITMAP_DATA(application_x_executable)   },
        { wxART_NORMAL_FILE,        BITMAP_DATA(text_x_generic)             },

        // There is no dialog-question in Tango so use the information icon
        // too, this is better for consistency and we do have a precedent for
        // doing this as Windows Vista/7 does the same thing natively.
        { wxART_ERROR,              BITMAP_DATA(dialog_error)               },
        { wxART_QUESTION,           BITMAP_DATA(dialog_information)         },
        { wxART_WARNING,            BITMAP_DATA(dialog_warning)             },
        { wxART_INFORMATION,        BITMAP_DATA(dialog_information)         },

        { wxART_MISSING_IMAGE,      BITMAP_DATA(image_missing)              },

        { wxART_COPY,               BITMAP_DATA(edit_copy)                  },
        { wxART_CUT,                BITMAP_DATA(edit_cut)                   },
        { wxART_PASTE,              BITMAP_DATA(edit_paste)                 },
        { wxART_DELETE,             BITMAP_DATA(edit_delete)                },
        { wxART_NEW,                BITMAP_DATA(document_new)               },
        { wxART_UNDO,               BITMAP_DATA(edit_undo)                  },
        { wxART_REDO,               BITMAP_DATA(edit_redo)                  },

        { wxART_PLUS,               BITMAP_DATA(list_add)                   },
        { wxART_MINUS,              BITMAP_DATA(list_remove)                },

        // Surprisingly Tango doesn't seem to have wxART_CLOSE.

        { wxART_QUIT,               BITMAP_DATA(application_exit)           },

        { wxART_FIND,               BITMAP_DATA(edit_find)                  },
        { wxART_FIND_AND_REPLACE,   BITMAP_DATA(edit_find_replace)          },
        { wxART_FULL_SCREEN,        BITMAP_DATA(fullscreen)                 },
        { wxART_REFRESH,            BITMAP_DATA(view_refresh)               },
        { wxART_STOP,               BITMAP_DATA(process_stop)               },

        // Note: when adding elements here, try to also add the corresponding
        //       icon to src/gtk/artgtk.cpp as the GTK art provider is supposed
        //       to have all the icons the Tango provider has, see configure.in.
    };

    #undef BITMAP_DATA

    for ( unsigned n = 0; n < WXSIZEOF(s_allBitmaps); n++ )
    {
        const BitmapEntry& entry = s_allBitmaps[n];
        if ( entry.id != id )
            continue;

        wxSize sizeDef = size != wxDefaultSize ? size : GetSizeHint(client);
        if (sizeDef == wxDefaultSize)
        {
            // We really need some default size here, so keep using the same
            // sizes we used for PNG-based implementation we had before.
            if ( client == wxART_MENU || client == wxART_BUTTON )
                sizeDef = wxSize(16, 16);
            else
                sizeDef = wxSize(24, 24);
        }
        return wxBitmapBundle::FromSVG(entry.data, entry.len, sizeDef);
    }

#else // !wxHAS_SVG
    wxUnusedVar(id);
    wxUnusedVar(client);
    wxUnusedVar(size);
#endif // wxHAS_SVG/!wxHAS_SVG

    // Not one of the bitmaps that we support.
    return wxBitmapBundle();
}

/* static */
void wxArtProvider::InitTangoProvider()
{
    wxArtProvider::PushBack(new wxTangoArtProvider);
}

#endif // wxUSE_ARTPROVIDER_TANGO
