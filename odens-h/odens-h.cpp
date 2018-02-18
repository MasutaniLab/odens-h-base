///
///@file odens-h.cpp
///@brief 試合用プログラム
///@par Copyright
/// Copyright (C) 2016 Team ODENS
///@par 履歴
///- 2016/03/11 升谷 保博 odens-h2
///

///
///@mainpage
///SSL Humanoidのシステムの行動決定のプログラム
///- SSL-Visionサーバからマルチキャスト通信で位置情報を受け取る
///- レフェリーボックスからマルチキャスト通信でレフェリー情報を受け取る
///- 位置情報とレフェリー情報に基づきコマンドを決定する
///- シリアルポートを介してロボットへコマンドを送る
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
#include "robot.h"
#include "estimator.h"
#include "game.h"
#include "role.h"
#include "logger.h"
using namespace std;


///odens-hメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }
  Config::print();

  getTimeInitialize();
  drawInitialize(Config::MyColor, Config::MyNumber); 
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

  //ロボットとの通信の設定
  Robot robot;
  if (robot.start(Config::RobotPortName, 50)) {
    cerr << "終了" << endl;
    return 1;
  }

  Estimator estimator;
  Game game(Config::MyColor);
  Role role(Config::MyColor, Config::MyNumber);

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
  while (loop) {
    RobotCommand com;
    srInfo sinfo;
    VisionInfo vinfo;
    int r = vh.get(sinfo, vinfo);
    if (r < 0) {
      cout << "ビジョンタイムアウト" << endl;
    } else if ( r > 1 ) {
      cout << "ビジョンフレーム番号差: " << r << endl;
    }

    double currentTime = getTime();

    srInfo sinfo2; //推定値
	Timed2D ballVel;

	static Orthogonal ball1=sinfo.ball;

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
      rinfo.command = Ref::NORMAL_START;
    }
    drawString(-FIELD_LENGTH2,FIELD_WIDTH2+100,16,s.c_str());
  
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
          drawReverseField();
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
      com = RobotCommandNone;
      robot.setCommand(com);
      drawString(0,-FIELD_WIDTH2-300,16,robot.getCommandString(com).c_str());
      draw(sinfo);
      continue; //ループの最初へ
    }

    //チームとしてのゲームの状態の判断
    GameMode mode = game.decideMode(rinfo, sinfo2.ball, currentTime);

    //行動決定
    role.run(com, ballVel,sinfo2,ball1, mode, currentTime);

    //コマンド設定
    robot.setCommand(com);

    //フィールド描画
    drawString(0,FIELD_WIDTH2+100,16,mode.getString().c_str());
    drawString(-FIELD_LENGTH2,-FIELD_WIDTH2-300,16,role.getStateString().c_str());
    drawString(0,-FIELD_WIDTH2-300,16,robot.getCommandString(com).c_str());
    draw(sinfo, sinfo2);
	if(!(sinfo.ball.isInvisible()))
		ball1 = sinfo.ball;
  }
  drawTerminate();
  return 0;
}

