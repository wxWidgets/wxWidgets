@echo off
Rem Replace the DLL version number in the VC++ project files.
Rem This has to be called from the root directory.

input Enter the old version name (e.g. wx22_0): %%oldName
input Enter the new version name (e.g. wx22_1): %%newName

call %wxwin\distrib\msw\filerepl.bat %oldName %newName %wxwin\distrib\msw\vc.rsp
echo Done.

