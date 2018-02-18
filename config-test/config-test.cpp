///
///@file config-test.cpp
///@brief Configクラスのテストプログラム
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///
#include <iostream>
#include <string>
#include "config.h"

using namespace std;
using namespace odens;

///config-testメイン関数
int main(int argc, char* argv[])
{
  if (Config::setup(argc, argv)) {
    return 1;
  }
  Config::print();

  return 0;
}

