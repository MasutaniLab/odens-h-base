///
///@file ric30.h
///@brief RIC30用RobotクラスとTaskクラスの宣言（公開用）
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/05 升谷 保博
///@addtogroup ric30 RIC30
///@brief RIC30のためのクラス
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
  namespace ric30 {
    ///
    ///@brief RIC30へ送るコマンド（モーション）
    ///
    enum Command {
      CommandNone = 0,      ///<なし（何もボタンを押していない）
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
    const uint16_t NONE = 0xffff; ///<何も押さない
    const uint16_t B1 = 0xffef; ///<ボタン1
    const uint16_t B2 = 0xffdf; ///<ボタン2
    const uint16_t B3 = 0xffbf; ///<ボタン3
    const uint16_t B4 = 0xff7f; ///<ボタン4
    const uint16_t R1 = 0xfff7; ///<ボタンR1
    const uint16_t R2 = 0xfffd; ///<ボタンR2
    const uint16_t R3 = 0xfbff; ///<ボタンR3
    const uint16_t L1 = 0xfffb; ///<ボタンL1
    const uint16_t L2 = 0xfffe; ///<ボタンL2
    const uint16_t L3 = 0xfdff; ///<ボタンL3
    const uint16_t SELECT = 0xfeff; ///<ボタンSELECT
    const uint16_t START = 0xf7ff; ///<ボタンSTART
    const uint16_t PU = 0xefff; ///<十字キー上
    const uint16_t PD = 0xbfff; ///<十字キー下
    const uint16_t PL = 0x7fff; ///<十字キー左
    const uint16_t PR = 0xdfff; ///<十字キー右

    ///RobotCommand列挙型の項目に対応する文字列を設定するマクロ
#define SET_COMMAND_STRING_TABLE(x) m_commandStringTable[x] = #x

///
///@brief シリアル通信でRIC30へコマンドを送信するクラス
///
    class RIC30 : public Robot {
    private:
      ///
      ///@brief RIC30へ送るパケットを作る
      ///@param[in] c  ボタンの状態を表す2byte
      ///@param[in] rx 右アナログスティック左右
      ///@param[in] ry 右アナログスティック前後
      ///@param[in] lx 左アナログスティック左右
      ///@param[in] ly 左アナログスティック前後
      ///@return パケットのバイト列
      ///
      Packet makePacket(
        uint16_t c = 0xffff,
        uint8_t rx = 0x80,
        uint8_t ry = 0x80,
        uint8_t lx = 0x80,
        uint8_t ly = 0x80
      )
      {
        Packet p;
        p.b.resize(9);
        p.b[0] = 0x6b;
        p.b[1] = 0xff;
        p.b[2] = (c >> 8) & 0x00ff;
        p.b[3] = c & 0x00ff;
        p.b[4] = rx;
        p.b[5] = ry;
        p.b[6] = lx;
        p.b[7] = ly;
        p.b[8] = 0xf3;
        return p;
      }
      ///
      ///@brief RIC30へ送るパケットを設定する
      ///@return なし
      ///
      void initializePacket()
      {
        std::cout << "RIC30::initializePacket()" << std::endl;

        m_packetTable.resize(CommandNOI);

        m_packetTable[CommandNone] = makePacket();
        m_packetTable[Forward] = makePacket(PU);
        m_packetTable[ForwardSmall] = makePacket(NONE, 0x80, 0x80, 0x80, 0x00);
        m_packetTable[Backward] = makePacket(PD);
        m_packetTable[BackwardSmall] = makePacket();
        m_packetTable[StepRight] = makePacket(PR);
        m_packetTable[StepRightSmall] = makePacket();
        m_packetTable[TurnRight] = makePacket(R1);
        m_packetTable[TurnRightSmall] = makePacket(NONE, 0x80, 0x80, 0xff, 0x80);
        m_packetTable[StepLeft] = makePacket(PL);
        m_packetTable[StepLeftSmall] = makePacket();
        m_packetTable[TurnLeft] = makePacket(L1);
        m_packetTable[TurnLeftSmall] = makePacket(NONE, 0x80, 0x80, 0x00, 0x80);
        m_packetTable[KickRight] = makePacket(B2);
        m_packetTable[KickLeft] = makePacket(B4);
        m_packetTable[KeeperRight] = makePacket(R2);
        m_packetTable[KeeperCenter] = makePacket(B1);
        m_packetTable[KeeperLeft] = makePacket(L2);
        m_packetTable[StandUp] = makePacket(B3);
        m_packetTable[TorqueOn] = makePacket(START);
        m_packetTable[TorqueOff] = makePacket(SELECT);

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
      RIC30()
      {
        std::cout << "RIC30 コンストラクタ" << std::endl;
        m_baud_rate = 115200;
        m_parity = boost::asio::serial_port_base::parity::none;
      }
      ric30::Command getCommand()
        ///
        ///@brief コマンドを表す列挙型の値を返す
        ///
      {
        return static_cast<ric30::Command>(Robot::getCommand());
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

  } //namespace ric30

  ///
  ///@brief RIC30のタスクを実行するクラス
  ///
  class TaskRIC30 : public Task {
  private:
    ric30::RIC30 m_robot; ///<利用する実ロボット
  public:
    ///
    ///@brief コンストラクタ
    ///@param[in] color  自チームの色
    ///@param[in] number 自分の番号
    ///
    TaskRIC30(int color, int number)
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

