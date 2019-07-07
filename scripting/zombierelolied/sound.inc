#include "zombierelolied/random_sound"

Handle g_hEffIdle[MAXPLAYERS + 1];
Handle g_hEffCmdTimer[MAXPLAYERS + 1];
int g_iEffCmdCnt[MAXPLAYERS + 1];

void randomSoundPrecache ( /*void*/ ) {
	char temp[256];
	for ( int i = 1; i <= SOUND_MOAN_MAX; i ++ ) {
		Format ( temp, 256, SOUND_MOAN_PATH, i );
		loringlib_AddFileToDownloadsTableEx ( "sound/%s", temp );
		PrecacheSoundAny ( temp, true );
//		PrintToServer ( "[ZR] \"%s\" Precached.", temp );
	}
	
	for ( int i = 1; i <= SOUND_GROAN_MAX; i ++ ) {
		Format ( temp, 256, SOUND_GROAN_PATH, i );
		loringlib_AddFileToDownloadsTableEx ( "sound/%s", temp );
		PrecacheSoundAny ( temp, true );
//		PrintToServer ( "[ZR] \"%s\" Precached.", temp );
	}
	
	for ( int i = 1; i <= SOUND_DEATH_MAX; i ++ ) {
		Format ( temp, 256, SOUND_DEATH_PATH, i );
		loringlib_AddFileToDownloadsTableEx ( "sound/%s", temp );
		PrecacheSoundAny ( temp, true );
//		PrintToServer ( "[ZR] \"%s\" Precached.", temp );
	}
	
	for ( int i = 1; i <= SOUND_DEATH_IDLE_MAX; i ++ ) {
		Format ( temp, 256, SOUND_DEATH_IDLE_PATH, i );
		loringlib_AddFileToDownloadsTableEx ( "sound/%s", temp );
		PrecacheSoundAny ( temp, true );
	}
}

void zombieSoundsClientInit ( int client ) {
	g_hEffIdle[client] = null;
	g_iEffCmdCnt[client] = 0;
	loringlib_KillTimer ( g_hEffCmdTimer[client] );
}

void zombieSoundsOnClientSpawn ( int client ) {
	if ( g_hEffIdle[client] != null )
		KillTimer ( g_hEffIdle[client] );
	g_hEffIdle[client] = null;
	
	g_iEffCmdCnt[client] = 0;
	loringlib_KillTimer ( g_hEffCmdTimer[client] );
}

void zombieSoundsOnClientDeath ( int client ) {
	if ( g_hEffIdle[client] != null )
		KillTimer ( g_hEffIdle[client] );
	g_hEffIdle[client] = null;
	
	g_iEffCmdCnt[client] = 0;
	loringlib_KillTimer ( g_hEffCmdTimer[client] );
	
	if ( !g_bZombie[client] )
		return;
		
	char pathtemp[SOUND_MAX_PATH];
	getRandomZombieSounds ( pathtemp, death );
	
	float soundpos[3];
	GetClientAbsOrigin ( client, soundpos );
	if ( GetEngineVersion () == Engine_CSGO )
		EmitAmbientSoundAny ( pathtemp, soundpos, _, SNDLEVEL_NORMAL );
	else
		EmitAmbientSound ( pathtemp, soundpos );
}

void zombieSoundsOnTakeDamages ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;

	if ( !g_bZombie[client] )
		return;
		
	float soundpos[3];
	GetClientAbsOrigin ( client, soundpos );
	
	/* 무작위 숫자 1에서 5중 1이 나오면 소리를 출력합니다. */
	if ( GetRandomInt ( 1, 5 ) == 1 ) {
		char pathtemp[SOUND_MAX_PATH];
		getRandomZombieSounds ( pathtemp, pain );
		if ( GetEngineVersion () == Engine_CSGO )
			EmitAmbientSoundAny ( pathtemp, soundpos, _, SNDLEVEL_NORMAL );
		else
			EmitAmbientSound ( pathtemp, soundpos );
	}
}


void zombieSoundsOnClientInfected ( int client ) {
	//	Infect Sounds
	float soundpos[3];
	GetClientAbsOrigin ( client, soundpos );
	
	char pathtemp[SOUND_MAX_PATH];
	getRandomZombieSounds ( pathtemp, death_idle );
	if ( GetEngineVersion () == Engine_CSGO )
		EmitAmbientSoundAny ( pathtemp, soundpos, _, SNDLEVEL_NORMAL );
	else
		EmitAmbientSound ( pathtemp, soundpos );
	//
	
	if ( g_hEffIdle[client] != null )
		KillTimer ( g_hEffIdle[client] );
		
	g_hEffIdle[client] = CreateTimer ( 25.0, timerLoopZombieIdle, client, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );
}

public Action timerLoopZombieIdle ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient ( client ) || !g_bZombie[client] ) {
		g_hEffIdle[client] = null;
		return Plugin_Stop;
	}
	
	float soundpos[3];
	char pathtemp[SOUND_MAX_PATH];
	GetClientAbsOrigin ( client, soundpos );
	getRandomZombieSounds ( pathtemp, idle );
	if ( GetEngineVersion () == Engine_CSGO )
		EmitAmbientSoundAny ( pathtemp, soundpos, _, SNDLEVEL_NORMAL );
	else
		EmitAmbientSound ( pathtemp, soundpos );
	
	return Plugin_Continue;
}

void zombieSoundsOnEnd ( /*void*/ ) {
	for ( int client = 0; client <= MAXPLAYERS; client ++ ) {
		g_iEffCmdCnt[client] = 0;
		loringlib_KillTimer ( g_hEffCmdTimer[client] );
	}
}