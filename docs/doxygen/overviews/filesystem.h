/////////////////////////////////////////////////////////////////////////////
// Name:        fs
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page fs_overview wxFileSystem

 The wxHTML library uses a @b virtual file systems mechanism
 similar to the one used in Midnight Commander, Dos Navigator,
 FAR or almost any modern file manager. It allows the user to access
 data stored in archives as if they were ordinary files. On-the-fly
 generated files that exist only in memory are also supported.
 @b Classes
 Three classes are used in order to provide virtual file systems mechanism:


  The #wxFSFile class provides information
 about opened file (name, input stream, mime type and anchor).
  The #wxFileSystem class is the interface.
 Its main methods are ChangePathTo() and OpenFile(). This class
 is most often used by the end user.
  The #wxFileSystemHandler is the core
 of virtual file systems mechanism. You can derive your own handler and pass it to
 the VFS mechanism. You can derive your own handler and pass it to
 wxFileSystem's AddHandler() method. In the new handler you only need to
 override the OpenFile() and CanOpen() methods.


 @b Locations
 Locations (aka filenames aka addresses) are constructed from four parts:


  @b protocol - handler can recognize if it is able to open a
 file by checking its protocol. Examples are "http", "file" or "ftp".
  @b right location - is the name of file within the protocol.
 In "http://www.wxwidgets.org/index.html" the right location is "//www.wxwidgets.org/index.html".
  @b anchor - an anchor is optional and is usually not present.
 In "index.htm#chapter2" the anchor is "chapter2".
  @b left location - this is usually an empty string.
 It is used by 'local' protocols such as ZIP.
 See Combined Protocols paragraph for details.


 @b Combined Protocols
 The left location precedes the protocol in the URL string.
 It is not used by global protocols like HTTP but it becomes handy when nesting
 protocols - for example you may want to access files in a ZIP archive:
 file:archives/cpp_doc.zip#zip:reference/fopen.htm#syntax
 In this example, the protocol is "zip", right location is
 "reference/fopen.htm", anchor is "syntax" and left location
 is "file:archives/cpp_doc.zip".
 There are @b two protocols used in this example: "zip" and "file".
 @b File Systems Included in wxHTML
 The following virtual file system handlers are part of wxWidgets so far:






 @b wxArchiveFSHandler




 A handler for archives such as zip
 and tar. Include file is wx/fs_arc.h. URLs examples:
 "archive.zip#zip:filename", "archive.tar.gz#gzip:#tar:filename".





 @b wxFilterFSHandler




 A handler for compression schemes such
 as gzip. Header is wx/fs_filter.h. URLs are in the form, e.g.:
 "document.ps.gz#gzip:".





 @b wxInternetFSHandler




 A handler for accessing documents
 via HTTP or FTP protocols. Include file is wx/fs_inet.h.





 @b wxMemoryFSHandler




 This handler allows you to access
 data stored in memory (such as bitmaps) as if they were regular files.
 See @ref memoryfshandler_overview for details.
 Include file is wx/fs_mem.h. URL is prefixed with memory:, e.g.
 "memory:myfile.htm"





 In addition, wxFileSystem itself can access local files.

 @b Initializing file system handlers
 Use wxFileSystem::AddHandler to initialize
 a handler, for example:

 @code
 #include wx/fs_mem.h

 ...

 bool MyApp::OnInit()
 {
     wxFileSystem::AddHandler(new wxMemoryFSHandler);
 ...
 }
 @endcode

 */


