///
///@file draw.cpp
///@brief 描画のための関数の宣言を集めたもの
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
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
#include "draw.h"
#include "util.h"

using namespace std;

namespace odens {

#define SCALE (0.1) ///<描画スケール1mmが何dotに相当するか
#define DT (1) ///<使っていない？
#define GUI_FRAME_RATE (30)///< 描画速度(default=30)[fps]

#define MAX_DRAW_OBJECT (256) ///<1フレームで描画可能な最大オブジェクト数
#define POINT         (1) ///<点
#define CROSS         (2) ///<十時
#define LINE          (3) ///<線
#define RECTANGLE     (4) ///<四角形
#define FILLRECTANGLE (5) ///<四角形（塗りつぶし）
#define CIRCLE        (6) ///<円
#define FILLCIRCLE    (7) ///<円（塗りつぶし）
#define STRING        (8) ///<文字列

#define VIEW_NONE   (0) ///<視覚情報なし
#define VIEW_GLOBAL (1) ///<視覚情報グローバル

//プロトタイプ宣言（このファイル内だけで使う関数）
void drawField();
void drawBall(Orthogonal p);
void drawBall(Orthogonal p, Orthogonal p2);
void drawRobot(int color, int num, Orthogonal p, bool number);
void drawRobot(int color, int num, Orthogonal p, bool number, Orthogonal p2, bool number2);
void drawFieldLine(Orthogonal p, Orthogonal q);

//このファイル内だけで使う大域変数
int mycolor;      ///<自機の色（チーム）
int mynumber;     ///<自機の番号
int mainwindow;   ///<ウィンドウ識別番号
bool gWindowEnable = false;    ///<ウィンドウを表示するかどうか?
bool gDrawPos      = true;    ///<座標の表示するかどうか
bool gPositiveIsRightSide;         ///<画面の右が青チームか?
bool gLogFileViewer= false;    ///<ログファイルビューアフラグ
int gDrawViewMode  = VIEW_NONE;///<視界情報フラグ(none or global or local)
int gRed   = 255;///<描画色(赤)
int gGreen = 128;///<描画色(緑)
int gBlue  = 255;///<描画色(青)
double gDrawTime = 0;         ///<描画時間
double gDrawFrameTime = 0.015;///<描画時間

///
///@brief ユーザ描画用構造体
///
struct DRAW_OBJECT
{
  int type;             ///<種類
  double x1;            ///<x座標1
  double y1;            ///<y座標1
  double x2;            ///<x座標2
  double y2;            ///<y座標2
  double r;             ///<半径
  unsigned char red;    ///<色の赤成分
  unsigned char green;  ///<色の緑成分
  unsigned char blue;   ///<色の青成分
  string str;           ///<文字列
};

///
///@brief ユーザ描画用クラス
///
class CDrawData
{
public:
  CDrawData();

  void SetPoint( double x, double y, unsigned rd,unsigned gr,unsigned bl);
  void SetCross( double x, double y, double size,unsigned rd,unsigned gr,unsigned bl);
  void SetLine(  double x1,double y1,double x2,double y2, unsigned rd,unsigned gr,unsigned bl);
  void SetRectangle(    double x, double y, double cx,double cy,unsigned rd,unsigned gr,unsigned bl);
  void SetFillRectangle(double x, double y, double cx,double cy,unsigned rd,unsigned gr,unsigned bl);
  void SetCircle(    double x, double y, double r,unsigned rd,unsigned gr,unsigned bl);
  void SetFillCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl);
  void SetString(    double x, double y, int size,unsigned rd,unsigned gr,unsigned bl,char *str);

  bool GetObject(int id,DRAW_OBJECT &data);
  void CleanData();
private:
  vector<DRAW_OBJECT> m_object; ///<描画オブジェクトの並び
  int m_num; ///<オブジェクトの数
  int m_max; ///<オブジェクトの最大数
}gDrawData; ///<CDrawDataクラスの大域オブジェクト

void drawCommandProc();


///
///@brief     描画用ウィンドウの作成。
///@param[in] color  自分のチームカラー ( @ref BLUE or @ref YELLOW )
///@param[in] number 自分の機体番号  ( 1 to 5 )
///@retval    -1 エラー
///@retval    -1以外 成功。 ( 作成されたウィンドウの識別子 )
///
///グローバルビューモード用関数
///
int drawInitialize(int color, int number)
{
  if(gWindowEnable)return mainwindow;

  //ウィンドウ作成
  int re=gopen((int)((FIELD_LENGTH+2*FIELD_MARGIN)*SCALE),(int)((FIELD_WIDTH+2*FIELD_MARGIN)*SCALE));
  
  if(re==-1)
  { //ウィンドウ初期化失敗
    printf("error - initialize window\n");
    return -1;
  }
  
  //初期化成功
  mycolor          = color;
  mynumber         = number;
  mainwindow       = re;
  gPositiveIsRightSide = true;
  gDrawTime        = getTime();
  gDrawFrameTime   = (double)(200/(GUI_FRAME_RATE))/200.0;
  
  //ウィンドウの表示レイヤと描画先レイヤ（バックバッファ）を設定
  layer(mainwindow,0,1);
  
  //ウィンドウの座標系を変更する(左下と右上の座標値を設定する)
  window(mainwindow,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN,
    FIELD_LENGTH2+FIELD_MARGIN,FIELD_WIDTH2+FIELD_MARGIN);
  
  //ウィンドウ描画文字列のフォント設定
  gsetfontset(mainwindow,FONTNAME);
 
  //フラグ設定
  gWindowEnable    = true;
  gDrawViewMode    = VIEW_GLOBAL;

  return re;
}

///
///@brief     フィールドを描画する
///@param[in] si フィールドの座標情報 (直交座標系)
///@return なし
///
///- グローバルビューモード用関数
///
void draw(const srInfo &si)
{
  if(!gWindowEnable || gDrawViewMode!=VIEW_GLOBAL)
  {
    gDrawData.CleanData();
    return;
  }


  //描画時間に達しているか？
  if(getTime()-gDrawTime<gDrawFrameTime)
  {
    gDrawData.CleanData();
    return;
  }
  gDrawTime = getTime();

  //フィールドを描画する
  drawField();

  //ユーザー描画  
  drawCommandProc();

  //ボールを描く
  drawBall(si.ball);

  //各ロボットを描く
  for ( int i = 1; i <= MAX_ROBOT_NUM; i++) 
  {
    drawRobot(BLUE,   i, si.robot[BLUE][i],   si.id[BLUE][i]);
    drawRobot(YELLOW, i, si.robot[YELLOW][i], si.id[YELLOW][i]);
  }
  
  //ウィンドウ更新
  copylayer(mainwindow,1,0);
}

///
///@brief     フィールドを描画する（推定値付き）
///@param[in] si フィールドの座標情報 (直交座標系)
///@param[in] si2 フィールドの座標情報の推定値 (直交座標系)
///@return なし
///
void draw(const srInfo &si, const srInfo &si2)
{
  if(!gWindowEnable || gDrawViewMode!=VIEW_GLOBAL)
  {
    gDrawData.CleanData();
    return;
  }


  //描画時間に達しているか？
  if(getTime()-gDrawTime<gDrawFrameTime)
  {
    gDrawData.CleanData();
    return;
  }
  gDrawTime = getTime();

  //フィールドを描画する
  drawField();

  //ユーザー描画  
  drawCommandProc();

  //ボールを描く
  drawBall(si.ball,si2.ball);

  //各ロボットを描く
  for ( int i = 1; i <= MAX_ROBOT_NUM; i++) 
  {
    drawRobot(BLUE,   i, 
      si.robot[BLUE][i],   si.id[BLUE][i], 
      si2.robot[BLUE][i],   si2.id[BLUE][i]);
    drawRobot(YELLOW, i, 
      si.robot[YELLOW][i], si.id[YELLOW][i], 
      si2.robot[YELLOW][i], si2.id[YELLOW][i]);
  }
  
  //ウィンドウ更新
  copylayer(mainwindow,1,0);
}

///
///@brief     フィールドを描画する VisionInfo版
///@param[in] vi フィールドの座標情報 (直交座標系)
///@return なし
///
///- グローバルビューモード用関数
///
void draw(const VisionInfo &vi)
{
  if(!gWindowEnable || gDrawViewMode!=VIEW_GLOBAL)
  {
    gDrawData.CleanData();
    return;
  }


  //描画時間に達しているか？
  if(getTime()-gDrawTime<gDrawFrameTime)
  {
    gDrawData.CleanData();
    return;
  }
  gDrawTime = getTime();

  //フィールドを描画する
  drawField();

  //ユーザー描画  
  drawCommandProc();

  //ボールを描く
  for (int i=0; i<vi.nBall; i++) {
    drawBall(vi.ball[i]);
  }

  //各ロボットを描く
  for ( int i = 0; i < vi.nRobot[BLUE]; i++) 
  {
    drawRobot(BLUE,   vi.number[BLUE][i], vi.robot[BLUE][i],   
      vi.number[BLUE][i]!=INVISIBLE);
  }
 
  for ( int i = 0; i < vi.nRobot[YELLOW]; i++) 
  {
    drawRobot(YELLOW, vi.number[YELLOW][i], vi.robot[YELLOW][i], 
      vi.number[YELLOW][i]!=INVISIBLE);
  }
  
  
  //ウィンドウ更新
  copylayer(mainwindow,1,0);
}


///
///@brief     フィールドを描画する
///@return なし
///
void
drawField()
{
  //描画色の変更
  newrgbcolor(mainwindow,25,115,25);
  
  //四角形描画（フィールドを緑で塗りつぶす）左下の座標を指定する。
  fillrect(mainwindow,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN,
           FIELD_LENGTH+2*FIELD_MARGIN,FIELD_WIDTH+2*FIELD_MARGIN);

  //描画色変更（白）
  newrgbcolor(mainwindow,255,255,255);

  //相手ゴール
  drawrect(mainwindow,FIELD_LENGTH2,-GOAL_WIDTH2,GOAL_DEPTH,GOAL_WIDTH);

  //自陣ゴール
  drawrect(mainwindow,-FIELD_LENGTH2-GOAL_DEPTH,-GOAL_WIDTH2,GOAL_DEPTH,GOAL_WIDTH);

  //フィールドライン
  drawrect(mainwindow,-FIELD_LENGTH2, -FIELD_WIDTH2, FIELD_LENGTH, FIELD_WIDTH);//四角形描画（塗りつぶしなし）
 
  line(mainwindow,0,-FIELD_WIDTH2,PENUP);  //線の初期位置を設定
  line(mainwindow,0, FIELD_WIDTH2,PENDOWN);//線の終端位置を設定

  //まん中の円
  circle(mainwindow,0,0,CIRCLE_RADIUS,CIRCLE_RADIUS);
  //左の四角形
  drawrect(mainwindow,-FIELD_LENGTH2,-GOAL_AREA_WIDTH2,GOAL_AREA_LENGTH,GOAL_AREA_WIDTH);
  //右の四角形
  drawrect(mainwindow,FIELD_LENGTH2-GOAL_AREA_LENGTH,-GOAL_AREA_WIDTH2,GOAL_AREA_LENGTH,GOAL_AREA_WIDTH);
}

///
///@brief     2点間の線分ドを描画する
///@param[in] p 端点1
///@param[in] q 端点2
///@return なし
///
void
drawFieldLine(Orthogonal p, Orthogonal q)
{
  if ( p.x == INVISIBLE ||q.x == INVISIBLE ) return;
  newrgbcolor(mainwindow,192,192,192);//描画色変更（ライトグレイ）
  line(mainwindow,(float)p.x, (float)p.y, PENUP);  //線の初期位置を設定
  line(mainwindow,(float)q.x, (float)q.y, PENDOWN);//線の終端位置を設定
}

///
///@brief     ボールの描画
///@param[in] p ボールの位置
///@return なし
///
void
drawBall(Orthogonal p)
{
  if (p.isInvisible()) return;
  newrgbcolor(mainwindow,255,128,64);//描画色変更（オレンジ）
  fillarc(mainwindow,(float)p.x,(float)p.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  if(gDrawPos) {//座標表示
    int sign = (gPositiveIsRightSide ? 1 : -1);
    newrgbcolor(mainwindow,255,0,0);
    drawstr(mainwindow,(float)(p.x-90*sign),(float)(p.y-180*sign),10,0,
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
drawBall(Orthogonal p, Orthogonal p2)
{
  if (p.isInvisible() && p2.isInvisible()) return;
  newrgbcolor(mainwindow,255,128,64);//描画色変更（オレンジ）
  if (!p.isInvisible()) {
    fillarc(mainwindow,(float)p.x,(float)p.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  }
  if (!p2.isInvisible()) {
    drawarc(mainwindow,(float)p2.x,(float)p2.y,BALL_RADIUS*1.5,BALL_RADIUS*1.5,0,360,1);
  }
  if(gDrawPos) {//座標表示
    int sign = (gPositiveIsRightSide ? 1 : -1);
    newrgbcolor(mainwindow,255,0,0);
    drawstr(mainwindow,(float)(p2.x-90*sign),(float)(p2.y-180*sign),10,0,
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
drawRobot(int color, int num, Orthogonal p, bool number)
{
  if (p.isInvisible()) return;
  if (color == BLUE) {
    newrgbcolor(mainwindow,0,0,255);//描画色変更（青）
  } else {
    newrgbcolor(mainwindow,255,255,0);//描画色変更（黄）
  }
  fillarc(mainwindow,(float)p.x,(float)p.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);

  newrgbcolor(mainwindow,0,0,0);//描画色変更（黒）

  if(p.theta != INVISIBLE)  {//方向描画
    line(mainwindow,(float)p.x, (float) p.y, PENUP);  //線の初期位置を設定
    line(mainwindow,(float)(p.x+cos(p.theta)*ROBOT_RADIUS*1.2), 
                    (float)(p.y+sin(p.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }

  int sign = (gPositiveIsRightSide ? 1 : -1);
  if(number) {
    //ロボット番号描画
    char str[3];
    str[0]='0'+num/10;
    str[1]='0'+num%10;
    str[2]='\0';
    drawstr(mainwindow,(float)(p.x-70*sign),(float)(p.y+40*sign),14,0,str);
  }
  if(gDrawPos) {//座標表示
    newrgbcolor(mainwindow,255,0,0);
    drawstr(mainwindow,(float)(p.x-90*sign),(float)(p.y-180*sign),10,0,
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
drawRobot(int color, int num, Orthogonal p, bool number, Orthogonal p2, bool number2)
{
  if (p.isInvisible() && p2.isInvisible()) return;
  if (color == BLUE) {
    newrgbcolor(mainwindow,0,0,255);//描画色変更（青）
  } else {
    newrgbcolor(mainwindow,255,255,0);//描画色変更（黄）
  }
  if (!p.isInvisible()) {
    fillarc(mainwindow,(float)p.x,(float)p.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);
  }
  if (!p2.isInvisible()) {
    drawarc(mainwindow,(float)p2.x,(float)p2.y,ROBOT_RADIUS,ROBOT_RADIUS,0,360,1);
  }

  newrgbcolor(mainwindow,0,0,0);//描画色変更（黒）
  //方向描画
  if (!p.isInvisible() && p.theta != INVISIBLE) {
    line(mainwindow,(float)p.x, (float) p.y, PENUP);  //線の初期位置を設定
    line(mainwindow,(float)(p.x+cos(p.theta)*ROBOT_RADIUS*1.2), 
      (float)(p.y+sin(p.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }
  if (!p2.isInvisible() && p2.theta != INVISIBLE) {
    line(mainwindow,(float)p2.x, (float) p2.y, PENUP);  //線の初期位置を設定
    line(mainwindow,(float)(p2.x+cos(p2.theta)*ROBOT_RADIUS*1.2), 
      (float)(p2.y+sin(p2.theta)*ROBOT_RADIUS*1.2), PENDOWN);//線の終端位置を設定
  }

  int sign = (gPositiveIsRightSide ? 1 : -1);
  if(number2) {
    //ロボット番号描画
    char str[3];
    str[0]='0'+num/10;
    str[1]='0'+num%10;
    str[2]='\0';
    drawstr(mainwindow,(float)(p2.x-70*sign),(float)(p2.y+40*sign),14,0,str);
  }
  if(gDrawPos) {//座標表示
    newrgbcolor(mainwindow,255,0,0);
    drawstr(mainwindow,(float)(p2.x-90*sign),(float)(p2.y-180*sign),10,0,
      "%5.0lf,%5.0lf,%4.0lf",p2.x,p2.y,p2.theta/M_PI*180);
  }
}

///
///@brief     自分のロボットの追加の描画
///@param[in] color  色
///@param[in] num    番号
///@param[in] p      位置
///@param[in] number 番号を描画するか？
///@return なし
///
void 
drawMyRobot(int color, int num, Orthogonal p,bool number)
{
  if ( p.x == INVISIBLE ) return;
  p.y *= -1;
  if(number)
  {
    if ( color == BLUE ) 
    {
      newrgbcolor(mainwindow,0,0,255);//描画色変更（青）
    } 
    else 
    {
      newrgbcolor(mainwindow,255,255,0);//描画色変更（黄）
    }

    fillarc(mainwindow,(float)p.x,(float)p.y,ROBOT_RADIUS*1.2,ROBOT_RADIUS*1.2,0,360,1);
  }
}

//----------------------------------------------------------------------
//共通の関数

///
///@brief     ウィンドウを閉じ、終了処理を行う
///@return なし
///
///- 座標系に関係無く共通
///
void drawTerminate()
{
  gWindowEnable = false;
  gcloseall();
}

///
///@brief ウィンドウに機体の座標を表示する
///@return なし
///
///- 座標系に関係無く共通
///
void drawShowPositionData()
{
  gDrawPos = !gDrawPos;
}

///
///@brief     (上下左右)反転表示する
///@return なし
///
///- 座標系に関係無く共通
///
void drawReverseField()
{
  gPositiveIsRightSide = !gPositiveIsRightSide;
  if(gPositiveIsRightSide)
  {
    window(mainwindow,-FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN-10,
                       FIELD_LENGTH2+FIELD_MARGIN,+FIELD_WIDTH2+FIELD_MARGIN+10);
  }
  else
  {
    window(mainwindow, FIELD_LENGTH2+FIELD_MARGIN,+FIELD_WIDTH2+FIELD_MARGIN+10,
                      -FIELD_LENGTH2-FIELD_MARGIN,-FIELD_WIDTH2-FIELD_MARGIN-10);
  }
}

///
///@brief     ウィンドウの表示/非表示を行う
///@return なし
///
///- 座標系に関係無く共通
///
void drawSetShowEnable()
{
  if(gDrawViewMode==VIEW_NONE)return;

  bool visible = !gWindowEnable;

  if(visible)
  {
    drawInitialize(mycolor,mynumber);
    printf("window show mode - enable\n");
  }
  else
  {
    gWindowEnable = false;

    //ウィンドウを閉じる
    //gclose(mainwindow);
    gcloseall();
    printf("window show mode - disable\n");
  }
}

///
///@brief     ログファイルビューアモードにする
///@return なし
///
void drawSetLogFileViewer()
{
  gLogFileViewer = true;
}


// =====================================================================

///
///@brief     コンストラクタ
///
CDrawData::CDrawData()
{
  m_num = 0;
  m_max = MAX_DRAW_OBJECT;
  m_object.reserve(MAX_DRAW_OBJECT);
}

///
///@brief 描画する点のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetPoint(double x, double y, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = POINT;
  data.x1    = x;
  data.y1    = y;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する×印のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] size 寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetCross(double x, double y,double size, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = CROSS;
  data.x1    = x;
  data.y1    = y;
  data.x2    = size;
  data.y2    = size;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する線のデータを登録する
///@param[in] x1 x座標1
///@param[in] y1 y座標1
///@param[in] x2 x座標2
///@param[in] y2 y座標2
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetLine(double x1,double y1,double x2,double y2, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = LINE;
  data.x1    = x1;
  data.y1    = y1;
  data.x2    = x2;
  data.y2    = y2;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する四角形のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] cx x寸法
///@param[in] cy y寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetRectangle(    double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = RECTANGLE;
  data.x1    = x;
  data.y1    = y;
  data.x2    = cx;
  data.y2    = cy;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する四角形のデータを登録する(塗りつぶし)
///@param[in] x x座標
///@param[in] y y座標
///@param[in] cx x寸法
///@param[in] cy y寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetFillRectangle(double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = FILLRECTANGLE;
  data.x1    = x;
  data.y1    = y;
  data.x2    = cx;
  data.y2    = cy;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;

}

///
///@brief 描画する円のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] r 半径
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = CIRCLE;
  data.x1    = x;
  data.y1    = y;
  data.r     = r;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する円のデータを登録する(塗りつぶし)
///@param[in] x x座標
///@param[in] y y座標
///@param[in] r 半径
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@return なし
///
void CDrawData::SetFillCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = FILLCIRCLE;
  data.x1    = x;
  data.y1    = y;
  data.r     = r;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  m_object.push_back(data);
  m_num++;
}

///
///@brief 描画する文字列のデータを登録する
///@param[in] x x座標
///@param[in] y y座標
///@param[in] size 寸法
///@param[in] rd 赤
///@param[in] gr 緑
///@param[in] bl 青
///@param[in] str 文字列
///@return なし
///
void CDrawData::SetString(double x, double y, int size, unsigned rd,unsigned gr,unsigned bl,char *str)
{
  if(m_num>=m_max)return;
  DRAW_OBJECT data;
  data.type  = STRING;
  data.x1    = x;
  data.y1    = y;
  data.r     = size;
  data.red   = rd;
  data.green = gr;
  data.blue  = bl;
  data.str   = str;
  m_object.push_back(data);
  m_num++;
}

///
///@brief  描画用のデータを取得する
///@param[in] id
///@param[out] data
///@retval true 正常終了
///@retval false 異常終了
///
bool CDrawData::GetObject(int id,DRAW_OBJECT &data)
{
  int num = (int)m_object.size();
  if(num<=id||id<0)return false;
  data = m_object.at(id);
  return true;
}

///
///@brief  クリーンする
///@return なし
///
void CDrawData::CleanData()
{
  m_object.clear();
  m_num = 0;
}

///
///@brief  色を調整する
///@param[in] color 色要素の値
///@return 0～255に収めた結果
///
inline unsigned char ChangeColor(int color)
{
  if(color<  0)return 0;
  if(color>255)return 255;
  return (unsigned char)color;
}

///
///@brief     登録されているウィンドウの描画オブジェクトをクリアする
///@return なし
///
///- 座標系に関係無く共通
///
void drawClear()
{
  gDrawData.CleanData();
}

///
///@brief     描画色を設定する
///@param[in] red   赤色 ( 0 to 255 )
///@param[in] green 緑色 ( 0 to 255 )
///@param[in] blue  青色 ( 0 to 255 )
///@return なし
///
///- 座標系に関係無く共通
///
void drawSetColor(int red,int green,int blue)
{
  gRed   = ChangeColor(red);
  gGreen = ChangeColor(green);
  gBlue  = ChangeColor(blue);
}


///
///@brief      現在の描画色を取得する
///@param[out] red   赤色 ( 0 to 255 )
///@param[out] green 緑色 ( 0 to 255 )
///@param[out] blue  青色 ( 0 to 255 )
///@return なし
///
///- 座標系に関係無く共通
///
void drawGetColor(int &red,int &green,int &blue)
{
  red   = gRed;
  green = gGreen;
  blue  = gBlue;
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawPoint(double x,double y)
{
  gDrawData.SetPoint(x,y,gRed,gGreen,gBlue);
}

///
///@brief     点を描画する
///@param[in] x,y            描画先座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawPoint( double x, double y , int red, int green, int blue)
{
  gDrawData.SetPoint(x,y,red,green,blue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCross(double x,double y,double size)
{
  gDrawData.SetCross(x,y,size,gRed,gGreen,gBlue);
}

///
///@brief     十字型のマークを描画する
///@param[in] x,y            中心座標 [mm]
///@param[in] size           十字の大きさ[mm](デフォルトでは30)＊省略可
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCross( double x, double y,int red, int green, int blue,double size)
{
  gDrawData.SetCross(x,y,size,red,green,blue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawLine(double x1,double y1,double x2,double y2)
{
  gDrawData.SetLine(x1,y1,x2,y2,gRed,gGreen,gBlue);
}

///
///@brief     2点間の線分を描画する
///@param[in] x1,y1          点1の座標 [mm]
///@param[in] x2,y2          点2の座標 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawLine(  double x1,double y1,double x2,double y2, int red, int green, int blue)
{
  gDrawData.SetLine(x1,y1,x2,y2,red,green,blue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     軸に平行な四角形を描画する
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillRectangle(double x, double y, double cx, double cy)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,gRed,gGreen,gBlue);
}

///
///@brief     軸に平行な四角形を描画する(内部を塗りつぶす)
///@param[in] x,y            左上の座標 [mm]
///@param[in] cx,cy          四角形の幅 [mm]
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillRectangle(double x, double y, double cx, double cy, int red, int green, int blue)
{
  gDrawData.SetFillRectangle(x,y,cx,cy,red,green,blue);
}

///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCircle(double x, double y, double r)
{
  gDrawData.SetCircle(x,y,r,gRed,gGreen,gBlue);
}


///
///@brief     円を描画する
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetCircle(x,y,r,red,green,blue);
}


///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillCircle(double x, double y, double r )
{
  gDrawData.SetFillCircle(x,y,r,gRed,gGreen,gBlue);
}

///
///@brief     円を描画する（内部塗りつぶし)
///@param[in] x,y            円の中心座標 [mm]
///@param[in] r              円の半径
///@param[in] red,green,blue 描画色(範囲 0 to 255)
///@return なし
///
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawFillCircle(double x, double y, double r, int red, int green, int blue)
{
  gDrawData.SetFillCircle(x,y,r,red,green,blue);
}

///
///@brief     文字列を描画する
///@param[in] x,y   描画先座標[mm]
///@param[in] size  文字サイズ(標準14)
///@param[in] str   描画文字列
///@return なし
///
///- printf()構文と同じように文字を描画できる。
///- 座標系に関係無く共通
///
void drawString(double x, double y, int size, const char *str, ... )
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
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,gRed,gGreen,gBlue,buf);
}

/*
void drawtestmode(rx,ry,bx,by);
*/

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
///- 座標系に関係無く共通
///- 描画色の設定は1回（呼び出した時）だけ有効
///
void drawString(double x, double y, int size,int red,int green,int blue, const char *str, ... )
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
    buf[len]                 = '\0';
  }

  gDrawData.SetString(x,y,size,red,green,blue,buf);
}



///
///@brief  ユーザー描画
///@return なし
///
void drawCommandProc()
{
  //ユーザー描画  
  int i=0;
  DRAW_OBJECT dobj;
  while(1)
  {
    if(!gDrawData.GetObject(i,dobj))break;
    newrgbcolor(mainwindow,dobj.red,dobj.green,dobj.blue);
    switch(dobj.type)
    {
    case POINT:  //点描画
      pset(mainwindow,(float)dobj.x1,(float)dobj.y1);
      break;
    case CROSS:  //×印描画
      line(mainwindow, (float)dobj.x1,(float)(dobj.y1-dobj.y2), PENUP);
      line(mainwindow, (float)dobj.x1,(float)(dobj.y1+dobj.y2), PENDOWN);
      line(mainwindow, (float)(dobj.x1-dobj.x2),(float)dobj.y1, PENUP);
      line(mainwindow, (float)(dobj.x1+dobj.x2),(float)dobj.y1, PENDOWN);
      break;
    case LINE:   //線描画
      line(mainwindow,(float)dobj.x1,(float)dobj.y1,PENUP);
      line(mainwindow,(float)dobj.x2,(float)dobj.y2,PENDOWN);
      break;
    case RECTANGLE:   //四角形
      drawrect(mainwindow,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case FILLRECTANGLE://四角形(塗りつぶし)
      fillrect(mainwindow,(float)dobj.x1,(float)dobj.y1,(float)dobj.x2,(float)dobj.y2);
      break;
    case CIRCLE:       //円描画
      circle(mainwindow,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r);
      break;
    case FILLCIRCLE:   //円描画(塗りつぶし)
      fillarc(mainwindow,(float)dobj.x1,(float)dobj.y1,(float)dobj.r,(float)dobj.r, 0.0f,360.0f,1);
      break;
    case STRING: //文字列描画
      drawstr(mainwindow,(float)dobj.x1,(float)dobj.y1,(int)dobj.r,0,dobj.str.c_str());
    }
    i++;
  }
  //クリア
  gDrawData.CleanData();
}

} //namespace odens


///@} doxygenのためのコメント（消してはいけない）


