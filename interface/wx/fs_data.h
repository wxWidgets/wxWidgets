/////////////////////////////////////////////////////////////////////////////
// Name:        wx/fs_data.h
// Purpose:     DATA scheme file system
// Author:      Vyacheslav Lisovski
// Copyright:   (c) 2023 Vyacheslav Lisovski
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxDataSchemeFSHandler

    File system handler for "data" URI scheme (RFC 2397).
    URI syntax: @c "data:[<mediatype>][;base64],<data>".

    The handler makes the data, included (encoded) into URI, available for
    components that use the wxFileSystem infrastructure.

    To make available for usage it should be registered somewhere within an
    initialization procedure:
    @code
    wxFileSystem::AddHandler(new wxDataSchemeFSHandler);
    @endcode

    @since 3.3.0
*/

class wxDataSchemeFSHandler : public wxFileSystemHandler
{
public:
    wxDataSchemeFSHandler();
};
