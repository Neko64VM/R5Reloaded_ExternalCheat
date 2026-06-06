#include "CEntity.h"
#include "../../../Framework/Memory/Memory.h"

bool CEntity::Update()
{
	std::vector<uint8_t> buffer;

	m_vecAbsOrigin = m.Read<Vector3>(m_address + offset::m_localOrigin);
	m_iHealth = m.Read<int>(m_address + offset::m_iHealth);

	if (this->IsDead() || Vec3_Empty(m_vecAbsOrigin) || m_iHealth <= 0)
		return false;

	m_vecAbsVelocity = m.Read<Vector3>(m_address + offset::m_vecAbsVelocity);
	m_shieldHealth = m.Read<int>(m_address + offset::m_shieldHealth);
	m_lastvisibletime = m.Read<float>(m_address + offset::m_lastvisibletime);
	camera_origin = m.Read<Vector3>(m_address + offset::camera_origin);

	return true;
}

void CEntity::UpdateStatic()
{
	m_pBoneArray = m.Read<uintptr_t>(m_address + offset::m_pBoneMatrix);
	m_iTeamNum = m.Read<int>(m_address + offset::m_iTeamNum);
	m_iMaxHealth = m.Read<int>(m_address + offset::m_iMaxHealth);
	m_shieldHealthMax = m.Read<int>(m_address + offset::m_shieldHealthMax);

	// Name
	uintptr_t NameAddress = m.Read<uintptr_t>(m_address + offset::m_szName);
	m.ReadString(NameAddress, m_szName, sizeof(m_szName));
}

void CEntity::GetName()
{
	uintptr_t NameAddress = m.Read<uintptr_t>(m_address + offset::m_szName);
	m.ReadString(NameAddress, m_szName, sizeof(m_szName));
}

CEntity CEntity::GetObservingTarget(const uintptr_t& entitylist)
{
	CEntity result{0};

	uint32_t Index = m.Read<uint32_t>(m_address + offset::m_hObserverTarget) & 0xFFFF;
	result.m_address = m.Read<uintptr_t>(entitylist + ((Index - 1) * 0x20));

	return result;
}

BoundingBox CEntity::GetBoundingBoxData(Matrix& ViewMatrix, const Vector2 gameSize)
{
	BoundingBox bbOut{};
	const Collision collision = GetCollision();
	Vector3 min = collision.vecMin + m_vecAbsOrigin;
	Vector3 max = collision.vecMax + m_vecAbsOrigin;

	Vector2 flb, brt, blb, frt, frb, brb, blt, flt;

	Vector3 points[8] = { Vector3(min.x, min.y, min.z), Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z),
				Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, max.z), Vector3(min.x, max.y, max.z),
				Vector3(min.x, min.y, max.z), Vector3(max.x, min.y, max.z) };

	if (!WorldToScreen(ViewMatrix, gameSize, points[3], flb) || !WorldToScreen(ViewMatrix, gameSize, points[5], brt) ||
		!WorldToScreen(ViewMatrix, gameSize, points[0], blb) || !WorldToScreen(ViewMatrix, gameSize, points[4], frt) ||
		!WorldToScreen(ViewMatrix, gameSize, points[2], frb) || !WorldToScreen(ViewMatrix, gameSize, points[1], brb) ||
		!WorldToScreen(ViewMatrix, gameSize, points[6], blt) || !WorldToScreen(ViewMatrix, gameSize, points[7], flt))
		return bbOut;

	Vector2 vec2_array[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	bbOut.left = flb.x;
	bbOut.top = flb.y;
	bbOut.right = flb.x;
	bbOut.bottom = flb.y;

	for (auto j = 1; j < 8; ++j)
	{
		if (bbOut.left > vec2_array[j].x)
			bbOut.left = vec2_array[j].x;
		if (bbOut.bottom < vec2_array[j].y)
			bbOut.bottom = vec2_array[j].y;
		if (bbOut.right < vec2_array[j].x)
			bbOut.right = vec2_array[j].x;
		if (bbOut.top > vec2_array[j].y)
			bbOut.top = vec2_array[j].y;
	}

	return bbOut;
}

bool CEntity::IsPlayer()
{
	return m_iSignifierName == "player";
}

bool CEntity::IsSpectator()
{
	return m.Read<int>(m_address + offset::m_iObserverMode) == 5;
}

uintptr_t CEntity::GetCurrentWeapon(const uintptr_t& entitylist)
{
	uint32_t Index = m.Read<uint32_t>(m_address + 0x1704) & 0xFFFF;

	return m.Read<uintptr_t>(entitylist + ((Index - 1) * 0x20));
}

std::string CEntity::GetWeaponName(const uintptr_t& entitylist)
{
	char szWeapon[64]{};
	uintptr_t latestWeapon = GetCurrentWeapon(entitylist);
	uintptr_t pWeaponName = m.Read<uintptr_t>(latestWeapon + 0x1870);
	m.ReadString(pWeaponName, szWeapon, sizeof(szWeapon));

	return std::string(szWeapon);
}

uintptr_t CEntity::GetHandViewModel(const uintptr_t& entitylist)
{
	uint32_t handIndex = m.Read<uint32_t>(m_address + 0x2848 + 0xC) & 0xFFFF;
	
	return m.Read<uintptr_t>(entitylist + ((handIndex - 1) * 0x20));
}

uintptr_t CEntity::GetWeaponViewModel(const uintptr_t& entitylist)
{
	uint32_t weaponIndex = m.Read<uint32_t>(m_address + offset::m_hViewModels) & 0xFFFF;

	return m.Read<uintptr_t>(entitylist + ((weaponIndex - 1) * 0x20));
}

Collision CEntity::GetCollision()
{
	return m.Read<Collision>(m_address + offset::m_Collision + 0x10);
}

int CEntity::GetFlag()
{
	return m.Read<int>(m_address + offset::m_fFlags);
}

bool CEntity::IsDead()
{
	return m.Read<int>(m_address + offset::m_lifeState) > 0;
}

bool CEntity::IsVisible(const float& baseTime)
{
	return m_lastvisibletime + 0.3f >= baseTime;
}

void CEntity::SetViewAngle(const Vector2& angle)
{
	m.Write<Vector2>(m_address + offset::m_vecViewAngle, angle);
}

Vector2 CEntity::GetViewAngle()
{
	return m.Read<Vector2>(m_address + offset::m_vecViewAngle);
}

Vector2 CEntity::GetSwayAngle()
{
	return m.Read<Vector2>(m_address + offset::m_vecSwayAngle);
}

Vector2 CEntity::GetPunchAngle()
{
	return m.Read<Vector2>(m_address + offset::m_vecPunchAngle);
}

Vector2 CEntity::GetWeaponPunchAngle()
{
	return m.Read<Vector2>(m_address + offset::m_vecPunchWeapon_Angle);
}

float CEntity::GetTimeBase()
{
	return m.Read<float>(m_address + 0x1D18);
}

Vector3 CEntity::GetBoneByID(int BoneId)
{
	Vector3 vOut = m_vecAbsOrigin;
	CBone bonePos = m.Read<CBone>(m_pBoneArray + (BoneId * 0x30));

	vOut.x += bonePos.x;
	vOut.y += bonePos.y;
	vOut.z += bonePos.z;

	return vOut;
}

CPlayerBoneArray CEntity::GetBoneArray()
{
	return m.Read<CPlayerBoneArray>(m_pBoneArray);
}

void CEntity::EnableGlow(GlowColor color, GlowMode mode)
{
	m.Write<int>(m_address + 0x310, 1);
	m.Write<int>(m_address + 0x320, 2);
	m.Write<GlowMode>(m_address + 0x27C, mode);
	m.Write<GlowColor>(m_address + 0x1D0, color);
}

void CEntity::DisableGlow()
{
	m.Write<int>(m_address + 0x310, 0);
	m.Write<int>(m_address + 0x320, 0);
}