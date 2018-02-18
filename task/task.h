///
///@file task.h
///@brief Taskクラスの宣言（公開用）
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup task Task
///@brief 各タスクの定義する基底クラス
///@{
///

#pragma once
#include "sr.h"
#include "robot.h"
#include "game.h"
#include "estimator.h" 

namespace odens {

///
///@brief ロボットのタスクを実行するクラス
///
class Task {
protected:
  int m_ourColor;     ///<自チームの色
  int m_theirColor;   ///<相手チームの色
  int m_myNumber;     ///<自機の番号
  int m_PrevTaskType; ///<前のタスクの種類
  Robot *m_probot; ///<Taskで使うロボット

public:
  ///
  ///@brief コンストラクタ
  ///@param[in] color 自チームの色
  ///@param[in] number 自機の番号
  ///
  Task::Task(int color, int number)
  {
    if (color == BLUE) {
      m_ourColor = BLUE;
      m_theirColor = YELLOW;
    } else {
      m_ourColor = YELLOW;
      m_theirColor = BLUE;
    }
    m_myNumber = number;
    m_PrevTaskType = 0;
  }
  ///
  ///@brief 下請けのRobotの通信開始
  ///@param[in] port ポート名
  ///@param[in] interval 送信間隔[ms]
  ///
  bool startRobot(std::string port, int interval) 
  {
    if (m_probot == nullptr) {
      std::cerr << "m_probotが未設定" << std::endl;
    }
    return m_probot->start(port, interval);
  }
  ///
  ///@brief 自チームの色を返す
  ///
  int getOurColor()
  {
    return m_ourColor;
  }
  ///
  ///@brief 相手チームの色を返す
  ///
  int getTheirColor()
  {
    return m_theirColor;
  }
  ///
  ///@brief 自分の番号を返す
  ///
  int getMyNumber()
  {
    return m_myNumber;
  }
  ///
  ///@brief コマンドを表す文字列を返す
  ///
  std::string getCommandString()
  {
    return m_probot->getCommandString();
  }
  ///
  ///@brief コマンドを表す整数値を返す
  ///
  RobotCommand getCommand()
  {
    return m_probot->getCommand();
  }
  ///
  ///@brief ビジョンの情報から転倒しているか判定する
  ///
  virtual bool isLying(const srInfo &info) = 0;
  ///
  ///@brief 何もしない
  ///
  virtual int none() = 0;
  ///
  ///@brief トルクオフ
  ///
  virtual int torqueOff() = 0;
  ///
  ///@brief トルクオン
  ///
  virtual int torqueOn() = 0;
  ///
  ///@brief 起き上がり
  ///
  virtual int standUp(const srInfo &info, double ctime) = 0;
  ///
  ///@brief 目的へ移動
  ///
  virtual int move(const srInfo &info, const Orthogonal &target) = 0;


};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
