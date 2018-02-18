///
///@file game.cpp
///@brief Gameクラスのメンバ関数の定義
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup game Game
///@{
///
#include "game.h"

namespace odens {

std::string GameMode::playTypeStringTable[PlayTypeNOI];
std::string GameMode::kickTypeStringTable[KickTypeNOI];

///PlayType列挙型の項目に対応する文字列を設定するマクロ
#define SET_PLAY_TYPE_STRING_TABLE(x) GameMode::playTypeStringTable[x] = #x

///KickType列挙型の項目に対応する文字列を設定するマクロ
#define SET_KICK_TYPE_STRING_TABLE(x) GameMode::kickTypeStringTable[x] = #x

///
///@brief コンストラクタ
///@param[in] color 自チームの色
///
Game::Game(int color)
{
  m_ourColor = color;
  m_prevPlayType = PlayTypeNone;
  m_kickType = KickTypeNone;
  m_setPlayTime = 0;

  SET_PLAY_TYPE_STRING_TABLE(PlayTypeNone);
  SET_PLAY_TYPE_STRING_TABLE(Halt);
  SET_PLAY_TYPE_STRING_TABLE(OutOfPlay);
  SET_PLAY_TYPE_STRING_TABLE(InPlay);
  SET_PLAY_TYPE_STRING_TABLE(SetPlay);
  SET_PLAY_TYPE_STRING_TABLE(PreSetPlay);

  SET_KICK_TYPE_STRING_TABLE(KickTypeNone);
  SET_KICK_TYPE_STRING_TABLE(OurKickOff);
  SET_KICK_TYPE_STRING_TABLE(OurPenaltyKick);
  SET_KICK_TYPE_STRING_TABLE(OurDirectFreeKick);
  SET_KICK_TYPE_STRING_TABLE(OurIndirectFreeKick);
  SET_KICK_TYPE_STRING_TABLE(TheirKickOff);
  SET_KICK_TYPE_STRING_TABLE(TheirPenaltyKick);
  SET_KICK_TYPE_STRING_TABLE(TheirDirectFreeKick);
  SET_KICK_TYPE_STRING_TABLE(TheirIndirectFreeKick);

}

///
///@brief ゲームモードを決定する
///@param[in] rinfo レフェリーボックスからの情報
///@param[in] ball 現在のボール位置
///@param[in] ctime 現在時刻
///@return 決定結果
///
GameMode Game::decideMode(const RefereeInfo &rinfo, const Orthogonal &ball, double ctime)
{
  const ref::Command &c = rinfo.command;
  GameMode mode;
  switch (c) {
  case ref::HALT:
    mode.play = Halt;
    mode.kick = KickTypeNone;
    break;
  case ref::STOP:
    mode.play = OutOfPlay;
    mode.kick = KickTypeNone;
    break;
  case ref::NORMAL_START:
    if (m_prevPlayType == InPlay) {
      mode.play = InPlay;
    } else {
      mode.play = SetPlay;
    }
    mode.kick = m_kickType;
    break;
  case ref::FORCE_START:
    mode.play = InPlay;
    mode.kick = KickTypeNone;
    break;
  case ref::PREPARE_KICKOFF_YELLOW:
    mode.play = PreSetPlay;
    m_kickType = (m_ourColor==YELLOW)?OurKickOff:TheirKickOff;
    mode.kick = m_kickType;
    break;
  case ref::PREPARE_KICKOFF_BLUE:
    mode.play = PreSetPlay;
    m_kickType = (m_ourColor==BLUE)?OurKickOff:TheirKickOff;
    mode.kick = m_kickType;
    break;
  case ref::PREPARE_PENALTY_YELLOW:
    mode.play = PreSetPlay;
    m_kickType = (m_ourColor==YELLOW)?OurPenaltyKick:TheirPenaltyKick;
    mode.kick = m_kickType;
    break;
  case ref::PREPARE_PENALTY_BLUE:
    mode.play = PreSetPlay;
    m_kickType = (m_ourColor==BLUE)?OurPenaltyKick:TheirPenaltyKick;
    mode.kick = m_kickType;
    break;
  case ref::DIRECT_FREE_YELLOW:
    if (m_prevPlayType == InPlay) {
      mode.play = InPlay;
    } else {
      mode.play = SetPlay;
      m_kickType = (m_ourColor==YELLOW)?OurDirectFreeKick:TheirDirectFreeKick;
    }
    mode.kick = m_kickType;
    break;
  case ref::DIRECT_FREE_BLUE:
    if (m_prevPlayType == InPlay) {
      mode.play = InPlay;
    } else {
      mode.play = SetPlay;
      m_kickType = (m_ourColor==BLUE)?OurDirectFreeKick:TheirDirectFreeKick;
    }
    mode.kick = m_kickType;
    break;
  case ref::INDIRECT_FREE_YELLOW:
    if (m_prevPlayType == InPlay) {
      mode.play = InPlay;
    } else {
      mode.play = SetPlay;
      m_kickType = (m_ourColor==YELLOW)?OurIndirectFreeKick:TheirIndirectFreeKick;
    }
    mode.kick = m_kickType;
    break;
  case ref::INDIRECT_FREE_BLUE:
    if (m_prevPlayType == InPlay) {
      mode.play = InPlay;
    } else {
      mode.play = SetPlay;
      m_kickType = (m_ourColor==BLUE)?OurIndirectFreeKick:TheirIndirectFreeKick;
    }
    mode.kick = m_kickType;
    break;
  case ref::TIMEOUT_YELLOW:
    mode.play = m_prevPlayType;
    mode.kick = KickTypeNone;
    break;
  case ref::TIMEOUT_BLUE:
    mode.play = m_prevPlayType;
    mode.kick = KickTypeNone;
    break;
  case ref::GOAL_YELLOW:
    mode.play = m_prevPlayType;
    mode.kick = KickTypeNone;
    break;
  case ref::GOAL_BLUE:
    mode.play = m_prevPlayType;
    mode.kick = KickTypeNone;
    break;
  }
  //cout << ctime-m_setPlayTime << endl;
  if (mode.play == SetPlay) {
    if (m_prevPlayType != SetPlay) {
      //SetPlayに切り替わった時
      m_setPlayBall = ball;  //TODO: ボールが見えていない場合の対応
      m_setPlayTime = ctime;
    } else if (ctime-m_setPlayTime > 20) { //20秒以上経過したら TODO: 可変にする
      mode.play = InPlay;
      m_kickType = KickTypeNone;
      mode.kick = m_kickType;
    } else if (mode.isTheirKick() && ball.distance(m_setPlayBall) > 100) { //100mm以上動いたら TODO: 可変にする
      mode.play = InPlay;
      m_kickType = KickTypeNone;
      mode.kick = m_kickType;
    }
  }
  m_prevPlayType = mode.play;
  return mode;
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
