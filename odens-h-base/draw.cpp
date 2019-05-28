///
///@file draw.cpp
///@brief Drawクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017, 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/02/02 升谷 保博 クラス化
///- 2019/02/01 升谷 保博 別スレッド化, CDrawDataを分離
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2008/02/13
///- 2007/11/29 片岡 賢太郎
///- 2007/09/27 片岡 賢太郎
///- 2007/04/15 升谷 保博
///- 2007/03/25 升谷 保博，片岡 賢太郎
///- 2007/02/28 片岡 賢太郎
///- 2006/05/17 升谷 保博，片岡 賢太郎
///- 2006/04/18 升谷 保博，片岡 賢太郎
///@addtogroup draw Draw
///@{
///

#if defined(_WINDOWS) || defined(WIN32) //Windows
  //Windows
  #include "wineggx.h"
  #define FONTNAME "ＭＳゴシック"

  #ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
  #endif

#elif LINUX //Linux
  //C言語へのリンケージ解決
  extern "C"
  {
    #include "eggx.h"
  }
  #define FONTNAME "Courier"
#endif

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <vector>
#include <cassert>
#include <stdarg.h>
#define DRAW_MAIN
#include "draw.h"
#include "util.h"
#include "cdrawdata.h"

using namespace std;

namespace odens {

///
///@brief     初期化
///@param[in] color  自分のチームカラー ( @ref BLUE or @ref YELLOW )
///@param[in] number 自分の機体番号  ( 1 to 3 )
///@param[in] interval 描画処理の時間間隔 [s]
///
int Draw::initialize(int color, int number, double interval)
{
  if (m_windowEnable)return m_window;

  //ウィンドウ作成
  int win = gopen((int)((FIELD_LENGTH+2*FIELD_MARGIN)*SCALE),(int)((FIELD_WIDTH+2*FIELD_MARGIN)*SCALE));
  
  if(win == -1){
    //ウィンドウ作成失敗
    cerr << "error - initialize window" << endl;
    return -1;
  }
  
  //初期化成功
  m_mycolor = color;
  m_mynumber = number;
  m_interval = interval;
  m_window = win;
  m_positiveIsRightSide = true;
  m_key = -1;
  
  //ノンブロックに設定
  gsetnonblock(ENABLE);

  //ウィンドウの表示レイヤと描画先レイヤ（バックバッファ）を設定
  layer(m_window,0,1);
  
  //ウィンドウの座標系を変更する(左下と右上の座標値を設定する)
  window(m_window,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN,
    FIELD_LENGTH2+FIELD_MARGIN,FIELD_WIDTH2+FIELD_MARGIN);
  
  //ウィンドウ描画文字列のフォント設定
  gsetfontset(m_window,FONTNAME);

  //フラグ設定
  m_windowEnable = true;

  boost::thread thread(&Draw::main, this);
  m_thread.swap(thread);

  return win;
}

///
///@brief 終了処理
///
void Draw::terminate()
{
  if (m_thread.joinable()) {
    m_loop = false;
    m_thread.join();
  }
  m_windowEnable = false;
  gclose(m_window);
}

///
///@brief
///@return なし
///
void Draw::main()
{
  srInfo si, si2;
  VisionInfo vi;
  DrawMode drawMode;
  Timer timer;
  m_loop = true;
  while (m_loop) {
    //時間間隔がintervalになるように眠る
    double dt = timer.sleep(m_interval);
    //cout << "draw dt: " << dt << endl;
    {
      boost::mutex::scoped_lock lock(m_mutex);
      drawMode = m_drawMode;
      if (drawMode == NoEstimation) {
        si = m_srInfo;
      } else if (drawMode == WithEstimation) {
        si = m_srInfo;
        si2 = m_srInfo2;
      } else if (drawMode == Vision) {
        vi = m_visionInfo;
      }
    }

    if (!m_windowEnable) continue;

    m_key = ::ggetch();

    //フィールドを描画する
    drawField();

    //ユーザー描画  
    m_drawDataBuffer.draw(m_window);

    if (drawMode == NoEstimation) {
      //ボールを描く
      drawBall(si.ball);
      //各ロボットを描く
      for (int i = 1; i <= MAX_ROBOT_NUM; i++) {
        drawRobot(BLUE, i, si.robot[BLUE][i], si.id[BLUE][i]);
        drawRobot(YELLOW, i, si.robot[YELLOW][i], si.id[YELLOW][i]);
      }
    } else if (drawMode == WithEstimation) {
      //ボールを描く
      drawBall(si.ball, si2.ball);
      //各ロボットを描く
      for (int i = 1; i <= MAX_ROBOT_NUM; i++) {
        drawRobot(BLUE, i,
          si.robot[BLUE][i], si.id[BLUE][i],
          si2.robot[BLUE][i], si2.id[BLUE][i]);
        drawRobot(YELLOW, i,
          si.robot[YELLOW][i], si.id[YELLOW][i],
          si2.robot[YELLOW][i], si2.id[YELLOW][i]);
      }
    } else if (drawMode == Vision) {
      //ボールを描く
      for (int i = 0; i<vi.nBall; i++) {
        drawBall(vi.ball[i]);
      }
      //各ロボットを描く
      for (int i = 0; i < vi.nRobot[BLUE]; i++) {
        drawRobot(BLUE, vi.number[BLUE][i], vi.robot[BLUE][i],
          vi.number[BLUE][i] != INVISIBLE);
      }
      for (int i = 0; i < vi.nRobot[YELLOW]; i++) {
        drawRobot(YELLOW, vi.number[YELLOW][i], vi.robot[YELLOW][i],
          vi.number[YELLOW][i] != INVISIBLE);
      }
    }

    //ウィンドウ更新
    copylayer(m_window, 1, 0);
  }
}

///
///@brief     フィールドを設定する
///@param[in] si フィールドの座標情報
///@return なし
///
void Draw::set(const srInfo &si)
{
  {
    boost::mutex::scoped_lock lock(m_mutex);
    m_drawMode = NoEstimation;
    m_srInfo = si;
    m_drawDataBuffer = m_drawData;
  }
  m_drawData.clear();
}

///
///@brief     フィールド情報を設定する（推定値付き）
///@param[in] si フィールドの座標情報)
///@param[in] si2 フィールドの座標情報の推定値
///@return なし
///
void Draw::set(const srInfo &si, const srInfo &si2)
{
  {
    boost::mutex::scoped_lock lock(m_mutex);
    m_drawMode = WithEstimation;
    m_srInfo = si;
    m_srInfo2 = si2;
    m_drawDataBuffer = m_drawData;
  }
  m_drawData.clear();
}

///
///@brief     フィールド情報を設定する VisionInfo版
///@param[in] vi フィールドの座標情報
///@return なし
///
void Draw::set(const VisionInfo &vi)
{
  {
    boost::mutex::scoped_lock lock(m_mutex);
    m_drawMode = Vision;
    m_visionInfo = vi;
    m_drawDataBuffer = m_drawData;
  }
  m_drawData.clear();
}


///
///@brief     フィールドを描画する
///@return なし
///
void
Draw::drawField()
{
  //描画色の変更
  ::newrgbcolor(m_window,25,115,25);
  
  //四角形描画（フィールドを緑で塗りつぶす）左下の座標を指定する。
  ::fillrect(m_window,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN,
           FIELD_LENGTH+2*FIELD_MARGIN,FIELD_WIDTH+2*FIELD_MARGIN);

  //描画色変更（白）
  ::newrgbcolor(m_window,255,255,255);

  //相手ゴール
  ::drawrect(m_window,FIELD_LENGTH2,-GOAL_WIDTH2,GOAL_DEPTH,GOAL_WIDTH);

  //自陣ゴール
  ::drawrect(m_window,-FIELD_LENGTH2-GOAL_DEPTH,-GOAL_WIDTH2,GOAL_DEPTH,GOAL_WIDTH);

  //フィールドライン
  ::drawrect(m_window,-FIELD_LENGTH2, -FIELD_WIDTH2, FIELD_LENGTH, FIELD_WIDTH);//四角形描画（塗りつぶしなし）
 
  ::line(m_window,0,-FIELD_WIDTH2,PENUP);  //線の初期位置を設定
  ::line(m_window,0, FIELD_WIDTH2,PENDOWN);//線の終端位置を設定

  //まん中の円
  ::circle(m_window,0,0,CIRCLE_RADIUS,CIRCLE_RADIUS);
  //左の四角形
  ::drawrect(m_window,-FIELD_LENGTH2,-GOAL_AREA_WIDTH2,GOAL_AREA_LENGTH,GOAL_AREA_WIDTH);
  //右の四角形
  ::drawrect(m_window,FIELD_LENGTH2-GOAL_AREA_LENGTH,-GOAL_AREA_WIDTH2,GOAL_AREA_LENGTH,GOAL_AREA_WIDTH);
}

///
///@brief     2点間の線分を描画する
///@param[in] p 端点1
///@param[in] q 端点2
///@return なし
///
void
Draw::drawFieldLine(Orthogonal p, Orthogonal q)
{
  if ( p.x == INVISIBLE ||q.x == INVISIBLE ) return;
  ::newrgbcolor(m_window,192,192,192);//描画色変更（ライトグレイ）
  ::line(m_window,(float)p.x, (float)p.y, PENUP);  //線の初期位置を設定
  ::line(m_window,(float)q.x, (float)q.y, PENDOWN);//線の終端位置を設定
}

///
///@brief     ボールの描画
///@param[in] p ボールの位置
///@return なし
///
void
Draw::drawBall(Orthogonal p)
{
  if (p.isInvisible()) return;
  ::newrgbcolor(m_window,255,128,64);//描画色変更（オレンジ）
  ::fillarc(m_window,(float)p.x,(float)p.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  if(m_drawPos) {//座標表示
    int sign = (m_positiveIsRightSide ? 1 : -1);
    ::newrgbcolor(m_window,255,0,0);
    ::drawstr(m_window,(float)(p.x-90*sign),(float)(p.y-180*sign),10,0,
      "%5.0lf,%5.0lf",p.x,p.y);
  }
}

///
///@brief     ボールの描画（推定値付き）
///@param[in] p ボールの位置
///@param[in] p2 ボールの位置の推定値
///@return なし
///
void
Draw::drawBall(Orthogonal p, Orthogonal p2)
{
  if (p.isInvisible() && p2.isInvisible()) return;
  ::newrgbcolor(m_window,255,128,64);//描画色変更（オレンジ）
  if (!p.isInvisible()) {
    ::fillarc(m_window,(float)p.x,(float)p.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  }
  if (!p2.isInvisible()) {
    ::drawarc(m_window,(float)p2.x,(float)p2.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  }
  if(m_drawPos) {//座標表示
    int sign = (m_positiveIsRightSide ? 1 : -1);
    ::newrgbcolor(m_window,255,0,0);
    ::drawstr(m_window,(float)(p2.x-90*sign),(float)(p2.y-180*sign),10,0,
      "%5.0lf,%5.0lf",p2.x,p2.y);
  }
}

///
///@brief     ロボットの描画
///@param[in] color  色
///@param[in] num    番号
///@param[in] p      位置
///@param[in] number 番号を描画するか？
///@return なし
///
void 
Draw::drawRobot(int color, int num, Orthogonal p, bool number)
{
  if (p.isInvisible()) return;
  if (color == BLUE) {
    ::newrgbcolor(m_window,0,0,255);//描画色変更（青）
  } else {
    ::newrgbcolor(m_window,255,255,0);//描画色変更（黄）
  }
  ::fillarc(m_window,(float)p.x,(float)p.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);

  ::newrgbcolor(m_window,0,0,0);//描画色変更（黒）

  if(p.theta != INVISIBLE)  {//方向描画
    ::line(m_window,(float)p.x, (float) p.y, PENUP);  //線の初期位置を設定
    ::line(m_window,(float)(p.x+cos(p.theta)*ROBOT_RADIUS*1.2), 
                    (float)(p.y+sin(p.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }

  int sign = (m_positiveIsRightSide ? 1 : -1);
  if(number) {
    //ロボット番号描画
    char str[3];
    str[0]='0'+num/10;
    str[1]='0'+num%10;
    str[2]='\0';
    ::drawstr(m_window,(float)(p.x-70*sign),(float)(p.y+40*sign),14,0,str);
  }
  if(m_drawPos) {//座標表示
    ::newrgbcolor(m_window,255,0,0);
    ::drawstr(m_window,(float)(p.x-90*sign),(float)(p.y-180*sign),10,0,
      "%5.0lf,%5.0lf,%4.0lf",p.x,p.y,p.theta/M_PI*180);
  }
}

///
///@brief     ロボットの描画（推定値付き）
///@param[in] color  色
///@param[in] num    番号
///@param[in] p      位置
///@param[in] number 番号を描画するか？
///@param[in] p2      位置（推定値）
///@param[in] number2 番号を描画するか？（推定値）
///@return なし
///
void 
Draw::drawRobot(int color, int num, Orthogonal p, bool number, Orthogonal p2, bool number2)
{
  if (p.isInvisible() && p2.isInvisible()) return;
  if (color == BLUE) {
    ::newrgbcolor(m_window,0,0,255);//描画色変更（青）
  } else {
    ::newrgbcolor(m_window,255,255,0);//描画色変更（黄）
  }
  if (!p.isInvisible()) {
    ::fillarc(m_window,(float)p.x,(float)p.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);
  }
  if (!p2.isInvisible()) {
    ::drawarc(m_window,(float)p2.x,(float)p2.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);
  }

  ::newrgbcolor(m_window,0,0,0);//描画色変更（黒）
  //方向描画
  if (!p.isInvisible() && p.theta != INVISIBLE) {
    ::line(m_window,(float)p.x, (float) p.y, PENUP);  //線の初期位置を設定
    ::line(m_window,(float)(p.x+cos(p.theta)*ROBOT_RADIUS*1.2), 
      (float)(p.y+sin(p.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }
  if (!p2.isInvisible() && p2.theta != INVISIBLE) {
    ::line(m_window,(float)p2.x, (float) p2.y, PENUP);  //線の初期位置を設定
    ::line(m_window,(float)(p2.x+cos(p2.theta)*ROBOT_RADIUS*1.2), 
      (float)(p2.y+sin(p2.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }

  int sign = (m_positiveIsRightSide ? 1 : -1);
  if(number2) {
    //ロボット番号描画
    char str[3];
    str[0]='0'+num/10;
    str[1]='0'+num%10;
    str[2]='\0';
    ::drawstr(m_window,(float)(p2.x-70*sign),(float)(p2.y+40*sign),14,0,str);
  }
  if(m_drawPos) {//座標表示
    ::newrgbcolor(m_window,255,0,0);
    ::drawstr(m_window,(float)(p2.x-90*sign),(float)(p2.y-180*sign),10,0,
      "%5.0lf,%5.0lf,%4.0lf",p2.x,p2.y,p2.theta/M_PI*180);
  }
}

///
///@brief ウィンドウに機体の座標を表示する
///@return なし
///
void Draw::showPositionData()
{
  m_drawPos = !m_drawPos;
}

///
///@brief     (上下左右)反転表示する
///@return なし
///
void Draw::reverseField()
{
  m_positiveIsRightSide = !m_positiveIsRightSide;
  if (m_positiveIsRightSide) {
    window(m_window,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN-10,
                       FIELD_LENGTH2+FIELD_MARGIN,+FIELD_WIDTH2+FIELD_MARGIN+10);
  } else {
    window(m_window, FIELD_LENGTH2+FIELD_MARGIN,+FIELD_WIDTH2+FIELD_MARGIN+10,
                      -FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN-10);
  }
}

///
///@brief     ウィンドウを表示する
///@return なし
///
void Draw::showWindow()
{
  if (m_windowEnable) return;

  //ウィンドウ作成
  int win = gopen((int)((FIELD_LENGTH + 2 * FIELD_MARGIN)*SCALE), (int)((FIELD_WIDTH + 2 * FIELD_MARGIN)*SCALE));

  if (win == -1) {
    //ウィンドウ作成失敗
    cerr << "error - initialize window" << endl;
    return;
  }
  m_windowEnable = true;
  m_window = win;

  //ウィンドウの表示レイヤと描画先レイヤ（バックバッファ）を設定
  layer(m_window, 0, 1);

  //ウィンドウ描画文字列のフォント設定
  gsetfontset(m_window, FONTNAME);

  if (m_positiveIsRightSide) {
    window(m_window, -FIELD_LENGTH2 - FIELD_MARGIN, -FIELD_WIDTH2 - FIELD_MARGIN - 10,
      FIELD_LENGTH2 + FIELD_MARGIN, +FIELD_WIDTH2 + FIELD_MARGIN + 10);
  } else {
    window(m_window, FIELD_LENGTH2 + FIELD_MARGIN, +FIELD_WIDTH2 + FIELD_MARGIN + 10,
      -FIELD_LENGTH2 - FIELD_MARGIN, -FIELD_WIDTH2 - FIELD_MARGIN - 10);
  }


}

///
///@brief     ウィンドウを消去する
///@return なし
///
void Draw::hideWindow()
{
  if (!m_windowEnable) return;

  m_windowEnable = false;
  m_key = -1;
  gclose(m_window);
}


///
///@brief  色を調整する
///@param[in] color 色要素の値
///@return 0～255に収めた結果
///
inline unsigned char Draw::changeColor(int color)
{
  if(color<  0)return 0;
  if(color>255)return 255;
  return (unsigned char)color;
}

///
///@brief     登録されているウィンドウの描画オブジェクトをクリアする
///@return なし
///
void Draw::clear()
{
  m_drawData.clear();
}

///
///@brief     描画色を設定する
///@param[in] red   赤色 ( 0 to 255 )
///@param[in] green 緑色 ( 0 to 255 )
///@param[in] blue  青色 ( 0 to 255 )
///@return なし
///
void Draw::setColor(int red,int green,int blue)
{
  m_red   = changeColor(red);
  m_green = changeColor(green);
  m_blue  = changeColor(blue);
}


///
///@brief      現在の描画色を取得する
///@param[out] red   赤色 ( 0 to 255 )
///@param[out] green 緑色 ( 0 to 255 )
///@param[out] blue  青色 ( 0 to 255 )
///@return なし
///
void Draw::getColor(int &red,int &green,int &blue)
{
  red   = m_red;
  green = m_green;
  blue  = m_blue;
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@return なし
///
void Draw::point(double x,double y)
{
  m_drawData.setPoint(x, y, m_red, m_green, m_blue);
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::point(double x, double y, int red, int green, int blue)
{
  m_drawData.setPoint(x, y, red, green, blue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@return なし
///
void Draw::cross(double x, double y, double size)
{
  m_drawData.setCross(x, y, size, m_red, m_green, m_blue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::cross(double x, double y, int red, int green, int blue, double size)
{
  m_drawData.setCross(x, y, size, red, green, blue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@return なし
///
void Draw::line(double x1, double y1, double x2, double y2)
{
  m_drawData.setLine(x1, y1, x2, y2, m_red, m_green, m_blue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::line(double x1, double y1, double x2, double y2, int red, int green, int blue)
{
  m_drawData.setLine(x1, y1, x2, y2, red, green, blue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
void Draw::rectangle(double x, double y, double cx, double cy)
{
  m_drawData.setRectangle(x, y, cx, cy, m_red, m_green, m_blue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::rectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  m_drawData.setRectangle(x, y, cx, cy, red, green, blue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
void Draw::fillRectangle(double x, double y, double cx, double cy)
{
  m_drawData.setFillRectangle(x, y, cx, cy, m_red, m_green, m_blue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::fillRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  m_drawData.setFillRectangle(x, y, cx, cy, red, green, blue);
}

///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
void Draw::circle(double x, double y, double r)
{
  m_drawData.setCircle(x, y, r, m_red, m_green, m_blue);
}


///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::circle(double x, double y, double r, int red, int green, int blue)
{
  m_drawData.setCircle(x, y, r, red, green, blue);
}


///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
void Draw::fillCircle(double x, double y, double r)
{
  m_drawData.setFillCircle(x, y, r, m_red, m_green, m_blue);
}

///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::fillCircle(double x, double y, double r, int red, int green, int blue)
{
  m_drawData.setFillCircle(x, y, r, red, green, blue);
}

///
///@brief     文字列を描画する
///@param[in] x,y   描画先座標[mm]
///@param[in] size  文字サイズ(標準14)
///@param[in] str   描画文字列
///@return なし
///
///- printf()構文と同じように文字を描画できる。
///
void Draw::string(double x, double y, int size, const char *str, ... )
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //可変数引数リスト
  va_start(argptr,str);       //initalize va_ function
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL文字付加
  } else {
    buf[len] = '\0';
  }

  m_drawData.setString(x, y, size, m_red, m_green, m_blue, buf);
}

///
///@brief     文字列を描画する
///@param[in] x,y   描画先座標[mm]
///@param[in] size  文字サイズ(標準14)
///@param[in] red   文字色(赤成分)
///@param[in] green 文字色(緑成分)
///@param[in] blue  文字色(青成分)
///@param[in] str   描画文字列
///@return なし
///
///- printf()構文と同じように文字を描画できる
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void Draw::string(double x, double y, int size, int red, int green, int blue, const char *str, ...)
{
  assert(str);

  if(!size)size = 10;

  char buf[128];

  va_list argptr;             //可変数引数リスト
  va_start(argptr,str);       //initalize va_ function
  int len = vsnprintf_s(buf,sizeof(buf),_TRUNCATE,str,argptr);//
  va_end(argptr);             //close     va_ function

  if (len==-1) {
    buf[sizeof(buf)-1] = '\0';//NULL文字付加
  } else {
    buf[len] = '\0';
  }

  m_drawData.setString(x, y, size, red, green, blue, buf);
}

///
///@brief  キー入力
///@return なし
///
int Draw::getch()
{
  static int keyPrev = -1;
  static Timer timer;
  int ret = -1;
  if (m_key != keyPrev || timer.elapsed() > 0.2) {
    ret = m_key;
    timer.reset();
  }
  keyPrev = m_key;
  return ret;
}

} //namespace odens


///@} doxygenのためのコメント（消してはいけない）
