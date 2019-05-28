///
///@file task-kxrl2.cpp
///@brief KXR-L2用 Taskクラスのメンバ関数の定義（公開用）
///@par Copyright
/// Copyright (C) 2016-2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/03/03 升谷 保博
///- 2018/02/18 升谷 保博 公開用
///- 2017/03/05 升谷 保博
///@addtogroup kxrl2
///@{
///

#include <cmath>
#include <vector>
#include <utility>
#include "util.h"
#include "kxrl2.h"
#include "draw.h"

using namespace std;

namespace odens {
  using namespace kxrl2;
  ///
  ///@brief 倒れているかどうかの判断
  ///@param[in] info フィールドの位置情報
  ///@retval false 倒れていない
  ///@retval true 倒れている
  ///
  bool TaskKXRL2::isLying(const srInfo &info) {
    Command com = m_robot.getCommand();
    return info.robot[m_ourColor][m_myNumber].isInvisible()
      && com != TorqueOff && com != TorqueOn;
  }

  ///
  ///@brief 何もしない
  ///@return 終了状態
  ///
  int TaskKXRL2::none()
  {
    m_PrevTaskType = TaskTypeNone;
    m_robot.setCommand(CommandNone);
    return 0;
  }

  ///
  ///@brief トルクオフ
  ///@return 終了状態
  ///
  int TaskKXRL2::torqueOff()
  {
    m_robot.setCommand(TorqueOff);
    return 0;
  }

  ///
  ///@brief トルクオン
  ///@return 終了状態
  ///
  int TaskKXRL2::torqueOn()
  {
    m_robot.setCommand(TorqueOn);
    return 0;
  }

  ///
  ///@brief 起き上がり
  ///@param[in] info フィールドの位置情報
  ///@param[in] ctime 現在の時刻
  ///@return 終了状態
  ///
  int TaskKXRL2::standUp(const srInfo &info, double ctime)
  {
    static double startTime;
    static bool isProceeding = false;
    Command com;
    if (m_PrevTaskType != TaskStandUp || !isProceeding) {
      //前のタスクが起き上がりでないか，起き上がりが進行中でなければ，
      startTime = ctime;
      isProceeding = true;
    }
    if (ctime - startTime < 0.167) {
      com = CommandNone;
    } else if (ctime - startTime < 0.333) {
      com = StandUp;
    } else {
      com = CommandNone;
      isProceeding = false;
    }

    m_PrevTaskType = TaskStandUp;
    m_robot.setCommand(com);
    return 0;
  }

  ///
  ///@brief 目標への移動
  ///@param[in] info フィールドの位置情報
  ///@param[in] target 目標位置
  ///@return 終了状態
  ///
  int TaskKXRL2::move(const srInfo &info, const Orthogonal &target)
  {
    Command com;
    const Orthogonal &robot = info.robot[m_ourColor][m_myNumber];
    if (robot.isInvisible()) {
      cerr << "自分が見えない" << endl;
      com = CommandNone;
      return 1;
    }
    if (target.isInvisible()) {
      cerr << "ターゲットが見えない" << endl;
      com = CommandNone;
      return 1;
    }
    Orthogonal targetLocal = target.transform(robot); //目標をロボット座標系へ変換
    double qG = targetLocal.theta; //ローカル座標系における目標の角度
    double qP = targetLocal.angle(); //目標へ向かう角度
    double d = targetLocal.distance(); //目標までの距離

    //以下で，qGとqPから並進の方法（前進，左進，右進，後進の四択）を決定する
    vector<pair<double, int>> vp; //回転角度の絶対値の和と並進の方法（0～3）の組の並び
    const double offset[4] = { 0, -M_PI / 2, +M_PI / 2, -M_PI }; //前進，左進，右進，後進
    const Command transCom[4] = { Forward, StepLeft,StepRight, Backward };
    for (int i = 0; i < 4; i++) {
      double q1 = normalizeAngle(qP + offset[i]); //q1:初期地点における回転角度
      double q3 = normalizeAngle(qG - q1);		  //q3:目標地点における回転角度
      vp.push_back(pair<double, int>(abs(q1) + abs(q3), i));
    }
    sort(vp.begin(), vp.end()); //角度の絶対値の和に対してソートする．
#if 0
    //デバッグ用
    for (size_t i = 0; i < vp.size(); i++) {
      cout << i << " " << vp[i].second << " " << vp[i].first << endl;
    }
    const string directionString[] = { "Forward", "StepLeft", "StepRight", "Backward" };
    cout << directionString[vp[0].second] << endl;
#endif
    //vp[0].second に最も回転の小さくなる並進の番号が入る
    double q1 = normalizeAngle(qP + offset[vp[0].second]);
    Command trans = transCom[vp[0].second];

    //状態遷移
    enum Mode { Far, Middle, Near };
    static Mode mode = Far;
    if (mode == Far) {
      if (abs(d) < 70)
        mode = Near;
      else if (abs(d) < 100) {
        mode = Middle;
      }
    } else if (mode == Middle) {
      if (abs(d) > 150) {
        mode = Far;
      } else if (abs(d) < 70) {
        mode = Near;
      }
    } else if (mode == Near) {
      if (abs(d) > 130) {
        mode = Far;
      }
    }

    //状態によるコマンドの決定
    int retval = 0;
    if (mode == Far) {
      if (abs(q1) > DEG2RAD(35)) {
        if (q1 > 0) {
          com = TurnLeft;
        } else {
          com = TurnRight;
        }
      } else {
        com = trans;
      }
    } else if (mode == Middle) {
      if (abs(q1) > DEG2RAD(20)) {
        if (q1 > 0) {
          com = TurnLeft;
        } else {
          com = TurnRight;
        }
      } else {
        com = trans;
      }
    } else if (mode == Near) {
      if (abs(qG) > DEG2RAD(60)) {
        if (qG > 0) {
          com = TurnLeft;
        } else {
          com = TurnRight;
        }
      } else if (abs(qG) > DEG2RAD(10)) {
        if (qG > 0) {
          com = TurnLeftSmall;
        } else {
          com = TurnRightSmall;
        }
      } else {
        //|qG|が10[deg]未満ならば完了と判断
        com = CommandNone;
        retval = 10;
      }
    }
    m_robot.setCommand(com);
    return retval;
  }

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
