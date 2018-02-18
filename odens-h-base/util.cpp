///
///@file util.cpp
///@brief 便利な関数
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///- 2008/02/13
///- 2007/10/25 片岡 賢太郎
///- 2007/03/25 片岡 賢太郎
///- 2006/04/26 升谷 保博，片岡 賢太郎
///@addtogroup util
///@{
///

#include "util.h"

// ----------------------------------------------------------------------
// キーボード入力
#if defined(_WINDOWS) || defined(WIN32) //Windows
  #include <conio.h>
#elif LINUX //Linux
  #include <termio.h>
  #include <unistd.h>
  struct termios originaltio;
#endif
#include <cstdlib>

namespace odens {
  
bool   gbInkeyInitiaze = false; ///<キーボード入力の初期設定が済んでいるか？

///
///@brief   キーボード入力の初期設定を行う
///@return なし
///
void inkeyInitialize()
{
#ifdef LINUX //Linux
  if(gbInkeyInitiaze)
  {
    inkeyTerminate();
  }

  struct termios worktio;

  tcgetattr(0, &originaltio);
  worktio = originaltio;
  worktio.c_cc[VTIME] = 0;
  worktio.c_cc[VMIN] = 0;
  worktio.c_lflag &= ~ICANON;
  worktio.c_lflag &= ~ECHO;
  tcsetattr(0, TCSANOW, &worktio);

  gbInkeyInitiaze = true;
  atexit(inkeyTerminate);
#endif
}

///
///@brief   キーボードからの入力を取得する
///@retval  -1 キーがなにも押されていない時
///@retval  -1以外 キーボードからの入力文字
///
///- キーが押されていても押されていなくても処理はすぐにもどる。
///- この関数を実行するには先に関数 @ref inkeyInitialize() を実行しなければならない。
///
int inkey()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(_kbhit())
  {
    return _getch();
  }
  return -1;
#elif LINUX //Linux
  int r;
  unsigned char c;

  if ( read(0, &c, 1) == 1 ) 
  {
    r = c;
  } 
  else 
  {
    r = -1;
  }
  return r;
#endif

}

///
///@brief   @ref inkey() の後始末
///@return なし 
///
void inkeyTerminate()
{
#ifdef LINUX //Linux
  tcsetattr(0, TCSANOW, &originaltio);
#endif
}

// ----------------------------------------------------------------------
// 時計関数
#if defined(_WINDOWS) || defined(WIN32) //Windows
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <winbase.h>
  #pragma comment(lib, "winmm.lib")
  #include <mmsystem.h>

  bool useQPC = false;//高精度カウンタ使用フラグ
  LONGLONG QPFrequency;
  LONGLONG basetime; 
#elif LINUX //Linux
  #include <sys/time.h>
  unsigned long basetime;
#endif



///
///@brief   計時関数の初期化
///@return なし
///
void getTimeInitialize()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(QueryPerformanceFrequency((LARGE_INTEGER*)&QPFrequency))
  {
    if(QueryPerformanceCounter((LARGE_INTEGER*)&basetime))
    {
      useQPC = true;
    }
  }
  else basetime = timeGetTime();
#elif LINUX //Linux
  struct timeval now;

  gettimeofday(&now,NULL);
  basetime = now.tv_sec;
#endif
}

///
///@brief     時間の取得
///@return    初期化後の経過時間 [sec]
///
///- 初期化後の経過時間を秒単位の実数で返す
///- この関数を実行するには先に関数 @ref getTimeInitialize() を実行しなければならない。
///
double getTime()
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  if(useQPC)
  {
    LONGLONG time;
    if(QueryPerformanceCounter((LARGE_INTEGER*)&time))
    {
      return (double)(time-basetime)/QPFrequency;
    }
  }
  return (double)(timeGetTime()-basetime)/1000.0;
#elif LINUX //Linux
  struct timeval now;

  gettimeofday(&now,NULL);
  return now.tv_sec - basetime + now.tv_usec*1e-6;
#endif
}

#ifdef LINUX //Linux
  #include <unistd.h>
#endif

///
///@brief     指定時間スリープする
///@param     time スリープする時間[msec]
///@retval    -1 エラー
///@retval    0  成功
///
int msleep(unsigned int time)
{
#if defined(_WINDOWS) || defined(WIN32) //Windows
  Sleep(time);
#elif LINUX //Linux
  return usleep(time*1000);
#endif
  return 0;
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
