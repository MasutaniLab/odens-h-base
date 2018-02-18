///
///@file robotismini.h
///@brief ROBOTIS MINI用RobotクラスとTaskクラスの宣言（公開用）
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/05 升谷 保博
///@addtogroup robotismini ROBOTIS MINI
///@brief ROBOTIS MINIのためのクラス
///@{
///

#pragma once
#include <iostream>
#include <string>
#include <cstdint>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "robot.h"
#include "task.h"
#include "estimator.h"

namespace odens {
  namespace robotismini {
    ///
    ///@brief ROBOTIS MINIへ送るコマンド（モーション）
    ///
    enum Command {
      CommandNone = 0,       ///<なし（何もボタンを押していない）
      Forward,              ///<前進
      ForwardSmall,         ///<前進小
      Backward,             ///<後進
      BackwardSmall,        ///<後進小
      StepRight,            ///<右横移動
      StepRightSmall,       ///<右横移動小
      TurnRight,            ///<右旋回
      TurnRightSmall,       ///<右旋回小
      StepLeft,             ///<左横移動
      StepLeftSmall,        ///<左横移動小
      TurnLeft,             ///<左旋回
      TurnLeftSmall,        ///<左旋回小
      KickRight,            ///<右キック
      KickLeft,             ///<左キック
      KeeperRight,          ///<開脚右
      KeeperCenter,         ///<開脚中央
      KeeperLeft,           ///<開脚左
      StandUp,              ///<起き上がり
      TorqueOn,             ///<トルクオン
      TorqueOff,            ///<トルクオフ
      CommandNOI            ///<項目数
    };

    //ボタン
    const uint16_t NONE = 0x0000; ///<何も押さない
    const uint16_t BU = 0X0001; ///<Uボタン
    const uint16_t BD = 0X0002; ///<Dボタン
    const uint16_t BL = 0X0004; ///<Lボタン
    const uint16_t BR = 0X0008; ///<Rボタン
    const uint16_t B1 = 0X0010; ///<1ボタン
    const uint16_t B2 = 0X0020; ///<2ボタン
    const uint16_t B3 = 0X0040; ///<3ボタン
    const uint16_t B4 = 0X0080; ///<4ボタン
    const uint16_t B5 = 0X0100; ///<5ボタン
    const uint16_t B6 = 0X0200; ///<6ボタン

    ///RobotCommand列挙型の項目に対応する文字列を設定するマクロ
#define SET_COMMAND_STRING_TABLE(x) m_commandStringTable[x] = #x

///
///@brief シリアル通信でROBOTIS MINIへコマンドを送信するクラス
///
    class ROBOTISMINI : public Robot {
    private:
      ///
      ///@brief ROBOTIS MINIへ送るパケットを作る
      ///@param[in] command  ボタンの状態を表す2byte
      ///@return パケットのバイト列
      ///
      Packet makePacket(uint16_t command = 0)
      {
        Packet p;
        p.b.resize(6);
        p.b[0] = 0xff;
        p.b[1] = 0x55;
        uint8_t low = command & 0xff;
        uint8_t high = (command >> 8) & 0xff;
        p.b[2] = low;
        p.b[3] = ~low;
        p.b[4] = high;
        p.b[5] = ~high;
        return p;
      }
      //
      ///@brief ROBOTIS MINIへ送るパケットを設定する
      ///@return なし
      ///
      void initializePacket()
      {
        std::cout << "ROBOTISMINI::initializePacket()" << std::endl;

        m_packetTable.resize(CommandNOI);

        m_packetTable[CommandNone] = makePacket();
        m_packetTable[Forward] = makePacket(BU);
        m_packetTable[ForwardSmall] = makePacket(BU | B6);
        m_packetTable[Backward] = makePacket(BD);
        m_packetTable[BackwardSmall] = makePacket(BD | B6);
        m_packetTable[StepRight] = makePacket(BR | B5);
        m_packetTable[StepRightSmall] = makePacket(BR | B6);
        m_packetTable[TurnRight] = makePacket(BR);
        m_packetTable[TurnRightSmall] = makePacket(BR | B1);
        m_packetTable[StepLeft] = makePacket(BL | B5);
        m_packetTable[StepLeftSmall] = makePacket(BL | B6);
        m_packetTable[TurnLeft] = makePacket(BL);
        m_packetTable[TurnLeftSmall] = makePacket(BL | B1);
        m_packetTable[KickRight] = makePacket(BU | B4);
        m_packetTable[KickLeft] = makePacket(BU | B2);
        m_packetTable[KeeperRight] = makePacket(BR | B1 | B5);
        m_packetTable[KeeperCenter] = makePacket(BL | B1 | B5 | B6);
        m_packetTable[KeeperLeft] = makePacket(BL | B1 | B5);
        m_packetTable[StandUp] = makePacket(BU | B1);
        m_packetTable[TorqueOn] = makePacket(BU | B1 | B5 | B6);
        m_packetTable[TorqueOff] = makePacket(BD | B3 | B5 | B6);

        m_commandStringTable.resize(CommandNOI);

        SET_COMMAND_STRING_TABLE(CommandNone);
        SET_COMMAND_STRING_TABLE(Forward);
        SET_COMMAND_STRING_TABLE(ForwardSmall);
        SET_COMMAND_STRING_TABLE(Backward);
        SET_COMMAND_STRING_TABLE(BackwardSmall);
        SET_COMMAND_STRING_TABLE(StepRight);
        SET_COMMAND_STRING_TABLE(StepRightSmall);
        SET_COMMAND_STRING_TABLE(TurnRight);
        SET_COMMAND_STRING_TABLE(TurnRightSmall);
        SET_COMMAND_STRING_TABLE(StepLeft);
        SET_COMMAND_STRING_TABLE(StepLeftSmall);
        SET_COMMAND_STRING_TABLE(TurnLeft);
        SET_COMMAND_STRING_TABLE(TurnLeftSmall);
        SET_COMMAND_STRING_TABLE(KickRight);
        SET_COMMAND_STRING_TABLE(KickLeft);
        SET_COMMAND_STRING_TABLE(KeeperRight);
        SET_COMMAND_STRING_TABLE(KeeperCenter);
        SET_COMMAND_STRING_TABLE(KeeperLeft);
        SET_COMMAND_STRING_TABLE(StandUp);
        SET_COMMAND_STRING_TABLE(TorqueOn);
        SET_COMMAND_STRING_TABLE(TorqueOff);
      }

    public:
      ROBOTISMINI()
      {
        std::cout << "ROBOTISMINI コンストラクタ" << std::endl;
        m_baud_rate = 57600;
        m_parity = boost::asio::serial_port_base::parity::none;
      }
      ///
      ///@brief コマンドを表す列挙型の値を返す
      ///
      robotismini::Command getCommand()
      {
        return static_cast<robotismini::Command>(Robot::getCommand());
      }
    };

    ///
    ///@brief ロボットのタスクの種類
    ///
    enum TaskType {
      TaskTypeNone = 0, ///<なし
      TaskStandUp,      ///<起き上がり
      TaskMove,         ///<目標への移動
      TaskTypeNOI       ///<項目数
    };

  } //namespace robotismini

  ///
  ///@brief ROBOTIS MINIのタスクを実行するクラス
  ///
  class TaskROBOTISMINI : public Task {
  private:
    robotismini::ROBOTISMINI m_robot;  ///<利用する実ロボット
  public:
    ///
    ///@brief コンストラクタ
    ///@param[in] color  自チームの色
    ///@param[in] number 自分の番号
    ///
    TaskROBOTISMINI(int color, int number)
      :Task(color, number)
    {
      m_probot = &m_robot;
    }
    bool isLying(const srInfo &info);
    int none();
    int torqueOff();
    int torqueOn();
    int standUp(const srInfo &info, double ctime);
    int move(const srInfo &info, const Orthogonal &target);
  };


}//namespace odens

///@} doxygenのためのコメント（消してはいけない）

