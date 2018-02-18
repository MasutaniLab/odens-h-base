///
///@file sr-test.cpp
///@brief Orthogonalテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
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
#include "estimator.h"

using namespace std;
using namespace odens;

///sr-testメイン関数
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

  Estimator estimator;

  cout << "メインループ開始" << endl;
  while (true) {
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
    estimator.update(sinfo2, sinfo, currentTime);

    Orthogonal &ball = sinfo2.ball;
    //点の存在領域をチェックする関数のテスト
    cout << "ball.isInField(): " << ball.isInField() 
      << ", ball.isInOurGoalArea(): " << ball.isInOurGoalArea()
      << ", ball.isInTheirGoalArea(): " << ball.isInTheirGoalArea() << endl;

    //ボールとゴールを結ぶ位置関係から決まる点の算出の例
    Orthogonal goal(FIELD_LENGTH2,0,0);
    drawLine(ball.x,ball.y,goal.x,goal.y,255,255,255);
    //原点ボール，x軸がゴールの方向の座標系の設定
    Orthogonal ballFrame = ball;
    ballFrame.theta = goal.angle(ball);
    //ボールの前方300mm, 左400mmの点（赤十字）
    Orthogonal pos1Local(300,400,0); //ボール座標系での位置
    Orthogonal pos1 = pos1Local.inverseTransform(ballFrame); //フィールド座標系に変換
    drawCross(pos1.x,pos1.y,255,0,0);
    //ボールの真後ろ200mm（青十字）
    Orthogonal pos2 = Orthogonal(-200,0,0).inverseTransform(ballFrame); //こういう書き方も可能
    drawCross(pos2.x,pos2.y,0,0,255);

    //フィールド描画
    draw(sinfo, sinfo2);
  }
  drawTerminate();
  return 0;
}

