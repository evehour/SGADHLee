#include "stdafx.h"
#include "Rect.h"

//////////////////////////////////////////////////////////////////////////

const Rect Rect::Empty = Rect(0, 0, 0, 0);

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
//////////////////////////////////////////////////////////////////////////
Rect::Rect( void )
{
	this->X = 0;
	this->Y = 0;
	this->Width = 0;
	this->Height = 0;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 생성자
///@param x : X
///@param y : Y
///@param width : Width
///@param height : Height
//////////////////////////////////////////////////////////////////////////
Rect::Rect( int x, int y, int width, int height )
{
	this->X = x;
	this->Y = y;
	this->Width = width;
	this->Height = height;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief ==
///@param value : 렉트
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::operator==( const Rect& value ) const
{
	if(X == value.X && Y == value.Y && Width == value.Width)
		return Height == value.Height;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief !=
///@param value : 렉트
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::operator!=( const Rect& value ) const
{
	if(X == value.X && Y == value.Y && Width == value.Width)
		return Height != value.Height;
	else
		return true;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 문자열로 변환
///@param 문자열
//////////////////////////////////////////////////////////////////////////
std::wstring Rect::ToString()
{
	std::string temp;
	char val[255];

	sprintf(val, "{X:%d ", X); temp += val; 
	sprintf(val, "Y:%d ", Y); temp += val; 
	sprintf(val, "Width:%d ", Width); temp += val; 
	sprintf(val, "Height:%d}", Height); temp += val;

	return String::ToWString(temp);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 왼쪽(X)
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
int Rect::Left() const
{
	return this->X;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 오른쪽(X + Width)
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
int Rect::Right() const
{
	return this->X + this->Width;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 위쪽(Y)
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
int Rect::Top() const
{
	return this->Y;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 아래쪽(Y + Height)
///@return 결과 값
//////////////////////////////////////////////////////////////////////////
int Rect::Bottom() const
{
	return this->Y + this->Height;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 중앙(X + Width / 2, Y + Height / 2)
///@return 결과 포인트
//////////////////////////////////////////////////////////////////////////
Point Rect::Center() const
{
	return Point(X + Width / 2, Y + Height / 2);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 위치
///@return 결과 포인트
//////////////////////////////////////////////////////////////////////////
Point Rect::Location() const
{
	return Point(X, Y);
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 위치 설정
///@param value : 위치 포인트
//////////////////////////////////////////////////////////////////////////
void Rect::Location( Point value )
{
	this->X = value.X;
	this->Y = value.Y;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 빈값인지 확인
///@param 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::IsEmpty() const
{
	if(Width == 0 && Height == 0 && X == 0)
		return Y == 0;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 오프셋 설정
///@param amount : 포인트
//////////////////////////////////////////////////////////////////////////
void Rect::Offset( Point amount )
{
	X += amount.X;
	Y += amount.Y;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 오프셋 설정
///@param offsetX : 오프셋X
///@param offsetY : 오프셋Y
//////////////////////////////////////////////////////////////////////////
void Rect::Offset( int offsetX, int offsetY )
{
	X += offsetX;
	Y += offsetY;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 위치값 조정(x-hAmount, y-vAmount, w+=hAmount*2, h+=vAmount*2)
///@param horizontalAmount : 가로값
///@param verticalAmount : 세로값
//////////////////////////////////////////////////////////////////////////
void Rect::Inflate( int horizontalAmount, int verticalAmount )
{
	X -= horizontalAmount;
	Y -= verticalAmount;
	Width += horizontalAmount * 2;
	Height += verticalAmount * 2;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief x, y의 값이 영역에 포함되는지 검사
///@param x : X
///@param y : Y
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::Contains( int x, int y )
{
	if(this->X <= x && x < this->X + this->Width && this->Y <= y)
		return y < this->Y + this->Height;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 포인트의 값이 영역에 포함되는지 검사
///@param value : 포인트
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::Contains( Point value )
{
	if(this->X <= value.X && value.X < this->X + this->Width && this->Y <= value.Y)
		return value.Y < this->Y + this->Height;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 영역의 값이 현재 영역에 포함되는지 검사
///@param value : 영역
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::Contains( Rect value )
{
	if(this->X <= value.X && value.X + value.Width <= this->X + this->Width && this->Y <= value.Y)
		return value.Y + value.Height <= this->Y + this->Height;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 영역의 교차여부 검사
///@param value : 영역
///@return 결과
//////////////////////////////////////////////////////////////////////////
bool Rect::Intersects( const Rect& value )
{
	if(value.X < this->X + this->Width && this->X < value.X + value.Width && value.Y < this->Y + this->Height)
		return this->Y < value.Y + value.Height;
	else
		return false;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 영역의 교차영역
///@param value : 영역
///@return 결과 위치
//////////////////////////////////////////////////////////////////////////
Rect Rect::Intersect( const Rect& value1, const Rect& value2 )
{
	int leftValue = value1.X > value2.X ? value1.X : value2.X;
	int topValue = value1.Y > value2.Y ? value1.Y : value2.Y;
	int rigthValue = (value1.X + value1.Width) < (value2.X + value2.Width) ? (value1.X + value1.Width) : (value2.X + value2.Width);
	int bottomValue = (value1.Y + value1.Height) < (value2.Y + value2.Height) ? (value1.Y + value1.Height) : (value2.Y + value2.Height);
	
	Rect rect;
	if (rigthValue > leftValue && bottomValue > topValue)
	{
		rect.X = leftValue;
		rect.Y = topValue;
		rect.Width = rigthValue - leftValue;
		rect.Height = bottomValue - topValue;
	}
	else
	{
		rect.X = 0;
		rect.Y = 0;
		rect.Width = 0;
		rect.Height = 0;
	}
	return rect;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief 영역의 통합
///@param value1 : 영역1
///@param value2 : 영역2
//////////////////////////////////////////////////////////////////////////
Rect Rect::Union( Rect value1, Rect value2 )
{
	int leftValue = value1.X < value2.X ? value1.X : value2.X;
	int topValue = value1.Y < value2.Y ? value1.Y : value2.Y;
	int rightValue = (value1.X + value1.Width) > (value2.X + value2.Width) ? (value1.X + value1.Width) : (value2.X + value2.Width);
	int bottomValue = (value1.Y + value2.Height) > (value2.Y + value2.Height) ? (value1.Y + value2.Height) : (value2.Y + value2.Height);

	Rect rect;
	rect.X = leftValue;
	rect.Y = topValue;
	rect.Width = rightValue - leftValue;
	rect.Height = bottomValue - topValue;
	
	return rect;
}