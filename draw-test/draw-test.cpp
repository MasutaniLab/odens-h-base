///
///@file draw-test.cpp
///@brief Draw�N���X�̃e�X�g�v���O����
///@par Copyright
/// Copyright (C) 2019 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par ����
///- 2019/02/03 ���J �۔� �V�K�쐬
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

///draw-test���C���֐�
int main(int argc, char* argv[])
{
  //�R�}���h���C���I�v�V�����Ɛݒ�t�@�C���ǂݍ���
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
  cout << "���C�����[�v�J�n" << endl;
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

    //�L�[���́i�R���\�[���j
    int k = inkey();
    if (k == 's') {
      draw.showWindow();
      cout << "�E�B���h�E����" << endl;
    }

    //�L�[���́i�O���t�B�b�N�X�j
    int c = draw.getch();
    if (c != -1) {
      switch (c) {
      case 'i':
        drawVisionInfo = !drawVisionInfo;
        if (drawVisionInfo) {
          cout << "�}�[�J�\���iVisionInfo�j" << endl;
        } else {
          cout << "���{�b�g�\���isrInfo�j" << endl;
        }
        break;
      case 'e':
        drawEstimatedInfo = !drawEstimatedInfo;
        if (drawEstimatedInfo) {
          cout << "����l����" << endl;
        } else {
          cout << "����l�Ȃ�" << endl;
        }
        break;
      case 'p':
        draw.showPositionData();
        cout << "�ʒu�\���ؑ�" << endl;
        break;
      case 'h':
        draw.hideWindow();
        cout << "�E�B���h�E����" << endl;
        break;
      case 'r':
        draw.reverseField();
        cout << "���E���]" << endl;
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
        cout << "�I��" << endl;
        break;
      default:
        cerr << "���o�^�̃L�[: " << char(c) << endl;
        printHelp();
      }
    }
  }
  cout << "���C�����[�v�I��" << endl;
  draw.terminate();
  return 0;
}

///
///@brief �L�[���蓖�Ă�\������
///@return �Ȃ�
///
void printHelp()
{
  cout
    << "�L�[���蓖��" << endl
    << "�i�O���t�B�b�N�X�E�B���h�E�j" << endl
    << "  r: �\�������E���]" << endl
    << "  i: �}�[�J�\���ƃ��{�b�g�\���̐؂�ւ�" << endl
    << "  e: ����l�̗L���̐؂�ւ�" << endl
    << "  p: �ʒu�\���̐؂�ւ�" << endl
    << "  h: �O���t�B�b�N�X����" << endl
    << "  1: �_�`��e�X�g" << endl
    << "  2: �\���`��e�X�g" << endl
    << "  3: �����`��e�X�g" << endl
    << "  4: ��`�`��e�X�g" << endl
    << "  5: �h��Ԃ���`�`��e�X�g" << endl
    << "  6: �~�`��e�X�g" << endl
    << "  7: �h��Ԃ��~�`��e�X�g" << endl
    << "  8: ������`��e�X�g" << endl
    << "  Esc: �I��" << endl
    << "�i�R���\�[���E�B���h�E�j" << endl
    << "  s: �O���t�B�b�N�X�\��" << endl
    << "  Ctrl+c: �I��" << endl;
}

///
///@brief �������
///@param[out] sinfo
///@param[out] sinfo2
///@param[out] vinfo
///@param[in] t ����
///@return �Ȃ�
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