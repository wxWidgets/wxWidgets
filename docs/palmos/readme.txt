wxWidgets 2.5.3 CVS 10-18-2004
------------------------------

This is a Palm OS 6 port of the wxWidgets toolkit.  Included in the source 
package is the complete source necessary to build the Palm OS port using the 
Palm OS Developer Studio.  Also included is the diff showing the changes that 
need to be incorporated into the wxWidgets 2.5.3 CVS source tree before the 
Palm OS port can be integrated.

I am releasing this port to the wxWidgets team under the terms of the wxWindows 
license.  A copy of the wxWindows license is included in the source package.

No third party libraries or code were used in making this port.  The port consists 
entirely of code I personally wrote combined with code that was included in the 
wxWidgets 2.5.3 CSV source tree.  Whenever possible, I have used the Palm OS 6 API, 
in places where the API was not practical to use, I wrote my own functions.

I am submitting this source package as an entry in the wxWidgets Palm OS 6 port 
challenge.  Details on this challenge are available at http://www.wxwindows.org/palmos.htm

Please note that this port is in a very early state.  Currently the port 
provides limited functionality for the wxFrame, wxMenu, wxStatusBar, 
wxTopLevelWindow, and wxWindow classes.  The other classes in the wxWidgets 
toolkit still need to be ported.  In it's current state, the Palm OS 6 port 
is capable of running the minimal wxWidgets sample

The easiest way to see the port in action is to build the source package with 
the Palm OS Developer Studio available at http://www.palmos.com/dev/dl/dl_tools/dl_pods/  
Here are the steps that you need to follow to build the package:

1) Unpack the ZIP file into a directory located within the Palm OS Developer 
   Studio workspace directory.  The path to the folder should look something 
   like this: "C:\Program Files\PalmSource\Palm OS Developer Suite\workspace\wxWidgets"

2) Open Palm OS Developer Studio and click on File->Import.

3) Select "Existing Project into Workspace" as the import source and click Next.

4) On the next screen click the Browse button next to Project contents and select
   the folder you unpacked the source package into.  Click on Finish to continue.

5) You should now see the project listed in the Project pane on the right side of the 
   window.  Click on the name of the project.  Now click on the Project menu and select
   "Rebuild Project"

6) After the build is complete, click on the Run menu and select ""  The Palm OS 6 simulator
   should start.  If this is the first time you have used the simulator, you will be prompted  
   to choose a ROM file.  You can find the ROM files in a folder like "C:\Program 
   Files\PalmSource\Palm OS Developer Suite\sdk-6\tools\Simulator\6.0.1\ROM"  It doesn't 
   really matter whether you choose a Release or a Debug ROM (the debug ROM generates extra 
   reports when an application crashes.)

7) You should see the Palm logo appear in the simulator window.  You may get a dialog box 
   asking you to choose a language.  Click OK to continue.  You should see the simulator 
   initialize the built-in Palm applications.  After a few moments, the wxWidgets minimal 
   sample should start.  Have fun!

Since this source package was based on the wxWidgets 2.5.3 CVS source tree dated October 18, 
2004 I don't anticipate any problems with merging the sources into the 2.5.3 source tree.

If you have any questions or comments about the port, please feel free to send an email to 
the wxWidgets developer list or directly to me at wbo@freeshell.org

Enjoy!
William Osborne
wbo@freeshell.org