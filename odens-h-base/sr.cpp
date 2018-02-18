///
///@file sr.cpp
///@brief Orthogonal構造体のメンバ関数
///@par Copyright
/// Copyright (C) 2016, 2017 Team ODENS, Masutani Lab, Osaka Electro-Communication University
///@par 履歴
///- 2017/12/31 升谷 保博 関数を減らす
///- 2017/03/04 升谷 保博 odens-h-base
///- 2016/03/11 升谷 保博 odens-h2
///@addtogroup sr
///@{
///

///
///@mainpage
///SSL Humanoidのシステムの行動決定のプログラムの基本ライブラリ
///- SSL-Visionサーバからマルチキャスト通信で位置情報を受け取る
///- レフェリーボックスからマルチキャスト通信でレフェリー情報を受け取る
///- 位置情報とレフェリー情報に基づきコマンドを決定する
///- シリアルポートを介してロボットへコマンドを送る
///

#include <cmath>
#include "sr.h"
#include "util.h"
#include"draw.h"
#include"robot.h"
#include "estimator.h"

using namespace std;

namespace odens {

    ///
///@brief 引数からオブジェクトまでの距離
///@param[in] p 点
///@return 距離
///
double Orthogonal::distance(const Orthogonal &p) const
{
	if (this->isInvisible()) {
		cerr << "Orthogonal::distance() このオブジェクトは見えていない" << endl;
	}
	if (p.isInvisible()) {
		cerr << "Orthogonal::distance() 引数は見えていない" << endl;
	}
	return sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y));
}

///
///@brief 原点からオブジェクトまでの距離
///@return 距離
///
double Orthogonal::distance() const
{
	if (this->isInvisible()) {
		cerr << "Orthogonal::distance() このオブジェクトは見えていない" << endl;
	}
	return sqrt(x*x+y*y);
}

///
///@brief 引数からオブジェクトへの方向がx軸と成す角度
///@param[in] p 点
///@return 角度 [rad]
///
double Orthogonal::angle(const Orthogonal &p) const
{
	if (this->isInvisible()) {
		cerr << "Orthogonal::angle() このオブジェクトは見えていない" << endl;
	}
	if (p.isInvisible()) {
		cerr << "Orthogonal::angle() 引数は見えていない" << endl;
	}
	return atan2(y-p.y, x-p.x);
}

///
///@brief 原点からオブジェクトへの方向がx軸と成す角度
///@return 角度 [rad]
///
double Orthogonal::angle() const
{
	if (this->isInvisible()) {
		cerr << "Orthogonal::angle() このオブジェクトは見えていない" << endl;
	}
	return atan2(y, x);
}

///
///@brief 引数が表す座標系への座標変換
///@param[in] p 座標系
///@return 座標変換の結果
///
Orthogonal Orthogonal::transform(const Orthogonal &p) const
{
	Orthogonal r(0,0,0);
	if (this->isInvisible()) {
		cerr << "Orthogonal::transform() このオブジェクトは見えていない" << endl;
		r.vanish();
	}
	if (p.isInvisible()) {
		cerr << "Orthogonal::transform() 引数は見えていない" << endl;
		r.vanish();
	}
	if (r.isInvisible()) {
		return r;
	}
	double cost = cos(p.theta);
	double sint = sin(p.theta);
	r.x = + (x-p.x)*cost + (y-p.y)*sint;
	r.y = - (x-p.x)*sint + (y-p.y)*cost;
	r.theta = normalizeAngle(theta-p.theta);
	return r;
}

///
///@brief 引数が表す座標系からの座標変換
///@param[in] p 座標系
///@return 座標変換の結果
///
Orthogonal Orthogonal::inverseTransform(const Orthogonal &p) const
{
	Orthogonal r(0,0,0);
	if (this->isInvisible()) {
		cerr << "Orthogonal::inverseTransform() このオブジェクトは見えていない" << endl;
		r.vanish();
	}
	if (p.isInvisible()) {
		cerr << "Orthogonal::inverseTransform() 引数は見えていない" << endl;
		r.vanish();
	}
	if (r.isInvisible()) {
		return r;
	}
	double cost = cos(p.theta);
	double sint = sin(p.theta);
	r.x = p.x + x*cost - y*sint;
	r.y = p.y + x*sint + y*cost;
	r.theta = normalizeAngle(theta+p.theta);
	return r;
}

///
///@brief オブジェクトがフィールド内にあるか？
///@retval  false フィールド内にない
///@retval  true フィールド内にある
///
bool Orthogonal::isInField() const
{
	return (-FIELD_LENGTH2 < x) && (x < FIELD_LENGTH2)
		&& (-FIELD_WIDTH2 < y) && (y < FIELD_WIDTH2);
}

///
///@brief オブジェクトが自チームのゴールエリア内にあるか？
///@retval  false ゴールエリア内にない
///@retval  true ゴールエリア内にある
///
bool Orthogonal::isInOurGoalArea() const
{
	return (x < -FIELD_LENGTH2+GOAL_AREA_LENGTH) 
		&& (-GOAL_AREA_WIDTH2 < y) && (y < GOAL_AREA_WIDTH2);
}

///
///@brief オブジェクトが相手チームのゴールエリア内にあるか？
///@retval  false ゴールエリア内にない
///@retval  true ゴールエリア内にある
///
bool Orthogonal::isInTheirGoalArea() const
{
	return (FIELD_LENGTH2-GOAL_AREA_LENGTH < x) 
		&& (-GOAL_AREA_WIDTH2 < y) && (y < GOAL_AREA_WIDTH2);
}

///
///@brief 出力ストリームに対する<<演算子のオーバロード
///@param[in] os 出力ストリーム
///@param[in] p 出力対象
///@return 結果の出力ストリーム
///
ostream& operator<<(ostream& os, const Orthogonal& p)
{
	os << p.x << ' ' << p.y << ' ' << p.theta;
	return os;
}

} //namespace odens

///@} doxygenのためのコメント（消してはいけない）
