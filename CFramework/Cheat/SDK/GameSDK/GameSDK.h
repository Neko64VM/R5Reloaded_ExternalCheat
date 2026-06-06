#pragma once
#include <iostream>
#include <SimpleMath/SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace offset
{
	// Main
	constexpr auto dwEntityList = 0x1F33F78;
	constexpr auto dwLocalPlayer = 0x22E93F8;
	constexpr auto ViewRender = 0xD4138F0;
	constexpr auto ViewMatrix = 0x1A93D0;
	constexpr auto TimeScale = 0x1843340; // base + this (float)

	constexpr auto bIsMenuOpened = 0x27D7D80; // bool

	// https://github.com/cryotb/R5R_AimAssist_Forcer
	constexpr auto AimAssistVal = 0x1718B00;

	// CBaseEntity
	constexpr auto m_fFlags = 0x98;
	constexpr auto m_pBoneMatrix = 0xEE0;
	constexpr auto m_vecAbsVelocity = 0x140;
	constexpr auto m_localOrigin = 0x14C;
	constexpr auto m_shieldHealth = 0x170;
	constexpr auto m_shieldHealthMax = 0x174;
	constexpr auto m_iHealth = 0x3E0;
	constexpr auto m_iTeamNum = 0x3F0;
	constexpr auto m_hOwnerEntity = 0x43c;
	constexpr auto m_Collision = 0x458;
	constexpr auto m_iMaxHealth = 0x510;
	constexpr auto m_iSignifierName = 0x518;
	constexpr auto m_lifeState = 0x730;
	constexpr auto m_nSkin = 0x0e48;
	constexpr auto camera_origin = 0x1b68;
	constexpr auto m_lastvisibletime = 0x1754;
	constexpr auto m_vecPunchBase_Angle = 0x20a4;
	constexpr auto m_vecPunchWeapon_Angle = 0x20bc;
	constexpr auto m_vecPunchAngle = 0x20bc;
	constexpr auto m_vecSwayAngle = 0x2178;
	constexpr auto m_vecViewAngle = 0x2188;
	constexpr auto m_hViewModels = 0x2848;
	constexpr auto m_iObserverMode = 0x2fcc;
	constexpr auto m_hObserverTarget = 0x2fd0;
	constexpr auto m_szName = 0x3BF8;
	constexpr auto m_flFov = 0x405C; // Default: 1.f
};

struct GlowMode {
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

struct GlowColor {
	float R, G, B;
};

extern bool Vec2_Empty(const Vector2& value);
extern bool Vec3_Empty(const Vector3& value);
extern void NormalizeAngles(Vector2& angle);
extern Vector2 CalcAngle(const Vector3& src, const Vector3& dst);
extern bool WorldToScreen(Matrix ViewMatrix, const Vector2& game_size, Vector3 vIn, Vector2& vOut);