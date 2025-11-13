#pragma once
struct InputState{
	int key[256];
	int prevKey[256];

	bool IsKeyDown(int key_code)const
	{
		return key[key_code]>=1 && prevKey[key_code]==0;
	}
	bool IsKeyStay(int key_code)const
	{
		return key[key_code] >= 1 && prevKey[key_code] >= 1;
	}
	bool IsKeyUp(int key_code)const
	{
		return key[key_code] == 0 && prevKey[key_code] >= 1;
	}
};