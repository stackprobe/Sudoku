IF NOT EXIST _0007.place GOTO END
mkInput.exe > Input.csv
..\..\..\MkSudoku.exe /NCI .
:END
PAUSE
