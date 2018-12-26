#pragma once

class IExecute
{
public:
	virtual ~IExecute() {} // 왜??????

	//초기화
	virtual void Initialize() = 0;
	//버퍼 생성 등 Update 직전에 시작
	virtual void Ready() = 0;
	//소멸자에서 호출
	virtual void Destroy() = 0;

	virtual void Update() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	//D3D에 맞게 설정 예정
	virtual void ResizeScreen() = 0;
};