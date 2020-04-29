C:\Factory\Tools\RDMD.exe /RC out

COPY /B Sudoku\Release\Sudoku.exe out
COPY /B FileFormat.txt out
COPY /B Readme_man.txt out\Readme.txt
C:\Factory\Tools\zcp.exe /B サンプルデータ out
COPY /B LICENSE out
COPY /B MkSudoku.exe out
COPY /B MkSudoku.txt out

C:\Factory\SubTools\EmbedConfig.exe --factory-dir-disabled out\MkSudoku.exe

C:\Factory\SubTools\zip.exe /O out Sudoku

IF NOT "%1" == "/-P" PAUSE
