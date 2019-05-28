///
///@file config.h
///@brief Configクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup config Config
///@brief コマンドラインと設定ファイルによって変数を設定するクラス
///@{
///

#pragma once
#include <string>
#include "sr.h"

namespace odens {

///
///@brief コマンドラインと設定ファイルから設定を読み込み静的に保持するクラス
///
class Config {
public:
  static std::string RobotType; ///<ロボットの種類
  static int MyNumber; ///<ロボット番号 
  static int MyColor; ///<自分のカラー
  static bool Pause; ///<一時停止で開始
  static bool Goalie; ///<ゴールキーパーか？
  static std::string RobotPortName; ///<シリアルポートの名前
  static std::string VisionAddress; ///<ビジョンのマルチキャストアドレス
  static int VisionPortNumber; ///<ビジョンのポート番号
  static bool Referee; ///<レフェリーを使う
  static std::string RefereeAddress; ///<レフェリーのマルチキャストアドレス
  static int RefereePortNumber; ///<レフェリーのポート番号
  static int Quadrant; ///<SSL-Visionの象限(0..3: 第1..4象限）
  static bool AttackRight; ///<SSL-Visionの右側へ攻める
  static int OurMarkerTable[MAX_ROBOT_NUM+1]; ///<自チームロボットのマーカ番号
  static int TheirMarkerTable[MAX_ROBOT_NUM+1]; ///<相手チームロボットのマーカ番号
  static bool Logger; ///<ログを取るか？
  static bool setup(int argc, char* argv[]);
  static void print();
};

} //namespace odens 

///@} doxygenのためのコメント（消してはいけない）
