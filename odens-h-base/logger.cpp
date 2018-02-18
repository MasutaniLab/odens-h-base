///
///@file logger.cpp
///@brief ログファイル出力
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup logger
///@{
///
#include "logger.h"
#include <string>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost::posix_time;

namespace odens {

    ///
///@brief ログファイルの名前を決め，オープンする．
///@param[in] color 自チームの色（ファイル名に使う）
///@param[in] number 自機の番号（ファイル名に使う）
///@retval false 正常終了
///@retval false 異常終了
///
bool Logger::open(int color, int number)
{
  time_facet *f = new time_facet("%Y%m%d%H%M%S");
  ostringstream oss;
  oss.imbue(locale(oss.getloc(), f));
  oss << second_clock::local_time();
  oss << (color==BLUE?'b':'y') << number << ".txt";
  m_fout.open(oss.str());
  if (!m_fout) {
    cerr << "オープン失敗: " << oss.str() << endl;
    return true;
  }
  f = new time_facet("%H:%M:%S%F");
  m_fout.imbue(locale(m_fout.getloc(), f));
  return false;
}

///
///@brief ログファイルに1行出力する
///@param[in] ctime 時刻
///@param[in] sinfo フィールドの位置情報
///@param[in] ballVel ボールの速度
///@param[in] rinfo レフェリー情報
///@param[in] mode 試合のモード
///@param[in] com ロボットへのコマンド
///@return なし
///
void Logger::write(double ctime, const srInfo &sinfo, const Timed2D &ballVel, 
                   const RefereeInfo &rinfo, const GameMode &mode, const RobotCommand &com)
{
  if (!m_fout) return;

  ptime now = microsec_clock::local_time();

  m_fout << now << " " << ctime << " " << sinfo.ball << " ";
  for (int c=BLUE; c<=YELLOW; c++) {
    for (int i=1; i<=MAX_ROBOT_NUM; i++) {
      m_fout << sinfo.robot[c][i] << " ";
    }
  }
  m_fout << ballVel.x << " " << ballVel.y << " " 
    << rinfo.command << " "
    << mode.play << " " << mode.kick << " "
    << com << endl; 
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
