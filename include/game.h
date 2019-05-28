///
///@file game.h
///@brief Gameクラスの宣言
///@par Copyright
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup game Game
///@brief プレイモードを決定するクラス
///@{
///- レフェリーボックスからの信号によって，「チームとしての」プレイモードを決定する．
///- キックを与えられた側の表現をBLUE,YELLOWから，Our（自チーム），Their（相手チーム）へ変換する．
///

#pragma once
#include "sr.h"
#include "referee.h"

namespace odens {

///
///@brief チームとしてのプレーの種類を表す列挙型
///
enum PlayType {
  PlayTypeNone = 0, ///<未定義
  Halt,             ///<停止（haltの後）
  OutOfPlay,        ///<プレー中断（stop gameの後）
  InPlay,           ///<通常のプレー
  SetPlay,          ///<セットプレー
  PreSetPlay,       ///<セットプレーの準備
  PlayTypeNOI       ///<項目数
};

///
///@brief プレーの種類に付随するキックの種類を表す列挙型
///
enum KickType {
  KickTypeNone = 0,     ///<未定義
  OurKickOff,           ///<自チームのキックオフ
  OurPenaltyKick,       ///<自チームのペナルティキック
  OurDirectFreeKick,    ///<自チームの直接フリーキック
  OurIndirectFreeKick,  ///<自チームの関節フリーキック
  TheirKickOff,         ///<相手チームのキックオフ
  TheirPenaltyKick,     ///<相手チームのペナルティキック
  TheirDirectFreeKick,  ///<相手チームの直接フリーキック
  TheirIndirectFreeKick,///<相手チームの関節フリーキック
  KickTypeNOI           ///<項目数
};

class Game;

///
///@brief プレーの種類とキックの種類を保持する構造体
///
struct GameMode {
  friend Game;   ///<@ref Game クラス
  PlayType play; ///<プレーの種類
  KickType kick; ///<キックの種類

  ///コンストラクタ
  GameMode()
  {
    play = PlayTypeNone;
    kick = KickTypeNone;
  }
  ///プレーの種類とキックの種類の文字列を得る
  std::string getString()
  {
    return playTypeStringTable[play] + " " + kickTypeStringTable[kick];
  }
  ///自チームのキックか？
  bool isOurKick() const
  {
    return kick == OurKickOff || kick == OurPenaltyKick || kick == OurDirectFreeKick || kick == OurIndirectFreeKick;
  }
  ///相手チームのキックか？
  bool isTheirKick() const
  {
    return kick == TheirKickOff || kick == TheirPenaltyKick || kick == TheirDirectFreeKick || kick == TheirIndirectFreeKick;
  }
private:
  static std::string playTypeStringTable[PlayTypeNOI]; ///<プレーの種類の文字列を保持
  static std::string kickTypeStringTable[KickTypeNOI]; ///<キックの種類の文字列を保持
};

///
///@brief レフェリーボックスからの信号などからチームとしてのプレーを決めるクラス
///
class Game {
private:
  int m_ourColor;           ///<自チームの色
  PlayType m_prevPlayType;  ///<前回のプレーの種類
  KickType m_kickType;      ///<現在のキックの種類
  Orthogonal m_setPlayBall; ///<セットプレー開始時のボールの位置
  double m_setPlayTime;     ///<セットプレー開始時刻
public:
  Game(int color);
  GameMode decideMode(const RefereeInfo &rinfo, const Orthogonal &ball, double ctime);
};

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
