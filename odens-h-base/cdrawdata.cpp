///
///@file cdrawdata.cpp
///@brief CDrawDataクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/02/01 升谷 保博
///@addtogroup draw
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
#include <boost/thread.hpp>
#include "draw.h"
#include "util.h"
#include "cdrawdata.h"

using namespace std;

namespace odens {

///
///@brief     コンストラクタ
///
CDrawData::CDrawData()
{
  m_num = 0;
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
void CDrawData::setPoint(double x, double y, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setCross(double x, double y,double size, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setLine(double x1,double y1,double x2,double y2, unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setRectangle(    double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setFillRectangle(double x,double y,double cx,double cy,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setFillCircle(double x, double y, double r,unsigned rd,unsigned gr,unsigned bl)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
void CDrawData::setString(double x, double y, int size, unsigned rd,unsigned gr,unsigned bl,char *str)
{
  if(m_num>= MAX_DRAW_OBJECT)return;
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
bool CDrawData::getObject(int id, DRAW_OBJECT &data)
{
  int num = (int)m_object.size();
  if(num<=id||id<0)return false;
  data = m_object.at(id);
  return true;
}

///
///@brief  クリアする
///@return なし
///
void CDrawData::clear()
{
  m_object.clear();
  m_num = 0;
}

///
///@brief  描画
///@return なし
///
void CDrawData::draw(int mainwindow)
{
  //ユーザー描画  
  int i = 0;
  DRAW_OBJECT dobj;
  while (true)
  {
    if (!getObject(i, dobj))break;
    ::newrgbcolor(mainwindow, dobj.red, dobj.green, dobj.blue);
    switch (dobj.type)
    {
    case POINT:  //点描画
      ::pset(mainwindow, (float)dobj.x1, (float)dobj.y1);
      break;
    case CROSS:  //×印描画
      ::line(mainwindow, (float)dobj.x1, (float)(dobj.y1 - dobj.y2), PENUP);
      ::line(mainwindow, (float)dobj.x1, (float)(dobj.y1 + dobj.y2), PENDOWN);
      ::line(mainwindow, (float)(dobj.x1 - dobj.x2), (float)dobj.y1, PENUP);
      ::line(mainwindow, (float)(dobj.x1 + dobj.x2), (float)dobj.y1, PENDOWN);
      break;
    case LINE:   //線描画
      ::line(mainwindow, (float)dobj.x1, (float)dobj.y1, PENUP);
      ::line(mainwindow, (float)dobj.x2, (float)dobj.y2, PENDOWN);
      break;
    case RECTANGLE:   //四角形
      ::drawrect(mainwindow, (float)dobj.x1, (float)dobj.y1, (float)dobj.x2, (float)dobj.y2);
      break;
    case FILLRECTANGLE://四角形(塗りつぶし)
      ::fillrect(mainwindow, (float)dobj.x1, (float)dobj.y1, (float)dobj.x2, (float)dobj.y2);
      break;
    case CIRCLE:       //円描画
      ::circle(mainwindow, (float)dobj.x1, (float)dobj.y1, (float)dobj.r, (float)dobj.r);
      break;
    case FILLCIRCLE:   //円描画(塗りつぶし)
      ::fillarc(mainwindow, (float)dobj.x1, (float)dobj.y1, (float)dobj.r, (float)dobj.r, 0.0f, 360.0f, 1);
      break;
    case STRING: //文字列描画
      ::drawstr(mainwindow, (float)dobj.x1, (float)dobj.y1, (int)dobj.r, 0, dobj.str.c_str());
    }
    i++;
  }
  //クリア
  clear();
}

} //namespace odens


///@} doxygenのためのコメント（消してはいけない）
