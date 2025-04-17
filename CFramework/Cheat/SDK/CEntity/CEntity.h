#pragma once
#include "../GameSDK/GameSDK.h"

struct BoundingBox {
	int left, right, top, bottom;
};

struct BoneEntry {
	char* junk0[0xCC]{};
	float x{};
	char* junk1[0xC]{};
	float y{};
	char* junk2[0xC]{};
	float z{};
};

struct BoneArray {
	BoneEntry entry[128]{};
};

class CEntity
{
public:
	uintptr_t m_address;

	// Player
	Vector3 m_vecAbsVelocity;
	Vector3 m_vecAbsOrigin;
	int m_shieldHealth;
	int m_shieldHealthMax;
	int m_iHealth;
	int m_iTeamNum;
	int m_iMaxHealth;
	int m_lifeState;
	float m_lastvisibletime;
	Vector3 camera_origin;
	char m_szName[32]{};
	std::string m_iSignifierName;
	uintptr_t m_pBoneArray;

	// Functions
	bool Update();
	void UpdateStatic();
	bool IsPlayer();
	bool IsDead();
	bool IsSpectator();

	Vector3 vecMin();
	Vector3 vecMax();
	BoundingBox GetBoundingBoxData(Matrix& ViewMatrix);

	int GetFlag();
	float GetTimeBase();
	void SetViewAngle(const Vector2& angle);
	Vector2 GetViewAngle();
	Vector2 GetSwayAngle();
	Vector2 GetPunchAngle();
	Vector2 GetWeaponPunchAngle();
	BoneArray GetBoneArray();
	Vector3 GetBoneByID(int BoneId);
	void EnableGlow(GlowColor color, GlowMode mode);
	void DisableGlow();
};