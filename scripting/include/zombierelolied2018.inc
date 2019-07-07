#if defined _zombierelolied2018_included_
	#endinput
#endif
#define _zombierelolied2018_included_

/**
 * @section		Team index
 */
#define		ZR_HUMAN_TEAM		3
#define		ZR_ZOMBIE_TEAM		2

/**
 * Gamecheck options index
 */
#define		ZR_GAMECHECK_OPTION_START		0
#define		ZR_GAMECHECK_OPTION_END			1
#define		ZR_GAMECHECK_OPTION_NEMESIS		2

/**
 * 클라이언트가 좀비인지 채크합니다.
 *
 * @param client		클라이언트
 * @return	클라이언트가 좀비라면 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsClientZombie ( int client );

/**
 * 클라이언트가 숙주 좀비인지 채크합니다.
 *
 * @param client		클라이언트
 * @return	클라이언트가 숙주 좀비라면 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsClientMotherZombie ( int client );

/** 
 * 클라이언트가 일반 좀비인지 채크합니다.
 *
 * @param client		클라이언트
 * @return  일반 좀비일 경우 true, 아니면 false를 반환합니다.
 */
stock bool ZR_IsClientNormalZombie ( int client ) {
	if ( client > 0 && client <= MaxClients && IsClientInGame ( client ) )
		if ( ZR_IsClientZombie ( client ) && !ZR_IsClientMotherZombie ( client ) )
			return true;
	
	return false;
}

/**
 * 좀비 수를 구합니다.
 *
 * @param alive			살아있는 좀비만 채크
 * @return 좀비 수
 */
stock int ZR_GetZombies ( bool alive = false ) {
	int result = 0;
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( i > 0 && i <= MaxClients && IsClientInGame ( i ) )
			if ( ( ZR_IsClientZombie ( i ) || ZR_IsClientMotherZombie ( i ) ) && alive ? IsPlayerAlive ( i ) : true )
				result ++;
	return result;
}

/**
 * 일반 좀비 수를 구합니다.
 *
 * @param checkAlive	살아있는 좀비만 채크
 * @return 일반 좀비 수
 */
stock int ZR_GetNormalZombies ( bool checkAlive = false ) {
	int result = 0;
	for ( int i = 0; i <= MaxClients; i ++ )
		if ( i > 0 && i <= MaxClients && IsClientInGame ( i ) )
			if ( ( ZR_IsClientZombie ( i ) && !ZR_IsClientMotherZombie ( i ) ) && checkAlive ? IsPlayerAlive ( i ) : true )
				result ++;
	return result;
}

/**
 * 숙주 좀비 수를 구합니다.
 *
 * @param checkAlive	살아있는 좀비만 채크
 * @return 숙주 좀비 수
 */
stock int ZR_GetMotherZombies ( bool checkAlive = false ) {
	int result = 0;
	for ( int i = 0; i <= MaxClients; i ++ )
		if ( i > 0 && i <= MaxClients && IsClientInGame ( i ) )
			if ( ZR_IsClientMotherZombie ( i ) && checkAlive ? IsPlayerAlive ( i ) : true )
				result ++;
	return result;
}

/**
 * 죽은 좀비 수를 구합니다.
 *
 * @return 좀비 수
 */
stock int ZR_GetDeadZombies () {
	int result = 0;
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( i > 0 && i <= MaxClients && IsClientInGame ( i ) )
			if ( ( ZR_IsClientZombie ( i ) || ZR_IsClientMotherZombie ( i ) ) && !IsPlayerAlive ( i ) )
				result ++;
	return result;
}

/**
 * 죽은 좀비 수를 구합니다.
 *
 * @return 좀비 수
 */
stock int ZR_GetNotAliveZombies () {
	return ZR_GetDeadZombies ();
}

/**
 * 클라이언트가 인간인지 채크합니다.
 *
 * @param client		클라이언트
 * @return	클라이언트가 인간이라면 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsClientHuman ( int client );

/**
 * 클라이언트가 안전한 인간인지 채크합니다.
 *
 * @param client		클라이언트
 * @note	Safe Human 이란? 이전 라운드에 숙주 좀비를 했다면 다음 라운드에 숙주 좀비가 되는 것을 방지해주는 bool형 변수 입니다.
 * @return	클라이언트가 안전한 인간이라면 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsClientSafeHuman ( int client );

/**
 * 클라이언트의 좀비 타입을 구합니다.
 *
 * @param client		클라이언트
 * @return	Zombie type.
 */
native int ZR_GetClientZombieType ( int client );

/**
 * 클라이언트의 좀비 클래스를 구합니다.
 *
 * @param client		클라이언트
 * @return	Zombie class index.
 */
native int ZR_GetClientZombieClass ( int client );

/**
 * 클라이언트의 넉백 크기를 구합니다.
 *
 * @param client		클라이언트
 * @param type			좀비 타입
 * @return	클라이언트 넉백 크기
 */
native float ZR_GetClientKnockbackScale ( int client, int type );

/**
 * 클라이언트의 넉백 크기를 설정합니다.
 *
 * @param client		클라이언트
 * @param type			좀비 타입
 * @param amount		넉백 크기
 */
native void ZR_SetClientKnockbackScale ( int client, int type, float amount );

/**
 * 클라이언트가 좀비로 선정 되었을 때 호출 됩니다. (일반, 숙주 좀비 모두 포함됨.)
 * @param client			클라이언트
 * @parma zombieType		좀비 타입
 * @param isMotherZombie	숙주 좀비이면 true, 아니면 false 입니다.
 * @noreturn
 */
forward void ZR_OnChangeZombie ( int client, int zombieType, bool isMotherZombie );

/**
 * 클라이언트가 숙주 좀비로 선정 되었을 때 호출 됩니다.
 * @param client			클라이언트
 * @parma zombieType		좀비 타입
 * @noreturn
 */
forward void ZR_OnChangeMotherZombie ( int client, int zombieType );

/**
 * 클라이언트가 인간으로 설정 되었을 때 호출 됩니다.
 * @param client			클라이언트
 */
forward void ZR_OnChangeHuman ( int client );

/**
 * 좀비 선택 Ticks (초) 마다 호출 합니다.
 * @param ticks				선택 남은 시간
 */
forward void ZR_OnZombieInfectStartTicks ( int ticks );

/**
 * 좀비 선택 (게임 시작)시 호출 됩니다.
 */
forward void ZR_OnZombieInfectStarted ();

/**
 * 네메시스 선택 시 호출 됩니다.
 * @param nemesis_counts	네메시스 수
 */
forward void ZR_OnChooseNemesis ( int nemesis_counts );

/**
 * 클라이언트가 네메시스로 선정 되었을 때 호출 됩니다.
 * @param client			클라이언트
 */
forward void ZR_OnChangeNemesis ( int client );

/**
 * 클라이언트가 ztele을 시작할 때 호출 됩니다.
 * @param client
 */
forward void ZR_OnClientZteleStarted ( int client );

/**
 * 클라이언트가 ztele 시간이 끝나고 텔레포트 했을 때 호출 됩니다.
 * @param client
 */
forward void ZR_OnClientZteleTeleported ( int client );

/**
 * 게임이 시작한 상태인지 채크합니다.
 *
 * @return 게임이 시작(카운트 다운 종료 후 숙주가 등장했을 때)일 경우 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsGameStart ();

/**
 * 게임이 종료된 상태인지 채크합니다.
 *
 * @return 게임이 종료되었을 경우 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsGameEnd ();

/**
 * 플레이어를 기다리고 있는 시간일 경우 채크합니다.
 *
 * @return 플레이어를 기다리는 중일 경우 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsGameWait ();

/**
 * 네메시스 라운드인지 채크합니다.
 *
 * @return 네메시스 라운드 일 경우 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsNemesisRound ();

/**
 * 클라이언트가 네메시스인지 채크합니다.
 *
 * @param client		클라이언트
 * @return	클라이언트가 네메시스라면 true, 아니면 false를 반환합니다.
 */
native bool ZR_IsClientNemesis ( int client );

/**
 * 조명탄이 터질 때 호출 됩니다.
 *
 * @param entity			조명탄 엔티티 값
 * @param owner				조명탄 주인
 */
forward void ZR_OnLightGrenadeExplode ( int entity, int owner );

/**
 * 얼음탄이 터질 때 호출 됩니다.
 *
 * @param entity			얼음탄 엔티티 값
 * @param owner				얼음탄 주인
 */
forward void ZR_OnFreezeGrenadeExplode ( int entity, int owner );

forward void ZR_OnThrowLightGrenade ( int entity, int owner );

forward void ZR_OnThrowFreezeGrenade ( int entity, int owner );

/**
 * 게임 상태 옵션을 설정합니다.
 *
 * @param option			옵션 값
 * @param optionValue		설정할 값
 */
native bool ZR_SetGameCheckOption ( int option, bool optionValue );