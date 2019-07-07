/**
 * @section		노말 기술
 */
#define SKILL_ID_AP_UPGRADE			0
#define SKILL_ID_GRENADE_MASTERLY	1
#define SKILL_ID_GRENADE_EXPLODE	2
#define SKILL_ID_TATICS_RELOAD		3
#define SKILL_ID_PHOENIX_FIRERATE	4
//	OLD SKILL ID
#define SKILL_ID_SPRAY_CONTROL		1

/**
 * @section		돌격병 기술
 */
#define SKILL_ID_ASSAULT_MASTERY		0
#define SKILL_ID_BREAK_LIMIT			1
#define SKILL_ID_ENCOURAGE				2
#define SKILL_ID_BREAK_LIMIT_MASTERY	3
#define SKILL_ID_ENCOURAGE_MASTERY		4
#define SKILL_ID_AMMO_CHARGE			5
#define SKILL_ID_LEADERSHIP				6
#define SKILL_ID_OVERHIT				7
#define SKILL_ID_HERO					8
//	OLD SKILL ID
#define SKILL_ID_SPRAY_CONTROL_MASTERY	2
#define SKILL_ID_FIRE_BOOST				3

/**
 * @section		저격병 기술
 */
#define SKILL_ID_ZOOMSHOT					0
#define SKILL_ID_VECTOR_BULLET				1	
#define SKILL_ID_PENETRATE_BULLET			2
#define SKILL_ID_VECTOR_BULLET_MASTERY		3
#define SKILL_ID_PENETRATE_BULLET_MASTERY	4
#define SKILL_ID_AUTOSNIPER_MASTERY			5
#define SKILL_ID_HAWKEYE					6
#define SKILL_ID_LAST_BLOW					7
#define SKILL_ID_LAST_BULLET				8
//	OLD SKILL ID
#define SKILL_ID_SNIPER_MASTERY			0
//#define SKILL_ID_AUTOSNIPER_MASTERY		1
#define SKILL_ID_EXPLODE_BULLET			2
#define SKILL_ID_EXPLODE_ACTION			3

/**
 * @section		의무병 기술
 */
#define SKILL_ID_HP_UPGRADE					0
#define SKILL_ID_MEDIC_GAS					1
#define SKILL_ID_MEDIC_BULLET				2
#define SKILL_ID_MEDIC_GAS_MASTERY			3
#define SKILL_ID_MEDIC_BULLET_MASTERY		4
#define SKILL_ID_MEDISHOT					5
#define SKILL_ID_POSIONSHOT					6
#define SKILL_ID_NANO_MACHINE_GAS			7
#define SKILL_ID_NANO_UPGRADE_MEDIC_BULLET	8
//	OLD SKILL ID
#define SKILL_ID_MEDIC_PISTOL			2
#define SKILL_ID_ZOMBIE_POISON			4

/**
 * @section		보급병 기술
 */
#define SKILL_ID_SMG_UPGRADE				0
#define SKILL_ID_AMMO_BOX					1
#define SKILL_ID_AMMO_BOX_MASTERY			2
#define SKILL_ID_SMG_SPAWN					3
#define SKILL_ID_BARRICADE_GRENADE			4
#define SKILL_ID_BARRICADE_GRENADE_MASTERY	5
#define SKILL_ID_FREEZE_BULLET				6
#define SKILL_ID_IGNITE_BULLET				7
#define SKILL_ID_ARMOR_PIERCING_BULLET		8
#define SKILL_ID_BARRICADE_EXPLOSION		9

/**
 * @section		전문병
 */
#define	SKILL_ID_SG_UPGRADE					0
#define SKILL_ID_PULSESHOT					1
#define SKILL_ID_LANDMINE					2
#define SKILL_ID_PULSESHOT_MASTERY			3
#define SKILL_ID_LANDMINE_MASTERY			4
#define SKILL_ID_BLOCKING_POWER				5
#define SKILL_ID_SG_EXPLODE_PELLET			6
#define SKILL_ID_PULSESHOT_EX				7
#define SKILL_ID_LANDMINE_EX				8

/**
 * @section		총잡이
 */
#define SKILL_ID_HG_UPGRADE					0
#define SKILL_ID_BULLETDANCE					1
#define SKILL_ID_DESPERADO					2
#define SKILL_ID_BULLETDANCE_MASTERY			3
#define SKILL_ID_DESPERADO_MASTERY			4
#define SKILL_ID_LEGSTUN					5
#define SKILL_ID_FLASHBANG_MASTERY			6
#define SKILL_ID_BULLETDANCE_EX				7
#define SKILL_ID_DESPERADO_EX				8

public void useClassActiveSkill ( int client, ClientClass clientClass, int skill_Id ) {
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	switch ( clientClass ) {
		//	Normal class skills.
	/*	case ClientClass_Normal: {
			switch ( skill_Id ) {
				case SKILL_ID_SPRAY_CONTROL:	clientClass_Normal_SprayControl ( client, skill_Id );
			}
		}	*/
		
		//	Assaulter class skills.
		case ClientClass_Assault: {
			switch ( skill_Id ) {
				case SKILL_ID_BREAK_LIMIT:		clientClass_Assault_BreakLimit ( client, skill_Id );
				case SKILL_ID_ENCOURAGE:		clientClass_Assault_Encourage ( client, skill_Id );
			}
		}
		
		//	Sniper class skills.
		case ClientClass_Sniper: {
			switch ( skill_Id ) {
				case SKILL_ID_VECTOR_BULLET:	clientClass_Sniper_ReloadSniperSpecialBullet ( client, skill_Id );
				case SKILL_ID_PENETRATE_BULLET:	clientClass_Sniper_ReloadSniperSpecialBullet ( client, skill_Id );
			}
		}
		
		//	Medic class skills.
		case ClientClass_Medic: {
			switch ( skill_Id ) {
				case SKILL_ID_MEDIC_BULLET:		clientClass_Medic_ReloadMedicBullet ( client, skill_Id );
				case SKILL_ID_MEDIC_GAS:		clientClass_Medic_DispatchMedicGas ( client, skill_Id );
			}
		}
		
		//	Supporter class skills.
		case ClientClass_Supporter: {
			switch ( skill_Id ) {
				case SKILL_ID_AMMO_BOX:			clientClass_Supporter_DispatchAmmobox ( client, skill_Id );
				case SKILL_ID_SMG_SPAWN:		clientClass_Supporter_SMGSpawn ( client, skill_Id );
				case SKILL_ID_BARRICADE_GRENADE:	clientClass_Support_GiveBarricadeGrenade ( client, skill_Id );
			}
		}
		
		//	Shotgunner class skills.
		case ClientClass_Shotgunner: {
			switch ( skill_Id ) {
				case SKILL_ID_PULSESHOT:		clientClass_Shotgunner_ReloadPulseshot ( client, skill_Id );
				case SKILL_ID_LANDMINE:			clientClass_Shotgunner_GiveLandmineGrenade ( client, skill_Id );
			}
		}
		
		//	Gunslinger class skills.
		case ClientClass_Gunslinger: {
			switch ( skill_Id ) {
				case SKILL_ID_BULLETDANCE:		clientClass_Gunslinger_StartBulletTime ( client, skill_Id );
				case SKILL_ID_DESPERADO:			clientClass_Gunslinger_StartDesperado ( client, skill_Id );
			}
		}
	}
}