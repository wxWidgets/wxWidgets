/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/artqt.cpp
// Purpose:     stock wxArtProvider instance with native Qt stock icons
// Author:      Kettab Ali
// Created:     2026-02-12
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/artprov.h"
#include "wx/bitmap.h"
#include "wx/qt/private/converter.h"

#include <QtGui/QIcon>

#include <QtWidgets/QApplication>
#include <QtWidgets/QStyle>

namespace // anonymous
{
// This helper function tries to return the standard icon QStyle::StandardPixmap
// corresponding to the wxArtID if it exists. It tries to return the corresponding
// theme icon using QIcon::fromTheme() function otherwise.
QIcon GetQtIconFromArtID(const wxArtID& id)
{
    QStyle::StandardPixmap standardIcon;

    if ( id == wxART_GO_BACK )
        standardIcon = QStyle::SP_ArrowBack;
    else if ( id == wxART_GO_FORWARD )
        standardIcon = QStyle::SP_ArrowForward;
    else if ( id == wxART_GO_UP )
        standardIcon = QStyle::SP_ArrowUp;
    else if ( id == wxART_GO_DOWN )
        standardIcon = QStyle::SP_ArrowDown;
    else if ( id == wxART_GO_TO_PARENT )
        standardIcon = QStyle::SP_FileDialogToParent;
    else if ( id == wxART_GO_HOME )
        standardIcon = QStyle::SP_DirHomeIcon;
    else if ( id == wxART_FILE_OPEN )
        standardIcon = QStyle::SP_DialogOpenButton;
    else if ( id == wxART_FILE_SAVE )
        standardIcon = QStyle::SP_DialogSaveButton;
    else if ( id == wxART_HELP )
        standardIcon = QStyle::SP_DialogHelpButton;
    else if ( id == wxART_LIST_VIEW )
        standardIcon = QStyle::SP_FileDialogListView;
    else if ( id == wxART_NEW_DIR )
        standardIcon = QStyle::SP_FileDialogNewFolder;
    else if ( id == wxART_HARDDISK )
        standardIcon = QStyle::SP_DriveHDIcon;
    else if ( id == wxART_FLOPPY )
        standardIcon = QStyle::SP_DriveFDIcon;
    else if ( id == wxART_CDROM )
        standardIcon = QStyle::SP_DriveCDIcon;
    else if ( id == wxART_FOLDER )
        standardIcon = QStyle::SP_DirIcon;
    else if ( id == wxART_FOLDER_OPEN )
        standardIcon = QStyle::SP_DirOpenIcon;
    else if ( id == wxART_NORMAL_FILE )
        standardIcon = QStyle::SP_FileIcon;
    else if ( id == wxART_TICK_MARK )
        standardIcon = QStyle::SP_DialogOkButton;
    else if ( id == wxART_CROSS_MARK )
        standardIcon = QStyle::SP_DialogCancelButton;
    else if ( id == wxART_ERROR )
        standardIcon = QStyle::SP_MessageBoxCritical;
    else if ( id == wxART_QUESTION )
        standardIcon = QStyle::SP_MessageBoxQuestion;
    else if ( id == wxART_WARNING )
        standardIcon = QStyle::SP_MessageBoxWarning;
    else if ( id == wxART_INFORMATION )
        standardIcon = QStyle::SP_MessageBoxInformation;
    else if ( id == wxART_CLOSE )
        standardIcon = QStyle::SP_DialogCloseButton;
    else if ( id == wxART_REFRESH )
        standardIcon = QStyle::SP_BrowserReload;
    else if ( id == wxART_STOP )
        standardIcon = QStyle::SP_BrowserStop;
    else
        standardIcon = QStyle::SP_CustomBase;

    if ( standardIcon != QStyle::SP_CustomBase )
    {
        return QApplication::style()->standardIcon(standardIcon);
    }

    // Try returning theme icon if there is no corresponding standard icon:

#define TRY_RETURN_THEMEICON(themeId)           \
    {                                           \
        if ( QIcon::hasThemeIcon(themeId) )     \
            return QIcon::fromTheme(themeId);   \
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    #define TRY_RETURN_THEMEICON2(themeId, unused) \
        return QIcon::fromTheme(QIcon::ThemeIcon::themeId);
#else // Qt version < 6.7
    #define TRY_RETURN_THEMEICON2(unused, themeId) TRY_RETURN_THEMEICON(themeId)
#endif // Qt version >= 6.7

    if ( id == wxART_ADD_BOOKMARK )
        TRY_RETURN_THEMEICON2(ListAdd, "list-add")
    else if ( id == wxART_DEL_BOOKMARK )
        TRY_RETURN_THEMEICON2(ListRemove, "list-remove")
    else if ( id == wxART_HELP_SETTINGS )
        TRY_RETURN_THEMEICON("preferences-desktop-font")
    else if ( id == wxART_HELP_FOLDER )
        TRY_RETURN_THEMEICON("folder")
    else if ( id == wxART_HELP_PAGE )
        TRY_RETURN_THEMEICON("text-x-generic")
    else if ( id == wxART_GOTO_FIRST )
        TRY_RETURN_THEMEICON("go-first")
    else if ( id == wxART_GOTO_LAST )
        TRY_RETURN_THEMEICON("go-last")
    else if ( id == wxART_FILE_SAVE_AS )
        TRY_RETURN_THEMEICON2(DocumentSaveAs, "document-save-as")
    else if ( id == wxART_PRINT )
        TRY_RETURN_THEMEICON2(DocumentPrint, "document-print")
    else if ( id == wxART_TIP )
        TRY_RETURN_THEMEICON2(DialogInformation, "dialog-information")
    else if ( id == wxART_REMOVABLE )
        TRY_RETURN_THEMEICON2(MediaFlash, "drive-removable-media")
    else if ( id == wxART_EXECUTABLE_FILE )
        TRY_RETURN_THEMEICON("system-run")
    else if ( id == wxART_MISSING_IMAGE )
        TRY_RETURN_THEMEICON2(ImageMissing, "image-missing")
    else if ( id == wxART_COPY )
        TRY_RETURN_THEMEICON2(EditCopy, "edit-copy")
    else if ( id == wxART_CUT )
        TRY_RETURN_THEMEICON2(EditCut, "edit-cut")
    else if ( id == wxART_PASTE )
        TRY_RETURN_THEMEICON2(EditPaste, "edit-paste")
    else if ( id == wxART_DELETE )
        TRY_RETURN_THEMEICON2(EditDelete, "edit-delete")
    else if ( id == wxART_NEW )
        TRY_RETURN_THEMEICON2(DocumentNew, "document-new")
    else if ( id == wxART_UNDO )
        TRY_RETURN_THEMEICON2(EditUndo, "edit-undo")
    else if ( id == wxART_REDO )
        TRY_RETURN_THEMEICON2(EditRedo, "edit-redo")
    else if ( id == wxART_PLUS )
        TRY_RETURN_THEMEICON2(ListAdd, "list-add")
    else if ( id == wxART_MINUS )
        TRY_RETURN_THEMEICON2(ListRemove, "list-remove")
    else if ( id == wxART_QUIT )
        TRY_RETURN_THEMEICON2(ApplicationExit, "application-exit")
    else if ( id == wxART_FIND )
        TRY_RETURN_THEMEICON2(EditFind, "edit-find")
    else if ( id == wxART_FIND_AND_REPLACE )
        TRY_RETURN_THEMEICON("edit-find-replace")
    else if ( id == wxART_FULL_SCREEN )
        TRY_RETURN_THEMEICON2(ViewFullscreen, "view-fullscreen")
    else if ( id == wxART_EDIT )
        TRY_RETURN_THEMEICON2(InsertText, "accessories-text-editor")
//  else if ( id == wxART_HELP_SIDE_PANEL )
//  else if ( id == wxART_HELP_BOOK )
//  else if ( id == wxART_GO_DIR_UP )
//  else if ( id == wxART_REPORT_VIEW )

    return QIcon();
}
} // anonymous

// ----------------------------------------------------------------------------
// wxQtArtProvider
// ----------------------------------------------------------------------------

class wxQtArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size) override;
};

wxBitmap wxQtArtProvider::CreateBitmap(const wxArtID& id,
                                       const wxArtClient& client,
                                       const wxSize& size)
{
    wxSize iconSize = size != wxDefaultSize
                    ? size : GetNativeDIPSizeHint(client);

    if ( iconSize != wxDefaultSize )
    {
        const QIcon qtIcon = GetQtIconFromArtID(id);

        if ( !qtIcon.isNull() )
        {
            return wxBitmap(qtIcon.pixmap(wxQtConvertSize(iconSize)));
        }
    }

    return wxNullBitmap;
}

// ----------------------------------------------------------------------------
// wxArtProvider::InitNativeProvider
// ----------------------------------------------------------------------------

/*static*/
void wxArtProvider::InitNativeProvider()
{
    PushBack(new wxQtArtProvider);
}

// ----------------------------------------------------------------------------
// wxArtProvider::GetNativeSizeHint()
// ----------------------------------------------------------------------------

/*static*/
wxSize wxArtProvider::GetNativeDIPSizeHint(const wxArtClient& client)
{
    QStyle* const qtStyle = QApplication::style();

    int pm = -1;

    if ( client == wxART_TOOLBAR )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_ToolBarIconSize);
    }
    else if ( client == wxART_MENU )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_SmallIconSize);
    }
    else if ( client == wxART_FRAME_ICON )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_SmallIconSize);
    }
    else if ( client == wxART_CMN_DIALOG ||
              client == wxART_MESSAGE_BOX )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_MessageBoxIconSize);
    }
    else if ( client == wxART_BUTTON )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_ButtonIconSize);
    }
    else if ( client == wxART_LIST )
    {
        pm = qtStyle->pixelMetric(QStyle::PM_ListViewIconSize);
    }

    return wxSize(pm, pm);
}
