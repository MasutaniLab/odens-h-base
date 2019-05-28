///
///@file odens-h-test.cpp
///@brief odens-hの基本機能のテストプログラム（公開用）
///@par Copyright
/// Copyright (C) 2016-2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/03/03 升谷 保博
///- 2019/02/03 升谷 保博 Drawのクラス化に対応
///- 2018/02/18 升谷 保博 KXR-L2を追加
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///

#include <iostream>
#include <string>
#include <cmath>
#include "sr.h"
#include "draw.h"
#include "util.h"
#include "config.h"
#include "visionhumanoid.h"
#include "referee.h"
#include "estimator.h"
#include "ric30.h"
#include "robotismini.h"
#include "khr3.h"
#include "kxrl2.h"
#include "game.h"
#include "logger.h"

using namespace std;
using namespace odens;

///odens-h-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }
  Config::print();

  getTimeInitialize();
  const double drawInterval = 1.0 / 30.0;
  draw.initialize(Config::MyColor, Config::MyNumber, drawInterval);
  inkeyInitialize();

  //SSL-Visionの設定
  VisionHumanoid vh;
  if (vh.start(Config::VisionAddress, Config::VisionPortNumber)) {
    cerr << "終了" << endl;
    return 1;
  }
  vh.setQuardrant(Config::Quadrant);
  vh.setAttackRight(Config::AttackRight);
  if (Config::MyColor == BLUE) {
    vh.setMarkerTable(Config::OurMarkerTable, Config::TheirMarkerTable);
  } else {
    vh.setMarkerTable(Config::TheirMarkerTable, Config::OurMarkerTable);
  }

  //レフェリーボックスの設定
  Referee ref;
  if (Config::Referee) {
    if (ref.start(Config::RefereeAddress, Config::RefereePortNumber)) {
     cerr << "終了" << endl;
      return 1;
    }
  }

  Estimator estimator;

  Game game(Config::MyColor);

  Task *ptask;
  cout << "RobotType: " << Config::RobotType << endl;
  if (Config::RobotType == "RIC30") { 
    ptask = new TaskRIC30(Config::MyColor, Config::MyNumber);
  } else if (Config::RobotType == "ROBOTISMINI") { 
    ptask = new TaskROBOTISMINI(Config::MyColor, Config::MyNumber);
  } else if (Config::RobotType == "KHR3") { 
    ptask = new TaskKHR3(Config::MyColor, Config::MyNumber);
  } else if (Config::RobotType == "KXRL2") {
    ptask = new TaskKXRL2(Config::MyColor, Config::MyNumber);
  } else {
    cerr << "RobotType 未登録" << endl;
    return 1;
  }

  //ロボットとの通信の設定
  if (ptask->startRobot(Config::RobotPortName, 50)) {
    cerr << "終了" << endl;
    return 1;
  }

  //ロガー
  Logger logger;
  if (Config::Logger) {
    logger.open(Config::MyColor, Config::MyNumber);
  }

  cout << "メインループ開始" << endl;
  if (Config::Pause) {
    cout << "一時停止中．r: 開始" << endl;
  }
  bool loop = true;
  double prevTime = getTime();
  while (loop) {
    srInfo sinfo;
    VisionInfo vinfo;
    double btime = getTime();
    int r = vh.get(sinfo, vinfo);
    if (r < 0) {
      cout << "ビジョンタイムアウト" << endl;
    } else if ( r > 1 ) {
      cout << "ビジョンフレーム番号差: " << r << endl;
    }

    double currentTime = getTime();
    //cout << "dt: " << currentTime - prevTime 
    //  << ", vision: " << currentTime - btime << endl;
    prevTime = currentTime;

    srInfo sinfo2; //推定値
    Timed2D ballVel;
    estimator.update(sinfo2,ballVel, sinfo, currentTime);

    //レフェリーの信号を調べる
    RefereeInfo rinfo;
    string s;
    if (Config::Referee) {
      switch (ref.get(rinfo)) {
      case 1:
        s = "未受信";
        break;
      case 2:
        s = "途絶";
        break;
      default:
        s = rinfo.commandString();
      }
    } else {
      s = "無効";
      rinfo.command = ref::NORMAL_START;
    }
    draw.string(-FIELD_LENGTH2,FIELD_WIDTH2+100,16,s.c_str());
  
    //キーの状態を調べる
    int c = inkey();
    if ( c != -1 ) {
      //何かキーが押された場合
      if (Config::Pause) {
        switch (c) {
        case 'q':
          loop = false;
          break;
        case 'r':
          cout << "再開．" << endl;
          Config::Pause = false;
          break;
        case 'R':
          draw.reverseField();
          break;
        case 'S':
          Config::AttackRight = !Config::AttackRight;
          vh.setAttackRight(Config::AttackRight);
          break;
        default:
          cerr << "未登録のキー: " << static_cast<char>(c) << endl;
        }
      } else {
        //動作中の時
        cout << "一時停止．q: 終了，r: 再開" << endl;
        Config::Pause = true;
      }
    }

    if (Config::Pause) {
      //一時停止の場合
      ptask->none();
      draw.string(0,-FIELD_WIDTH2-300,16,ptask->getCommandString().c_str());
      draw.set(sinfo);
      continue; //ループの最初へ
    }

    //チームとしてのゲームの状態の判断
    GameMode mode = game.decideMode(rinfo, sinfo2.ball, currentTime);

    if (rinfo.command == ref::HALT || rinfo.command == ref::STOP) {
      ptask->none();
    } else if (ptask->isLying(sinfo2)) {
      ptask->standUp(sinfo2, currentTime);
    } else if (sinfo2.ball.isInvisible()) {
      ptask->none();
    } else {
      ptask->move(sinfo2, sinfo2.ball);
    }

    //フィールド描画
    draw.string(0,FIELD_WIDTH2+100,16,mode.getString().c_str());
    draw.string(0,-FIELD_WIDTH2-300,16,ptask->getCommandString().c_str());
    draw.set(sinfo, sinfo2);

    //ログ出力
    logger.write(currentTime, sinfo, ballVel, rinfo, mode, ptask->getCommand());
  }
  draw.terminate();
  return 0;
}

