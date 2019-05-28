///
///@file estimator.cpp
///@brief Estimatorクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup estimator
///@{
///

#include "estimator.h"

namespace odens {

///
///@brief 保持している値をすべてクリア
///@return なし
///
void Estimator::clear()
{
  ball.vanish();
  ballTime = 0;
  for (int i=0; i<2; i++) {
    for (int j=1; j<=MAX_ROBOT_NUM; j++) {
      robot[i][j].vanish();
      robotTime[i][j] = 0;
    }
  }
}

///
///@brief 位置を推定し保持している値を更新
///@param[out] sinfo2 推定結果
///@param[in] sinfo 現在の位置情報
///@param[in] ctime 現在の時刻
///@return なし
///
///- ctimeではなく， sinfo.timeを使う方がいいかもしれない．
///
void Estimator::update(srInfo &sinfo2, const srInfo &sinfo, double ctime)
{
  //ボールの推定
  if (sinfo.ball.isInvisible()) {
    //見えていなければ
    if (ball.isInvisible() || ctime-ballTime > 10.0) { //TODO 要検討
      //過去データがないか，古ければ
      sinfo2.ball.vanish();
    } else {
      //過去データがあり，古くなければ
      sinfo2.ball = ball;
    }
  } else {
    //見えていれば，
    sinfo2.ball = sinfo.ball;
    ball = sinfo.ball;
    ballTime = ctime;
  }

  //各ロボットの推定
  for (int i=BLUE; i<=YELLOW; i++) {
    for (int j=1; j<=MAX_ROBOT_NUM; j++) {
      if (sinfo.robot[i][j].isInvisible()) {
        //見えていなければ
        if (robot[i][j].isInvisible() || ctime-robotTime[i][j] > 1.0) { //TODO 1.0は要検討
          //過去データがないか，古ければ
          sinfo2.robot[i][j].vanish();
          sinfo2.id[i][j] = false;
        } else {
          //過去データがあり，古くなければ
          sinfo2.robot[i][j] = robot[i][j];
          sinfo2.id[i][j] = id[i][j];
        }
      } else {
        //見えていれば，
        if (!robot[i][j].isInvisible() && ctime-robotTime[i][j] <= 1.0 //TODO 1.0は要検討
          && id[i][j] && robot[i][j].distance(sinfo.robot[i][j]) > 240 ) { //TODO 240は要検討
          //過去データがあり，古くなく，idが真であり，現在データと離れていれば，
          sinfo2.robot[i][j] = robot[i][j];
          sinfo2.id[i][j] = id[i][j];
        } else {
          robot[i][j] = sinfo2.robot[i][j] = sinfo.robot[i][j];
          id[i][j] = sinfo2.id[i][j] = sinfo.id[i][j];
          robotTime[i][j] = ctime;
        }
      }
    }
  }
  sinfo2.time = sinfo.time;
}

///
///@brief 位置を推定し保持している値を更新（ボール速度推定版）
///@param[out] sinfo2 推定結果
///@param[out] ballVel ボールの推定速度 [mm/s]
///@param[in] sinfo 現在の位置情報
///@param[in] ctime 現在の時刻
///@return なし
///
///- ctimeではなく， sinfo.timeを使う方がいいかもしれない．
///
void Estimator::update(srInfo &sinfo2, Timed2D &ballVel, const srInfo &sinfo, double ctime)
{
  static int errorCount = 0;
  Timed2D ball(sinfo.ball.x, sinfo.ball.y, ctime);
  //ボールの推定
  if (!ball.isInvisible()) {
    //見えていれば
    ballDeque.push_back(ball);
  }
  //古いデータを取り除く
  while (ballDeque.size() > 0) {
    if (ctime - ballDeque.front().time <= 1.0) break; //TODO 1[s]は要検討
    ballDeque.pop_front();
  }
  Timed2D ball2;
  if (ballDeque.empty()) { //保持データがない場合
    ball2.vanish();
    ballVel = Timed2D(0,0,ctime);
  } else if (ballDeque.size() < 3) { //保持データ最小値 要検討
    ball2 = ballDeque.back();
    ballVel = Timed2D(0,0,ctime);
  } else {
    //最小二乗法 x = vx*(t-tc)+x0, y = vy*(t-tc)+y0
    double stx =0, sty = 0, st = 0, sx = 0, sy =0, st2 = 0;
    size_t n = ballDeque.size();
    for (size_t i=0; i<n; i++) {
      Timed2D &b = ballDeque[i];
      double t = b.time-ctime; //現時刻を基準とする
      stx += t*b.x;
      sty += t*b.y;
      st += t;
      sx += b.x;
      sy += b.y;
      st2 += t*t;
    }
    double det = n*st2-st*st;
    if (det==0) {
      ball2 = ballDeque.back();
      ballVel = Timed2D(0,0,ctime);
    } else {
      ball2.x = (st2*sx-stx*st)/det;
      ball2.y = (st2*sy-sty*st)/det;
      ballVel.x = (n*stx-st*sx)/det;
      ballVel.y = (n*sty-st*sy)/det;
      if (ballVel.abs()<10) {
        ball2.x = sx/n;
        ball2.y = sy/n;
        ballVel = Timed2D(0,0,ctime);
      }
      //推定値との隔たりが連続して大きい回数を数える
      if (ball2.distance(ball) > 100 && !ball.isInvisible()) { //TODO 距離200[mm]は要検討
        errorCount++;
      } else {
        errorCount = 0;
      }
    }
  }
  if (errorCount > 1) { //TODO エラー回数の上限は要検討
    ballDeque.clear();
    errorCount = 0;
  }
  sinfo2.ball = Orthogonal(ball2.x,ball2.y,0);
//  cout << ballDeque.size() << " " << errorCount << " " << ball2.distance(ball) << endl;
  //各ロボットの推定
  for (int i=BLUE; i<=YELLOW; i++) {
    for (int j=1; j<=MAX_ROBOT_NUM; j++) {
      if (sinfo.robot[i][j].isInvisible()) {
        //見えていなければ
        if (robot[i][j].isInvisible() || ctime-robotTime[i][j] > 1.0) { //TODO 1.0は要検討
          //過去データがないか，古ければ
          sinfo2.robot[i][j].vanish();
          sinfo2.id[i][j] = false;
        } else {
          //過去データがあり，古くなければ
          sinfo2.robot[i][j] = robot[i][j];
          sinfo2.id[i][j] = id[i][j];
        }
      } else {
        //見えていれば，
        if (!robot[i][j].isInvisible() && ctime-robotTime[i][j] <= 1.0 //TODO 1.0は要検討
          && id[i][j] && robot[i][j].distance(sinfo.robot[i][j]) > 240 ) { //TODO 240は要検討
          //過去データがあり，古くなく，idが真であり，現在データと離れていれば，
          sinfo2.robot[i][j] = robot[i][j];
          sinfo2.id[i][j] = id[i][j];
        } else {
          robot[i][j] = sinfo2.robot[i][j] = sinfo.robot[i][j];
          id[i][j] = sinfo2.id[i][j] = sinfo.id[i][j];
          robotTime[i][j] = ctime;
        }
      }
    }
  }
  sinfo2.time = sinfo.time;
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
