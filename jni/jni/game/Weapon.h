//
// Created by plaka on 07.04.2023.
//

#pragma once

#include <cstdint>
#include "common.h"


enum class eWeaponSlot : uint32_t {
    UNARMED,
    MELEE,
    HANDGUN,
    SHOTGUN,
    SMG,        // Used for drive-by's
    RIFLE,
    SNIPER,
    HEAVY,
    THROWN,
    SPECIAL,
    GIFT,
    PARACHUTE,
    DETONATOR,
};
constexpr auto NUM_WEAPON_SLOTS = static_cast<size_t>(eWeaponSlot::DETONATOR) + 1u;
enum eWeaponType : uint32_t {
    WEAPON_UNARMED = 0,
    WEAPON_BRASSKNUCKLE = 1,
    WEAPON_GOLFCLUB = 2,
    WEAPON_NIGHTSTICK = 3,
    WEAPON_KNIFE = 4,
    WEAPON_BASEBALLBAT = 5,
    WEAPON_SHOVEL = 6,
    WEAPON_POOL_CUE = 7,
    WEAPON_KATANA = 8,
    WEAPON_CHAINSAW = 9,

    // gifts
    WEAPON_DILDO1 = 10,
    WEAPON_DILDO2 = 11,
    WEAPON_VIBE1 = 12,
    WEAPON_VIBE2 = 13,
    WEAPON_FLOWERS = 14,
    WEAPON_CANE = 15,

    WEAPON_GRENADE = 16,
    WEAPON_TEARGAS = 17,
    WEAPON_MOLOTOV = 18,
    WEAPON_ROCKET = 19,
    WEAPON_ROCKET_HS = 20,
    WEAPON_FREEFALL_BOMB = 21,

    // FIRST SKILL WEAPON
    WEAPON_PISTOL = 22,
    WEAPON_PISTOL_SILENCED = 23,
    WEAPON_DESERT_EAGLE = 24,
    WEAPON_SHOTGUN = 25,
    WEAPON_SAWNOFF_SHOTGUN = 26,
    WEAPON_SPAS12_SHOTGUN = 27,
    WEAPON_MICRO_UZI = 28,
    WEAPON_MP5 = 29,
    WEAPON_AK47 = 30,
    WEAPON_M4 = 31,
    WEAPON_TEC9 = 32,

    // END SKILL WEAPONS

    WEAPON_COUNTRYRIFLE = 33,
    WEAPON_SNIPERRIFLE = 34,
    WEAPON_RLAUNCHER = 35,
    WEAPON_RLAUNCHER_HS = 36,
    WEAPON_FLAMETHROWER = 37,
    WEAPON_MINIGUN = 38,
    WEAPON_REMOTE_SATCHEL_CHARGE = 39,
    WEAPON_DETONATOR = 40,
    WEAPON_SPRAYCAN = 41,
    WEAPON_EXTINGUISHER = 42,
    WEAPON_CAMERA = 43,
    WEAPON_NIGHTVISION = 44,
    WEAPON_INFRARED = 45,
    WEAPON_PARACHUTE = 46,
    WEAPON_LAST_WEAPON = 47,

    WEAPON_ARMOUR = 48,

    // these are possible ways to die
    WEAPON_RAMMEDBYCAR = 49,
    WEAPON_RUNOVERBYCAR = 50,
    WEAPON_EXPLOSION = 51,
    WEAPON_UZI_DRIVEBY = 52,
    WEAPON_DROWNING = 53,
    WEAPON_FALL = 54,
    WEAPON_UNIDENTIFIED = 55,
    WEAPON_ANYMELEE = 56,
    WEAPON_ANYWEAPON = 57,
    WEAPON_FLARE = 58
};
enum eWeaponState : uint32_t {
    WEAPONSTATE_READY = 0,
    WEAPONSTATE_FIRING,
    WEAPONSTATE_RELOADING,
    WEAPONSTATE_OUT_OF_AMMO,
    WEAPONSTATE_MELEE_MADECONTACT,
};

#pragma pack(push, 1)
struct CWeapon
{
    eWeaponType m_nType;
    eWeaponState m_nState;
    uint32_t dwAmmoInClip;
    uint32_t m_nTotalAmmo;
    uint32_t m_nTimer;
    bool m_bFirstPersonWeaponModeSelected;
    bool m_bDontPlaceInHand;
    uint8_t pad[2];
    uintptr_t *m_pWeaponFxSys;

public:
    bool IsTypeMelee();
    bool IsType2Handed();
    bool IsTypeProjectile();

    bool HasWeaponAmmoToBeUsed();

    bool IsFiredWeapon() const;
};
#pragma pack(pop)
