This directory contains files for the Digitalmars compiler, currently 8.34 beta
to resolve externals which are not present in the .lib files released by DigitalMars.

To find the names of the functions

//create a library from the dll e.g.
implib ws.lib c:\WINNT\system32\wsock32.dll

//make a listing
lib -l ws.lib

//read the listing e.g.
type ws.lst | more

Chris Elliott, biol75@york.ac.uk, 07 May 03
