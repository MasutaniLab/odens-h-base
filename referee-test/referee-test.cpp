///
///@file referee-test.cpp
///@brief Refereeクラスのテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///
#include <iostream>
#include "util.h"
#include "referee.h"
#include "config.h"

using namespace std;
using namespace odens;

///referee-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }

  Referee ref;
  if (ref.start(Config::RefereeAddress, Config::RefereePortNumber)) {
    cerr << "終了" << endl;
    return 1;
  }

  cout << "メインループ開始" << endl;
  while (true) {
    RefereeInfo rinfo;
    switch (ref.get(rinfo)) {
    case 1:
      cout << "未受信" << endl;
      break;
    case 2:
      cout << "途絶" << endl;
      break;
    default:
      cout << "packetTimestamp=" << rinfo.packetTimestamp 
        << ", stage=" << rinfo.stageString() << "(" << rinfo.stage << ")" 
        << ", stageTimeLeft=" << rinfo.stageTimeLeft 
        << ", command=" << rinfo.commandString() << "(" << rinfo.command << ")"
        << ", commendCounter=" << rinfo.commandCounter
        << ", score[BLUE]=" << rinfo.score[BLUE] 
        << ", score[YELLOW]=" << rinfo.score[YELLOW]
        << endl;
    }
    msleep(1000);
  }
  return 0;
}
