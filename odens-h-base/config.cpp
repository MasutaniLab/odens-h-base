///
///@file config.cpp
///@brief コマンドラインと設定ファイルによって変数を設定する
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup config Config
///@{
///

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/program_options.hpp>
#include "config.h"

using namespace std;
using namespace boost::program_options;

namespace odens {

bool setRobotTable(int table[], const string &name, const string &s);
const string &makeString(bool x, const string &sTrue, const string &sFalse);

//規定値の設定
string  Config::RobotType = "RIC30";
int     Config::MyNumber = 1; 
int     Config::MyColor = BLUE;
bool    Config::Pause = true;
bool    Config::Goalie = false;
string  Config::RobotPortName = "COM7";
string  Config::VisionAddress = "224.5.23.2";
int     Config::VisionPortNumber = 10006;
bool    Config::Referee = true;
string  Config::RefereeAddress = "224.5.23.1";
int     Config::RefereePortNumber = 10003;  
int     Config::Quadrant = 0;
bool    Config::AttackRight = true;
int     Config::OurMarkerTable[MAX_ROBOT_NUM+1] = {0,0,1,2};
int     Config::TheirMarkerTable[MAX_ROBOT_NUM+1] = {0,3,4,5};
bool    Config::Logger = false;

///
///@brief コマンドラインと設定ファイルによって変数を設定する
///@param[in] argc コマンドラインの引数の数
///@param[in] argv コマンドラインの引数
///@retval false 正常終了
///@retval false 異常終了
///
bool Config::setup(int argc, char* argv[])
{
  //コマンドラインの仕様
  options_description opt("オプション");
  opt.add_options()
    ("help,h", "ヘルプを表示")
    ("conf,c", value<string>(), "設定ファイル")
    ("num1,1", "1号機")
    ("num2,2", "2号機")
    ("num3,3", "3号機")
    ("blue,b", "青チーム")
    ("yellow,y", "黄チーム")
    ("goalie,g", "ゴールキーパ")
    ("run,r", "即実行")
    ("attackRight,R", "右へ攻める")
    ("attackLeft,L", "左へ攻める")
    ;
  //設定ファイルの仕様
  options_description opt2("設定ファイル");
  try {
  opt2.add_options()
    ("RobotType", value<string>(), "ロボットの種類")
    ("MyNumber", value<int>(), "ロボット番号")
    ("MyColor", value<string>(), "自分のカラー")
    ("Pause", value<bool>(), "一時停止で開始")
    ("Goalie", value<bool>(), "ゴールキーパー")
    ("RobotPortName", value<string>(), "シリアルポートの名前")
    ("VisionAddress", value<string>(), "ビジョンのマルチキャストアドレス")
    ("VisionPortNumber", value<int>(), "ビジョンのポート番号")
    ("Referee", value<bool>(), "レフェリーを使う")
    ("RefereeAddress", value<string>(), "レフェリーのマルチキャストアドレス")
    ("RefereePortNumber", value<int>(), "レフェリーのポート番号")
    ("Quadrant", value<int>(), "SSL Visionの象限-1")
    ("AttackRight", value<bool>(), "右へ攻める")
    ("OurMarkerTable", value<string>(), "自チームロボットのマーカ番号対応")
    ("TheirMarkerTable", value<string>(), "相手チームロボットのマーカ番号対応")
    ("Logger", value<bool>(), "ログを取るか？")
    ;
  } catch(exception& e) {
    cerr << e.what() << endl;
    return true;
  }
  // argc, argv を解析して、結果をvmに格納
  variables_map vm;
  try {
    store(parse_command_line(argc, argv, opt), vm);
  } catch(exception& e) {
    cerr << "コマンドライン引数エラー: " << e.what() << endl;
    cerr << opt << endl;
    return true;
  }
  notify(vm);
  if (vm.count("help")) {
    cerr << opt << endl; // ヘルプ表示
    return true;
  }

  string confFileName = "odens.conf";
  if (vm.count("conf")) {
    confFileName = vm["conf"].as<string>();
  }
  ifstream ifs(confFileName.c_str());
  if (!ifs) {
    cerr << "ファイルを開けない: " << confFileName << endl;
    return true;;
  }
  variables_map vm2;
  try {
    store(parse_config_file(ifs, opt2), vm2);
  } catch(exception& e) {
    cerr << "設定ファイルエラー: " << e.what() << endl;
    return true;;
  }
  notify(vm2);

  //設定ファイルの設定の利用
  if (vm2.count("RobotType")) {
    RobotType = vm2["RobotType"].as<string>();
  }
  if (vm2.count("MyNumber")) {
    MyNumber = vm2["MyNumber"].as<int>();
  }
  if (vm2.count("MyColor")) {
    string s = vm2["MyColor"].as<string>();
    if (s == "BLUE" || s == "blue") {
      MyColor = BLUE;
    } else if (s == "YELLOW" || s == "yellow") {
      MyColor = YELLOW;
    } else {
      cerr << "MyColorの値が無効: " << s << endl;
      return true;;
    }
  }
  if (vm2.count("Goalie")) {
    Goalie = vm2["Goalie"].as<bool>();
  }
  if (vm2.count("RobotPortName")) {
    RobotPortName= vm2["RobotPortName"].as<string>();
  }
  if (vm2.count("VisionAddress")) {
    VisionAddress= vm2["VisionAddress"].as<string>();
  }
  if (vm2.count("VisionPortNumber")) {
    VisionPortNumber= vm2["VisionPortNumber"].as<int>();
  }
  if (vm2.count("Referee")) {
    Referee = vm2["Referee"].as<bool>();
  }
  if (vm2.count("RefereeAddress")) {
    RefereeAddress = vm2["RefereeAddress"].as<string>();
  }
  if (vm2.count("RefereePortNumber")) {
    RefereePortNumber = vm2["RefereePortNumber"].as<int>();
  }
  if (vm2.count("Quadrant")) {
    Quadrant = vm2["Quadrant"].as<int>();
  }
  if (vm2.count("AttackRight")) {
    AttackRight = vm2["AttackRight"].as<bool>();
  }
  if (vm2.count("OurMarkerTable")) {
    string s = vm2["OurMarkerTable"].as<string>();
    if (setRobotTable(OurMarkerTable, "OurMarkerTable", s)) {
      return true;
    }
  }
  if (vm2.count("TheirMarkerTable")) {
    string s = vm2["TheirMarkerTable"].as<string>();
    if (setRobotTable(TheirMarkerTable, "TheirMarkerTable", s)) {
      return true;
    }
  }
  if (vm2.count("Logger")) {
    Logger = vm2["Logger"].as<bool>();
  }
  //コマンドラインの設定の利用
  if (vm.count("num1")) {
    MyNumber = 1;
  }
  if (vm.count("num2")) {
    MyNumber = 2;
  }
  if (vm.count("num3")) {
    MyNumber = 3;
  }
  if (vm.count("blue")) {
    MyColor = BLUE;
  }
  if (vm.count("yellow")) {
    MyColor = YELLOW;
  }
  if (vm.count("goalie")) {
    Goalie = true;
  }
  if (vm.count("run")) {
    Pause = false;
  }
  if (vm.count("attackRight")) {
    AttackRight = true;
  }
  if (vm.count("attackLeft")) {
    AttackRight = false;
  }
  return false; //正常終了
}

///
///@brief 設定する変数の値を表示する．
///@return なし
///
void Config::print()
{
  cout << "RobotType: " << RobotType << endl;
  cout << "MyNumber: " << MyNumber << endl;
  cout << "MyColor: " << makeString(MyColor==BLUE,"BLUE", "YELLOW") << endl;
  cout << "Goalie: " << makeString(Goalie, "true", "false") << endl;
  cout << "RobotPortName: " << RobotPortName << endl;
  cout << "VisionAddress: " << VisionAddress << endl;
  cout << "VisionPortNumber: " << VisionPortNumber << endl;
  cout << "Referee: " << makeString(Referee, "true", "false") << endl;
  cout << "RefereeAddress: " << RefereeAddress << endl;
  cout << "RefereePortNumber: " << RefereePortNumber << endl;
  cout << "Quadrant: " << Quadrant << endl;
  cout << "AttackRight: " << makeString(AttackRight, "true", "false") << endl;
  for (int i=1; i<=MAX_ROBOT_NUM; i++) {
    cout << "OurMarkerTable[" << i << "]: " << OurMarkerTable[i] << endl;
  }
  for (int i=1; i<=MAX_ROBOT_NUM; i++) {
    cout << "TheirMarkerTable[" << i << "]: " << TheirMarkerTable[i] << endl;
  }

}

///
///@brief stringから対応表を設定する
///@param[out] table 対応表
///@param[in] name 内容を区別するための文字列
///@param[in] s 値の並びが入った文字列
///@retval false 正常終了
///@retval false 異常終了
///
bool setRobotTable(int table[], const string &name, const string &s)
{
  istringstream is(s);
  for (int i=1; i<=MAX_ROBOT_NUM; i++) {
    int x;
    is >> x;
    if (!is) {
      cerr << name << " 読み込みエラー: " << s << endl;
      return true;
    }
    if (x<0 || x >=MAX_MARKER_NUM) {
      cerr << name << " 範囲外の値: " << x << endl;
      return true;
    }
    table[i] = x;
  }
  return false;
}

///
///@brief 真偽によって
///@param[in] x 真偽の値
///@param[in] sTrue 真の場合の文字列
///@param[in] sFalse 偽の場合の文字列
///@return 結果の文字列
///
const string &makeString(bool x, const string &sTrue, const string &sFalse)
{
  if (x) {
    return sTrue;
  } else {
    return sFalse;
  }
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
