///
///@file vision.h
///@brief Visionクラスの宣言
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup vision Vision
///@brief SSL-Visionサーバから位置情報を受信する
///@{
///

#pragma once
#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/asio.hpp>
#include "sr.h"

namespace odens {

///
///@brief SSL-Visionサーバから位置情報を受信するクラス
///
class Vision {
private:
  boost::thread m_thread;                 ///<スレッド
  boost::mutex m_mutex;                   ///<ミューテックス（排他制御に利用）
  boost::condition_variable m_condition;  ///<条件変数（同期に利用）
  bool m_loop;                            ///<別スレッドの繰り返しのフラグ
  boost::asio::io_service m_io;           ///<ASIOのIOサービス
  boost::asio::ip::udp::socket m_socket;  ///<通信のためのソケット
  VisionInfo m_visionInfo;                ///<得られた位置情報（排他制御の対象）
  bool m_active;                          ///<通信の状態を表すフラグ

  void main();

public:
  ///コンストラクタ
  Vision()
    :m_io(),
    m_socket(m_io)
  {
    std::cout << "Visionコンストラクタ" << std::endl;
  }
  ///デストラクタ
  ~Vision()
  {
    std::cout << "Visionデストラクタ" << std::endl;
    if (m_thread.joinable()) {
      m_loop = false;
      m_thread.join();
    }
  }
  bool start(std::string address, int port);
  int get(VisionInfo &info);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
