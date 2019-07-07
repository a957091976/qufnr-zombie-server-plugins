public void onRegisterEvent ( bool hooking ) {
	if ( hooking ) {
		HookEventEx ( "round_end", onRoundEnd );
		HookEventEx ( "player_spawn", onPlayerSpawned );
		HookEventEx ( "player_death", onPlayerDeadth );
		HookEventEx ( "bullet_impact", onBulletImpacted );
	}

	else {
		UnhookEvent ( "round_end", onRoundEnd );
		UnhookEvent ( "player_spawn", onPlayerSpawned );
		UnhookEvent ( "bullet_impact", onBulletImpacted );
		UnhookEvent ( "player_death", onPlayerDeadth );
	}
}

/**
 * On round ending.
 */
public void onRoundEnd ( Event ev, const char[] name, bool dontBroadcast ) {
	rewardOfAliveHumans ( ev.GetInt ( "winner" ) );
}

/**
 * On player spawned.
 */
public void onPlayerSpawned ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	if ( g_iConfZombieMod == 1 ) {
		if ( GetClientTeam ( id ) == CS_TEAM_CT )
			CS_UpdateClientModel ( id );
	}
	
	CreateTimer ( 0.3, timerEquipItemPost, id, TIMER_FLAG_NO_MAPCHANGE );
}

/**
 * On player deaded.
 */
public void onPlayerDeadth ( Event ev, const char[] name, bool dontBroadcast ) {
	int victim = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	int attacker = GetClientOfUserId ( ev.GetInt ( "attacker" ) );
	int assister = -1;
	if ( GetEngineVersion () == Engine_CSGO )
		assister = GetClientOfUserId ( ev.GetInt ( "assister" ) );
	
	toggleMirror ( victim, 0 );	//	Change firstperson mode.
	rewardOfKills ( victim, attacker, assister );
	
	if ( !IsPlayerAlive ( victim ) ) {
		removeTrailParticles ( victim );
		removeAuraParticles ( victim );
	}
}

/**
 * On bullet impacted.
 */
public void onBulletImpacted ( Event ev, const char[] name, bool dontBroadcast ) {
	int id = GetClientOfUserId ( ev.GetInt ( "userid" ) );
	if ( !StrEqual ( g_strLaserBulletData[id][LASERBULLET_EQUIP_CHECK], "1" ) )
		return;
	
	if ( g_iConfZombieMod == 1 )
		if ( GetClientTeam ( id ) != CS_TEAM_CT )
			return;
	
	float laser[3];
	float eye[3];
	static char weapon[32];
	
	laser[0] = ev.GetFloat ( "x" );
	laser[1] = ev.GetFloat ( "y" );
	laser[2] = ev.GetFloat ( "z" );
	
	GetClientWeapon ( id, weapon, sizeof ( weapon ) );
	if ( StrContains ( weapon, "knife" ) != -1
		|| StrEqual ( weapon, "weapon_hegrenade" )
		|| StrEqual ( weapon, "weapon_smokegrenade" )
		|| StrEqual ( weapon, "weapon_flashbang" )
		|| StrEqual ( weapon, "weapon_tagrenade" )
		|| StrEqual ( weapon, "weapon_healthshot" )
		|| StrEqual ( weapon, "weapon_molotov" )
		|| StrEqual ( weapon, "weapon_incgrenade" )
		|| StrEqual ( weapon, "weapon_decoy" )
		|| StrEqual ( weapon, "weapon_c4" ) )
		return;
		
	GetClientEyePosition ( id, eye );
	float distance = GetVectorDistance ( eye, laser );
	float percent = ( 0.4 / ( distance / 100.0 ) );
	
	float difference[3];
	difference[0] = eye[0] + ( ( laser[0] - eye[0] ) * percent );
	difference[1] = eye[1] + ( ( laser[1] - eye[1] ) * percent ) - 0.08;
	difference[2] = eye[2] + ( ( laser[2] - eye[2] ) * percent );
	
	Handle trace = TR_TraceRayFilterEx ( difference, eye, MASK_SOLID_BRUSHONLY, RayType_EndPoint, traceFilterPlayers );
	TR_GetEndPosition ( eye, trace );
	delete trace;
	
//	eye[2] -= 6.355;
		
	int color[4];
	if ( StringToInt ( g_strLaserBulletData[id][LASERBULLET_OPTION] ) == OPTION_RAINBOW ) {
		g_iFireCount[id] ++;
		if ( g_iFireCount[id] > 13 )
			g_iFireCount[id] = 0;
		
		switch ( g_iFireCount[id] ) {
			case 0: {
				color[0] = 255;
				color[1] = 0;
				color[2] = 0;
			}
			case 1: {
				color[0] = 153;
				color[1] = 56;
				color[2] = 0;
			}
			case 2: {
				color[0] = 168;
				color[1] = 78;
				color[2] = 25;
			}
			case 3: {
				color[0] = 200;
				color[1] = 166;
				color[2] = 60;
			}
			case 4: {
				color[0] = 190;
				color[1] = 180;
				color[2] = 60;
			}
			case 5: {
				color[0] = 195;
				color[1] = 240;
				color[2] = 90;
			}
			case 6: {
				color[0] = 123;
				color[1] = 160;
				color[2] = 25;
			}
			case 7: {
				color[0] = 11;
				color[1] = 120;
				color[2] = 3;
			}
			case 8: {
				color[0] = 0;
				color[1] = 130;
				color[2] = 150;
			}
			case 9: {
				color[0] = 70;
				color[1] = 130;
				color[2] = 225;
			}
			case 10: {
				color[0] = 100;
				color[1] = 35;
				color[2] = 190;
			}
			case 11: {
				color[0] = 135;
				color[1] = 70;
				color[2] = 225;
			}
			case 12: {
				color[0] = 170;
				color[1] = 20;
				color[2] = 150;
			}
			case 13: {
				color[0] = 135;
				color[1] = 0;
				color[2] = 60;
			}
		}
	//	color[0] = RoundFloat ( Sine ( GetEngineTime () / g_fConfRainbowBulletFrequency ) * 4.0 ) * 127 + 128;
	//	color[1] = RoundFloat ( Sine ( GetEngineTime () / g_fConfRainbowBulletFrequency ) * 4.0 + 2.0 ) * 127 + 128;
	//	color[2] = RoundFloat ( Sine ( GetEngineTime () / g_fConfRainbowBulletFrequency ) * 4.0 + 4.0 ) * 127 + 128;
		color[3] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_ALPHA] );
	}
	
	else {
		color[0] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_RED] );
		color[1] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_GREEN] );
		color[2] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_BLUE] );
		color[3] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_ALPHA] );
	}
	
	TE_SetupBeamPoints ( difference, laser, PrecacheModel ( g_strLaserBulletData[id][LASERBULLET_MODELPATH] ), 0, 0, 0, 0.5, 1.5, 1.5, 1, 0.0, color, 0 );
	TE_SendToAll ();
}


public bool traceFilterPlayers ( int entity, int contentsMask, any client )
{
	return !entity || entity > MaxClients;
}	