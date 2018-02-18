///
///@file sr.h
///@brief サッカーロボットの諸定数，基本構造体
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/12/31 升谷 保博 関数を減らす
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2008/02/13
///- 2007/09/28  片岡賢太郎
///- 2007/04/15  升谷 保博
///- 2007/03/25  片岡 賢太郎
///- 2007/02/21  片岡 賢太郎
///- 2006/10/22  片岡 賢太郎
///- 2006/04/18  升谷 保博，片岡 賢太郎
///@addtogroup sr SR
///@brief サッカーロボットの諸定数，基本構造体
///@{
///

#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cmath>

namespace odens {

#define MAX_MARKER_NUM (16)///< マーカの数の最大値
#define MAX_BALL_NUM (10)///<ボールの数の最大値
#define MAX_ROBOT_NUM (3)///< 1チームのロボット台数
#define BLUE      (0)///< 青チーム
#define YELLOW    (1)///< 黄チーム
#define INVISIBLE (99999)   ///< オブジェクトが見えない

#define FIELD_WIDTH         (3005.0)      ///< フィールドの横幅[mm]
#define FIELD_LENGTH        (4505.0)      ///< フィールドの長さ[mm]
#define FIELD_WIDTH2  (FIELD_WIDTH/2.0)   ///< フィールドの横幅の半分[mm]
#define FIELD_LENGTH2 (FIELD_LENGTH/2.0)  ///< フィールドの長さの半分[mm]
#define FIELD_MARGIN         (350.0)      ///< ラインから壁までの長さ[mm]
#define GOAL_WIDTH           (1000.0)      ///< ゴールの長さ[mm]
#define GOAL_WIDTH2			  (GOAL_WIDTH/2.0)      ///< ゴールの長さの半分[mm]
#define GOAL_DEPTH           (300.0)      ///< ゴールの奥行き[mm]
#define GOAL_AREA_WIDTH            (1600.0)///< ゴールラインの幅[mm]
#define GOAL_AREA_WIDTH2           (GOAL_AREA_WIDTH/2.0)///< ゴールラインの幅の半分[mm]
#define GOAL_AREA_LENGTH            (600.0)///< ゴールラインの長さ[mm]
#define GOAL_AREA_LENGTH2           (GOAL_AREA_LENGTH/2.0)///< ゴールラインの長さの半分[mm]
#define CIRCLE_RADIUS        (500.0)      ///< センターサークルの半径[mm]
#define ROBOT_RADIUS         (75.0)      ///< ロボットの半径[mm]
#define ROBOT_WIDTH             (70.0) ///<ロボットの幅[mm]
#define ROBOT_WIDTH2         (ROBOT_WIDTH/2.0) ///<ロボットの幅の半分[mm]
#define ROBOT_LENGTH         (220.0) ///<ロボットの長さ？[mm]
#define ROBOT_LENGTH2         (ROBOT_LENGTH/2.0) ///<ロボットの長さ？の半分[mm]
#define MARKER_WIDTH         (120.0) ///<マーカの幅[mm]
#define MARKER_WIDTH2         (MARKER_WIDTH/2.0) ///<マーカの幅の半分[mm]
#define MARKER_LENGTH         (120.0) ///<マーカの長さ[mm]
#define MARKER_LENGTH2         (MARKER_LENGTH/2.0) ///<マーカの長さの半分[mm]
#define BALL_RADIUS           (35.0)      ///< ボールの半径ヒト型ver[mm]

///
///@brief   ロボットやボールの座標データ(直交座標)を表す構造体
///
///- x,y   座標[mm]
///- theta 機体の向き[rad]
///- 座標値および機体の向きがわからない場合、対応する x, y, theta に値 @ref INVISIBLE が入る
///
struct Orthogonal
{
  double x; ///< x座標 [mm]
  double y; ///< y座標 [mm]
  double theta; ///< 方向（x軸と成す角度）[rad]

  ///コンストラクタ
  Orthogonal()
  {
    x = y = theta = INVISIBLE;
  }
  ///コンストラクタ
  Orthogonal(double x_, double y_, double theta_)
  {
    x = x_; y = y_; theta = theta_;
  }
  ///オブジェクトが不可視か？
  bool isInvisible() const
  {
    return x == INVISIBLE;
  }
  ///オブジェクトを不可視にする
  void vanish()
  {
    x = y = theta = INVISIBLE;
  }
  double distance(const Orthogonal &p) const;
  double distance() const;
  double angle(const Orthogonal &p) const;
  double angle() const;
  Orthogonal transform(const Orthogonal &p) const;
  Orthogonal inverseTransform(const Orthogonal &p) const;
  bool isInField() const;
  bool isInOurGoalArea() const;
  bool isInTheirGoalArea() const;
#if 0
  bool Detectobstacle(const Orthogonal & p1,const Orthogonal & p2,const double r);
  Orthogonal linecircle(const Orthogonal &p1,const Orthogonal &p2,const double r);
  Orthogonal crosobstacle(const Orthogonal &p1,const Orthogonal &p2,const double r);
#endif

  ///出力ストリームに対する<<演算子のオーバロード
  friend std::ostream& operator<<(std::ostream& os, const Orthogonal& p);
};

///
///@brief フィールドの全ての物体の位置情報を保持する構造体
///
struct srInfo
{
  Orthogonal ball;                      ///<ボール位置
  Orthogonal robot[2][MAX_ROBOT_NUM+1]; ///<ロボット位置（0番要素は不使用）
  bool       id[2][MAX_ROBOT_NUM+1];    ///<ロボット番号が得られているか？（0番要素は不使用）
  double     time;                      ///<データ取得時刻
};

///
///@brief ビジョンサーバから提供される位置情報を保持する構造体
///
///- 意味的にはvision.hに置くべきだが，ファイル同士の依存性が高くなるので，ここに置く．
///
struct VisionInfo
{
  int        nBall;                     ///<ボールの数
  Orthogonal ball[MAX_BALL_NUM];        ///<ボールの座標（複数）
  int        nRobot[2];                 ///<ロボットの数
  Orthogonal robot[2][MAX_MARKER_NUM];  ///<ロボットの位置
  int        number[2][MAX_MARKER_NUM]; ///<ロボットのマーカ番号
  double     time;                      ///<データ取得時刻
  int        frameNumber;               ///<フレーム番号
  int        cameraId;                  ///<カメラのID

  ///コンストラクタ
  VisionInfo()
  {
    nBall = nRobot[BLUE] = nRobot[YELLOW] = 0;
  }
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
