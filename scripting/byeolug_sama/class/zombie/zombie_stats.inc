/**
 * 라운드 레벨 당 증가하는 체력 량
 */
#define 	ZOMBIESTAT_DEFAULT_ZOMBIE_HEALTH	250
#define		ZOMBIESTAT_FAST_ZOMBIE_HEALTH		130
#define		ZOMBIESTAT_HEAVY_ZOMBIE_HEALTH		460


/**
 * 라운드 레벨에 따라 좀비 스텟이 변동된다.
 */
public void ZOMBIESTAT_ChangeZombieOnSetStats ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		int defaultHealth = loringlib_GetEntityMaxHealth ( client );
		switch ( ZR_GetClientZombieClass ( client ) ) {
			case ZR_ZOMBIE_CLASS_NORMAL:	loringlib_SetEntityMaxHealth ( client, defaultHealth + ( g_iRoundLevel * ZOMBIESTAT_DEFAULT_ZOMBIE_HEALTH ) );
			case ZR_ZOMBIE_CLASS_FAST:		loringlib_SetEntityMaxHealth ( client, defaultHealth + ( g_iRoundLevel * ZOMBIESTAT_FAST_ZOMBIE_HEALTH ) );
			case ZR_ZOMBIE_CLASS_HEAVY:		loringlib_SetEntityMaxHealth ( client, defaultHealth + ( g_iRoundLevel * ZOMBIESTAT_HEAVY_ZOMBIE_HEALTH ) );
		}
		
		loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
	}
}