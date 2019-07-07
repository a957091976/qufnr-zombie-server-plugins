#define ZMARKET_AMMO_SMALL		0
#define ZMARKET_AMMO_MEDIUM		1
#define ZMARKET_AMMO_LARGE		2

int g_iConfModeType = 0;
int g_iConfInfectCountdown = 0;
int g_iConfGameMaxMoney = 0;
int g_iConfGameStartMoney = 0;
int g_iConfGameZombieStartMoney = 0;
int g_iConfZombieZteleCount = 0;
int g_iConfHumanHealth = 0;
int g_iConfHumanZteleCount = 0;
int g_iConfZteleCanUseZombie = 0;
int g_iConfZteleCanUseHuman = 0;
int g_iConfGameAmmoType = 0;
int g_iConfGameCanUseZombieType = 0;
int g_iConfGameMapNightMode = 0;
int g_iConfBroadcastMotherZombieType = 0;
int g_iConfDebug = 0;
int g_iConfZmarketEnable = 0;
int g_iConfZmarketAmmoAmount[3] = { 0, ... };
int g_iConfZmarketAmmoPrice[3] = { 0, ... };
int g_iConfZMarketAmmoBuyEnable = 0;
int g_iConfNemesisEnable = 0;
int g_iConfNemesisInfectCountdown = 0;
int g_iConfNemesisPlayerCount = 0;
float g_fConfNemesisRoundChance = 0.0;
float g_fConfZombieRespawnTime = 0.0;
float g_fConfHumanRespawnTime = 0.0;
float g_fConfZombieZteleCooldown = 0.0;
float g_fConfHumanSpeed = 0.0;
float g_fConfHumanGravity = 0.0;
float g_fConfHumanZteleCooldown = 0.0;
float g_fConfZteleAutoCancelDistance = 0.0;

/**
 * Keyvalue config file generator.
 */
public void kvConfLoader ( /*void*/ ) {
	//	기본 소스모드 cfg는 정확하지 않아 keyvalue로 대체합니다.
	//	* 수정이 다소로울 수 있습니다.
	static char temp[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, "configs/zombierelolied.cfg.txt" );

	KeyValues kv = new KeyValues ( "zombierelolied_configs" );
	if ( kv.ImportFromFile ( temp ) ) {
		kv.JumpToKey ( "configs", false );
		
		g_iConfDebug						= kv.GetNum ( "zombierelolied_debug_mode" );
		
		g_iConfModeType						= kv.GetNum ( "zombierelolied_modetype" );
		g_iConfInfectCountdown				= kv.GetNum ( "zombierelolied_infect_cooldown" );
		g_iConfNemesisEnable				= kv.GetNum ( "zombierelolied_nemesis_enable" );
		g_fConfNemesisRoundChance			= kv.GetFloat ( "zombierelolied_nemesis_round_chance" );
		g_iConfNemesisPlayerCount			= kv.GetNum ( "zombierelolied_nemesis_player_counts" );
		g_iConfNemesisInfectCountdown		= kv.GetNum ( "zombierelolied_nemesis_infect_cooldown" );
		
		g_iConfGameMaxMoney					= kv.GetNum ( "zombierelolied_game_max_money" );
		g_iConfGameStartMoney				= kv.GetNum ( "zombierelolied_game_start_money" );
		g_iConfGameZombieStartMoney			= kv.GetNum ( "zombierelolied_game_zombie_start_money" );
		g_iConfGameAmmoType					= kv.GetNum ( "zombierelolied_game_ammo_type" );
		g_iConfGameMapNightMode				= kv.GetNum ( "zombierelolied_game_map_night" );
		g_iConfGameCanUseZombieType			= kv.GetNum ( "zombierelolied_game_can_change_zombie_type" );
		g_iConfBroadcastMotherZombieType	= kv.GetNum ( "zombierelolied_game_broadcast_motherzombie_type" );

		g_iConfHumanHealth					= kv.GetNum ( "zombierelolied_human_health" );
		g_fConfHumanGravity					= kv.GetFloat ( "zombierelolied_human_gravity" );
		g_fConfHumanSpeed					= kv.GetFloat ( "zombierelolied_human_speed" );
		
		g_fConfZombieRespawnTime			= kv.GetFloat ( "zombierelolied_zombie_respawn_time" );
		g_fConfHumanRespawnTime				= kv.GetFloat ( "zombierelolied_human_respawn_time" );
		
		g_iConfZteleCanUseZombie			= kv.GetNum ( "zombierelolied_ztele_can_use_zombie" );
		g_iConfZteleCanUseHuman				= kv.GetNum ( "zombierelolied_ztele_can_use_human" );
		g_fConfZteleAutoCancelDistance		= kv.GetFloat ( "zombierelolied_ztele_auto_cancel_distance" );
		
		g_iConfZombieZteleCount				= kv.GetNum ( "zombierelolied_zombie_ztele_counts" );
		g_fConfZombieZteleCooldown			= kv.GetFloat ( "zombierelolied_zombie_ztele_cooldown" );
		g_iConfHumanZteleCount				= kv.GetNum ( "zombierelolied_human_ztele_counts" );
		g_fConfHumanZteleCooldown			= kv.GetFloat ( "zombierelolied_human_ztele_cooldown" );
		
		g_iConfZmarketEnable = kv.GetNum ( "zombierelolied_zmarket_enable" );
		g_iConfZMarketAmmoBuyEnable = kv.GetNum ( "zombierelolied_zmarket_ammobuy_enable" );
		g_iConfZmarketAmmoAmount[ZMARKET_AMMO_SMALL] = kv.GetNum ( "zombierelolied_zmarket_smallammo_amount" );
		g_iConfZmarketAmmoAmount[ZMARKET_AMMO_MEDIUM] = kv.GetNum ( "zombierelolied_zmarket_mediumammo_amount" );
		g_iConfZmarketAmmoAmount[ZMARKET_AMMO_LARGE] = kv.GetNum ( "zombierelolied_zmarket_largeammo_amount" );
		g_iConfZmarketAmmoPrice[ZMARKET_AMMO_SMALL] = kv.GetNum ( "zombierelolied_zmarket_smallammo_price" );
		g_iConfZmarketAmmoPrice[ZMARKET_AMMO_MEDIUM] = kv.GetNum ( "zombierelolied_zmarket_mediumammo_price" );
		g_iConfZmarketAmmoPrice[ZMARKET_AMMO_LARGE] = kv.GetNum ( "zombierelolied_zmarket_largeammo_price" );
		
		delete kv;
	}
	
	else {
		delete kv;
		SetFailState ( "Could not locate \"%s\".", temp );
	}
}

public Action timerServerCmdPost ( Handle timer ) {
	ServerCommand ( "mp_restartgame 1" );	//	게임 재시작
	
	//	봇 디버깅
	ServerCommand ( "bot_join_team ct" );	//	봇 참가 팀 CT
	ServerCommand ( "bot_join_after_player 0" );	//	플레이어가 없어도 봇 접속
	ServerCommand ( "bot_quota 0" );	//	봇 갯수
	
	//	게임 룰 콘바
	ServerCommand ( "mp_humanteam all" );	//	인간 팀
	ServerCommand ( "mp_free_armor 2" );	//	아머 자동 지급
	ServerCommand ( "mp_freezetime 0" );	//	프리즈 타임
	ServerCommand ( "mp_autokick 0" );		//	자동 킥
	ServerCommand ( "mp_limitteams 0" );	//	팀 제한
	ServerCommand ( "mp_autoteambalance 0" );	//	팀 벨런스
	ServerCommand ( "mp_maxmoney %i", g_iConfGameMaxMoney );	//	최대 자금
	ServerCommand ( "mp_startmoney %i", g_iConfGameStartMoney );	//	시작 자금
	ServerCommand ( "mp_teamname_1 \"인간\"" );		//	CT팀 이름
	ServerCommand ( "mp_teamname_2 \"좀비\"" );		//	TR팀 이름
	ServerCommand ( "mp_force_pick_time 99999" );		//	강제 팀 선택 시간
	ServerCommand ( "mp_timelimit 25" );
//	ServerCommand ( "mp_ignore_round_win_conditions 0" );
	
	// 서버 콘바
	ServerCommand ( "sv_clamp_unsafe_velocities 0" );
	ServerCommand ( "sv_accelerate 5" );
	ServerCommand ( "sv_airaccelerate 12.0" );
	ServerCommand ( "sv_timebetweenducks 0" );	// CSGO 특유 덕킹 쿨다운을 제거합니다.
//	ServerCommand ( "sv_staminajumpcost 0" );	// 점프 쿨다운 스테미나를 제거합니다.
//	ServerCommand ( "sv_staminalandcost 0.025" );	// 점프 경직
//	ServerCommand ( "sv_staminarecoveryrate 60" ); // 스테미나가 돌아오는 rate
	ServerCommand ( "sv_stopspeed 60" );
	ServerCommand ( "sv_teamid_overhead 0" );		// 플레이어 아이디 위에 표시
	ServerCommand ( "sv_occlude_players 0" );
	ServerCommand ( "sv_show_team_equipment_prohibit 0" );
	ServerCommand ( "sv_gameinstructor_disable 1" );
	ServerCommand ( "sv_ladder_scale_speed 1" );	// 사다리 속도
	ServerCommand ( "sv_friction 4.5" );	// prestrafe
	ServerCommand ( "sv_disable_radar 1" );	//	레이더 삭제
	ServerCommand ( "sv_gameinstructor_disable 0" );	//	알림 설정 on
	
	ServerCommand ( "ammo_grenade_limit_default 255" );
	ServerCommand ( "ammo_grenade_limit_flashbang 255" );
	ServerCommand ( "ammo_grenade_limit_total 255" );
	
	// 비활성화 콘바
	ServerCommand ( "sm_rcon mp_do_warmup_period 0" );	//	웜업 없애기
	ServerCommand ( "sm_cvar sv_turbophysics 1" );	// 0은 밀기 비활성화, 1은 활성화합니다.
	ServerCommand ( "sm_cvar phys_pushscale 3.2" );	// 무기로 밀때 강도를 조정합니다.
	ServerCommand ( "sm_cvar sv_pushaway_force 0" ); // E키로 밀 때 밀기의 강도
//	ServerCommand ( "sm_cvar sv_pushaway_max_force 99999" ); // 밀기 최대 강도
	ServerCommand ( "sm_cvar sv_pushaway_min_player_speed 75" );
	ServerCommand ( "sm_cvar net_maxfilesize 64" );	// 파일 다운로드 제한
	ServerCommand ( "sm_cvar mp_tagging_scale 99999.0" );	// 테깅
	ServerCommand ( "sm_cvar sv_penetration_type 0" );		// 모델과 사이의 총알 관통
	FindConVar ( "mp_tagging_scale" ).SetFloat ( 99999.0 );
	FindConVar ( "sv_penetration_type" ).SetInt ( 0 );
}

public void OnConfigsExecuted ( /*void*/ ) {
	CreateTimer ( 0.0, timerServerCmdPost );
}

void OnPrecacheParticles () {
	loringlib_PrecacheEffect ( "ParticleEffect" );
	PrecacheGeneric ( "particles/qufnr/zombie_spawn_mark.pcf", true );
	loringlib_AddFileToDownloadsTableEx ( "particles/qufnr/zombie_spawn_mark.pcf" );
	loringlib_PrecacheParticleName ( "zombie_infection" );
	loringlib_PrecacheParticleName ( "zombie_infectedfull" );
	loringlib_PrecacheParticleName ( "zombie_infection2" );
}