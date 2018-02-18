///
///@file util.h
///@brief 便利なマクロや関数
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2008/02/13
///- 2007/10/25 片岡 賢太郎
///- 2007/03/25 片岡 賢太郎
///- 2006/04/26 升谷 保博，片岡 賢太郎
///@addtogroup util Utility
///@brief 便利なマクロや関数
///@{
///

#pragma once
#include <cmath>

namespace odens {

// ------------------------------------------------------
// 関数プロトタイプ宣言

// 数学関係
double normalizeAngle(double x);              //角度を-PIから +PIに正規化する
double normalizeAngle2PI(double x);           //角度を  0から+2PIに正規化する
int    sign( double x);                       //引数の符号(プラスマイナス)を返す
int    sign_eq(double x,double y);            //符号が同じかどうか？
double bound(double x,double low,double high);//値をlowからhighの間に収める
double absmax(double x,double y);             //絶対値の大きい方を返す
double absmin(double x,double y);             //絶対値の小さい方を返す

// キーボード入力
void inkeyInitialize();     //inkey()関数を初期化して使用できるようにする
int  inkey();               //キーボードからの入力を受け取る

// 時計関係
void   getTimeInitialize(); //getTime()関数を初期化して使用できるようにする
double getTime();           //時間を取得する

// システム関係
int  msleep(unsigned int time);      //指定の時間(ミリ秒)スリープする


// ------------------------------------------------------
// マクロ定義

// 数学マクロ

///
///@brief     角度を [degree] 単位から [radian] 単位に変換する。
///@def       DEG2RAD(x)
///@param[in] x 角度[degree]
///@return    角度[radian]
///
#define DEG2RAD(x) ((double)(x)*M_PI/180.0)

///
///@brief     角度を [radian] 単位から [degree] 単位に変換する。
///@def       RAD2DEG(x)
///@param[in] x 角度[radian]
///@return    角度[degree]
///
#define RAD2DEG(x) ((double)(x)*180.0/M_PI)

// ------------------------------------------------------
// インライン関数

// 数学関係

///
///@brief     角度を正規化する
///@param[in] x 角度[radian]
///@return    -Pi から +Pi
///
///- 角度を-Piから+Piの間に正規化する。
///
inline double normalizeAngle(double x)
{
  if      (x<-M_PI) x += M_PI*2*(double)((int)(-x/(2*M_PI)+0.5));
  else if (x> M_PI) x -= M_PI*2*(double)((int)( x/(2*M_PI)+0.5));
  return x;
}


///
///@brief     角度を正規化する
///@param[in] x 角度[radian]
///@return    0 から +2Pi
///
///- 角度を0から2PIの間に正規化する。
///
inline double normalizeAngle2PI(double x)
{
  if      (x<0)      x += 2*M_PI*(double)((int)(-x/(2*M_PI)+1));
  else if (x>2*M_PI) x -= 2*M_PI*(double)((int)( x/(2*M_PI)  ));
  return x;
}

///
///@brief     引数の符号（プラスマイナス）を返す
///@param[in] x 数値
///@retval    -1 引数の値が0より小さい場合
///@retval    0  引数が0の時
///@retval    +1 引数の値が0より大きい場合
///
inline int sign( double x)
{
  if     ( x < 0 ) return -1;
  else if( x > 0 ) return  1;
  return 0;
}

///
///@brief     符号（プラスマイナス）が同じかどうか調べる
///@param[in] x  調べたい変数1
///@param[in] y  調べたい変数2
///@retval    1  xとyの符号は同じ
///@retval    0  xとyの符号が違う
///
inline int sign_eq(double x,double y)
{
  return((x >= 0) == (y >= 0));
}

///
///@brief     値を範囲内に収める
///@param[in] x    値のチェックをする変数
///@param[in] low  範囲の小さい方の値
///@param[in] high 範囲の大きい方の値
///@return    lowからhighの間に収まった値
///
inline double bound(double x,double low,double high)
{
  if(x < low ) x = low;
  if(x > high) x = high;
  return(x);
}

///
///@brief     絶対値の大きい方を返す
///@param[in] x 値のチェックをする変数
///@param[in] y 値のチェックをする変数
///@return    xかyの絶対値の大きい方
///
inline double absmax(double x,double y)
{
  return((fabs(x) > fabs(y))? x : y);
}

///
///@brief     絶対値の小さい方を返す
///@param[in] x 値のチェックをする変数
///@param[in] y 値のチェックをする変数
///@return    xかyの絶対値の小さい方
///
inline double absmin(double x,double y)
{
  return((fabs(x) < fabs(y))? x : y);
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
