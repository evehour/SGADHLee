#pragma once

class IExecute
{
public:
	virtual ~IExecute() {} // ��??????

	//�ʱ�ȭ
	virtual void Initialize() = 0;
	//���� ���� �� Update ������ ����
	virtual void Ready() = 0;
	//�Ҹ��ڿ��� ȣ��
	virtual void Destroy() = 0;

	virtual void Update() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	//D3D�� �°� ���� ����
	virtual void ResizeScreen() = 0;
};