///
///@file visionhumanoid.h
///@brief VisionHumanoidクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup visionhumanoid VisionHumanoid
///@brief SSL-Visionサーバから位置情報受信しSSL Humanoid用に変換する
///@{
///

#pragma once
#include "sr.h"
#include "vision.h"

namespace odens {

    ///
///@brief SSL-Visionサーバから位置情報受信しSSL Humanoid用に変換するクラス
///
class VisionHumanoid {
private:
  Vision m_vision;    ///<実際にデータを受信するVisionクラスのオブジェクト
  int m_quadrant;     ///<SSL-Visionのどの象限を使うか（第1～4象限が0～3に対応）
  double m_xOrg;      ///<SSL Humanoidの座標系の原点のx座標
  double m_yOrg;      ///<SSL Humanoidの座標系の原点のy座標
  bool m_attackRight; ///<自チームが右側（SSL-Vision座標系のx正方向）へ攻めるか？
  int m_sign;         ///<攻める方向によって決まる符号（1または-1）
  int m_markerTable[2][MAX_ROBOT_NUM+1];   ///<両チームの各ロボットのマーカ番号を保持する表（0は不使用）
  const static double xOrgTable[4];   ///<各象限の座標系の原点のx座標を保持する配列
  const static double yOrgTable[4];   ///<各象限の座標系の原点のy座標を保持する配列
public:
  ///コンストラクタ
  VisionHumanoid()
  {
    m_quadrant = 0;
    m_xOrg = xOrgTable[m_quadrant];
    m_yOrg = yOrgTable[m_quadrant];
    m_attackRight = true;
    m_sign = 2*m_attackRight-1;
  }
  ///m_visionの開始
  bool start(std::string address, int port )
  {
    return m_vision.start(address, port);
  }
  ///象限の設定
  void setQuardrant(int q)
  {
    if (q<0 || q>3) {
      return;
    }
    m_quadrant = q;
    m_xOrg = xOrgTable[m_quadrant];
    m_yOrg = yOrgTable[m_quadrant];
  }
  ///攻める方向の設定
  void setAttackRight(bool a)
  {
    m_attackRight = a;
    m_sign = 2*m_attackRight-1;
  }
  ///マーカとロボットの対応表の設定
  void setMarkerTable(int bt[], int yt[])
  {
    for (int i=0; i<=MAX_ROBOT_NUM; i++) {
      m_markerTable[BLUE][i] = bt[i];
      m_markerTable[YELLOW][i] = yt[i];
    }
  }
  int get(srInfo &sinfo, VisionInfo &vinfo);
private:
  void convert(Orthogonal &p, const Orthogonal &v, bool notheta);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
