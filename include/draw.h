///
///@file draw.h
///@brief 描画のための関数の宣言を集めたもの
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2007/09/27 片岡 賢太郎-
///- 2007/03/25 升谷 保博，片岡 賢太郎
///- 2006/04/18 升谷 保博，片岡 賢太郎
///@addtogroup draw Draw
///@brief 描画
///@{
///

#pragma once
 
#include "sr.h"

namespace odens {

int  drawInitialize(int color, int number);
void draw(const srInfo &si);
void draw(const srInfo &si, const srInfo &si2);
void draw(const VisionInfo &vi);
void drawTerminate();
void drawShowPositionData();
void drawReverseField();
void drawSetShowEnable();
void drawSetColor(int red,int green,int blue);
void drawGetColor(int &red,int &green,int &blue);
void drawPoint( double x, double y );
void drawPoint( double x, double y, int red, int green, int blue);
void drawCross( double x, double y, double size = 30);
void drawCross( double x, double y, int red, int green, int blue, double size = 30);
void drawLine(  double x1,double y1,double x2,double y2 );
void drawLine(  double x1,double y1,double x2,double y2, int red, int green, int blue);
void drawRectangle(double x, double y, double cx, double cy);
void drawRectangle(double x, double y, double cx, double cy, int red, int green, int blue);
void drawFillRectangle(double x, double y, double cx, double cy);
void drawFillRectangle(double x, double y, double cx, double cy, int red, int green, int blue);
void drawCircle(double x, double y, double r );
void drawCircle(double x, double y, double r, int red, int green, int blue);
void drawFillCircle(double x, double y, double r );
void drawFillCircle(double x, double y, double r, int red, int green, int blue);
void drawString(double x, double y, int size, const char *str, ... );
void drawString(double x, double y, int size,int red,int green,int blue,const char *str, ... );

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
