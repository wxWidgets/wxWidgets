Welcome to wxWidgets Configuration Tool!
=============================================

wxWidgets Configuration Tool is a tool for
helping to configure wxWidgets.

For further information, please see the manual, and the
following site:

http://www.wxwidgets.org

------------------------------------------------------
Installing wxWidgets Configuration Tool on Windows
------------------------------------------------------

Run wxconfigtool-x.yz-setup.exe and follow the instructions
to install wxWidgets Configuration Tool.

You should find a shortcut on your desktop, and you can
also find it in a group similar to this:

Start -> Program Files -> wxWidgets Configuration Tool x.y

------------------------------------------------------
Installing wxWidgets Configuration Tool on Unix
------------------------------------------------------

Unarchive wxconfigtool-x.yz.tar.gz to a suitable location
in your filesystem. A directory of the form wxconfigtool-x.yz
(where x.yz is the version number) will be created.

Add the location to your PATH and run the application with
'wxconfigtool'. You may wish to set the environment variable
WXCONFIGTOOLDIR so that wxWidgets Configuration Tool can find its data files.

For example:

  % cd ~
  % tar xvfz wxconfigtool-1.01.tar.gz
  % export WXCONFIGTOOLDIR=`pwd`/wxconfigtool-1.01
  % export PATH=$PATH:$WXCONFIGTOOLDIR
  % wxconfigtool

If you don't want to change your PATH, you could place a
script in a location already on your PATH, such as
/usr/local/bin. For example:

  #!/bin/sh
  # Invokes wxWidgets Configuration Tool
  export WXCONFIGTOOLDIR=/home/mydir/wxconfigtool-1.01
  $WXCONFIGTOOLDIR/wxconfigtool $*

If you wish to associate the wxWidgets Configuration Tool file extension
(pjd) with wxWidgets Configuration Tool, you will need to edit your
~/.mailcap file (or create one if it doesn't exist)
and add an entry similar to this:

    application/wxconfigtool; wxconfigtool %s
    
and add this ~/.mime.types:
    
    type=application/wxconfigtool \
    desc="wxWidgets Configuration Tool Document" \
    exts="pjd"


------------------------------------------------------
(c) Julian Smart, June 2003

