public void registerEvents () {
	HookEvent ( "player_death", onPlayerDeath );
	HookEvent ( "player_disconnect", onPlayerDisconnect );
	HookEvent ( "player_hurt", onPlayerHurted );
	HookEvent ( "player_hurt", onPlayerHurtPre, EventHookMode_Pre );
	HookEvent ( "player_spawn", onPlayerSpawned );
	HookEvent ( "weapon_fire", onWeaponFire );
	HookEvent ( "round_start", onRoundStart );
	HookEvent ( "round_end", onRoundEnd );
	HookEvent ( "round_freeze_end", onRoundFreezeEnd );
	HookEvent ( "bullet_impact", onBulletImpact, EventHookMode_Pre );
}

public void onRoundStart ( Event ev, const char[] name, bool dontBroadcast ) {
	clientClass_Supporter_RemoveAmmoboxAndBarricadeEdict ();
	
	ROUNDLEVEL_RoundStartOnSetRoundLevel ();
	
	BOSS_RoundStartOnBossSpawnWait ();
	
	REWARD_RoundStartOnResetRewardValues ();
	
	ITEMINIT_RoundStartOnPotionUseCountClear ();
	
	SURVIVE_TIME_RoundStartOnResetSurviveTime ();
	
	STRONGZOMBIE_RoundStartOnResetStrongZombieData ();
	
	PARTY_RoundStartOnTeleportToPartyManager ();
	
	ReceiveDollar_StartOnCreateTimer ();
	
	ZOMBIESKILL_ZombieSkillVarClear ();
	
	ITEMDROP_OnClearDroppedItemIndex ();
}

public void onRoundEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	int winner = ev.GetInt ( "winner" );
	
	ROUNDLEVEL_RoundEndOnCalcRoundLevel ( winner );
	
//	roundEndOnExpReward ();
	roundEndOnSurviveReward ( winner );
	roundEndOnPartyAllAliveReward ();
	
	ResetMe_RoundEndOnResetData ();
	
	STRONGZOMBIE_RoundEndOnMotherZombieCheckTimerKill ();
	
	ReceiveDollar_EndOnKillTimer ();
}

public void onRoundFreezeEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	STRONGZOMBIE_RoundStartOnStrongZombieCheckTimer ();
}

public void onPlayerDeath ( Event ev, const char[] name, bool dontBroadcast ) {
	int victim = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	int assister = GetClientOfUserId ( ev.GetInt ( "assister" ) );
	bool headshot = ev.GetBool ( "headshot" );
	
//	getRewardToAttacker ( victim, attacker, assister );
	getRewardToZombie ( victim, attacker );
//	getRewardToPartyMembers ( victim, attacker, assister );
	
	ITEMDROP_OnZombieKillNormalItemDrop ( victim, attacker, headshot );	//	item drop
	
	if ( loringlib_IsValidClient ( victim ) )
		if ( SDKHookEx ( victim, SDKHook_WeaponCanUse, onHumanWeaponPickup ) )
			SDKUnhook ( victim, SDKHook_WeaponCanUse, onHumanWeaponPickup );	//	Pickup unhook
}

public void onPlayerDisconnect ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	
	clearSkillCooldownData ( id );
}

public void onPlayerSpawned ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	
	clearSkillCooldownData ( id );
	
	ZOMBIEPOISON_SpawnOnClearZombiePoison ( id );
	ITEMINIT_SpawnOnEquipWeapon ( id );
	
	clientClass_Assault_ClearAssaultData ( id );
	clientClass_Sniper_ClearSniperData ( id );
	clientClass_Medic_ClearMedicData ( id );
	clientClass_Supporter_ClearSupporterData ( id );
	ClientClass_Gunslinger_ClearGunslingerData ( id );
	
	STRONGZOMBIE_SpawnOnChangeStrongZombie ( id );
	
	PARTY_SpawnOnSetupGlow ( id );
	
	if ( loringlib_IsValidClient ( id ) )
		SDKHook ( id, SDKHook_WeaponCanUse, onHumanWeaponPickup );	//	Pickup hook
}

public void onWeaponFire ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	
	char weapon[32];	ev.GetString ( "weapon", weapon, sizeof ( weapon ) );
	
	clientClass_Assault_WeaponFireOnBreakLimit ( id, weapon );
	clientClass_Assault_AmmoCharge ( id, weapon );
//	clientClass_Normal_SprayControl_WeaponFire ( id, weapon );
//	clientClass_Sniper_ExplodeBullet ( id );
	Sniper_WeaponFireOnPenetrateBullet ( id, weapon );
	clientClass_Supporter_WeaponFireOnArmorPiercingAmmo ( id, weapon );
	ClientClass_Shotgunner_FireOnPulseshot ( id, weapon );
	clientClass_Gunslinger_FireOnDesperado ( id, weapon );
	clientClass_Gunslinger_CheckPistolFire ( id, weapon );
	WeaponLowAmmo_FireOnAlertLowAmmoSound ( id, weapon );
}

public void onPlayerHurted ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	int hitgroup = ev.GetInt ( "hitgroup" );
	int dmg = ev.GetInt ( "dmg_health" );
	char weapon[32];	ev.GetString ( "weapon", weapon, sizeof ( weapon ) );

	clientclass_Supporter_SpecialBullet ( id, attacker, dmg, weapon );
	clientClass_Shotgunner_HurtOnBlockingPower ( id, attacker, weapon );
//	clientClass_Gunslinger_HurtOnBulletDance ( id, attacker, dmg, weapon );
	clientClass_Gunslinger_HurtOnLegstun ( id, attacker, hitgroup, weapon );
	
	NewbieHelp_HurtOnAlertHealth ( id );
	
	ZBUY_HurtOnZombieDamageDollars ( id, attacker, dmg, weapon );
	ZBUY_HurtOnPlayHitSound ( id, attacker, weapon );
}

public void onPlayerHurtPre ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	int dmg = ev.GetInt ( "dmg_health" );
	char weapon[32];
	ev.GetString ( "weapon", weapon, sizeof ( weapon ) );
	
	clientClass_Shotgunner_HurtOnGetExplodeBulletDamage ( id, attacker, dmg, weapon );
}

public Action onBulletImpact ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	float pos[3];
	pos[0] = ev.GetFloat ( "x" );
	pos[1] = ev.GetFloat ( "y" );
	pos[2] = ev.GetFloat ( "z" );
	
	clientClass_Sniper_PenetrateBulletImpact ( client, pos );
	clientClass_Shotgunner_ImpactOnExplodeBullet ( client, pos );
	
	return Plugin_Continue;
}

//	Infect start end.
public void ZR_OnZombieInfectStarted () {

}

//	light grenade explode.
public void ZR_OnLightGrenadeExplode ( int entity, int client ) {
	clientClass_Gunslinger_LightGrenadeStun ( entity, client );
}

//	light grenade spawned.
public void ZR_OnThrowLightGrenade ( int entity, int client ) {
	clientClass_Gunslinger_LightGrenadeSDKHooks ( entity, client );
}