@echo off
Rem Replace the DLL version number in the VC++ project files

input Enter the old version name (e.g. msw232): %%oldName
input Enter the new version name (e.g. msw233): %%newName

call %wxwin\distrib\msw\filerepl.bat %oldName %newName %wxwin\distrib\msw\vc.rsp
echo Done.

