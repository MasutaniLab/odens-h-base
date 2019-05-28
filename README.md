# ODENSチームのRoboCup SSL Humanoid用基本ライブラリ

大阪電気通信大学 総合情報学部 情報学科  
升谷 保博  
2019年5月28日  

## 概要

- 4カメラ用のSSL-Visionから1カメラ分のデータをネットワーク経由で受け取り，
SSL Humanoidのフィールド座標系に変換している．

- RoboCup SSL用のレフェリーボックスからネットワーク経由でデータを受け取る．

- ロボット本体が実行すべきモーション（事前に登録された動作）の番号をシリアルポート経由で
送出する．

- ロボット本堤に依存する処理は，基底クラスを用意しており，それを継承したクラスを作る
ことにより複数のロボットに対応できる．見本として，RIC30，KHR-3HV，ROBOTIS-MINI,
KXR-L2用のクラスを用意している．

- 設定ファイルとコマンドラインオプションで各種の設定が可能である．

## インストール

### はじめに

- Visual Studio 2015において64bit（x64）で開発することを前提にしている．

- Boostなどのライブラリを利用しているので，ディスクの容量がそれなりに必要．

- どこかに作業のディレクトリを作る．それを`H`と書くことにする．
  後述のboostとprotobufのファイル以外は全てHの下に置く．

### boostのインストール

- https://sourceforge.net/projects/boost/files/boost-binaries/1.69.0/
  から`boost_1_69_0-msvc-14.0-64.exe`をダウンロード．
  似たファイルが多いので間違えないように（14.1ではなく14.0を選ぶこと）．
- `boost_1_69_0-msvc-14.0-64.exe` を実行してインストール．
  インストール場所をデフォルトのままでなく，必ず`C:\boost\boost_1_69_0`にすること．


### Protocol Buffers のインストール

- https://github.com/google/protobuf/releases/tag/v3.7.0 から
  `protobuf-all-3.7.0.zip` をダウンロード．
- `protobuf-all-3.7.0.zip`を適当な場所に展開．
  ディレクトリ`protobuf-3.7.0`ができる．
- CMake (cmake-gui)を実行
  - Where is the source code に `～/protobuf-3.7.0/cmake`
  - Where to build the binaries に `～/protobuf-3.7.0/cmake/build`
  - Configure をクリックし，で `Visual Studio 14 2015 Win64` を選択．
  - Configure 処理後に表示されるオプション一覧において，
    - `protobuf_BUILD_TESTS` のチェックを外す．
    - `protobuf_MSVC_STATIC_RUNTIME` のチェックを外す．
  - 再度 Configure をクリック．
  - Generate をクリック．
- `～\protobuf-3.7.0\cmake\build\protobuf.sln`をVisual Studio 2015で開く．
- ソリューション構成を`Release x64`にする．
- メニューのビルド→ソリューションのビルド．
-  `C:\Program Files\protobuf`が既にある場合は，削除するか，別名に変更する．
- `C:\Program Files` の下に`protobuf`フォルダを作る．拒否されるが続行する．
- `protobuf` フォルダを右クリックでプロパティ，セキュリティタブで編集をクリック，
  Usersを選び，アクセス許可で，フルコントロールの許可をチェックしてOK．
- Visual Studio 2015において`INSTALL`プロジェクトをビルド．
- `C:\Program Files\protobuf` の下にファイルがインストールされることを確認する．

### Doxygenのインストール

- https://sourceforge.net/projects/doxygen/files/rel-1.8.15/ から
  `doxygen-1.8.15-setup.exe` をダウンロード．
  （http://www.doxygen.nl/download.html が本家だが遅いので）
- `doxygen-1.8.15-setup.exe`を実行しインストール．


### Windows版 EGGXのインストール

- https://github.com/MasutaniLab/wineggx.git から
  `H\wineggx`にGitクローンする．
- CMake (cmake-gui)を実行
  - Where is the source code に `H/wineggx`
  - Where to build the binaries に `H/wineggx/build`
  - Configure をクリックし，で `Visual Studio 14 2015 Win64` を選択．
  - Generate をクリック．
- `H\wineggx\build\wineggx.sln`をVisual Studio 2015で開く．
  - ソリューション構成を`Release x64`にする．
  - メニューのビルド→ソリューションのビルド．
  - プロジェクト`INSTALL`をビルド．
  - `H\wineggx\lib\wineggx.lib`がインストールされる．

### odens-h-baseのクローン

- https://github.com/MasutaniLab/odens-h-base.git から
  `H\odens-h-base`にGitクローンする．
- ソリューション構成を`Release x64`にする．
- メニューのビルド→ソリューションのビルド．
- - `H\odens-h-base\Release\`にある`odens-sample.conf`を`odens.conf`という名前でコピー（別名保存）する．

## Doxygenによるドキュメント生成

- Doxywizardを起動する．
- メニューのFile→Open...で，`H\odens-h-base\Doxyfile`を指定する．
- Runタブの`Run doxygen`をクリックする．
- ウエブブラウザで，`H\odens-h-base\doc\hrml\index.html` を開く．

## 各プロジェクトの概要

### config-test

- Configクラスのテストプログラム
- コマンドラインや設定ファイルの仕様を変更した場合は，これでテストする．

### draw-test

- Drawクラスのテストプログラム．
- Configクラスも使っている．

### game-test

- Gameクラスのテストプログラム．
- ゲームのモードの判断を変更した場合はこれでテストする．
- ConfigクラスとRefereeクラスも使っている．
- ビジョンも使うべき？

### logger-test

- Loggerクラスのテストプログラム．

### odens-h-base

- 基本機能のライブラリ．

### odens-h-test

- odens-h-baseの全ての機能のテストプログラム

### protoc

- protocコマンドで，SSL-Vision用とレフェリーボックス用のデータ定義ファ
  イル`*.proto`から，`*.pb.cc`と`*.pb.h`を生成する．
- 単独で利用することはない．

### referee-test

- Refreeクラスのテストプログラム．
- レフェリーボックスからの情報受信を確認する場合には，これを使う．
- Configクラスも使っている．

### robot-test

- Robotクラスのテストプログラム．
- ロボットへのコマンド送信を確認する場合には，これを使う．
- Configクラスも使っている．

### sr-test

- Orthogonal構造体の例プログラム．
- Vision, VisiionHumanoid, Estimator, Configクラスも使っている．

### vision-test

- Vision, VisiionHumanoid, Estimatorクラスのテストプログラム．
- SSL-Visionサーバからの情報受信を確認する場合には，これを使う．
- SSL Humanoid座標系への変換やマーカ番号からロボット番号への変換を確
  認する場合には，これを使う．
- Esimatorクラスの動作確認にもこれを使う．
- Configクラスも使っている．

## 実行方法

- どのプロジェクトも，Visual Studioの中から実行するのではなく，コマンド
  プロンプト内でexeファイルの名前を入力して実行することを想定している．

- `Windows+r`（ファイル名を指定して実行）で`cmd`と入力．コマンドプロン
  プトのウィンドウが現れる．

- `cd`コマンドで，作業ディレクトリを変更する．
  ~~~
    cd H\odens-h-base\Release
  ~~~

  と入力．パス名はエクスプローラからドラッグ＆ドロップが可能．

- 全機能のテストプログラムであれば，
  ~~~
    odens-h-test
  ~~~

  と入力することで実行される．

- Condfigクラスを使ったプログラムでは，作業ディレクトリにある
  `odens.conf`という設定ファイルを実行の最初に読む．

- 一部の設定は，コマンドラインにオプションを付けることによっても設定可能．  
  例：
  ~~~
    odens-h-test --blue --num2
    odens-h-test -b -2
  ~~~
  設定ファイルよりもコマンドラインの設定が優先される．

- コマンドラインのオプションで，別の名前の設定ファイルを指定することも
  できる．  
  例：
  ~~~
    odens-h-test --conf aaa.conf
    odens-h-test -c aaa.conf
  ~~~

## 処理の流れ

- Visionクラスは，別スレッドでSSL-Visionサーバからのデータを待ち受けて
  おり，受信する度に共有領域にデータを書き込み，合図を送る．

- VisionHumanoidクラスは，Visionクラスから同期的にデータを読み出し，
  SSL Humanoidの座標系へ変換し，マーカ番号をロボット番号に変換する．

- Estimatorクラスは，位置情報を受け取り，一時的に欠落したデータを補った
  推定位置情報を生成する．

- Refereeクラスは，別スレッドでレフェリーボックスからのデータを待ち受け
  ており，受信する度に共有領域にデータを書き込む．合図は送らない．

- Gameクラスは，レフェリーの信号とボールの状態からチームとしてのゲーム
  のモードを判断する．

- Taskクラスは，基本的な行動を実行する機能の基底クラスで，実ロボットに対応した
  クラスで継承する．試合用のプログラムでは，上位の処理から呼び出され，
  ロボットへ送信するコマンドを決定する．

- Robotクラスは，別スレッドでシリアルポート経由でロボットへ繰り返しコマ
  ンドを送信する基底クラスで，実ロボットに対応したクラスで継承する．
  送信するコマンドは，メインのスレッドから共有領域に書き込まれる．


## 各クラス・構造体・列挙型の概要

Doxygenが生成したドキュメントを参照のこと


## プロジェクトのプロパティ

各プロジェクトのプロパティは，個別に設定するのではなく，共通のプロパティ
シート `H\odens-h-base\odens-h-base.props` を読むように設定している．
したがって，このファイルの設定を変更すれば，全てのプロジェクトに反映さ
れる．


## TODO（プログラムとしての）

- SSL-VisionやRefereeBoxとの通信が切れた状態で，プログラムを正常終了で
  きない（Ctrl+cで終わることはできる）問題の解決．原因は把握している．
  boost asioでは同期通信のタイムアウトに対応していないため．非同期の機
  能で書き直すのが望ましい．

- Vision, Referee, Robotの別スレッドで例外が発生した場合，exit()を呼び
  出してプログラムを強制的に終了させているが，これは危険ではないか？

- CMake化．

## 履歴

- 2019年5月28日
  - 2019年版
  - 利用しているboost, protobuf, wineggtx, doxygenのバージョンを変更．
  - Drawクラスの導入
  - その他 バグフィックス等
   
- 2018年2月18日
  - 初公開

以上．
