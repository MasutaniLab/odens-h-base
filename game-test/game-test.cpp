///
///@file game-test.cpp
///@brief Gameクラスのテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///
#include <iostream>
#include <string>
#include "sr.h"
#include "util.h"
#include "config.h"
#include "referee.h"
#include "game.h"

using namespace std;
using namespace odens;

///game-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }

  getTimeInitialize();

  //レフェリーボックスの設定
  Referee ref;
  if (Config::Referee) {
    if (ref.start(Config::RefereeAddress, Config::RefereePortNumber)) {
     cerr << "終了" << endl;
      return 1;
    }
  }

  Game game(Config::MyColor);

  cout << "メインループ開始" << endl;
  while (true) {
    //レフェリーの信号を調べる
    RefereeInfo rinfo;
    switch (ref.get(rinfo)) {
    case 1:
      cout << "未受信" << endl;
      break;
    case 2:
      cout << "途絶" << endl;
      break;
    default:
      double currentTime = getTime();
      srInfo sinfo;
      GameMode mode = game.decideMode(rinfo, sinfo.ball, currentTime);
      cout << mode.getString() << endl;
    }
    msleep(100);
  }
  return 0;
}

