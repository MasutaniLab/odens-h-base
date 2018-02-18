///
///@file robot.cpp
///@brief Robotクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup robot
///@{
///
#include "robot.h"
#include "util.h"

using namespace std;
using namespace boost::asio;

namespace odens {

///
///@brief ロボットへコマンドを送信するスレッドを開始する
///@param[in] port シリアルポートの名前
///@param[in] interval 送信間隔 [ms]
///@retval false 正常終了
///@retval true 異常終了
///
bool Robot::start(string port, int interval)
{
  initializePacket();
  m_interval = interval;
  m_packet = m_packetTable[0];
  try {
    //シリアルポートの初期設定
    m_serial.open(port);
    m_serial.set_option(serial_port_base::baud_rate(m_baud_rate));
    m_serial.set_option(serial_port_base::character_size(8));
    m_serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
    m_serial.set_option(serial_port_base::parity(m_parity));
    m_serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    //スレッド開始
    boost::thread thread(&Robot::main, this);
    m_thread.swap(thread);
  } catch(exception &e) {
    cerr << "Robot::start() 例外: " << e.what() << endl;
    return true;
  }
  return false;
}

///
///@brief ロボットへパケットを繰り返し送信する（別スレッドで実行）
///@return なし
///
///- Robot::start()の中でこの関数を別スレッドで起動する．
///- この関数で例外が発生した場合にプログラムを終了してしまっていいのか？
///
void Robot::main()
{
  cout << "Robot::main() 開始" << endl;
  try {
    m_loop = true;
    while (m_loop) { 
      {
        boost::mutex::scoped_lock lock(m_mutex);
        m_serial.write_some(buffer(m_packet.b));
      }
      msleep(m_interval);
    }
  } catch(exception &e) {
    cerr << "Robot::main() 例外: " << e.what() << endl;
    exit(1);
  }
}

///
///@brief ロボットへ送るパケットを非同期に設定する
///@param[in] com コマンド
///@return なし
///
///- m_mutex によって排他制御している．
///
void Robot::setCommand(const RobotCommand &com)
{
  if (com < 0 || m_packetTable.size() <= com) {
    cerr << "com = " << com << "は範囲外" << endl;
    return;
  }
  //m_packetTable[com].print();
  {
    boost::mutex::scoped_lock lock(m_mutex);
    m_packet = m_packetTable[com];
  }
  m_com = com;
}

///
///@brief コマンドの意味する文字列を返す
///@param[in] com コマンド
///@return コマンドに対応する文字列
///
string Robot::getCommandString(const RobotCommand &com)
{
  if (com < 0 || m_packetTable.size() <= com) {
    cerr << "com = " << com << "は範囲外" << endl;
    return "";
  }
  return m_commandStringTable[com];
}

///
///@brief 送信したコマンドの意味する文字列を返す
///@return コマンドに対応する文字列
///
string Robot::getCommandString()
{
  return m_commandStringTable[m_com];
}

///
///@brief コマンドの数を返す
///@return コマンドの数
///
size_t Robot::size()
{
  return m_packetTable.size();
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
