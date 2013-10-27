; This is a fragment that is supposed to be pasted into your autoexp.dat file
; found somewhere under MSVS installation directory (its exact location depends
; MSVS version so the simplest thing to do is to just search for it there).

[AutoExpand]
wxPoint=<x>,<y>
wxSize=<x>*<y>
wxRect=<x>,<y> <width>*<height>

wxLongLong=<m_ll>
wxString=<m_impl>

[Visualizer]
wxDateTime{
        preview  (#if ($e.IsValid()) ([wxDumpDate(&($e)),s]) #else (#("NIL")))
}
