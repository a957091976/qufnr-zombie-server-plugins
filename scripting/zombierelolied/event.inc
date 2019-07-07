#define MAX_TEXT_MESSAGE	46
char g_strTextMsg[MAX_TEXT_MESSAGE][1][128] = {
	{ "#Cstrike_TitlesTXT_Game_join_ct" }
	, { "#Cstrike_TitlesTXT_Game_join_ct_auto" }
	, { "#Cstrike_TitlesTXT_Game_join_terrorist" }
	, { "#Cstrike_TitlesTXT_Game_join_terrorist_auto" }
	, { "#Player_Cash_Award_Killed_Enemy" }
	, { "#Team_Cash_Award_Win_Hostages_Rescue" }
	, { "#Team_Cash_Award_Win_Defuse_Bomb" }
	, { "#Team_Cash_Award_Win_Time" }
	, { "#Team_Cash_Award_Elim_Bomb" }
	, { "#Team_Cash_Award_Elim_Hostage" }
	, { "#Team_Cash_Award_T_Win_Bomb" }
	, { "#Player_Point_Award_Assist_Enemy_Plural" }
	, { "#Player_Point_Award_Assist_Enemy" }
	, { "#Player_Point_Award_Killed_Enemy_Plural" }
	, { "#Player_Point_Award_Killed_Enemy" }
	, { "#Player_Cash_Award_Kill_Hostage" }
	, { "#Player_Cash_Award_Damage_Hostage" }
	, { "#Player_Cash_Award_Get_Killed" }
	, { "#Player_Cash_Award_Respawn" }
	, { "#Player_Cash_Award_Interact_Hostage" }
	, { "#Player_Cash_Award_Killed_Enemy" }
	, { "#Player_Cash_Award_Rescued_Hostage" }
	, { "#Player_Cash_Award_Bomb_Defused" }
	, { "#Player_Cash_Award_Bomb_Planted" }
	, { "#Player_Cash_Award_Killed_Enemy_Generic" }
	, { "#Player_Cash_Award_Killed_VIP" }
	, { "#Player_Cash_Award_Kill_Teammate" }
	, { "#Team_Cash_Award_Win_Hostage_Rescue" }
	, { "#Team_Cash_Award_Loser_Bonus" }
	, { "#Team_Cash_Award_Loser_Zero" }
	, { "#Team_Cash_Award_Rescued_Hostage" }
	, { "#Team_Cash_Award_Hostage_Interaction" }
	, { "#Team_Cash_Award_Hostage_Alive" }
	, { "#Team_Cash_Award_Planted_Bomb_But_Defused" }
	, { "#Team_Cash_Award_CT_VIP_Escaped" }
	, { "#Team_Cash_Award_T_VIP_Killed" }
	, { "#Team_Cash_Award_no_income" }
	, { "#Team_Cash_Award_Generic" }
	, { "#Team_Cash_Award_Custom" }
	, { "#Team_Cash_Award_no_income_suicide" }
	, { "#Team_Cash_Award_Loser_Bonus_Neg" }
	, { "#Team_Cash_Award_Survive_GuardianMode_Wave" }
	, { "#Player_Cash_Award_ExplainSuicide_YouGotCash" }
	, { "#Player_Cash_Award_ExplainSuicide_TeammateGotCash" }
	, { "#Player_Cash_Award_ExplainSuicide_EnemyGotCash" }
	, { "#Player_Cash_Award_ExplainSuicide_Spectators" }
};

/**
 * Event Hooking
 *
 * @param hooking		Event hooking.
 */
stock void hookEvents ( bool hooking = true ) {
	if ( hooking ) {
		HookEventEx ( "round_start", onRoundStart );
		HookEventEx ( "round_start", onRoundPreStart, EventHookMode_Pre	);
		HookEventEx ( "round_freeze_end", onRoundFreezeEnd );
		HookEventEx ( "round_end", onRoundEnd );
		HookEventEx ( "player_spawn", onPlayerSpawned );
		HookEventEx ( "player_death", onPlayerDeath );
		HookEventEx ( "weapon_fire", onWeaponFired );
		HookEventEx ( "player_team", onPlayerTeamSelect, EventHookMode_Pre );
		HookEventEx ( "smokegrenade_detonate", onSmokegrenadeDetonated, EventHookMode_Pre );
		HookEventEx ( "player_disconnect", onPlayerDisconnect, EventHookMode_Pre );
		HookEventEx ( "player_jump", onPlayerJumped );
		HookEventEx ( "player_footstep", onPlayerFootSteped );
		HookEventEx ( "player_hurt", onPlayerHurt );
		
		HookUserMessage ( GetUserMessageId ( "TextMsg" ), onTextMessageBroadcastDisable, true );
	}
	
	else {
		UnhookEvent ( "round_start", onRoundStart );
		UnhookEvent ( "round_start", onRoundPreStart, EventHookMode_Pre	);
		UnhookEvent ( "round_freeze_end", onRoundFreezeEnd );
		UnhookEvent ( "round_end", onRoundEnd );
		UnhookEvent ( "player_spawn", onPlayerSpawned );
		UnhookEvent ( "player_death", onPlayerDeath );
		UnhookEvent ( "weapon_fire", onWeaponFired );
		UnhookEvent ( "player_team", onPlayerTeamSelect, EventHookMode_Pre );
		UnhookEvent ( "smokegrenade_detonate", onSmokegrenadeDetonated, EventHookMode_Pre );
		UnhookEvent ( "player_disconnect", onPlayerDisconnect, EventHookMode_Pre );
		UnhookEvent ( "player_jump", onPlayerJumped );
		UnhookEvent ( "player_footstep", onPlayerFootSteped );
		UnhookEvent ( "player_hurt", onPlayerHurt );
	}
}

/**
 * Command Hooking
 *
 */
public void hookCommands () {
	AddCommandListener ( onPlayerChooseTeam, "jointeam" );
	AddCommandListener ( onClientSuicide, "kill" );
	AddCommandListener ( onClientSuicide, "explode" );
	AddCommandListener ( onClientSuicide, "explodevector" );
}

/**
 * On round started.
 */
public void onRoundStart ( Event ev, const char[] name, bool dontBroadcast ) {
	ServerCommand ( "sv_full_alltalk 1" );

	roundEndOnRoundStart ();
	
	removeBuyzone ();
	onRemoveMapEntity ();
	
	infectStart ();
//	giveGamestartItems ();
}

/**
 * On round pre started.
 */
public Action onRoundPreStart ( Event ev, const char[] name, bool dontBroadcast ) {
	roundStartOnPreSettings ();
	removeWeaponItems ();
	return Plugin_Continue;
}

/**
 * On round freeze end to started.
 */
public void onRoundFreezeEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	roundEndOnFreezeEnd ();
}

/**
 * On round ended.
 */
public void onRoundEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	ServerCommand ( "sv_full_alltalk 1" );
	
	killNemesisDynamicLight ( -1 );
	roundEndOnRoundEnd ();
	infectRoundEnd ();
}

/**
 * On client spawned.
 */
public void onPlayerSpawned ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	
	spawnToSetupZombieStats ( client );
	zombieSoundsOnClientSpawn ( client );
	setSpawnPosition ( client );
	spawnSetupZtele ( client );
	changeDefaultToHuman ( client );
	zmarketResetClientValues ( client );
}

/**
 * On client dead.
 */
public void onPlayerDeath ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	
	killTheZombie ( client, attacker );
	
	zombieUnhook ( client );
	zombieRespawn ( client, attacker );
	humanRespawn ( client );
	createHealthRechargeTimer ( client );
	zombieSoundsOnClientDeath ( client );
	killNemesisDynamicLight ( client );
}

/**
 * On weapon fired.
 */
public int onWeaponFired ( Event ev, const char[] name, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	infinityWeaponClips ( client );
}

/**
 * On client select the team.
 */
public Action onPlayerTeamSelect ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	if ( IsPlayerAlive ( id ) )
		if ( !ev.GetBool ( "disconnect" ) )
			ev.SetBool ( "silent", true );
	return Plugin_Continue;
}

public Action onPlayerChooseTeam ( int client, const char[] cmd, int args ) {
#if ZR_ENABLE_TEAM_SELECT_ACTION
	char teamid[4];
	GetCmdArg ( 1, teamid, sizeof ( teamid ) );
	if ( StrEqual ( teamid, "0" ) ||
		StringToInt ( teamid ) > 1 ) {
		if ( IsPlayerAlive ( client ) )
			return Plugin_Handled;
		
		PrintToServer ( "Choose team on %N. (arg: %s)", client, teamid );
		ChangeClientTeam ( client, CS_TEAM_CT );
		CreateTimer ( 0.5, timerCheckClientJoinTeamToDead, client );
		return Plugin_Handled;
	}
	
	return Plugin_Handled;
#else
	return Plugin_Handled;
#endif
}

/**
 * On detonated smoke grenade.
 */
public Action onSmokegrenadeDetonated ( Event ev, const char[] name, bool dontBroadcast ) {
	int entity = ev.GetInt ( "entityid" );
	onSmokegrenadeFreezeExplode ( entity );
	
	return Plugin_Handled;
}

/**
 * On player disconnecting.
 */
public Action onPlayerDisconnect ( Event ev, const char[] name, bool dontBroadcast ) {
//	int disconnecter = GetClientOfUserId ( ev.GetInt ( "userid" ) );
//	clientDisconnectToGameRestart ();
//	killHealthRechargeTimer ( disconnecter );
}

public Action timerCheckClientJoinTeamToDead ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( IsPlayerAlive ( client ) )
		return Plugin_Handled;
	
	if ( g_bNemesisRound ) {
		PrintToChat ( client, "[ZR] 네메시스 모드 상태에서는 스폰할 수 없습니다." );
		return Plugin_Handled;
	}
	
	CS_RespawnPlayer ( client );
//	infectHumanToZombie ( client );
	return Plugin_Handled;
}

/**
 * Client can use items.
 */
public Action onWeaponPickup ( int client, int weapon ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
		
	if ( g_bZombie[client] )
		return Plugin_Handled;
		
	return Plugin_Continue;
}

/**
 * Blocking client suicide commands.
 */
public Action onClientSuicide ( int client, const char[] cmd, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	return Plugin_Handled;
}

/**
 * On player jumped.
 */
public void onPlayerJumped ( Event ev, const char[] err, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	nemesisJumped ( client );
}

/**
 * On player footstep.
 */
public void onPlayerFootSteped ( Event ev, const char[] err, bool dontBroadcast ) {
	int client = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	nemesisFootstep ( client );
}

public void onPlayerHurt ( Event ev, const char[] err, bool dontBroadcast ) {
	int victim = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	int damage = ev.GetInt ( "dmg_health" );
	int hitgroup = ev.GetInt ( "hitgroup" );
	char weapon[32];
	ev.GetString ( "weapon", weapon, sizeof ( weapon ) );
	onZombieKnockbackNewVector ( victim, attacker, damage, hitgroup, weapon );
}

/**
 * TextMsg disabler.
 */
public Action onTextMessageBroadcastDisable ( UserMsg msgId, Handle msg, const int[] clients, int clientNum, bool reliable, bool init ) {
	static char buffer[254];
	Protobuf pb = UserMessageToProtobuf ( msg );
	pb.ReadString ( "params", buffer, sizeof ( buffer ), 0 );
	
	for ( int i = 0; i < MAX_TEXT_MESSAGE; i ++ )
		if ( StrEqual ( buffer, g_strTextMsg[i][0] ) )
			return Plugin_Handled;
			
	return Plugin_Continue;
}