///
///@file vision.cpp
///@brief Visionクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup vision
///@{
///

#include <iostream>
#include "vision.h"
#include <boost/asio.hpp>
#include "messages_robocup_ssl_detection.pb.h"
#include "messages_robocup_ssl_geometry.pb.h"
#include "messages_robocup_ssl_wrapper.pb.h"
#include "util.h"

using namespace std;
using boost::asio::ip::udp;

namespace odens {

///
///@brief 初期化の後にSSL-Visionサーバからの情報を受信するスレッドを開始する
///@param[in] address マルチキャストアドレス
///@param[in] port マルチキャストのポート番号
///@retval false 正常終了
///@retval true 異常終了
///
bool Vision::start(string address, int port)
{
  //cout << "Vision::start() 開始" << endl;
  try
  {
    //マルチキャスト通信の初期化
    boost::asio::ip::address listen_address 
      = boost::asio::ip::address::from_string("0.0.0.0");
    boost::asio::ip::address multicast_address 
      = boost::asio::ip::address::from_string(address);
    udp::endpoint listen_endpoint(listen_address, port);
    m_socket.open(listen_endpoint.protocol());
    m_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    m_socket.bind(listen_endpoint);
    m_socket.set_option(boost::asio::ip::multicast::join_group(multicast_address));
    //スレッド開始
    boost::thread thread(&Vision::main, this);
    m_thread.swap(thread);
  } catch (exception& e) {
    cerr << "Vision::start() 例外: " << e.what() << endl;
    return true;
  }
  return false;
}

///
///@brief SSL-Visionサーバからの情報を繰り返し受信する（別スレッドで実行）
///@return なし
///
///- Vision::start()の中でこの関数を別スレッドで起動する．
///- この関数で例外が発生した場合にプログラムを終了してしまっていいのか？
///
void Vision::main()
{
  cout << "Vision::main() 開始" << endl;
  try {
    m_loop = true;
    while (m_loop) { 
      //パケット受信
      char buffer[65536];
      udp::endpoint sender_endpoint;
      size_t length = m_socket.receive_from(boost::asio::buffer(buffer, sizeof(buffer)), sender_endpoint);

      //パース
      SSL_WrapperPacket packet;
      if (!packet.ParseFromArray(buffer, int(length))) {
        cerr << "Vision::main() パース失敗";
        continue;
      }

      if (!packet.has_detection()) {
        //視覚情報を含んでいない場合はループの最初へ
        continue;
      }
      VisionInfo info;
      SSL_DetectionFrame detection = packet.detection();
      info.frameNumber = detection.frame_number();
      //cout << getTime() << " " << info.frameNumber << endl;
      info.cameraId = detection.camera_id();
      info.nBall = detection.balls_size();
      info.nRobot[BLUE] =  detection.robots_blue_size();
      info.nRobot[YELLOW] =  detection.robots_yellow_size();
      for (int i=0; i<info.nBall; i++) {
        SSL_DetectionBall ball = detection.balls(i);
        info.ball[i].x = ball.x();
        info.ball[i].y = ball.y();
        info.ball[i].theta = 0;
      }
      for (int i=0; i<info.nRobot[BLUE] ; i++) {
        SSL_DetectionRobot robot = detection.robots_blue(i);
        info.robot[BLUE][i].x = robot.x();
        info.robot[BLUE][i].y = robot.y();
        info.robot[BLUE][i].theta = robot.orientation();
        if ( robot.has_robot_id() ) {
          info.number[BLUE][i] = robot.robot_id();
        } else {
          info.number[BLUE][i] = INVISIBLE;
        }
      }
      for (int i=0; i<info.nRobot[YELLOW]; i++) {
        SSL_DetectionRobot robot = detection.robots_yellow(i);
        info.robot[YELLOW][i].x = robot.x();
        info.robot[YELLOW][i].y = robot.y();
        info.robot[YELLOW][i].theta = robot.orientation();
        if ( robot.has_robot_id() ) {
          info.number[YELLOW][i] = robot.robot_id();
        } else {
          info.number[YELLOW][i] = INVISIBLE;
        }
      }
      {
        boost::mutex::scoped_lock lock(m_mutex);
        m_visionInfo = info;
        m_active = true;
        m_condition.notify_all();
      }
    }
    m_socket.close();
  } catch (exception& e) {
    cerr << "Vision::main() 例外: " << e.what() << endl;
    exit(1);
  }
}

///
///@brief SSL-Visionサーバからの情報を同期的に得る
///@param[in] info 位置情報
///@retval 0 正常終了
///@retval -1 タイムアウト
///@retval 2以上 受信の抜け（飛び）がある
///
///- m_mutex によって排他制御し，m_condition で同期を取っている．
///- 別スレッドで新たな情報を受け取るか，タイムアウトになるまでこの関数は終わらない．
///- タイムアウトの設定は1秒で固定でいいのか？
///
int Vision::get(VisionInfo &info)
{
  static int prevFrameNumber = 0;
  boost::mutex::scoped_lock lock(m_mutex);
  if (m_condition.timed_wait(lock,boost::posix_time::milliseconds(1000))) {
    info = m_visionInfo;
    int d = info.frameNumber - prevFrameNumber;
    prevFrameNumber = info.frameNumber;
    if (d == 1) {
      return 0;
    } else {
      return d; //前回とのフレーム番号の差が2以上
    }
  } else {
    return -1; //タイムアウト
  }
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
