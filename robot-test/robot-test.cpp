///
///@file robot-test.cpp
///@brief Robotクラスのテストプログラム（公開用）
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2018/02/18 升谷 保博 KXR-L2を追加
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///
#include <iostream>
#include <cstdint>
#include "util.h"
#include "robot.h"
#include "ric30.h"
#include "robotismini.h"
#include "khr3.h"
#include "kxrl2.h"
#include "config.h"

using namespace std;
using namespace odens;

///キーとロボットのコマンド対応を表示する
void printHelp(Robot *pr);
int test(Robot *pr);

///robot-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }

  inkeyInitialize();

  Robot *probot;
  cout << "RobotType: " << Config::RobotType << endl;
  if (Config::RobotType == "RIC30") {
    probot = new ric30::RIC30();
  } else if (Config::RobotType == "ROBOTISMINI") {
    probot = new robotismini::ROBOTISMINI();
  } else if (Config::RobotType == "KHR3") {
    probot = new khr3::KHR3();
  } else if (Config::RobotType == "KXRL2") {
    probot = new kxrl2::KXRL2();
  } else {
    cerr << "RobotType 未登録" << endl;
    return 1;
  }

  if (probot->start(Config::RobotPortName, 50)) {
    cerr << "終了" << endl;
    return 1;
  }
  msleep(1000);
  printHelp(probot);
  int size = int(probot->size());
  while (true) {
    int c = inkey(); //キーを待たずに読む
    if (c != -1) {
      //何かキーが押されている場合
      int com = size; //無効な値としてsizeを利用
      if (c == '?') {
        printHelp(probot);
      } else if (c == ' ') {
        com = 0;
      } else if ('0' <= c && c <= '9') {
        com = c - '0';
      } else if ('a' <= c && c <= 'a' + size - 10 - 1) {
        com = c - 'a' + 10;
      } else {
        cout << "未登録のキー: " << static_cast<char>(c) << endl;
      }
      if (com != size) {
        //適切なキーが押された場合
        cout << probot->getCommandString(com) << endl;
        probot->setCommand(com);
      }
    }
    msleep(16);
  }
  return 0;

}

void printHelp(Robot *pr)
{
  for (size_t i = 0; i < pr->size(); i++) {
    char c;
    if (i <= 9) {
      c = static_cast<char>('0' + i);
    } else {
      c = static_cast<char>('a' + i - 10);
    }
    cout << c << ": " << pr->getCommandString(i) << endl;
  }
  cout << "?: この一覧の表示" << endl;
}