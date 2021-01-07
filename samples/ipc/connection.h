/////////////////////////////////////////////////////////////////////////////
// Name:        connection.h
// Purpose:     DDE sample: MyConnection class
// Author:      Vadim Zeitlin
// Created:     2008-02-11 (extracted from client.cpp)
// Copyright:   (c) 1999 Julian Smart
//                  2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLE_IPC_CONNECTION_H_
#define _WX_SAMPLE_IPC_CONNECTION_H_

// This simple connection class adds logging of all operations
class MyConnectionBase : public wxConnection
{
protected:
    void Log(const wxString& command,
             const wxString& topic,
             const wxString& item,
             const void *data,
             size_t size,
             wxIPCFormat format)
    {
        wxString s;
        if (topic.IsEmpty() && item.IsEmpty())
            s.Printf("%s(", command);
        else if (topic.IsEmpty())
            s.Printf("%s(item=\"%s\",", command, item);
        else if (item.IsEmpty())
            s.Printf("%s(topic=\"%s\",", command, topic);
        else
            s.Printf("%s(topic=\"%s\",item=\"%s\",", command, topic, item);

        switch (format)
        {
          case wxIPC_TEXT:
              s += wxString(static_cast<const char *>(data), size);
              break;

#if wxUSE_UNICODE
          case wxIPC_UNICODETEXT:
              s += wxString(static_cast<const wchar_t *>(data), size);
              break;
#endif // wxUSE_UNICODE

          case wxIPC_UTF8TEXT:
              s += wxString::FromUTF8(static_cast<const char *>(data), size);
              break;

          case wxIPC_PRIVATE:
              if ( size == 3 )
              {
                  const char *bytes = static_cast<const char *>(data);
                  s << '"' << bytes[0] << bytes[1] << bytes[2] << '"';
              }
              else
              {
                  s << "\"???\"";
              }
              break;

          case wxIPC_INVALID:
              s += "[invalid data]";
              break;

          default:
              s += "[unknown data]";
              break;
        }

        wxLogMessage("%s,%lu)", s, (unsigned long)size);
    }
};

#endif // _WX_SAMPLE_IPC_CONNECTION_H_
