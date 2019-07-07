#define		FLASHBANG_EXPLODE_SOUND		"items/nvg_on.wav"	/**< Flashbang explode sound. */
#define		FLASHBANG_LIGHT_DURATION	42.0				/**< Light durations (seconds). */
#define		SMOKEGRENADE_EXPLODE_RADIUS	250.0				/**< Smoke Grenade explode radius. */
#define		SMOKEGRENADE_FREEZE_TIME	1.8
char g_strSmokeGrenadeExplodeSounds[3][256] = {
//	"physics/metal/metal_barrel_sand_impact_bullet1.wav"
//	, "physics/metal/metal_barrel_sand_impact_bullet2.wav"
//	, "physics/metal/metal_barrel_sand_impact_bullet3.wav"
	"weapons/incgrenade/inc_grenade_detonate_1.wav"
	, "weapons/incgrenade/inc_grenade_detonate_2.wav"
	, "weapons/incgrenade/inc_grenade_detonate_3.wav"
};

/**
 * Entity spawn to hooking spawn post.
 */
public void onSpawnEntPost ( int entity ) {
	if ( !IsValidEdict ( entity ) )
		return;
		
	SDKHook ( entity, SDKHook_SpawnPost, onEntitySpawnEntPost );
}

public void onEntitySpawnEntPost ( int entity ) {
	static char classname[32];
	if ( !IsValidEdict ( entity ) )
		return;
		
	GetEdictClassname ( entity, classname, sizeof ( classname ) );
	if ( StrContains ( classname, "projectile", false ) == -1 )
		return;
	
	loringlib_SetEntityNoblock ( entity, true );
	
	int colors[4];
	int modelIdx = -1;
	colors[3] = 255;
	//	HE Grenade Projectile
	if ( !strcmp ( classname, "hegrenade_projectile" ) ) {
		colors[0] = 255;
		colors[1] = 0;
		colors[2] = 0;
		modelIdx = PrecacheModel ( "materials/sprites/laserbeam.vmt" );
		IgniteEntity ( entity, 1.2 );	//	Fire
	}
	
	//	Flashbang Projectile
	else if ( !strcmp ( classname, "flashbang_projectile" ) ) {
		for ( int i = 0; i <= 3; i ++ )
			colors[i] = 255;
		modelIdx = PrecacheModel ( "materials/sprites/laserbeam.vmt" );
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_THROW_LIGHT_GRENADE] );
		Call_PushCell ( entity );
		Call_PushCell ( loringlib_GetEntityOwner2 ( entity ) );
		Call_Finish ();
	}
	
	//	Smoke Grenade Projectile
	else if ( !strcmp ( classname, "smokegrenade_projectile" ) ) {
		colors[0] = 0;
		colors[1] = 150;
		colors[2] = 150;
		modelIdx = PrecacheModel ( "materials/sprites/tp_beam001.vmt" );
		
		Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_THROW_FREEZE_GRENADE] );
		Call_PushCell ( entity );
		Call_PushCell ( loringlib_GetEntityOwner2 ( entity ) );
		Call_Finish ();
	}
	
	//	Molotov Projectile
	else if ( !strcmp ( classname, "molotov_projectile" ) || !strcmp ( classname, "incgrenade_projectile" ) ) {
		colors[0] = 120;
		colors[1] = 0;
		colors[2] = 0;
		modelIdx = PrecacheModel ( "materials/sprites/laserbeam.vmt" );
	}
	
	else	return;
	TE_SetupBeamFollow ( entity, modelIdx, 0, GetRandomFloat ( 0.7, 1.5 ), 8.5, 8.5, 0, colors );
	TE_SendToAll ();
}

/**
 * Create a flashbang.
 */
public void onFlashbangCreated ( int entity, const char[] classname ) {
	if ( !IsValidEdict ( entity ) )
		return;
		
	if ( !strcmp ( classname, "flashbang_projectile" ) )
		CreateTimer ( 1.1, timerDoFlashbangExplode, entity );
}

/**
 * Flashbang exploded.
 */
public Action timerDoFlashbangExplode ( Handle timer, int entity ) {
	if ( !IsValidEdict ( entity ) )
		return Plugin_Stop;
	
	float pos[3];
	static char classname[32];
	GetEdictClassname ( entity, classname, sizeof ( classname ) );
	if ( strcmp ( classname, "flashbang_projectile" ) == -1 )
		return Plugin_Stop;
		
	int owner = loringlib_GetEntityOwner2 ( entity );
	GetEntPropVector ( entity, Prop_Send, "m_vecOrigin", pos );
	pos[2] += 45.500;
	
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_LIGHT_GRENADE_EXPLODE] );
	Call_PushCell ( entity );
	Call_PushCell ( owner );
	Call_Finish ();
	
	PrecacheSound ( FLASHBANG_EXPLODE_SOUND );
	EmitSoundToAll ( FLASHBANG_EXPLODE_SOUND, entity, SNDCHAN_WEAPON, _, _, _, _, _, pos );
	
	loringlib_EffectLightDynamic ( pos, "255, 255, 255, 255", 350.0, 185.0, FLASHBANG_LIGHT_DURATION );
	AcceptEntityInput ( entity, "kill" );
	
	return Plugin_Stop;
}

/**
 * Create a smoke grenade.
 */
public void onSmokeGrenadeCreated ( int entity, const char[] classname ) {
	if ( !IsValidEdict ( entity ) )
		return;
		
	if ( !strcmp ( classname, "smokegrenade_projectile" ) )
		CreateTimer ( 0.8, timerExplodeSmokeGrenade, entity );
}

public Action timerExplodeSmokeGrenade ( Handle timer, int entity ) {
	onSmokegrenadeFreezeExplode ( entity );
	return Plugin_Stop;
}

public void onSmokegrenadeFreezeExplode ( int entity ) {
	if ( !IsValidEdict ( entity ) )
		return;
	
	static char classname[32];
	GetEdictClassname ( entity, classname, sizeof ( classname ) );
	if ( strcmp ( classname, "smokegrenade_projectile" ) == -1 )
		return;
	
	int owner = loringlib_GetEntityOwner2 ( entity );
	if ( !loringlib_IsValidClient__PlayGame ( owner ) ) {
		AcceptEntityInput ( entity, "kill" );
		return;
	}
	
	//	Owner is zombie?
	if ( g_bZombie[owner] && GetClientTeam ( owner ) == CS_TEAM_T ) {
		AcceptEntityInput ( entity, "kill" );
		return;
	}
		
	float pos[3];
	GetEntPropVector ( entity, Prop_Send, "m_vecOrigin", pos );
	pos[2] += 10.0;
	
	Call_StartForward ( g_hForwardHandlers[ZR_FORWARD_ON_FREEZE_GRENADE_EXPLODE] );
	Call_PushCell ( entity );
	Call_PushCell ( owner );
	Call_Finish ();
	
	int random = GetRandomInt ( 0, 2 );
	PrecacheSound ( g_strSmokeGrenadeExplodeSounds[random] );
	EmitSoundToAll ( g_strSmokeGrenadeExplodeSounds[random], entity, SNDCHAN_WEAPON, _, _, _, _, _, pos );
	
	TE_SetupBeamRingPoint ( pos, 5.0, SMOKEGRENADE_EXPLODE_RADIUS, PrecacheModel ( "materials/sprites/laserbeam.vmt" ), 0, 1, 1, 0.3, 25.0, 25.0, { 64, 64, 255, 255 }, 1, 0 );
	TE_SendToAll ();
	
	for ( int victim = 1; victim <= MaxClients; victim ++ ) {
		if ( !loringlib_IsValidClient__PlayGame ( victim ) )
			continue;
		
		if ( loringlib_GetNearTrue ( entity, victim, pos, SMOKEGRENADE_EXPLODE_RADIUS ) &&
			GetClientTeam ( victim ) != GetClientTeam ( owner ) &&
			( GetClientTeam ( owner ) == CS_TEAM_CT && !g_bZombie[owner] ) ) {
			loringlib_FreezeEntity ( victim, SMOKEGRENADE_FREEZE_TIME, _ );
			CreateTimer ( SMOKEGRENADE_FREEZE_TIME + 0.1, timerVictimFreezeVelmodif, victim, TIMER_FLAG_NO_MAPCHANGE );
		}
	}
	
	AcceptEntityInput ( entity, "kill" );
}

public Action timerVictimFreezeVelmodif ( Handle timer, any victim ) {
	if ( loringlib_IsValidClient__PlayGame ( victim ) )
		SetEntPropFloat ( victim, Prop_Send, "m_flVelocityModifier", 0.0 );
}