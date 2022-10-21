                    wxWin Bombs
                 by Pasquale Foggia

1. The aim of the program
wxWin Bombs is the wxWin implementation of the minesweeper game you find
under MSWindows 3.1+. Later the rules of the game will be explained for
the lucky ones of you that have never used Windows.

2. Installation
If you are reading this file, I suppose you have successfully unpacked the
files in a directory of your hard disk :-). You should already have
installed wxWin on your system.
Now you have to modify makefile.bcc
(if a Windows user) or makefile.unx (if you use a real OS) setting the 
proper values for the directories. Finally, you have to run:
  make -f makefile.gcc
for Windows or
  nmake -f makefile.vc
if you use a MicroSoft compiler, or:
  make -f makefile.unx
for Unix.

If you are lucky, you will find the bombs executable, ready to be run.

3. Test
Bombs has been tested under the following platforms:
  PC + MSWindos 3.1 + wxWin 1.60 + Borland C 3.1
  Sun SPARCstation 20 + SunOS + xview + wxWin 1.63 + gcc 2.3.3
and all seems to work fine.

4. The author
This program has been developed by Pasquale Foggia, a PhD student 
in Computer Engineering at the "Federico II" University of Naples, Italy.
You can contacting him using the following address:
  foggia@amalfi.dis.unina.it

5. Disclaimer
This program is freeware. You can do everything you want with it, including
copying and modifying, without the need of a permission from the author.
On the other hand, this program is provided AS IS, with NO KIND OF WARRANTY.
The author will be in NO CASE responsible for damages directly or indirectly 
caused by this program. Use it AT YOUR OWN RISK, or don't use it at all. 

6. The rules of the game
Your aim is to discover all the bombs in a mined field. If you click with
the left mouse button on a cell containing a bomb, your game ends. 
Otherwise, the number of bombs in the 8 neighbour cells will be displayed. 
When you have clicked all the cells without a bomb, you win.
You can also use the right button (or left button+shift) to mark a cell
you think hides a bomb, in order to not click it accidentally.

7. Concluding remarks
I hope someone of you will enjoy this program. However, I enjoyed writing
it (thanks to Julian Smart and all the other wxWin developers).   
In the near future I plan to implement under wxWin the great 'empire'
(is there someone that still remember it?), IMHO one of the most addictive
strategy games. If someone is interested, please contact me by e-mail.
I beg you pardon for my approximative english.

						Pasquale Foggia
						foggia@amalfi.dis.unina.it


------
A note from Julian Smart: Many thanks to Pasquale for the contribution.
I've taken the liberty of making a few changes.

1) I've made the status line have a single field so that you
can see the 'cells remaining' message properly.

2) I've changed the title from "wxWin Bombs" (which, as a statement,
is an unfortunate reflection of the reality of earlier versions of
wxWindows :-)) to wxBombs.

3) Added SetClientData to resize the window on Restart; eliminated
scrollbars; made the frame unresizable.

4) Added makefile.dos for VC++ 1.x, makefile.wat for Watcom C++.
