///
///@file cdrawdata.h
///@brief CDrawDataクラスの宣言
///@par Copyright
/// Copyright (C) 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/02/01 升谷 保博
///@addtogroup draw
///@brief 描画のための内部クラス
///@{
///

#pragma once
 
#include <string>
#include <vector>
#include "sr.h"

namespace odens {

///
///@brief ユーザ描画用クラス
///
class CDrawData
{
  ///
  ///@brief 描画オブジェクトの種類
  ///
  enum Type { POINT, CROSS, LINE, RECTANGLE, FILLRECTANGLE, CIRCLE, FILLCIRCLE, STRING };

  ///
  ///@brief ユーザ描画用構造体
  ///
  struct DRAW_OBJECT
  {
    Type type;             ///<種類
    double x1;            ///<x座標1
    double y1;            ///<y座標1
    double x2;            ///<x座標2
    double y2;            ///<y座標2
    double r;             ///<半径
    unsigned char red;    ///<色の赤成分
    unsigned char green;  ///<色の緑成分
    unsigned char blue;   ///<色の青成分
    std::string str;           ///<文字列
  };

public:
  CDrawData();
  void setPoint(double x, double y, unsigned rd, unsigned gr, unsigned bl);
  void setCross(double x, double y, double size, unsigned rd, unsigned gr, unsigned bl);
  void setLine(double x1, double y1, double x2, double y2, unsigned rd, unsigned gr, unsigned bl);
  void setRectangle(double x, double y, double cx, double cy, unsigned rd, unsigned gr, unsigned bl);
  void setFillRectangle(double x, double y, double cx, double cy, unsigned rd, unsigned gr, unsigned bl);
  void setCircle(double x, double y, double r, unsigned rd, unsigned gr, unsigned bl);
  void setFillCircle(double x, double y, double r, unsigned rd, unsigned gr, unsigned bl);
  void setString(double x, double y, int size, unsigned rd, unsigned gr, unsigned bl, char *str);
  void clear();
  void draw(int mainwindow);
private:
  std::vector<DRAW_OBJECT> m_object; ///<描画オブジェクトの並び
  int m_num; ///<オブジェクトの数
  const static int MAX_DRAW_OBJECT = 256; ///<オブジェクトの最大数
  bool getObject(int id, DRAW_OBJECT &data);
};


} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
