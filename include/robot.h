///
///@file robot.h
///@brief Robotクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup robot Robot
///@brief Robotへコマンドを送る基底クラス
///@{
///

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace odens {

typedef size_t RobotCommand; ///<ロボットへ送るコマンドを表す非負の値

///
///@brief ロボットへ送るバイト列
///
struct Packet {
  std::vector<uint8_t> b;     ///<バイト列

  ///データ列を16進で表示
  void print()
  {
    for (size_t i=0; i<b.size(); i++) {
      std::cout << std::hex << int(b[i]) <<",";
    }
    std::cout << std::endl;
  }
};

///
///@brief シリアル通信でロボットへパケットを送信するクラス
///
class Robot {
private:
  boost::thread m_thread; ///<スレッド
  boost::mutex m_mutex;   ///<ミューテックス
  bool m_loop;            ///<別スレッドの繰り返しのフラグ
  boost::asio::io_service m_io;        ///<ASIOのIOサービス
  boost::asio::serial_port m_serial;   ///<シリアルポート
  Packet m_packet;   ///<送信するパケット
  int m_interval;         ///<送信する間隔 [ms]
  RobotCommand m_com; ///<送信したコマンド

  ///
  ///@brief パケットのバイト列をあらかじめ作成する（ロボットごとに関数を定義する）
  ///
  virtual void initializePacket() = 0;
  void main();

protected:
  std::vector<Packet> m_packetTable;   ///<各コマンドのパケットを登録するベクター
  std::vector<std::string> m_commandStringTable; ///<各コマンドの文字列を登録するベクター
  int m_baud_rate; ///<通信速度 [bps]
  boost::asio::serial_port_base::parity::type m_parity; ///<パリティチェックの種類

public:
  ///コンストラクタ
  Robot()
    :m_io(),
    m_serial(m_io)
  {
    std::cout << "Robot コンストラクタ" << std::endl;
  }
  ///デストラクタ
  ~Robot()
  {
    std::cout << "Robot デストラクタ" << std::endl;
    if (m_thread.joinable()) {
      m_loop = false;
      m_thread.join();
    }
  }
  bool start(std::string port, int interval);
  ///
  ///@brief 送信したコマンドを返す
  ///
  RobotCommand getCommand()
  {
    return m_com;
  }
  void setCommand(const RobotCommand &com);
  std::string getCommandString(const RobotCommand &com);
  std::string getCommandString();
  size_t size();
};

}//namespace odens

///@} doxygenのためのコメント（消してはいけない）

