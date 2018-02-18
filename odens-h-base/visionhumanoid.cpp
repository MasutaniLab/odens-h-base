///
///@file visionhumanoid.cpp
///@brief VisionHumanoidクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup visionhumanoid
///@{
///

#include "visionhumanoid.h"

namespace odens {

const double VisionHumanoid::xOrgTable[4] 
  = {FIELD_LENGTH2,-FIELD_LENGTH2,-FIELD_LENGTH2,FIELD_LENGTH2};
const double VisionHumanoid::yOrgTable[4]
  = {FIELD_WIDTH2,FIELD_WIDTH2,-FIELD_WIDTH2,-FIELD_WIDTH2};

///
///@brief VisionクラスがSSL-Visionから得た情報をSSL Humanoidの座標系に変換し，
/// マーカ番号をロボット番号に変換したものを同期的に得る．
///@param[out] sinfo ボールを1個にして，ロボット番号に変換した情報
///@param[out] vinfo 座標変換だけをした情報
///@retval 0 正常終了
///@retval -1 タイムアウト
///@retval 2以上 受信の抜け（飛び）がある
///
///- m_visionが新たな情報を受け取るか，タイムアウトになるまでこの関数は終わらない．
///
int VisionHumanoid::get(srInfo &sinfo, VisionInfo &vinfo)
{
  VisionInfo oinfo;
  int r = m_vision.get(oinfo);
  if (r < 0) {
    return r;
  }
  //座標変換
  vinfo = oinfo;
  for (int i=0; i<oinfo.nBall; i++) {
    convert(vinfo.ball[i], oinfo.ball[i], true);      
  }
  for (int c=BLUE; c<=YELLOW; c++) {
    for (int i=0; i<oinfo.nRobot[c]; i++) {
      convert(vinfo.robot[c][i], oinfo.robot[c][i], false);
    }
  }

  //ボールを一つ選ぶ
  sinfo.ball = vinfo.ball[0];

  //マーカからロボットへの変換
  for (int c=BLUE; c<=YELLOW; c++) {
    const Orthogonal invisible(INVISIBLE, INVISIBLE, INVISIBLE);
    int count = 0;
    bool check[MAX_MARKER_NUM];
    for (int j=0; j<vinfo.nRobot[c]; j++ ) {
      check[j] = false;
    }
    //対応表に基づく割り当て
    for (int i=1; i<=MAX_ROBOT_NUM; i++) {
      sinfo.robot[c][i]=invisible;
      sinfo.id[c][i]=false;
      for (int j=0; j<vinfo.nRobot[c]; j++ ) {
        if (vinfo.number[c][j] == m_markerTable[c][i]) {
          sinfo.robot[c][i]=vinfo.robot[c][j];
          sinfo.id[c][i]=true;
          check[j] = true;
          count++;
          break;
        }
      }
    }
    //対応表で全てが決まらない場合は，見えているものがあれば空いている号機へ割り当て
    if (count < MAX_ROBOT_NUM) {
      for (int i=1; i<=MAX_ROBOT_NUM; i++) {
        if (sinfo.id[c][i] == false) {
          for (int j=0; j<vinfo.nRobot[c]; j++ ) {
            if (check[j] == false) {
              sinfo.robot[c][i]=vinfo.robot[c][j];
              check[j] = true;
              break;
            }
          }
        }
      }
    }
  }
  return r;
}

///
///@brief SSL-Visionの座標系からSSL HUmanoidの座標系へ変換する
///@param[out] p SSL Humanoidの座標系の位置
///@param[in] v SSL-Visionの座標系の位置
///@param[in] notheta 方向の変更をしないか？
///@return なし
///
void VisionHumanoid::convert(Orthogonal &p, const Orthogonal &v, bool notheta)
{
  p.x = m_sign*(v.x-m_xOrg);
  p.y = m_sign*(v.y-m_yOrg);
  if ( notheta ) {
    p.theta = 0;
  } else if (m_sign == 1) {
    p.theta = v.theta;
  } else {
    if ( v.theta > 0 ) {
      p.theta = v.theta-M_PI;
    } else {
      p.theta = v.theta+M_PI;
    }
  }
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
