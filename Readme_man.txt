===============================================================================

　数独解析プログラム（コマンドライン版）

===============================================================================


■ソフトの概要

　数独の問題を解くコンソールアプリケーションです。
　入力・出力データ共にＣＳＶ形式のファイルを使用します。
　対応している問題は以下のとおりです。

　1. 一般的な数独の問題（９×９）
　2. 巨大なまたは小さな数独の問題（４×４、１６×１６、２５×２５など）
　3. 対角線上にも全ての数字が揃う問題
　4. ブロックが正方形ではない問題（長方形、変形、飛び地）
　5. 偶数・奇数の条件がある問題
　6. 合体ナンプレ（いくつかの問題が重なっている問題）
　7. カラーナンプレ
　8. 不等号ナンプレ
　9. ジョイントナンプレ
　10. これらを組み合わせた問題

　※ブロックとは、縦と横の他に全ての数字が揃わなければならない（たいてい矩形の）領域を指します。
　　一般的な数独の問題では、９個ある３×３の領域のことです。

　問題を作成するツールも同梱しています。


■動作環境

　Windows 10 Pro または Windows 10 Home


■インストール方法

　アーカイブの中身をローカルディスク上の任意の場所にコピーして下さい。


■アンインストール方法

　レジストリなどは一切使っていません。
　ファイルを削除するだけでアンインストールできます。


■実行方法

　Sudoku.exe [/D data-directory] [/O output-file] [/E error-file] [/L loadeddata-file] [/OL]

　　data-directory

　　　データディレクトリのパスを記述します。
　　　相対パスは実行時のカレントディレクトリからの相対パスになります。
　　　省略した場合は実行時のカレントディレクトリになります。

　　　このディレクトリには以下の入力ファイルを置いてください。

　　　　Frame.csv       フレーム指定ファイル
　　　　Square.csv      スクウェア指定ファイル
　　　　Group.csv       グループ指定ファイル
　　　　XInequal.csv    水平方向の不等号指定ファイル
　　　　YInequal.csv    垂直方向の不等号指定ファイル
　　　　XJoint.csv      水平方向のジョイント指定ファイル
　　　　YJoint.csv      垂直方向のジョイント指定ファイル
　　　　Condition.csv   セルの条件指定ファイル
　　　　Relation.csv    セルとセルの関係指定ファイル
　　　　Input.csv       初期値指定ファイル

　　　※入力ファイルは個別に省略できます。全てのファイルを準備する必要はありません。
　　　　例えば９×９の一般的な数独なら、必要なファイルは Frame.csv, Input.csv だけです。

　　　※入力ファイルの記述については「FileFormat.txt」を参照して下さい。

　　　※具体的な入力データについては「サンプルデータ」ディレクトリを参照して下さい。

　　output-file

　　　解析結果ファイル名を指定します。
　　　相対パスは実行時のカレントディレクトリからの相対パスになります。
　　　省略した場合はデータディレクトリ上の Output.csv という名前になります。

　　error-file

　　　エラー情報ファイル名を指定します。
　　　相対パスは実行時のカレントディレクトリからの相対パスになります。
　　　省略した場合はデータディレクトリ上の Error.csv という名前になります。

　　loadeddata-file

　　　ロードデータファイル名を指定します。
　　　相対パスは実行時のカレントディレクトリからの相対パスになります。
　　　省略した場合はデータディレクトリ上の LoadedData.csv という名前になります。

　　/OL オプション

　　　入力データの読み込みが終わった時点（解析開始の直前）でロードデータファイルを出力します。


　　※オプションの順序を入れ替えることはできません。

　　※出力ファイルの内容については「FileFormat.txt」を参照して下さい。

　　※ここに明記していない作業ファイルは出力しませんので、
　　　データディレクトリや実行時のカレントディレクトリに関係ないファイルがあっても問題ありません。


■中断方法

　解析に時間が掛かり過ぎているため中断したいという場合、以下のコマンドを実行して下さい。
　実行中の解析が中断されます。

　　Sudoku.exe /S


　※解析の中断はエラーとして扱います。

　※中断するまで数秒～数分掛かります。
　　その前に解を発見したか別のエラーになった場合、中断の要求は無視されます。

　※ /S オプションで起動したプロセスは「解析プロセスの終了を待たずに」終了します。

　※解析プロセスの起動と /S オプションでの起動がほぼ同時であった場合、中断の要求が無視されることがあります。


　解析プロセスを確実に停止させるためには、以下のように実行することをおすすめします。

　　DO UNTIL 解析プロセス終了
　　　Sudoku.exe /S を実行する。-- ブロッキング・モードで実行すること。
　　　１～２秒待つ。-- CPU使用率が上がらないようにするため。待ち時間は適当
　　LOOP


■問題作成ツール

　MkSudoku.txt を参照して下さい。


■取り扱い種別

　フリーソフト


■作者への連絡先

　stackprobes@gmail.com

　不具合や要望など気軽にご連絡下さい。

