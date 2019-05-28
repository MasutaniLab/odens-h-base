///
///@file draw-test.cpp
///@brief Drawクラスのテストプログラム
///@par Copyright
/// Copyright (C) 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2019/02/03 升谷 保博 新規作成
///

#include <iostream>
#include "sr.h"
#include "draw.h"
#include "util.h"
#include "config.h"

using namespace std;
using namespace odens;

void printHelp();
void makeInfo(srInfo &sinfo, srInfo &sinfo2, VisionInfo &vinfo, double t);

///draw-testメイン関数
int main(int argc, char* argv[])
{
  //コマンドラインオプションと設定ファイル読み込み
  if (Config::setup(argc, argv)) {
    return 1;
  }

  bool drawVisionInfo = false;
  bool drawEstimatedInfo = false;
  bool drawPoint = false;
  bool drawCross = false;
  bool drawLine = false;
  bool drawRectangle = false;
  bool drawFillRectangle = false;
  bool drawCircle = false;
  bool drawFillCircle = false;
  bool drawString = false;

  const double drawInterval = 1.0 / 30.0;
  draw.initialize(Config::MyColor, Config::MyNumber, drawInterval);
  inkeyInitialize();

  printHelp();
  Timer timer;

  bool loop = true;
  cout << "メインループ開始" << endl;
  while (loop) {
    timer.sleep(0.02);
    double t = timer.elapsed();

    srInfo sinfo, sinfo2;
    VisionInfo vinfo;

    makeInfo(sinfo, sinfo2, vinfo, t);

    double q = 2 * M_PI*t / 20.0;
    double x = FIELD_WIDTH2*cos(q);
    double y = FIELD_WIDTH2*sin(q);

    if (drawPoint) {
      draw.point(x, y, 127, 127, 127);
      draw.point(x + 50, y, 127, 255, 127);
      draw.point(x, y + 50, 127, 127, 255);
      draw.point(x - 50, y, 127, 0, 127);
      draw.point(x, y - 50, 127, 127, 0);
    }

    if (drawCross) {
      draw.cross(-x, -y, 255, 255, 255);
    }

    if (drawLine) {
      draw.line(0, 0, -x, -y, 255, 0, 0);
    }

    if (drawRectangle) {
      draw.rectangle(0, 0, -x, y, 0, 255, 0);
    }

    if (drawFillRectangle) {
      draw.fillRectangle(0, 0, x, -y, 0, 255, 255);
    }

    if (drawCircle) {
      draw.circle(- x / 2, - y / 2, FIELD_WIDTH2 / 2, 255, 255, 0);
    }

    if (drawFillCircle) {
      draw.fillCircle(x / 2, y / 2, FIELD_WIDTH2 / 2, 255, 0, 255);
    }

    if (drawString) {
      draw.string(x+50, y+50, 20, 255, 255, 255, "%6.1lf", atan2(y,x)*180/M_PI);
    }

    if (drawVisionInfo) {
      draw.set(vinfo);
    } else if (drawEstimatedInfo) {
      draw.set(sinfo, sinfo2);
    } else {
      draw.set(sinfo);
    }

    //キー入力（コンソール）
    int k = inkey();
    if (k == 's') {
      draw.showWindow();
      cout << "ウィンドウ生成" << endl;
    }

    //キー入力（グラフィックス）
    int c = draw.getch();
    if (c != -1) {
      switch (c) {
      case 'i':
        drawVisionInfo = !drawVisionInfo;
        if (drawVisionInfo) {
          cout << "マーカ表示（VisionInfo）" << endl;
        } else {
          cout << "ロボット表示（srInfo）" << endl;
        }
        break;
      case 'e':
        drawEstimatedInfo = !drawEstimatedInfo;
        if (drawEstimatedInfo) {
          cout << "推定値あり" << endl;
        } else {
          cout << "推定値なし" << endl;
        }
        break;
      case 'p':
        draw.showPositionData();
        cout << "位置表示切替" << endl;
        break;
      case 'h':
        draw.hideWindow();
        cout << "ウィンドウ消去" << endl;
        break;
      case 'r':
        draw.reverseField();
        cout << "左右反転" << endl;
        break;
      case '1':
        drawPoint = !drawPoint;
        cout << "drawPoint" << endl;
        break;
      case '2':
        drawCross = !drawCross;
        cout << "drawCross" << endl;
        break;
      case '3':
        drawLine = !drawLine;
        cout << "drawLine" << endl;
        break;
      case '4':
        drawRectangle = !drawRectangle;
        cout << "drawRectangle" << endl;
        break;
      case '5':
        drawFillRectangle = !drawFillRectangle;
        cout << "drawFillRectangle" << endl;
        break;
      case '6':
        drawCircle = !drawCircle;
        cout << "drawCircle" << endl;
        break;
      case '7':
        drawFillCircle = !drawFillCircle;
        cout << "drawFillCircle" << endl;
        break;
      case '8':
        drawString = !drawString;
        cout << "drawString" << endl;
        break;
      case 0x1b:
        loop = false;
        cout << "終了" << endl;
        break;
      default:
        cerr << "未登録のキー: " << char(c) << endl;
        printHelp();
      }
    }
  }
  cout << "メインループ終了" << endl;
  draw.terminate();
  return 0;
}

///
///@brief キー割り当てを表示する
///@return なし
///
void printHelp()
{
  cout
    << "キー割り当て" << endl
    << "（グラフィックスウィンドウ）" << endl
    << "  r: 表示を左右反転" << endl
    << "  i: マーカ表示とロボット表示の切り替え" << endl
    << "  e: 推定値の有無の切り替え" << endl
    << "  p: 位置表示の切り替え" << endl
    << "  h: グラフィックス消去" << endl
    << "  1: 点描画テスト" << endl
    << "  2: 十字描画テスト" << endl
    << "  3: 線分描画テスト" << endl
    << "  4: 矩形描画テスト" << endl
    << "  5: 塗りつぶし矩形描画テスト" << endl
    << "  6: 円描画テスト" << endl
    << "  7: 塗りつぶし円描画テスト" << endl
    << "  8: 文字列描画テスト" << endl
    << "  Esc: 終了" << endl
    << "（コンソールウィンドウ）" << endl
    << "  s: グラフィックス表示" << endl
    << "  Ctrl+c: 終了" << endl;
}

///
///@brief 情報を作る
///@param[out] sinfo
///@param[out] sinfo2
///@param[out] vinfo
///@param[in] t 時刻
///@return なし
///
void makeInfo(srInfo &sinfo, srInfo &sinfo2, VisionInfo &vinfo, double t)
{
  double x, y;
  x = FIELD_LENGTH2*sin(2 * M_PI*t / 10.0);
  y = FIELD_WIDTH2*sin(2 * M_PI*t / 10.0);

  sinfo.ball = Orthogonal(x, y, 0);
  sinfo2.ball = Orthogonal(x + 100, y + 100, 0);
  vinfo.nBall = 3;
  vinfo.ball[0] = Orthogonal(x, y, 0);
  vinfo.ball[1] = Orthogonal(x - 100, y + 100, 0);
  vinfo.ball[2] = Orthogonal(x + 100, y - 100, 0);

  double a = 2 * M_PI*t / 5.0;
  double ca = cos(a);
  double sa = sin(a);
  double b = 2 * M_PI*t / 0.5;
  double cb = cos(b);
  double sb = sin(b);

  for (int c = BLUE; c <= YELLOW; c++) {
    int sign = c * 2 - 1;
    for (int i = 1; i <= MAX_ROBOT_NUM; i++) {
      double x = -sign * FIELD_LENGTH2 / 2 + sign * i * FIELD_LENGTH2 / 8 * ca;
      double y = i * FIELD_LENGTH2 / 8 * sa;
      double theta = sign * a + M_PI / 2;
      sinfo.robot[c][i] = Orthogonal(x, y, theta);
      sinfo2.robot[c][i] = Orthogonal(x + 100 * cb, y + 100 * sb, theta);
      sinfo.id[c][i] = i + 3 * c;
      sinfo2.id[c][i] = i + 3 * c;
    }
  }

  vinfo.nRobot[BLUE] = 4;
  vinfo.nRobot[YELLOW] = 2;
  for (int c = BLUE; c <= YELLOW; c++) {
    int sign = c * 2 - 1;
    for (int i = 0; i <= vinfo.nRobot[c]; i++) {
      double x = -sign * FIELD_LENGTH2 / 2 + sign * (i + 1) * FIELD_LENGTH2 / 8 * ca;
      double y = (i + 1) * FIELD_LENGTH2 / 8 * sa;
      double theta = sign * a + M_PI / 2;
      vinfo.robot[c][i] = Orthogonal(x, y, theta);
      vinfo.number[c][i] = i + 4 * c;
    }
  }
}