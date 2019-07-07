#define GUNSLINGER_BULLETDANCE_ACTIVE_TIME			5.0		/**< 불릿 댄스 지속 시간 */
#define GUNSLINGER_BULLETDANCE_PASSIVE_DAMAGE		0.25	/**< 불릿 댄스 패시브 데미지 */
#define GUNSLINGER_BULLETDANCE_PASSIVE_RELOADSPD	0.15	/**< 불릿 댄스 패시브 재장전 속도 */
#define GUNSLINGER_BULLETDANCE_MASTERY_DURATION_ADD	0.3		/**< 불릿 댄스 마스터리를 배울 시 불릿 타임 지속 시간 증가 값 (초) */
#define GUNSLINGER_BULLETDANCE_DAMAGE				50		/**< 불릿 댄스 근처 적군 피해량 */
#define GUNSLINGER_BULLETDANCE_VICINITY_DAMAGE_TICKS	0.3	/**< 불릿 타임 근처 적군 피해 틱 시간 (초) */

#define GUNSLINGER_DESPERADO_ACTIVE_TIME		3.0		/**< 데스페라도 지속 시간 */
#define GUNSLINGER_DESPERADO_DAMAGE				400.0	/**< 데스페라도 피해량 */
#define GUNSLINGER_DESPERADO_AMMO_AMOUNTS		3		/**< 데스페라도 특수탄 갯수 */
#define GUNSLINGER_DESPERADO_TARGET_DISTANCE	2500.0	/**< 데스페라도 타겟팅 범위 */
#define GUNSLINGER_DESPERADO_TARGET_AIM_FOV		120.0	/**< 데스페라도 타겟팅 에임 시야 범위 */
#define GUNSLINGER_DESPERADO_TARGET_EFFECT_DURATION	0.5	/**< 데스페라도 타겟팅 파티클 지속 시간 */
#define GUNSLINGER_DESPERADO_TARGET_KNOCKBACK	800.0	/**< 데스페라도 타겟 넉백 */
#define GUNSLINGER_DESPERADO_PASSIVE_DAMAGE		0.8		/**< 데스페라도 패시브 데미지 */
#define GUNSLINGER_DESPERADO_PASSIVE_FLASHBANG_STUN_DISTANCE	250.0	/**< 데스페라도 패시브 플래시뱅 스턴 거리 */
#define GUNSLINGER_DESPERADO_PASSIVE_FLASHBANG_STUN_SCALE	2.0			/**< 데스페라도 패시브 플래시뱅 스턴 시간 */
#define GUNSLINGER_DESPERADO_EX_TARGET_DMG_DURATION			0.3			/**< 데스페라도 마스터 기술 틱 데미지 값 */

#define GUNSLINGER_BULLETDANCE_DAMAGE_DISTANCE		280.0		/**< 불릿 댄스 데미지 거리 */
#define GUNSLINGER_BULLETDANCE_VELMODIF_SCALE		0.05		/**< 불릿 댄스 맞을 때 경직 */

#define GUNSLINGER_LEGSTUN_DEFAULT_SCALE			0.2		/**< 영악함 기본 경직 값 */

#define GUNSLINGER_FLASHBANG_MASTERY_KNOCKBACK		550.0	/**< 플래쉬뱅 넉백 */

#define GUNSLINGER_BULLETDANCE_EX_KNOCKBACK_SCALE	1000.0	/**< 단말마 넉백 */
#define GUNSLINGER_BULLETDANCE_EX_VELMODIF_SCALE	0.1	/**< 단말마 경직 */

#define SOUND_GUNSLINGER_BULLETDANCE_EX_EXPLODE	"qufnr/gunslinger/gunslinger_bulletdance_explode.mp3"
#define SOUND_GUNSLINGER_DESPERADO_FIRE			"qufnr/gunslinger/gunslinger_desperado_fire.mp3"
#define SOUND_GUNSLINGER_USE_BULLETTIME			"qufnr/gunslinger/gunslinger_use_bullettime.mp3"
#define SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MIN	1
#define SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MAX	4
#define SOUND_GUNSLINGER_BULLETDANCE			"qufnr/gunslinger/gunslinger_bulletdance_0%d.mp3"

static bool g_bGunslingerUseBulletTime[MAXPLAYERS + 1] = { false, ... };
static int g_iGunslingerDesperadoAmmo[MAXPLAYERS + 1] = { 0, ... };
static float g_fGunslingerDesperadoTargetEffDuration[MAXPLAYERS + 1] = { 0.0, ... };
static float g_fGunslingerDesperadoExTicks[MAXPLAYERS + 1] = { 0.0, ... };
static bool g_bGunslingerPistolFired[MAXPLAYERS + 1] = { false, ... };
static bool g_bGunslingerTakeBulletDance[MAXPLAYERS + 1][MAXPLAYERS + 1];
static int g_iGunslingerBulletDanceExEff[MAXPLAYERS + 1] = { INVALID_ENT_REFERENCE, ... };

void ClientClass_Gunslinger_PrecacheSounds () {
	PrecacheSound ( SOUND_GUNSLINGER_BULLETDANCE_EX_EXPLODE, true );
	PrecacheSound ( SOUND_GUNSLINGER_DESPERADO_FIRE, true );
	PrecacheSound ( SOUND_GUNSLINGER_USE_BULLETTIME, true );
	static char format[256];
	for ( int i = SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MIN; i <= SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MAX; i ++ ) {
		Format ( format, sizeof ( format ), SOUND_GUNSLINGER_BULLETDANCE, i );
		PrecacheSound ( format, true );
	}
}

void ClientClass_Gunslinger_ClearGunslingerData ( int client ) {
	for ( int i = 0; i < sizeof ( g_bGunslingerTakeBulletDance ); i ++ )
		if ( g_bGunslingerTakeBulletDance[i][client] )
			g_bGunslingerTakeBulletDance[i][client] = false;
}

/**
 * HG 숙련
 */
stock float clientClass_Gunslinger_PistolUpgrade ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		//	HG 숙련을 배웠을 경우
		if ( g_iSkill[clientClass][SKILL_ID_HG_UPGRADE][client] > 0 ) {
			if ( weapon != -1 ) {
				char edictname[32];
				GetEdictClassname ( weapon, edictname, sizeof ( edictname ) );
				bool pistol = false;
				for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
					if ( StrContains ( edictname, g_strClassWeapon_Gunslinger[i], false ) != -1 )
						pistol = true;
				
				//	권총이 맞을 경우?
				if ( pistol ) {
					return dmg * ( ( ROUND_DMG_PERCENTAGE + ( g_iSkill[clientClass][SKILL_ID_HG_UPGRADE][client] * g_fSkillAction[clientClass][SKILL_ID_HG_UPGRADE] ) ) );
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 데스페라도 패시브
 * @note 데미지 증가
 */
stock float clientClass_Gunslinger_DesperadoPassiveDamage ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		if ( g_iSkill[clientClass][SKILL_ID_DESPERADO][client] > 0 ) {
			if ( weapon != -1 ) {
				char edictname[32];
				GetEdictClassname ( weapon, edictname, sizeof ( edictname ) );
				bool pistol = false;
				if ( StrContains ( edictname, "deagle", false ) != -1 ||
					StrContains ( edictname, "revolver", false ) != -1 )
					pistol = true;
				
				//	권총이 맞을 경우?
				if ( pistol ) {
					return dmg * ( ( ROUND_DMG_PERCENTAGE + GUNSLINGER_DESPERADO_PASSIVE_DAMAGE ) );
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 섬광탄 투척 시 SDKHook 적용
 * @note ZR_OnThrowLightGrenade
 */
void clientClass_Gunslinger_LightGrenadeSDKHooks ( int entity, int owner ) {
	if ( loringlib_IsValidClient ( owner ) )
		SDKHook ( entity, SDKHook_StartTouch, lightGrenadeTouchWorld );
}

public void lightGrenadeTouchWorld ( int entity, int other ) {
	if ( IsValidEdict ( entity ) ) {
		int owner = loringlib_GetEntityOwner2 ( entity );
		if ( loringlib_IsValidClient ( owner ) ) {
			if ( g_iClientClass[owner] == ClientClass_Gunslinger ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				//	비열한 수법을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_FLASHBANG_MASTERY][owner] > 0 ) {
					clientClass_Gunslinger_LightGrenadeStun ( entity, owner );
					AcceptEntityInput ( entity, "Kill" );
				}
			}
		}
	}
}

/**
 * 데스페라도 패시브
 * @note 섬광탄 폭발 시 근처 좀비 스턴
 */
void clientClass_Gunslinger_LightGrenadeStun ( int entity, int owner ) {
	if ( loringlib_IsValidClient ( owner ) ) {
		if ( ZR_IsClientHuman ( owner ) ) {
			if ( g_iClientClass[owner] == ClientClass_Gunslinger ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				//	비열한 수법을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_FLASHBANG_MASTERY][owner] > 0 ) {
					float entPos[3];
					loringlib_GetEntityOriginEx ( entity, entPos );
					entPos[2] += 2.5;
					TE_SetupBeamRingPoint ( entPos, 1.0, 150.0, PrecacheModel ( "materials/particle/beam_taser.vmt" ), 0, 1, 1, 0.3, 20.0, 0.2, { 255, 255, 255, 255 }, 1, 0 );
					TE_SendToAll ();
					
					loringlib_CreateParticleEx ( entity, 0, entPos, NULL_VECTOR, "gunslinger_flash", false, 1.0 );
					PrecacheSound ( "weapons/flashbang/flashbang_explode1.wav" );
					EmitAmbientSound ( "weapons/flashbang/flashbang_explode1.wav", entPos );
					
					float dir[3];
					dir[0] = GetRandomFloat ( -1.0, 1.0 );
					dir[1] = GetRandomFloat ( -1.0, 1.0 );
					dir[2] = GetRandomFloat ( -1.0, 1.0 );
					TE_SetupSparks ( entPos, dir, 20, 5 );
					TE_SendToAll ();
					
					float targetEye[3], targetPos[3];
					float duration = GUNSLINGER_DESPERADO_PASSIVE_FLASHBANG_STUN_SCALE + ( float ( g_iSkill[clientClass][SKILL_ID_FLASHBANG_MASTERY][owner] ) * g_fSkillAction[clientClass][SKILL_ID_FLASHBANG_MASTERY] );
					
					for ( int target = 1; target <= MaxClients; target ++ ) {
						if ( loringlib_IsValidClient__PlayGame ( target ) ) {
							if ( !ZR_IsClientHuman ( target ) ) {
								GetClientAbsOrigin ( target, targetPos );
								if ( GetVectorDistance ( targetPos, entPos ) <= GUNSLINGER_DESPERADO_PASSIVE_FLASHBANG_STUN_DISTANCE ) {
									GetClientEyePosition ( target, targetEye );
									targetEye[2] += 5.0;
									loringlib_CreateParticleEx ( target, 0, targetEye, NULL_VECTOR, "gunslinger_flash_star", true, duration );
									
									loringlib_KnockbackToClient ( owner, target, GUNSLINGER_FLASHBANG_MASTERY_KNOCKBACK, true );
									DataPack data;
									CreateDataTimer ( 0.1, timerGunslingerFlashbangStunDisable, data, TIMER_FLAG_NO_MAPCHANGE );
									data.WriteFloat ( GetGameTime () + duration );
									data.WriteCell ( target );
									
								//	loringlib_ShowFadeUserMessage ( target, 1, 1, { 230, 200, 90, 80 } );
								//	data.WriteFloat ( loringlib_GetEntityLaggedmovement ( target ) );
								//	data.WriteFloat ( GetEntityGravity ( target ) );
									
								//	SetEntityMoveType ( target, MOVETYPE_NONE );
								//	loringlib_SetEntityLaggedmovement ( target, 0.0 );
								//	SetEntityGravity ( target, 1.0 );
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 * 플레시뱅 스턴 종료
 */
public Action timerGunslingerFlashbangStunDisable ( Handle timer, DataPack dp ) {
	dp.Reset ();
	
	static float duration;
	static int client;
	duration = dp.ReadFloat ();
	client = dp.ReadCell ();
	
	if ( GetGameTime () >= duration ) {
		if ( loringlib_IsValidClient ( client ) )
			loringlib_ShowFadeUserMessage ( client, 1, 1, { 0, 0, 0, 0 } );
		
		return Plugin_Stop;
	}
	
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !IsPlayerAlive ( client ) || ZR_IsClientHuman ( client ) ) {
			loringlib_ShowFadeUserMessage ( client, 1, 1, { 0, 0, 0, 0 } );
			return Plugin_Stop;
		}
		
		SetEntPropFloat ( client, Prop_Send, "m_flVelocityModifier", 0.0 );
	}
	
	return Plugin_Continue;
}



/**
 * 불릿 타임 패시브
 * @note 데미지 증가
 */
stock float clientClass_Gunslinger_BulletTimePassiveDamage ( int client, float dmg, int weapon ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 ) {
			if ( weapon != -1 ) {
				char edictname[32];
				GetEdictClassname ( weapon, edictname, sizeof ( edictname ) );
				bool pistol = false;
				for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
					if ( StrContains ( edictname, g_strClassWeapon_Gunslinger[i], false ) != -1 )
						pistol = true;
				
				if ( StrContains ( edictname, "deagle", false ) != -1 ||
					StrContains ( edictname, "revolver", false ) != -1 )
					pistol = false;
				
				//	권총이 맞을 경우?
				if ( pistol ) {
					return dmg * ( ( ROUND_DMG_PERCENTAGE + GUNSLINGER_BULLETDANCE_PASSIVE_DAMAGE ) );
				}
			}
		}
	}
	
	return dmg;
}

/**
 * 불릿 타임 패시브
 * @note 재장전 속도 증가
 */
stock float clientClass_Gunslinger_BulletTimePassiveReloadSpd ( int client ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 ) {
			char edictname[32];
			loringlib_GetActiveWeaponName ( client, edictname, sizeof ( edictname ) );
			bool pistol = false;
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
				if ( StrContains ( edictname, g_strClassWeapon_Gunslinger[i], false ) != -1 )
					pistol = true;
			
			if ( StrContains ( edictname, "deagle", false ) != -1 ||
				StrContains ( edictname, "revolver", false ) != -1 )
				pistol = false;
			
			//	권총이 맞을 경우?
			if ( pistol ) {
				return GUNSLINGER_BULLETDANCE_PASSIVE_RELOADSPD;
			}
		}
	}
	
	return 0.0;
}

/**
 * 불릿 타임 패시브
 * @note 권총 자동 발사
 */
void clientClass_Gunslinger_SetFullautoPistol ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			if ( ZR_IsClientHuman ( client ) ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 ) {
					static char activeName[32];
					loringlib_GetActiveWeaponName ( client, activeName, sizeof ( activeName ) );
					
					bool pistol = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
						if ( StrContains ( g_strClassWeapon_Gunslinger[i], activeName, false ) != -1 )
							pistol = true;
					
					if ( pistol )
						SetEntProp ( client, Prop_Send, "m_iShotsFired", 0 );
				}
			}
		}
	}
}

/**
 * 불릿 타임 사용
 */
void clientClass_Gunslinger_StartBulletTime ( int client, int skill ) {
	//	전문병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Gunslinger )
		return;
	
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	//	기술을 사용중일 경우
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	//	기술 쿨이 있을 경우
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	//	AP가 부족할 경우
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	
	float duration = GUNSLINGER_BULLETDANCE_ACTIVE_TIME;
	if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] > 0 )
		duration += float ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] ) * GUNSLINGER_BULLETDANCE_MASTERY_DURATION_ADD;
	
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	pos[2] += 2.0;
	
	if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 )
		loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_exbulletdance_aura", true, duration );
	else
		loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_bullettime2", true, duration );
	EmitAmbientSound ( SOUND_GUNSLINGER_USE_BULLETTIME, pos, client );
	
	g_bGunslingerUseBulletTime[client] = true;
	
	qufnrTools_DisplayProgressBar ( client, duration );
	CreateTimer ( duration, timerBulletTimeDisable, GetClientUserId ( client ), TIMER_FLAG_NO_MAPCHANGE );
	g_bGunslingerPistolFired[client] = false;
	
//	CreateTimer ( GUNSLINGER_BULLETDANCE_VICINITY_DAMAGE_TICKS, timerBulletTimeVicinityDamage, GetClientUserId ( client ), TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * 불릿 댄스 사용 시 발사 마다 근처의 적군 피해 주기
 */
/*public Action timerBulletTimeVicinityDamage ( Handle timer, int userid ) {
	static int client;
	client = GetClientOfUserId ( userid );
	
	if ( client < 1 || client > MaxClients )
		return Plugin_Stop;
	
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( !ZR_IsClientHuman ( client ) )
			return Plugin_Stop;
	
		int clientClass = view_as<int> ( ClientClass_Gunslinger );
		if ( g_iClientClass[client] != ClientClass_Gunslinger ||
			!g_bGunslingerUseBulletTime[client] ||
			!g_bSkillUse[clientClass][SKILL_ID_BULLETDANCE][client] )
			return Plugin_Stop;
		
		static char weapon[32];
		static int active = -1;
		active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
		
		bool pistol = false;
		for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
			if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false ) != -1 )
				pistol = true;
		
		if ( pistol && active != -1 ) {
			if ( !loringlib_IsWeaponReloading ( active ) && g_bGunslingerPistolFired[client] && loringlib_GetClip1 ( active ) > 0 ) {
				float pos[3];
				GetClientAbsOrigin ( client, pos );
				pos[2] += 30.0;
				if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 )
					loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_exbulletdance", true, 0.5 );
				else
					loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_bulletdance", true, 0.5 );
				
				static char sound[256];
				Format ( sound, sizeof ( sound ), SOUND_GUNSLINGER_BULLETDANCE, GetRandomInt ( SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MIN, SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MAX ) );
				EmitAmbientSound ( sound, pos, client );
				
				float targetPos[3];
				for ( int target = 1; target <= MaxClients; target ++ ) {
					if ( loringlib_IsValidClient__PlayGame ( target ) ) {
						if ( !ZR_IsClientHuman ( target ) ) {
							if ( loringlib_IsTargetInSightRange ( client, target, 360.0, GUNSLINGER_BULLETDANCE_DAMAGE_DISTANCE ) ) {
								loringlib_MakeDamage ( client, target, GUNSLINGER_BULLETDANCE_DAMAGE + ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE_MASTERY] ) ), "weapon_flashbang", DMG_POISON );
								SetEntPropFloat ( target, Prop_Send, "m_flVelocityModifier", GUNSLINGER_BULLETDANCE_VELMODIF_SCALE );
								
								//	단말마 채크
								if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 ) {
									if ( !g_bGunslingerTakeBulletDance[client][target] ) {
										GetClientEyePosition ( target, targetPos );
										targetPos[2] -= 25.0;
										g_bGunslingerTakeBulletDance[client][target] = true;
										g_iGunslingerBulletDanceExEff[target] = loringlib_CreateActivatorParticle ( client, target, true, targetPos, NULL_VECTOR, "gunslinger_exbulletdance_explode_ready", 10.0 );
										g_iGunslingerBulletDanceExEff[target] = EntIndexToEntRef ( g_iGunslingerBulletDanceExEff[target] );
									}
								}
							}
						}
					}
				}
			}
		}
		
		return Plugin_Continue;
	}
	
	return Plugin_Stop;
}	*/

void clientClass_Gunslinger_BulletDanceHitEff ( int client, int dmgType, float dmgpos[3] ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 &&
			g_bSkillUse[clientClass][SKILL_ID_BULLETDANCE][client] &&
			( dmgType & DMG_BULLET ) ) {
			if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 )
				loringlib_CreateParticleEx ( client, 0, dmgpos, NULL_VECTOR, "gunslinger_exbulletdance_explode_shot_child6", true, 1.0 );
			else
				loringlib_CreateParticleEx ( client, 0, dmgpos, NULL_VECTOR, "gunslinger_bulletdance_hit", true, 1.0 );
		}
	}
}

/**
 * 불릿 댄스 권총 발사 채크
 * @note weapon_fire
 */
void clientClass_Gunslinger_CheckPistolFire ( int client, char[] weapon ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as<int> ( ClientClass_Gunslinger );
		if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 &&
			g_bSkillUse[clientClass][SKILL_ID_BULLETDANCE][client] ) {
			bool pistol = false;
			for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
				if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false ) != -1 )
					pistol = true;
			
			if ( pistol ) {
				int active = loringlib_GetActiveWeapon ( client );
				if ( active != -1 ) {
					if ( loringlib_GetClip1 ( active ) > 0 ) {
						//	발사하지 않았을 경우
						if ( !g_bGunslingerPistolFired[client] ) {
							g_bGunslingerPistolFired[client] = true;
							
							if ( !loringlib_IsWeaponReloading ( active ) ) {
								float pos[3], eye[3];
								GetClientAbsOrigin ( client, pos );
								GetClientEyePosition ( client, eye );
								pos[2] += 30.0;
								if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 )
									loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_exbulletdance", true, 0.5 );
								else
									loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_bulletdance", true, 0.5 );
								
								static char sound[256];
								Format ( sound, sizeof ( sound ), SOUND_GUNSLINGER_BULLETDANCE, GetRandomInt ( SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MIN, SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MAX ) );
								EmitAmbientSound ( sound, pos, client );
								
								float targetPos[3];
								for ( int target = 1; target <= MaxClients; target ++ ) {
									if ( loringlib_IsValidClient__PlayGame ( target ) ) {
										if ( !ZR_IsClientHuman ( target ) ) {
										//	if ( loringlib_IsTargetInSightRange ( client, target, 360.0, GUNSLINGER_BULLETDANCE_DAMAGE_DISTANCE ) ) {
											if ( loringlib_GetNearTrue ( client, target, eye, GUNSLINGER_BULLETDANCE_DAMAGE_DISTANCE ) ) {
												loringlib_MakeDamage ( client, target, GUNSLINGER_BULLETDANCE_DAMAGE + ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] * RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE_MASTERY] ) ), weapon, DMG_POISON );
												SetEntPropFloat ( target, Prop_Send, "m_flVelocityModifier", GUNSLINGER_BULLETDANCE_VELMODIF_SCALE );
												
												//	단말마 채크
												if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 ) {
													if ( !g_bGunslingerTakeBulletDance[client][target] ) {
														GetClientEyePosition ( target, targetPos );
														targetPos[2] -= 25.0;
														g_bGunslingerTakeBulletDance[client][target] = true;
														g_iGunslingerBulletDanceExEff[target] = loringlib_CreateActivatorParticle ( client, target, false, targetPos, NULL_VECTOR, "gunslinger_exbulletdance_explode_ready", 10.0 );
														g_iGunslingerBulletDanceExEff[target] = EntIndexToEntRef ( g_iGunslingerBulletDanceExEff[target] );
													}
												}
											}
										}
									}
								}
							}
							
							CreateTimer ( GUNSLINGER_BULLETDANCE_VICINITY_DAMAGE_TICKS, timerPistolFireCheck, client, TIMER_FLAG_NO_MAPCHANGE );
							return;
						}
					}
				}
				
				g_bGunslingerPistolFired[client] = false;
			}
		}
	}
}

public Action timerPistolFireCheck ( Handle timer, any client ) {
	if ( loringlib_IsValidClient ( client ) )
		if ( g_bGunslingerPistolFired[client] )
			g_bGunslingerPistolFired[client] = false;
	return Plugin_Stop;
}

public Action timerBulletTimeDisable ( Handle timer, int userid ) {
	static int client;
	client = GetClientOfUserId ( userid );
	
	if ( client < 1 || client > MaxClients )
		return Plugin_Stop;
	
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		g_bSkillUse[clientClass][SKILL_ID_BULLETDANCE][client] = false;
		g_iSkillCooldownTicks[clientClass][SKILL_ID_BULLETDANCE][client] = g_iSkillCooldown[clientClass][SKILL_ID_BULLETDANCE];
		g_bSkillCooldown[clientClass][SKILL_ID_BULLETDANCE][client] = true;
		
		startBulletDanceExtendSkill ( client );
	}
	
	g_bGunslingerUseBulletTime[client] = false;
	return Plugin_Stop;
}

/**
 * 단말마
 */
void startBulletDanceExtendSkill ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {		
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			int clientClass = view_as<int> ( ClientClass_Gunslinger );
			if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 ) {
				//	클라이언트가 좀비일 경우 무효
				if ( !ZR_IsClientHuman ( client ) ) {
					for ( int i = 0; i < sizeof ( g_bGunslingerTakeBulletDance[] ); i ++ )
						if ( g_bGunslingerTakeBulletDance[client][i] )
							g_bGunslingerTakeBulletDance[client][i] = false;
					return;
				}
				
				float pos[3];
				//	타겟을 구한다.
				for ( int i = 1; i <= MaxClients; i ++ ) {
					if ( loringlib_IsValidClient ( i ) ) {
						//	불릿 댄스를 맞았을 경우
						if ( g_bGunslingerTakeBulletDance[client][i] ) {
							//	타겟이 살아있고 좀비일 경우
							if ( IsPlayerAlive ( i ) && !ZR_IsClientHuman ( i ) ) {
								loringlib_MakeDamage ( client, i, RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE_EX] ), "weapon_c4", DMG_POISON );
								loringlib_KnockbackToClient ( client, i, GUNSLINGER_BULLETDANCE_EX_KNOCKBACK_SCALE, true );
								SetEntPropFloat ( i, Prop_Send, "m_flVelocityModifier", GUNSLINGER_BULLETDANCE_EX_VELMODIF_SCALE );
							}
							
							g_bGunslingerTakeBulletDance[client][i] = false;
							GetClientAbsOrigin ( i, pos );
							pos[2] += 20.0;
							loringlib_CreateParticleEx ( i, 0, pos, NULL_VECTOR, "gunslinger_exbulletdance_explode_shot", false, 5.0 );
							EmitAmbientSound ( SOUND_GUNSLINGER_BULLETDANCE_EX_EXPLODE, pos, i );
							loringlib_ParticleRemove ( g_iGunslingerBulletDanceExEff[i] );
						}
					}
				}
			}
		}
	}
}

/**
 * 불릿 타임 효과 발동
 * @return 해당 효과 값
 */
/*
stock float clientClass_Gunslinger_BulletTimeEffect ( int client ) {
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		if ( loringlib_IsValidClient__PlayGame ( client ) ) {
			if ( ZR_IsClientHuman ( client ) ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				//	불릿 타임을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE][client] > 0 ) {
					if ( g_bGunslingerUseBulletTime[client] ) {
						char weapon[32];
						loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
						bool pistol = false;
						for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
							if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false ) != -1 )
								pistol = true;
						
						//	권총일 경우?
						if ( pistol ) {
							//	불릿 타임 마스터리를 배웠을 경우
							if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] > 0 ) {
								return g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE] + ( float ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE_MASTERY] );
							}
							
							else
								return g_fSkillAction[clientClass][SKILL_ID_BULLETDANCE];
						}
					}
				}
			}
		}
	}
	
	return 0.0;
}	*/

/**
 * 불릿 댄스
 */
/*void clientClass_Gunslinger_HurtOnBulletDance ( int victim, int client, int dmg, char[] weapon ) {
	if ( loringlib_IsValidClient__PlayGame ( victim ) && loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			if ( ZR_IsClientHuman ( client ) && !ZR_IsClientHuman ( victim ) ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				//	불릿 타임을 사용중일 경우?
				if ( g_bSkillUse[clientClass][SKILL_ID_BULLETDANCE][client] ) {
					//	불릿 댄스를 배웠을 경우
					if ( g_iSkill[clientClass][SKILL_ID_BULLETDANCE_EX][client] > 0 ) {
						bool pistol = false;
						for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
							if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false ) != -1 )
								pistol = true;
							
						if ( pistol ) {
							DataPack data;
							CreateDataTimer ( 0.1, timerBulletDanceTakeDamagePost, data, TIMER_FLAG_NO_MAPCHANGE );
							data.WriteCell ( GetClientUserId ( client ) );
							data.WriteCell ( dmg );
						}
					}
				}
			}
		}
	}
}

public Action timerBulletDanceTakeDamagePost ( Handle timer, DataPack data ) {
	data.Reset ();
	static int client;
	client = GetClientOfUserId ( data.ReadCell () );
	
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	static int dmg;
	dmg = data.ReadCell ();
	
	float pos[3];
	GetClientAbsOrigin ( client, pos );
	pos[2] += 30.0;
	loringlib_CreateParticleEx ( client, 0, pos, NULL_VECTOR, "gunslinger_bulletdance", true, 0.5 );
	static char sound[256];
	Format ( sound, sizeof ( sound ), SOUND_GUNSLINGER_BULLETDANCE, GetRandomInt ( SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MIN, SOUND_GUNSLINGER_BULLETDANCE_RANDOM_MAX ) );
	EmitAmbientSound ( sound, pos, client );
	
	for ( int target = 1; target <= MaxClients; target ++ ) {
		if ( loringlib_IsValidClient__PlayGame ( target ) ) {
			if ( !ZR_IsClientHuman ( target ) ) {
				if ( loringlib_IsTargetInSightRange ( client, target, 360.0, GUNSLINGER_BULLETDANCE_DAMAGE_DISTANCE ) ) {
					loringlib_MakeDamage ( client, target, dmg, "weapon_flashbang", DMG_POISON );
					SetEntPropFloat ( target, Prop_Send, "m_flVelocityModifier", GUNSLINGER_BULLETDANCE_VELMODIF_SCALE );
				}
			}
		}
	}
	
	return Plugin_Stop;
}	*/

/**
 * 데스페라도
 */
void clientClass_Gunslinger_StartDesperado ( int client, int skill ) {
	//	전문병이 아닐 경우
	if ( g_iClientClass[client] != ClientClass_Gunslinger )
		return;
	
	//	좀비일 경우
	if ( ZR_IsClientZombie ( client ) )
		return;
	
	int clientClass = view_as <int> ( g_iClientClass[client] );
	//	기술을 사용중일 경우
	if ( g_bSkillUse[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG ( client )
		return;
	}
	
	//	기술 쿨이 있을 경우
	if ( g_bSkillCooldown[clientClass][skill][client] ) {
		CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG ( client )
		return;
	}
	
	int consume_ap = g_iSkillConsumeAP[clientClass][skill];
	//	AP가 부족할 경우
	if ( consume_ap > loringlib_GetEntityArmor ( client ) ) {
		CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG ( client )
		return;
	}
	
	g_bSkillUse[clientClass][skill][client] = true;
	loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) - consume_ap );
	CLIENT_SKILL_USE_MSG ( client, g_strSkillName[clientClass][skill] )
	g_iGunslingerDesperadoAmmo[client] = GUNSLINGER_DESPERADO_AMMO_AMOUNTS;
	g_fGunslingerDesperadoTargetEffDuration[client] = GetGameTime ();
	CreateTimer ( GUNSLINGER_DESPERADO_ACTIVE_TIME, timerDesperadoDisable, GetClientUserId ( client ), TIMER_FLAG_NO_MAPCHANGE );
	g_fGunslingerDesperadoExTicks[client] = GetGameTime () + GUNSLINGER_DESPERADO_EX_TARGET_DMG_DURATION;
	SDKHook ( client, SDKHook_PostThinkPost, clientClass_Gunslinger_AutoTargetting );
}

/**
 * 데스페라도 타겟팅
 */
public void clientClass_Gunslinger_AutoTargetting ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			int clientClass = view_as<int> ( ClientClass_Gunslinger );
			if ( ZR_IsClientHuman ( client ) && g_iGunslingerDesperadoAmmo[client] > 0 && g_bSkillUse[clientClass][SKILL_ID_DESPERADO][client] ) {
				int target = Gunslinger_GetClosestClient ( client );
				if ( target > 0 ) {
					char weapon[32];
					int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
					bool pistol = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
						if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false) != -1 )
							pistol = true;
						
					if ( active > 0 && IsValidEdict ( active ) && pistol ) {
						if ( GetEngineVersion () == Engine_CSGO || GetEngineVersion () == Engine_CSS ) {
							SetEntPropFloat ( active, Prop_Send, "m_fAccuracyPenalty", 0.0 );
							
							SetEntPropVector ( client, Prop_Send, "m_aimPunchAngle", NULL_VECTOR );
							SetEntPropVector ( client, Prop_Send, "m_aimPunchAngleVel", NULL_VECTOR );
							SetEntPropVector ( client, Prop_Send, "m_viewPunchAngle", NULL_VECTOR );
						}
						
						else {
							SetEntPropVector ( client, Prop_Send, "m_aimPunchAngle", NULL_VECTOR );
							SetEntPropVector ( client, Prop_Send, "m_aimPunchAngleVel", NULL_VECTOR );
						}
	
						Gunslinger_AimLockAtClient ( client, target );
						float pos[3];
						GetClientEyePosition ( target, pos );
						pos[2] -= 10.0;
						
						if ( g_fGunslingerDesperadoTargetEffDuration[client] <= GetGameTime () ) {
							g_fGunslingerDesperadoTargetEffDuration[client] = GetGameTime () + GUNSLINGER_DESPERADO_TARGET_EFFECT_DURATION;
							loringlib_CreateActivatorParticle ( client, target, true, pos, NULL_VECTOR, "gunslinger_mccree_aimstart", GUNSLINGER_DESPERADO_TARGET_EFFECT_DURATION );
						}
						
						
						//	데스페라도 마스터 기술
						if ( g_iSkill[clientClass][SKILL_ID_DESPERADO_EX][client] > 0 ) {
							if ( g_fGunslingerDesperadoExTicks[client] <= GetGameTime () ) {
								g_fGunslingerDesperadoExTicks[client] = GetGameTime () + GUNSLINGER_DESPERADO_EX_TARGET_DMG_DURATION;
								loringlib_CreateParticleEx ( target, 0, pos, NULL_VECTOR, "gunslinger_mccree_himpact", false, GUNSLINGER_DESPERADO_EX_TARGET_DMG_DURATION );
								
								loringlib_MakeDamage ( client, target, RoundFloat ( g_fSkillAction[clientClass][SKILL_ID_DESPERADO_EX] ), weapon, DMG_POISON );
							}
						}
					}
				}
			}
			
			else {
				g_bSkillUse[clientClass][SKILL_ID_DESPERADO][client] = false;
				g_iSkillCooldownTicks[clientClass][SKILL_ID_DESPERADO][client] = g_iSkillCooldown[clientClass][SKILL_ID_DESPERADO];
				g_bSkillCooldown[clientClass][SKILL_ID_DESPERADO][client] = true;
				SDKUnhook ( client, SDKHook_PostThinkPost, clientClass_Gunslinger_AutoTargetting );
				return;
			}
		}
	}
}

/**
 * 데스페라도 사용 할때 발사
 * @note	weapon_fire 에서
 */
void clientClass_Gunslinger_FireOnDesperado ( int client, char[] fireWeapon ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			if ( ZR_IsClientHuman ( client ) ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				if ( g_bSkillUse[clientClass][SKILL_ID_DESPERADO][client] && g_iGunslingerDesperadoAmmo[client] > 0 ) {
					bool pistol = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
						if ( StrContains ( g_strClassWeapon_Gunslinger[i], fireWeapon, false ) != -1 )
							pistol = true;
					
					//	권총일 경우
					if ( pistol ) {
						static char weapon[32];
						Format ( weapon, sizeof ( weapon ), "weapon_%s", fireWeapon );
						
						float damageScale = 0.0;
						
						//	데스페라도 마스터리
						if ( g_iSkill[clientClass][SKILL_ID_DESPERADO_MASTERY][client] > 0 )
							damageScale = float ( g_iSkill[clientClass][SKILL_ID_DESPERADO_MASTERY][client] ) * g_fSkillAction[clientClass][SKILL_ID_DESPERADO_MASTERY];
						
						int target = loringlib_GetClientAimPlayer ( client );
						if ( loringlib_IsValidClient__PlayGame ( target ) && !ZR_IsClientHuman ( target ) ) {
							float pos[3];
							GetClientEyePosition ( client, pos );
							EmitAmbientSound ( SOUND_GUNSLINGER_DESPERADO_FIRE, pos, client );
							
							g_iGunslingerDesperadoAmmo[client] --;
							
							float damagepos[3];
							GetClientEyePosition ( target, damagepos );
							damagepos[2] -= 10.0;
							loringlib_CreateParticleEx ( client, 0, damagepos, NULL_VECTOR, "gunslinger_mccree_impact", false, 1.0 );
							loringlib_MakeDamage ( client, target, RoundFloat ( GUNSLINGER_DESPERADO_DAMAGE + damageScale ), weapon, DMG_POISON );
						
							float vec[3];
							pos[2] = 0.0;
							damagepos[2] = 0.0;
							MakeVectorFromPoints ( pos, damagepos, vec );
							NormalizeVector ( vec, vec );
							ScaleVector ( vec, GUNSLINGER_DESPERADO_TARGET_KNOCKBACK );
							
							float vel[3];
							GetEntPropVector ( target, Prop_Data, "m_vecVelocity", vel );
							AddVectors ( vel, vec, vec );
							TeleportEntity ( target, NULL_VECTOR, NULL_VECTOR, vec );
						
						//	PrintToChatAll ( "Target: %N, Damage: %d", target, RoundFloat ( 200.0 * damageScale ) );
						}
					}
				}
			}
		}
	}
}

/**
 * 데스페라도 사용 도중 적중 시
 * @note TakeDamage에서
 */
stock float clientClass_Gunslinger_DesperadoTargettingToAttack ( int victim, int client, float dmg, int dmgType ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
			if ( !ZR_IsClientHuman ( victim ) && ZR_IsClientHuman ( client ) ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				if ( g_bSkillUse[clientClass][SKILL_ID_DESPERADO][client] && g_iGunslingerDesperadoAmmo[client] > 0 )
					if ( dmgType & DMG_BULLET )
						return 0.0;
			}
		}
	}
	
	return dmg;
}

public Action timerDesperadoDisable ( Handle timer, int userid ) {
	static int client;
	client = GetClientOfUserId ( userid );
	
	if ( client < 1 || client > MaxClients )
		return Plugin_Stop;
	
	if ( g_iGunslingerDesperadoAmmo[client] <= 0 )
		return Plugin_Stop;
	
	if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
		int clientClass = view_as <int> ( ClientClass_Gunslinger );
		g_bSkillUse[clientClass][SKILL_ID_DESPERADO][client] = false;
		g_iSkillCooldownTicks[clientClass][SKILL_ID_DESPERADO][client] = g_iSkillCooldown[clientClass][SKILL_ID_BULLETDANCE];
		g_bSkillCooldown[clientClass][SKILL_ID_DESPERADO][client] = true;
	}
	
	g_iGunslingerDesperadoAmmo[client] = 0;
	return Plugin_Stop;
}

/**
 * 영악함
 * @note player_hurt에서
 */
void clientClass_Gunslinger_HurtOnLegstun ( int victim, int client, int hitgroup, char[] weapon ) {
	if ( loringlib_IsValidClient ( victim ) && loringlib_IsValidClient ( client ) ) {
		if ( ZR_IsClientHuman ( client ) && !ZR_IsClientHuman ( victim ) ) {
			if ( g_iClientClass[client] == ClientClass_Gunslinger ) {
				int clientClass = view_as <int> ( ClientClass_Gunslinger );
				//	영악함을 배웠을 경우
				if ( g_iSkill[clientClass][SKILL_ID_LEGSTUN][client] > 0 ) {
					bool pistol = false;
					for ( int i = 0; i < sizeof ( g_strClassWeapon_Gunslinger ); i ++ )
						if ( StrContains ( g_strClassWeapon_Gunslinger[i], weapon, false ) != -1 )
							pistol = true;
					
					bool hentaigroup = false;
					//	왼쪽 다리거나, 오른쪽 다리일 경우
					if ( hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG )
						hentaigroup = true;
					
					if ( pistol && hentaigroup ) {
						DataPack data;
						CreateDataTimer ( 0.1, timerLegstunPost, data, TIMER_FLAG_NO_MAPCHANGE );
						data.WriteCell ( GetClientUserId ( victim ) );
						data.WriteFloat ( GUNSLINGER_LEGSTUN_DEFAULT_SCALE - ( float ( g_iSkill[clientClass][SKILL_ID_LEGSTUN][client] ) * g_fSkillAction[clientClass][SKILL_ID_LEGSTUN] ) );
					}
				}
			}
		}
	}
}

public Action timerLegstunPost ( Handle timer, DataPack data ) {
	data.Reset ();
	static int client;
	static float stunScale;
	client = GetClientOfUserId ( data.ReadCell () );
	stunScale = data.ReadFloat ();
	
	if ( loringlib_IsValidClient__PlayGame ( client ) )
		if ( !ZR_IsClientHuman ( client ) )
			SetEntPropFloat ( client, Prop_Send, "m_flVelocityModifier", stunScale );
	return Plugin_Stop;
}

stock int Gunslinger_GetClosestClient ( int client ) {
	float clientOrigin[3], targetOrigin[3];
	GetClientAbsOrigin ( client, clientOrigin );
	
	int clientTeam = GetClientTeam ( client );
	int closestTarget = -1;
	
	float closestDistance = -1.0;
	float targetDistance;
	
	for ( int i = 1; i <= MaxClients; i ++ ) {
		if ( loringlib_IsValidClient ( i ) ) {
			if ( client == i || GetClientTeam ( i ) == clientTeam || !IsPlayerAlive ( i ) )
				continue;
				
			GetClientAbsOrigin ( i, targetOrigin );
			targetDistance = GetVectorDistance ( clientOrigin, targetOrigin );
			if ( targetDistance > closestDistance && closestDistance > -1.0 )
				continue;
				
			if ( !Gunslinger_ClientCanSeeTarget ( client, i ) )
				continue;
				
			if ( GetEngineVersion () == Engine_CSGO )
				if ( GetEntPropFloat ( i, Prop_Send, "m_fImmuneToGunGameDamageTime" ) > 0.0 )
					continue;
					
			if ( GUNSLINGER_DESPERADO_TARGET_DISTANCE != 0.0 && targetDistance > GUNSLINGER_DESPERADO_TARGET_DISTANCE )
				continue;
				
			if ( GUNSLINGER_DESPERADO_TARGET_AIM_FOV != 0.0 && !loringlib_IsTargetInSightRange ( client, i, GUNSLINGER_DESPERADO_TARGET_AIM_FOV, GUNSLINGER_DESPERADO_TARGET_DISTANCE ) )
				continue;
				
			closestDistance = targetDistance;
			closestTarget = i;
		}
	}
	
	return closestTarget;
}

stock void Gunslinger_AimLockAtClient ( int attacker, int victim ) {
	float victimPos[3], victimAngles[3], attackerPos[3], finalPos[3];
	GetClientEyePosition ( attacker, attackerPos );
	GetClientEyePosition ( victim, victimPos );
	victimPos[2] -= 20.0;
	GetClientEyeAngles ( victim, victimAngles );
	
	float velFinal[3];
	Gunslinger_AddInFrontOf ( victimPos, victimAngles, 7.0, velFinal );
	MakeVectorFromPoints ( attackerPos, velFinal, finalPos );
	
	GetVectorAngles ( finalPos, finalPos );
	
	/*
	float punch[3];
	if ( GetEngineVersion () == Engine_CSGO || GetEngineVersion () == Engine_CSS )
		GetEntPropVector ( attacker, Prop_Send, "m_aimPunchAngle", punch );
		
	else
		GetEntPropVector ( attacker, Prop_Send, "m_vecPunchAngle", punch );
		
	if ( FindConVar ( "weapon_recoil_scale" ) != null ) {
		finalPos[0] -= punch[0] * GetConVarFloat ( FindConVar ( "weapon_recoil_scale" ) );
		finalPos[1] -= punch[1] * GetConVarFloat ( FindConVar ( "weapon_recoil_scale" ) );
	}
	*/
	
	TeleportEntity ( attacker, NULL_VECTOR, finalPos, NULL_VECTOR );
}

stock void Gunslinger_AddInFrontOf ( float vecOrigin[3], float vecAngle[3], float units, float outPut[3] ) {
	float view[3];
	getViewVector ( vecAngle, view );
	
	outPut[0] = view[0] * units + vecOrigin[0];
	outPut[1] = view[1] * units + vecOrigin[1];
	outPut[2] = view[2] * units + vecOrigin[2];
}

stock void getViewVector ( float vecAngle[3], float outPut[3] ) {
	outPut[0] = Cosine ( vecAngle[1] / ( 180 / FLOAT_PI ) );
	outPut[1] = Sine ( vecAngle[1] / ( 180 / FLOAT_PI ) );
	outPut[2] = Sine ( vecAngle[0] / ( 180 / FLOAT_PI ) );
}

stock bool Gunslinger_ClientCanSeeTarget ( int client, int target, float distance = 0.0, float height = 50.0 ) {
	float clientPos[3], targetPos[3];
	GetEntPropVector ( client, Prop_Send, "m_vecOrigin", clientPos );
	clientPos[2] += height;
	
	GetClientEyePosition ( target, targetPos );
	if ( distance == 0.0 || GetVectorDistance ( clientPos, targetPos, false ) < distance ) {
		Handle trace = TR_TraceRayFilterEx ( clientPos, targetPos, MASK_SOLID_BRUSHONLY, RayType_EndPoint, baseTraceFilter );
		if ( TR_DidHit ( trace ) ) {
			delete trace;
			return false;
		}
		
		delete trace;
		return true;
	}
	
	return false;
}

public bool baseTraceFilter ( int ent, int mask, int data ) {
	return ent == data;
}