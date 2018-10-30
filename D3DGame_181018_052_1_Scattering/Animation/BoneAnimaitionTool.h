#pragma once

class GameModel;

class BoneAnimationTool
{
public:
	BoneAnimationTool(ExecuteValues* values, GameModel* model);
	~BoneAnimationTool();

	void Update();
	void Render();
	void PostRender();

	void SaveFile(wstring fName);
	void LoadFile(wstring fName);
private:
	struct BoneData
	{
		int BoneIdx;
		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Position;

	public:
		BoneData()
		{
			BoneIdx = 0;
			Scale = { 1,1,1 };
			Rotation = { 0,0,0,1 };
			Position = { 0,0,0 };
		}
	};

	struct AnimationData
	{
		float TimeRating;
		vector<BoneData> Datas;

		bool operator<(const AnimationData& a) const
		{
			return TimeRating < a.TimeRating;
		}
	};

	vector<AnimationData> clips;

private:
	ExecuteValues* values;
	GameModel* targetModel;

	string pickedPartName;

	struct PickedData
	{
		int BoneIdx;
		D3DXVECTOR3 Scale;
		D3DXVECTOR3 Rotation;
		D3DXQUATERNION qRotation;
		D3DXVECTOR3 Position;

	public:
		PickedData()
		{
			BoneIdx = 0;
			Scale = { 1,1,1 };
			Rotation = { 0,0,0 };
			qRotation = { 0,0,0,1 };
			Position = { 0,0,0 };
		}
	} pickedData;

private:
	bool isPlay;
	bool isRepeat;
	float playSpeedRate;
	float currentSec;
	vector<AnimationData>::iterator currentPose, nextPose;

private:
	void SaveClip();
	void SettingPose();
	void SettingStopPose(int boneIdx = -1);

	void StopAnimation();
	void LerpPart(vector<AnimationData>::iterator a, vector<AnimationData>::iterator b, float& t);
};