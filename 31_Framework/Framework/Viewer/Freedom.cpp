#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom()
	:Camera()
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	// 오른쪽 버튼
	if (Mouse::Get()->Press(1) == false) return;

	// 상대공간
	Vector3 f = Forward();
	Vector3 u = Up();
	Vector3 r = Right();

	// 절대공간에서 이동됨
	//Vector3 f = Vector3(0, 0, 1);
	//Vector3 u = Vector3(0, 1, 0);
	//Vector3 r = Vector3(1, 0, 0);

	// Move
	{
		Vector3 P;
		Position(&P);

		if (Keyboard::Get()->Press('W'))
			P = P + f * move * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			P = P - f * move * Time::Delta();

		if (Keyboard::Get()->Press('D'))
			P = P + r * move * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			P = P - r * move * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			P = P + u * move * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			P = P - u * move * Time::Delta();

		Position(P);
	}

	// Rotation
	{
		Vector3 R;
		Rotation(&R);

		Vector3 val = Mouse::Get()->GetMoveValue();
		// 마우스가 위아래로(val.y) 움직이면 머리가 위아래(R.x 회전)로 끄덕인다.
		R.x = R.x + val.y * rotation * Time::Delta();
		// 마우스가 좌우로 가면 카메라도 좌우(y축 회전)
		R.y = R.y + val.x * rotation * Time::Delta();
		R.z = 0.0f;

		Rotation(R);
	}
}

void Freedom::Speed(float move, float rotation)
{
	this->move = move;
	this->rotation = rotation;
}
