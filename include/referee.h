///
///@file referee.h
///@brief Refereeクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup referee Referee
///@brief レフェリーボックスから信号を受信するクラス
///@{
///
#pragma once
#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "sr.h"

namespace odens {

  namespace ref {
    ///
    ///@brief レフェリーボックスのステージを表す列挙体
    ///
    enum Stage {
      NORMAL_FIRST_HALF_PRE = 0,
      NORMAL_FIRST_HALF = 1,
      NORMAL_HALF_TIME = 2,
      NORMAL_SECOND_HALF_PRE = 3,
      NORMAL_SECOND_HALF = 4,
      EXTRA_TIME_BREAK = 5,
      EXTRA_FIRST_HALF_PRE = 6,
      EXTRA_FIRST_HALF = 7,
      EXTRA_HALF_TIME = 8,
      EXTRA_SECOND_HALF_PRE = 9,
      EXTRA_SECOND_HALF = 10,
      PENALTY_SHOOTOUT_BREAK = 11,
      PENALTY_SHOOTOUT = 12,
      POST_GAME = 13,
      StageNOI
    };

    ///
    ///@brief レフェリーボックスのコマンドを表す列挙体
    ///
    enum Command {
      HALT = 0,
      STOP = 1,
      NORMAL_START = 2,
      FORCE_START = 3,
      PREPARE_KICKOFF_YELLOW = 4,
      PREPARE_KICKOFF_BLUE = 5,
      PREPARE_PENALTY_YELLOW = 6,
      PREPARE_PENALTY_BLUE = 7,
      DIRECT_FREE_YELLOW = 8,
      DIRECT_FREE_BLUE = 9,
      INDIRECT_FREE_YELLOW = 10,
      INDIRECT_FREE_BLUE = 11,
      TIMEOUT_YELLOW = 12,
      TIMEOUT_BLUE = 13,
      GOAL_YELLOW = 14,
      GOAL_BLUE = 15,
      CommandNOI
    };
  }

class Referee; //friendのために先に宣言しておく

///
///@brief レフェリーボックスの情報を保持する構造体
///
struct RefereeInfo {
  uint64_t packetTimestamp; ///<パケットのタイムスタンプ
  ref::Stage stage;                  ///<ステージ
  int32_t stageTimeLeft;    ///<残り時間
  ref::Command command;              ///<コマンド
  uint32_t commandCounter;  ///<コマンドのカウンタ
  uint32_t score[2];        ///<両チームの得点
  friend Referee;                           ///<@ref Referee クラス

  ///コマンドの文字列を返す
  std::string RefereeInfo::commandString()
  {
    return commandStringTable[command];
  }
  ///ステージの文字列を返す
  std::string RefereeInfo::stageString()
  {
    return stageStringTable[stage];
  }
private:
  static std::string commandStringTable[ref::CommandNOI]; ///<コマンドの文字列を保持する配列
  static std::string stageStringTable[ref::StageNOI];   ///<ステージの文字列を保持する配列
};

///
///@brief レフェリーボックスからの信号を受信するクラス
///
class Referee {
private:
  boost::thread m_thread;     ///<スレッド
  boost::mutex m_mutex;       ///<ミューテックス（排他制御に利用）
  bool m_loop;                ///<別スレッドの繰り返しのフラグ
  boost::asio::io_service m_io;            ///<ASIOのIOサービス
  boost::asio::ip::udp::socket m_socket;   ///<通信のためのソケット
  RefereeInfo m_refereeInfo;  ///<得られたレフェリーボックスの情報（排他制御の対象）
  bool m_active;              ///<通信の状態を表すフラグ

  void main();
public:
  ///コンストラクタ
  Referee()
    :m_io(),
    m_socket(m_io)
  {
   std::cout << "Referee コンストラクタ" << std::endl;
  }
  ///デストラクタ
  ~Referee()
  {
    std::cout << "Referee デストラクタ" << std::endl;
    if (m_thread.joinable()) {
      m_loop = false;
      m_thread.join();
    }
  }
  bool start(std::string address, int port);
  int get(RefereeInfo &info);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
