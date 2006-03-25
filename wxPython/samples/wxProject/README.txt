This sample comes from an IBM developerWorks article at
http://www-106.ibm.com/developerworks/library/l-wxpy/index.html

Modifications by Franz Steinhaeusler 08.Feb.2006:
  - tried to meet the wxPython Style Guide 
      (http://wiki.wxpython.org/index.cgi/wxPython_20Style_20Guide)
      (also take care to use id's as less as possible)
  - added docstrings 
  - used wx Namespace (recommend use after wxPython 2.5) 
      and Bind instead directly EVT_MENU
  - set indentation to 4 spaces
  - used MsgDlg for better overview
  - added info (for adding or removing files without a root item)
  - completed the menu function: File open.
  - if a file was changed, and you want to quit the app, you are now 
      asked to save the file.
  - there was a traceback with self.tree.GetFirstChild(self.root). fixed.
  - close handler (save file, if you also close with mouse or alt-F4)

Modifications by Chris Barker, 08.Feb.2006:
  - changed first line to #!/usr/bin/env python.
  - Removing a bunch of superfluous IDs of the menu items and 
      event handlers and in splitterwindow.
    
Modifications by Franz Steinhaeusler 08.Feb.2006:
  - added short program description.
  - removed the string module (it is somewhat "deprecated".
  - added some comments
  - changed treecontrol to default style. 
      (I think, because of the plus and minus signs, 
       it is much clearer then, whether the root
       item is empty or not. If there is no sign, the
       root item doesn't have any children).
         


