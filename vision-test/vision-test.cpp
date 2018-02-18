///
///@file vision-test.cpp
///@brief Vision, VisionHumanoid, Estimator クラスのテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///

#include <iostream>
#include "sr.h"
#include "draw.h"
#include "util.h"
#include "visionhumanoid.h"
#include "estimator.h"
#include "config.h"

using namespace std;
using namespace odens;

void printHelp();

///vision-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }

  bool drawVisionInfo = true;

  getTimeInitialize();
  drawInitialize(Config::MyColor, Config::MyNumber); 
  inkeyInitialize();

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

  Estimator estimator;

  printHelp();

  cout << "メインループ開始" << endl;
  while (true) {
    srInfo sinfo;
    VisionInfo vinfo;
    int r = vh.get(sinfo, vinfo);
    if (r < 0) {
      cout << "タイムアウト" << endl;
    } else if ( r > 1 ) {
      cout << "フレーム番号差: " << r << endl;
    }

    double currentTime = getTime();

    srInfo sinfo2; //推定値
    Timed2D ballVel; 
    estimator.update(sinfo2, ballVel, sinfo, currentTime);

    if (r == 0) { //ビジョンの情報が得られている場合
      if (drawVisionInfo) {
        draw(vinfo);
      } else {
        double dt = 1.0;
        drawLine(sinfo2.ball.x,sinfo2.ball.y,
          sinfo2.ball.x+ballVel.x*dt,sinfo2.ball.y+ballVel.y*dt,
          255,0,0);
        draw(sinfo, sinfo2);
      }
    }
    //キー入力
    int c = inkey();
    if ( c != -1 ) {
      switch (c) {
      case 'r':
        drawReverseField();
        cout << "表示を左右反転（座標は変更なし）" << endl;
        break;
      case 'i':
        drawVisionInfo = !drawVisionInfo;
        if (drawVisionInfo) {
          cout << "マーカ表示（VisionInfo）" << endl;
        } else {
          cout << "ロボット表示（srInfo）" << endl;
        }
        break;
      case 'q':
        Config::Quadrant=(Config::Quadrant+1)%4;
        vh.setQuardrant(Config::Quadrant);
        cout << "第" << Config::Quadrant+1 << "象限" << endl;
        break;
      case 's':
        Config::AttackRight = !Config::AttackRight;
        vh.setAttackRight(Config::AttackRight);
        if (Config::AttackRight) {
          cout << "右側に攻める（SSL-Visionの右側が正）" << endl;
        } else {
          cout << "左側に攻める（SSL-Visionの左側が正）" << endl;
        }
        break;
      default:
        cerr << "未登録のキー: " << char(c) << endl;
        printHelp();
      }
    }
  }
  return 0;
}

///
///@brief キー割り当てを表示する
///@return なし
///
void printHelp()
{
  cout
    << "【使い方】" << endl
    << "r: 表示を左右反転（drawReverseField()）" << endl
    << "i: マーカ表示とロボット表示の切り替え" << endl
    << "q: SSL-Visionの利用する象限の切り替え" << endl
    << "s: 攻める方向の切り替え" << endl
    << "Ctrl+c: 終了" << endl;
}
