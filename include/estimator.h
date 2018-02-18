///
///@file estimator.h
///@brief Orthogonal用位置推定
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup estimator Estimator
///@brief Orthogonal用位置推定
///@{
///

#pragma once
#include <deque>
#include "sr.h"

namespace odens {

///
///@brief 時刻付き2次元ベクトル
///
struct Timed2D {
  double x;  ///< x座標 [mm]
  double y;  ///< y座標 [mm]
  double time; ///< 時間 [s]
  Timed2D(){};
  ///
  ///@brief 初期値を与えるコンストラクタ
  ///
  Timed2D(double x_, double y_, double time_)
  {
    x = x_; y = y_; time = time_;
  }
  ///
  ///@brief 見えていないか？
  ///
  bool isInvisible() const
  {
    return x == INVISIBLE;
  }
  ///
  ///@brief 引数との間の距離
  ///
  double distance(const Timed2D &p) const
  {
    return sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y));
  }
  ///
  ///@brief ベクトルの絶対値
  ///
  double abs() const
  {
    return sqrt(x*x+y*y);
  }
  ///
  ///@brief 見えなくする
  ///
  void vanish() {
    x = INVISIBLE; y = INVISIBLE; time = 0;
  }
};

///
///@brief Orthogonal用位置推定クラス
///
class Estimator {
private:
  Orthogonal ball;                      ///<記憶するボール位置
  double ballTime;                      ///<ボール位置を記憶した時刻
  Orthogonal robot[2][MAX_ROBOT_NUM+1]; ///<記憶する各ロボット位置
  bool id[2][MAX_ROBOT_NUM+1];          ///<記憶する各ロボットの番号が得られているか？
  double robotTime[2][MAX_ROBOT_NUM+1]; ///<各ロボット位置を記憶した時刻
  std::deque<Timed2D> ballDeque;             ///<過去のボールデータを保持する両端キュー
public:
  void clear();
  //コンストラクタ
  Estimator()
  {
    clear();
  }
  void update(srInfo &sinfo2, const srInfo &sinfo, double ctime);
  void update(srInfo &sinfo2, Timed2D &ballVel, const srInfo &sinfo, double ctime);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
