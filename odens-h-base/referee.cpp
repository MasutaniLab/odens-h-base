///
///@file referee.cpp
///@brief Refereeクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup referee
///@{
///
#include "referee.h"
#include "referee.pb.h"

using namespace std;
using boost::asio::ip::udp;

namespace odens {

//RefereeInfoの静的メンバ変数
string RefereeInfo::commandStringTable[ref::CommandNOI];
string RefereeInfo::stageStringTable[ref::StageNOI];

///SSL_Referee_Command列挙型の項目に対応する文字列を設定するマクロ
#define SET_COMMAND_STRING_TABLE(x) RefereeInfo::commandStringTable[ref::x] = #x

///SSL_Referee_Stage列挙型の項目に対応する文字列を設定するマクロ
#define SET_STAGE_STRING_TABLE(x) RefereeInfo::stageStringTable[ref::x] = #x

///
///@brief 初期化の後にレフェリーボックスからの信号を受信するスレッドを開始する
///@param[in] address マルチキャストアドレス
///@param[in] port マルチキャストのポート番号
///@retval false 正常終了
///@retval true 異常終了
///
bool Referee::start(string address, int port)
{
  //cout << "Referee::start() 開始" << endl;

  m_refereeInfo.packetTimestamp = 0;
  m_refereeInfo.stage = ref::NORMAL_FIRST_HALF_PRE;
  m_refereeInfo.stageTimeLeft = 0;
  m_refereeInfo.command = ref::HALT;
  m_refereeInfo.commandCounter = 0;
  m_refereeInfo.score[BLUE] = 0;
  m_refereeInfo.score[YELLOW] = 0;
  m_active = false;

  SET_COMMAND_STRING_TABLE(HALT);
  SET_COMMAND_STRING_TABLE(STOP);
  SET_COMMAND_STRING_TABLE(NORMAL_START);
  SET_COMMAND_STRING_TABLE(FORCE_START);
  SET_COMMAND_STRING_TABLE(PREPARE_KICKOFF_YELLOW);
  SET_COMMAND_STRING_TABLE(PREPARE_KICKOFF_BLUE);
  SET_COMMAND_STRING_TABLE(PREPARE_PENALTY_YELLOW);
  SET_COMMAND_STRING_TABLE(PREPARE_PENALTY_BLUE);
  SET_COMMAND_STRING_TABLE(DIRECT_FREE_YELLOW);
  SET_COMMAND_STRING_TABLE(DIRECT_FREE_BLUE);
  SET_COMMAND_STRING_TABLE(INDIRECT_FREE_YELLOW);
  SET_COMMAND_STRING_TABLE(INDIRECT_FREE_BLUE);
  SET_COMMAND_STRING_TABLE(TIMEOUT_YELLOW);
  SET_COMMAND_STRING_TABLE(TIMEOUT_BLUE);
  SET_COMMAND_STRING_TABLE(GOAL_YELLOW);
  SET_COMMAND_STRING_TABLE(GOAL_BLUE);

  SET_STAGE_STRING_TABLE(NORMAL_FIRST_HALF_PRE);
  SET_STAGE_STRING_TABLE(NORMAL_FIRST_HALF);
  SET_STAGE_STRING_TABLE(NORMAL_HALF_TIME);
  SET_STAGE_STRING_TABLE(NORMAL_SECOND_HALF_PRE);
  SET_STAGE_STRING_TABLE(NORMAL_SECOND_HALF);
  SET_STAGE_STRING_TABLE(EXTRA_TIME_BREAK);
  SET_STAGE_STRING_TABLE(EXTRA_FIRST_HALF_PRE);
  SET_STAGE_STRING_TABLE(EXTRA_FIRST_HALF);
  SET_STAGE_STRING_TABLE(EXTRA_HALF_TIME);
  SET_STAGE_STRING_TABLE(EXTRA_SECOND_HALF_PRE);
  SET_STAGE_STRING_TABLE(EXTRA_SECOND_HALF);
  SET_STAGE_STRING_TABLE(PENALTY_SHOOTOUT_BREAK);
  SET_STAGE_STRING_TABLE(PENALTY_SHOOTOUT);
  SET_STAGE_STRING_TABLE(POST_GAME);

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
    boost::thread thread(&Referee::main, this);
    m_thread.swap(thread);
  } catch (exception& e) {
    cerr << "Referee::start() 例外: " << e.what() << endl;
    return true;
  }
  return false;
}

///
///@brief レフェリーボックスからの信号を繰り返し受信する（別スレッドで実行）
///@return なし
///
///- Referee::start()の中でこの関数を別スレッドで起動する．
///- この関数で例外が発生した場合にプログラムを終了してしまっていいのか？
///
void Referee::main()
{
  cout << "Referee::main() 開始" << endl;
  try {
    m_loop = true;
    while (m_loop) { 
      //パケット受信
      char buffer[65536];
      udp::endpoint sender_endpoint;
      size_t length 
        = m_socket.receive_from(boost::asio::buffer(buffer, sizeof(buffer)), sender_endpoint);

      //パース
      SSL_Referee referee;
      if (!referee.ParseFromArray(buffer, int(length))) {
        cerr << "Referee::main() パース失敗";
        continue;
      }
      {
        boost::mutex::scoped_lock lock(m_mutex);
        m_refereeInfo.packetTimestamp = referee.packet_timestamp();
        m_refereeInfo.stage = static_cast<ref::Stage>(referee.stage());
        m_refereeInfo.stageTimeLeft = referee.stage_time_left();
        m_refereeInfo.command = static_cast<ref::Command>(referee.command());
        m_refereeInfo.commandCounter = referee.command_counter();
        m_refereeInfo.score[BLUE] = referee.blue().score();
        m_refereeInfo.score[YELLOW] = referee.yellow().score();
        m_active = true;
      }
    }
  } catch (exception& e) {
    cerr << "Referee::main() 例外: " << e.what() << endl;
    exit(1);
  }
}

///
///@brief レフェリーボックスからの情報を非同期に得る
///@param[out] info レフェリーボックスの情報
///@retval 0 正常終了
///@retval 1 未受信
///@retval 2 途絶（前回とタイムスタンプの変化がない）
///
///- m_mutex によって排他制御している．
///
int Referee::get(RefereeInfo &info)
{
  static google::protobuf::uint64 prevTimestamp = 0;
  bool active;
  {
    boost::mutex::scoped_lock lock(m_mutex);
    active = m_active;
    info = m_refereeInfo;
  }
  if (active == false) {
    return 1;
  }
  if (info.packetTimestamp == prevTimestamp) {
    return 2;
  }
  prevTimestamp = info.packetTimestamp;
  return 0;
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
