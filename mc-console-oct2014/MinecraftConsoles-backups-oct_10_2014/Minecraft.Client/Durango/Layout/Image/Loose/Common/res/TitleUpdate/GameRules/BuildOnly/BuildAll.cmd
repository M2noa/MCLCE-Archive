@ECHO OFF

call .\BuildGameRule.cmd Tutorial
call .\BuildGameRule_PS3.cmd Tutorial
call .\BuildGameRule_Windows64.cmd Tutorial
call .\BuildGameRule_Durango.cmd Tutorial
call .\BuildGameRule_Orbis.cmd Tutorial
call .\BuildGameRule_PSVita.cmd Tutorial

pause
