///
///@file logger-test.cpp
///@brief Loggerクラスのテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///
#include <iostream>
#include "logger.h"
#include "util.h"

using namespace std;
using namespace odens;

///config-testメイン関数
int main()
{
  Logger logger;
  srInfo sinfo;
  Timed2D ballVel(1,2,3);
  RefereeInfo rinfo;
  rinfo.command = ref::HALT;
  GameMode mode;
  RobotCommand com = 0;

  logger.open(YELLOW,3);
  for (double t=0; t<10; t+=1/60.) {
    logger.write(t, sinfo, ballVel, rinfo, mode, com);
    msleep(10);
  }
  return 0;
}

