#pragma once
#include "../GameSDK/GameSDK.h"

struct BoundingBox {
	int left, right, top, bottom;
};

struct alignas(0x20) Collision {
	Vector3 vecMin{};
	Vector3 vecMax{};
};

struct CBone {
	char padding0[0xCC]{};
	float x{};
	char padding1[0xC]{};
	float y{};
	char padding2[0xC]{};
	float z{};
};

struct CPlayerBoneArray {
	CBone entry[128]{};
};

class CEntity
{
public:
	// コンストラクタでm_addressに代入し初期化
	CEntity(uintptr_t address) : m_address(address) {};

	uintptr_t m_address{ 0 };

	// Player
	Vector3 m_vecAbsVelocity{};
	Vector3 m_vecAbsOrigin{};
	int m_shieldHealth{ 0 };
	int m_shieldHealthMax{ 0 };
	int m_iHealth{ 0 };
	int m_iTeamNum{ 0 };
	int m_iMaxHealth{ 0 };
	int m_lifeState{ 0 };
	float m_lastvisibletime{ 0.f };
	Vector3 camera_origin{};
	char m_szName[32]{};
	std::string m_iSignifierName;
	uintptr_t m_pBoneArray{ 0 };

	// Functions
	bool Update();
	void UpdateStatic();
	bool IsPlayer();
	bool IsDead();
	bool IsVisible(const float& baseTime);
	bool IsSpectator();

	Collision GetCollision();
	BoundingBox GetBoundingBoxData(Matrix& ViewMatrix, const Vector2 gameSize);

	CEntity GetObservingTarget(const uintptr_t& entitylist);
	uintptr_t GetCurrentWeapon(const uintptr_t& entitylist);
	uintptr_t GetHandViewModel(const uintptr_t& entitylist);
	uintptr_t GetWeaponViewModel(const uintptr_t& entitylist);

	int GetFlag();
	float GetTimeBase();
	void GetName();
	void SetViewAngle(const Vector2& angle);
	Vector2 GetViewAngle();
	Vector2 GetSwayAngle();
	Vector2 GetPunchAngle();
	Vector2 GetWeaponPunchAngle();
	std::string GetWeaponName(const uintptr_t& entitylist);
	CPlayerBoneArray GetBoneArray();
	Vector3 GetBoneByID(int BoneId);
	void EnableGlow(GlowColor color, GlowMode mode);
	void DisableGlow();
};