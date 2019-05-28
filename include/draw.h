///
///@file draw.h
///@brief Drawクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017, 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/02/02 升谷 保博 クラス化
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2007/09/27 片岡 賢太郎
///- 2007/03/25 升谷 保博，片岡 賢太郎
///- 2006/04/18 升谷 保博，片岡 賢太郎
///@addtogroup draw Draw
///@brief EGGXを使った描画のためのクラス
///@{
///

#pragma once
#include <boost/thread.hpp>
#include "sr.h"
#include "cdrawdata.h"

namespace odens {

///
///@brief EGGXを利用してフィールドを描画するクラス
///
class Draw {
public:
  Draw() { m_windowEnable = false; };
  ~Draw() {};
  int initialize(int color, int number, double interval);
  void terminate();
  void set(const srInfo &si);
  void set(const srInfo &si, const srInfo &si2);
  void set(const VisionInfo &vi);
  void showPositionData();
  void reverseField();
  void showWindow();
  void hideWindow();
  void clear();
  void setColor(int red, int green, int blue);
  void getColor(int &red, int &green, int &blue);
  void point(double x, double y);
  void point(double x, double y, int red, int green, int blue);
  void cross(double x, double y, double size = 30);
  void cross(double x, double y, int red, int green, int blue, double size = 30);
  void line(double x1, double y1, double x2, double y2);
  void line(double x1, double y1, double x2, double y2, int red, int green, int blue);
  void rectangle(double x, double y, double cx, double cy);
  void rectangle(double x, double y, double cx, double cy, int red, int green, int blue);
  void fillRectangle(double x, double y, double cx, double cy);
  void fillRectangle(double x, double y, double cx, double cy, int red, int green, int blue);
  void circle(double x, double y, double r);
  void circle(double x, double y, double r, int red, int green, int blue);
  void fillCircle(double x, double y, double r);
  void fillCircle(double x, double y, double r, int red, int green, int blue);
  void string(double x, double y, int size, const char *str, ...);
  void string(double x, double y, int size, int red, int green, int blue, const char *str, ...);
  int getch();

private:
  const double SCALE = 0.1;  ///<描画スケール1mmが何dotに相当するか
  int m_mycolor;      ///<自機の色（チーム）
  int m_mynumber;     ///<自機の番号
  double m_interval; ///<描画処理の時間間隔 [s]
  int m_window;   ///<ウィンドウ識別番号
  bool m_windowEnable = false;    ///<ウィンドウを表示するかどうか?
  bool m_drawPos = true;    ///<座標の表示するかどうか
  bool m_positiveIsRightSide;         ///<画面の右が正か?
  int m_red = 255;///<描画色(赤)
  int m_green = 128;///<描画色(緑)
  int m_blue = 255;///<描画色(青)
  boost::thread m_thread; ///<スレッド
  boost::mutex m_mutex;   ///<ミューテックス（排他制御に利用）
  bool m_loop;            ///<別スレッドの繰り返しのフラグ
  srInfo m_srInfo; ///<フィールド情報のバッファ
  srInfo m_srInfo2; ///<フィールド情報の推定値のバッファ
  VisionInfo m_visionInfo; ///<ビジョン情報のバッファ
  ///
  ///@brief フィールド上の情報の描画モード
  ///
  enum DrawMode { NoEstimation, WithEstimation, Vision };
  DrawMode m_drawMode; ///<描画モード
  CDrawData m_drawData; ///<ユーザ描画データ
  CDrawData m_drawDataBuffer; ///<ユーザ描画データのバッファ
  int m_key; ///<入力されたキー

  void drawField();
  void drawBall(Orthogonal p);
  void drawBall(Orthogonal p, Orthogonal p2);
  void drawRobot(int color, int num, Orthogonal p, bool number);
  void drawRobot(int color, int num, Orthogonal p, bool number, Orthogonal p2, bool number2);
  void drawFieldLine(Orthogonal p, Orthogonal q);
  void main();
  inline unsigned char changeColor(int color);
};

//Drawクラスの大域変数．本当はシングルトンにすべき．
#ifdef DRAW_MAIN
Draw draw; ///< Drawクラスの大域変数
#else
extern Draw draw; ///< Drawクラスの大域変数
#endif
} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
