///
///@file kxrl2.h
///@brief KXR-L2用RobotクラスとTaskクラスの宣言（公開用）
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2018/02/18 升谷 保博 公開用
///- 2017/03/05 升谷 保博
///@addtogroup kxr-l2 KXR-L2
///@brief KXR-L2のためのクラス
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
  namespace kxrl2 {
    ///
    ///@brief ロボットへ送るコマンド（モーション）
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
      KickRightSide,        ///<右横キック
      KickRightBack,        ///<右後ろキック
      KickLeft,             ///<左キック
      KickLeftSide,         ///<左横キック
      KickLeftBack,         ///<左後ろキック
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
    const uint16_t BU = 0x0001; ///<十字キー上
    const uint16_t BD = 0x0002; ///<十字キー下
    const uint16_t BR = 0x0004; ///<十字キー右
    const uint16_t BL = 0x0008; ///<十字キー左
    const uint16_t B1 = 0x0010; ///<さんかく
    const uint16_t B2 = 0x0020; ///<ばつ
    const uint16_t B3 = 0x0040; ///<まる
    const uint16_t B4 = 0x0100; ///<しかく
    const uint16_t L1 = 0x0200; ///<L1
    const uint16_t L2 = 0x0400; ///<L2
    const uint16_t R1 = 0x0800; ///<R1
    const uint16_t R2 = 0x1000; ///<R2

    ///RobotCommand列挙型の項目に対応する文字列を設定するマクロ
#define SET_COMMAND_STRING_TABLE(x) m_commandStringTable[x] = #x

        ///
        ///@brief シリアル通信でKHR-3HVへコマンドを送信するクラス
        ///
    class KXRL2 : public Robot {
    private:
      ///
      ///@brief KHR-3HVへ送るパケットを作る
      ///@param[in] c  ボタンの状態を表す2byte
      ///@param[in] pa1 スライダーPA1
      ///@param[in] pa2 スライダーPA2
      ///@param[in] pa3 スライダーPA3
      ///@param[in] pa4 スライダーPA4
      ///@return パケットのバイト列
      ///
      Packet makePacket(
        uint16_t c = 0x0000,
        uint8_t pa1 = 0x00,
        uint8_t pa2 = 0x00,
        uint8_t pa3 = 0x00,
        uint8_t pa4 = 0x00
      )
      {
        Packet p;
        p.b.resize(13);
        p.b[0] = 0x0d;
        p.b[1] = 0x00;
        p.b[2] = 0x02;
        p.b[3] = 0x50;
        p.b[4] = 0x03;
        p.b[5] = 0x00;
        uint8_t low = c & 0xff;
        uint8_t high = (c >> 8) & 0xff;
        p.b[6] = high;
        p.b[7] = low;
        p.b[8] = pa1;
        p.b[9] = pa2;
        p.b[10] = pa3;
        p.b[11] = pa4;
        uint8_t sum = 0;
        for (int i = 0; i < 12; i++) {
          sum += p.b[i];
        }
        p.b[12] = sum;//チェックサム
        return p;
      }
      //
      ///@brief KHR-3HVへ送るパケットを設定する
      ///@return なし
      ///
      void initializePacket()
      {
        std::cout << "KXRL2::initializePacket()" << std::endl;

        m_packetTable.resize(CommandNOI);

        m_packetTable[CommandNone] = makePacket();
        m_packetTable[Forward] = makePacket(BU);
        m_packetTable[ForwardSmall] = makePacket(BU | L1);
        m_packetTable[Backward] = makePacket(BD);
        m_packetTable[BackwardSmall] = makePacket(BD | L1);  //未実装
        m_packetTable[StepRight] = makePacket(BR);
        m_packetTable[StepRightSmall] = makePacket(BR | L1);  //未実装
        m_packetTable[TurnRight] = makePacket(BU | BR);
        m_packetTable[TurnRightSmall] = makePacket(BU | BR | L1);
        m_packetTable[StepLeft] = makePacket(BL);
        m_packetTable[StepLeftSmall] = makePacket(B1 | B2 | B3 | B4);  //未実装
        m_packetTable[TurnLeft] = makePacket(BU | BL);
        m_packetTable[TurnLeftSmall] = makePacket(BU | BL | L1);
        m_packetTable[KickRight] = makePacket(B3);
        m_packetTable[KickRightSide] = makePacket(B3 | R1 | R2);  //未実装
        m_packetTable[KickRightBack] = makePacket(B3 | BD);  //未実装
        m_packetTable[KickLeft] = makePacket(B4);
        m_packetTable[KickLeftSide] = makePacket(B4 | R1 | R2);  //未実装
        m_packetTable[KickLeftBack] = makePacket(B4 | BD);  //未実装
        m_packetTable[KeeperRight] = makePacket(BR | B1);  //未実装
        m_packetTable[KeeperCenter] = makePacket(B2);
        m_packetTable[KeeperLeft] = makePacket(BL | B1);  //未実装
        m_packetTable[StandUp] = makePacket(B1 | R2 | L2);
        m_packetTable[TorqueOn] = makePacket(L2 | R2);
        m_packetTable[TorqueOff] = makePacket(L1 | R1);

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
        SET_COMMAND_STRING_TABLE(KickRightSide);
        SET_COMMAND_STRING_TABLE(KickRightBack);
        SET_COMMAND_STRING_TABLE(KickLeft);
        SET_COMMAND_STRING_TABLE(KickLeftSide);
        SET_COMMAND_STRING_TABLE(KickLeftBack);
        SET_COMMAND_STRING_TABLE(KeeperRight);
        SET_COMMAND_STRING_TABLE(KeeperCenter);
        SET_COMMAND_STRING_TABLE(KeeperLeft);
        SET_COMMAND_STRING_TABLE(StandUp);
        SET_COMMAND_STRING_TABLE(TorqueOn);
        SET_COMMAND_STRING_TABLE(TorqueOff);
      }

    public:
      KXRL2()
      {
        std::cout << "KXRL2 コンストラクタ" << std::endl;
        m_baud_rate = 115200;
        m_parity = boost::asio::serial_port_base::parity::even;
      }
      ///
      ///@brief コマンドを表す列挙型の値を返す
      ///
      KXRL2::Command getCommand()
      {
        return static_cast<kxrl2::Command>(Robot::getCommand());
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

  } //namespace khr3

  ///
  ///@brief ROBOTIS MINIのタスクを実行するクラス
  ///
  class TaskKXRL2 : public Task {
  private:
    kxrl2::KXRL2 m_robot;  ///<利用する実ロボット
  public:
    ///
    ///@brief コンストラクタ
    ///@param[in] color  自チームの色
    ///@param[in] number 自分の番号
    ///
    TaskKXRL2(int color, int number)
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
