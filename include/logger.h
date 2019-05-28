///
///@file logger.h
///@brief Loggerクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup logger Logger
///@brief ログファイル出力のためのクラス
///@{
///
#include <iostream>
#include <fstream>
#include "sr.h"
#include "estimator.h"
#include "referee.h"
#include "game.h"
#include "robot.h"

namespace odens {

///
///@brief ログファイルを出力するクラス
///
class Logger {
private:
  std::ofstream m_fout; ///<出力するファイルのストリーム
public:
  bool open(int color, int number);
  void write(double ctime, const srInfo &sinfo, const Timed2D &ballVel, 
    const RefereeInfo &rinfo, const GameMode &mode, const RobotCommand &com);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
