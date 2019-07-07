
#define ZBUY_MESSAGE_PREFIX			"\x04[Zbuy]"
#define	ZBUY_NEED_MORE_ACCOUNT(%1)		PrintToChat ( %1, " %s \x01자금이 부족합니다.", ZBUY_MESSAGE_PREFIX );
#define ZBUY_ZOMBIE_UPGRADE_FULL(%1)	PrintToChat ( %1, " %s \x01이미 좀비 강화 수치를 모두 채웠습니다.", ZBUY_MESSAGE_PREFIX );

#define HIT_SOUND		"buttons/arena_switch_press_02.wav"
#define DOLLAR_RECEIVE_SOUND	"qufnr/dollars.mp3"	/**< 달러 지급 소리 */
#define DOLLAR_RECEIVE_MIN	5000
#define DOLLAR_RECEIVE_MAX	6000

#define	ZBUY_ZOMBIE_UPGRADE_SOUND		"qufnr/zombie_department/zombie_upgrade.mp3"
#define	ZBUY_ZOMBIE_UPGRADE_SOUND_FINAL	"qufnr/zombie_department/zombie_upgrade_final.mp3"

#define ZBUY_AMMO_BUY_PRICE		5000

/**
 * 명령어 레지스터
 */
public void ZBUY_RegisterCommand () {
	RegConsoleCmd ( "sm_zbuy", clientCommandZbuyMenu );
}

/**
 * 마테리얼 프리캐시
 */
public void ZBUY_MapStartOnPrecacheAll () {
	PrecacheSound ( DOLLAR_RECEIVE_SOUND, true );
	PrecacheSound ( HIT_SOUND, true );
	PrecacheSound ( ZBUY_ZOMBIE_UPGRADE_SOUND, true );
	PrecacheSound ( ZBUY_ZOMBIE_UPGRADE_SOUND_FINAL, true );
}	

/**
 * 달러 보급 시간 생성
 * @note round_start
 */
public void ReceiveDollar_StartOnCreateTimer () {
	if ( ZR_IsGameWait () || loringlib_GetPlayGameClients () < 1 )
		return;
	
	float time = GetConVarFloat ( FindConVar ( "mp_roundtime" ) );
	time *= 60.0;
	
	g_iReceiveDollarTime = RoundFloat ( time - 90.0 );
	if ( g_iReceiveDollarTime <= 120 )
		return;
	
	g_hReceiveDollarTimer = CreateTimer ( 1.0, timerReceiveDollarTimer, _, TIMER_FLAG_NO_MAPCHANGE | TIMER_REPEAT );
}

/**
 * 달러 보급 시간 타이머 킬
 * @note round_end
 */
public void ReceiveDollar_EndOnKillTimer () {
	if ( g_hReceiveDollarTimer != null ) {
		KillTimer ( g_hReceiveDollarTimer );
		g_hReceiveDollarTimer = null;
	}
}

public Action timerReceiveDollarTimer ( Handle timer ) {
	if ( ZR_IsGameEnd () ) {
		g_hReceiveDollarTimer = null;
		return Plugin_Stop;
	}
	
	g_iReceiveDollarTime --;
	if ( g_iReceiveDollarTime <= 0 ) {
		SetHudTextParamsEx ( -1.0, 0.645, 3.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 2, 0.3, 0.05, 0.1 );
		int dollars;
		char dollar_format[16];
		for ( int i = 1; i <= MaxClients; i ++ ) {
			if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
				dollars = GetRandomInt ( DOLLAR_RECEIVE_MIN, DOLLAR_RECEIVE_MAX );
				loringlib_NumberFormat ( dollars, dollar_format, sizeof ( dollar_format ) );
				ShowSyncHudText ( i, g_hHudSyncArray[HUDSYNC_RECEIVE_DOLLAR], "$%s 를 받았습니다!", dollar_format );
				loringlib_AddEntityAccount ( i, dollars );
				EmitSoundToClient ( i, DOLLAR_RECEIVE_SOUND );
			}
		}
		
		g_hReceiveDollarTimer = null;
		return Plugin_Stop;
	}
	
	else {
		if ( g_iReceiveDollarTime == 30 ) {
		//	PrintToChatAll ( " \x04[Receive Dollars] \x0130초 후 달러를 받게됩니다." );
			SetHudTextParamsEx ( -1.0, 0.645, 5.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 2, 0.3, 0.05, 0.1 );
			for ( int i = 1; i <= MaxClients; i ++ ) {
				if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
					ShowSyncHudText ( i, g_hHudSyncArray[HUDSYNC_RECEIVE_DOLLAR], "30초 후 달러 보급을 받게됩니다." );
					PrintToChat ( i, " \x04[Receive Dollars] \x0130초 후 달러 보급을 받게됩니다." );
				}
			}
		}
		
		switch ( g_iReceiveDollarTime ) {
			case 5, 4, 3, 2, 1: {
			//	PrintToChatAll ( " \x04[Receive Dollars] \x01%d초 후 달러를 받습니다.", g_iReceiveDollarTime );
				SetHudTextParamsEx ( -1.0, 0.645, 1.0, { 200, 165, 60, 255 }, { 255, 222, 150, 255 }, 0, 0.3, 0.5, 0.1 );
				for ( int i = 1; i <= MaxClients; i ++ ) {
					if ( loringlib_IsValidClient ( i ) && ZR_IsClientHuman ( i ) ) {
						ShowSyncHudText ( i, g_hHudSyncArray[HUDSYNC_RECEIVE_DOLLAR], "%d초 후 달러 보급을 받습니다.", g_iReceiveDollarTime );
						PrintToChat ( i, " \x04[Receive Dollars] \x01%d초 후 달러 보급을 받습니다.", g_iReceiveDollarTime );
					}
				}
			}
		}
	}
	
	return Plugin_Continue;
}

/**
 * 인간 히트사운드
 * @note player_hurt
 */
public void ZBUY_HurtOnPlayHitSound ( int victim, int attacker, char[] weapon ) {
	if ( loringlib_IsValidClient ( victim ) && loringlib_IsValidClient ( attacker ) ) {
		if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) ) {
			if ( StrContains ( weapon, "knife", false ) != -1 ||
				StrEqual ( weapon, "hegrenade", false ) ||
				StrEqual ( weapon, "flashbang", false ) ||
				StrEqual ( weapon, "smokegrenade", false ) ||
				StrEqual ( weapon, "decoy", false ) )
				return;
			
			EmitSoundToClient ( attacker, HIT_SOUND, _, _, _, _, 0.3 );
		}
	}
}

/**
 * 좀비가 피해입을 때 달러준다.
 * @note player_hurt
 */
public void ZBUY_HurtOnZombieDamageDollars ( int victim, int attacker, int damage, char[] weapon ) {
	if ( loringlib_IsValidClient ( victim ) && loringlib_IsValidClient ( attacker ) ) {
		if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) ) {
			if ( StrContains ( weapon, "knife", false ) != -1 ) {
				loringlib_AddEntityAccount ( victim, RoundToZero ( damage / 2.5 ) );
				if ( loringlib_GetEntityAccount ( victim ) > FindConVar ( "mp_maxmoney" ).IntValue )
					loringlib_SetEntityAccount ( victim, FindConVar ( "mp_maxmoney" ).IntValue );
				return;
			}
			
			loringlib_AddEntityAccount ( victim, RoundFloat ( damage / 1.2 ) );
			if ( loringlib_GetEntityAccount ( victim ) > FindConVar ( "mp_maxmoney" ).IntValue )
				loringlib_SetEntityAccount ( victim, FindConVar ( "mp_maxmoney" ).IntValue );
		}
	}
}

/**
 * sm_zbuy 메뉴
 */
public Action clientCommandZbuyMenu ( int client, int args ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		Menu menu = new Menu ( zbuyMenuHandle );
		menu.SetTitle ( "ZBuy\n\tType: %s\n　", ZR_IsClientHuman ( client ) ? "HUMAN" : "ZOMBIE" );
		
		if ( ZR_IsClientHuman ( client ) ) {
			menu.AddItem ( "", "무기 구입 (!zmarket)" );
			menu.AddItem ( "", "투척 무기 구입" );
			menu.AddItem ( "", "탄창 보급" );
		}
		
		else
			menu.AddItem ( "", "좀비 강화" );
		
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	return Plugin_Handled;
}

public int zbuyMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( !IsPlayerAlive ( param1 ) )	return;
			
			if ( ZR_IsClientHuman ( param1 ) ) {
				switch ( param2 ) {
					case 0:		FakeClientCommand ( param1, "sm_zmarket" );
					case 1:		buyGrenadeMenu ( param1 );
					case 2:		buyAmmoMenu ( param1 );
				}
			}
			else {
				switch ( param2 ) {
					case 0:		zombieUpgradeMenu ( param1 );
				}
			}
		}
	}
}

/**
 * 좀비 강화
 */
public void zombieUpgradeMenu ( int client ) {
	if ( !IsPlayerAlive ( client ) || ZR_IsClientHuman ( client ) )
		return;
	
	Menu menu = new Menu ( zbuyZombieUpgradeMenuHandle );
	menu.SetTitle ( "ZBUY\n\t좀비 강화\n\t\t강화 단계: %d / %d", g_iZombieUpgrade[client], ZBUY_ZOMBIE_UPGRADE_MAX );
	
	static char selectItem[256];
	static char buffer[256];
	buffer = "좀비 기술 설명:";
	Format ( selectItem, sizeof ( selectItem ), "좀비 강화 [$%d]\n\t\t강화 수치 MAX 시 좀비 기술 능력치가 한단계 UP!\n\t\t숙주좀비는 강화숙주좀비로 진화하게 됩니다.\n　", ZBUY_ZOMBIE_UPGRADE_PRICE );
	if ( g_iZombieUpgrade[client] >= ZBUY_ZOMBIE_UPGRADE_MAX )
		Format ( selectItem, sizeof ( selectItem ), "좀비 강화 [MAX]\n\t\t강화 수치 MAX 시 좀비 기술 능력치가 한단계 UP!\n\t\t숙주좀비는 강화숙주좀비로 진화하게 됩니다.\n　" );
	
	menu.AddItem ( "", selectItem, ( g_iZombieUpgrade[client] >= ZBUY_ZOMBIE_UPGRADE_MAX || g_bIsStrongMotherZombie[client] ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	
	switch ( ZR_GetClientZombieClass ( client ) ) {
		case ZR_ZOMBIE_CLASS_NORMAL:	Format ( buffer, sizeof ( buffer ), "%s\n\t\t좀비 폭탄 기술: 바라보는 방향에 좀비 폭탄을 던진다.", buffer );
		case ZR_ZOMBIE_CLASS_FAST:		Format ( buffer, sizeof ( buffer ), "%s\n\t\t도약 기술: 바라보는 방향으로 돌진한다.", buffer );
		case ZR_ZOMBIE_CLASS_HEAVY:		Format ( buffer, sizeof ( buffer ), "%s\n\t\t이뮤니티 기술: 일정 시간동안 넉백이 대폭 감소하며 기술 사용 시 체력이 증가한다.", buffer );
		default:						Format ( buffer, sizeof ( buffer ), "%s\n\t\tERROR: Invalid Zombie Class!!!", buffer );
	}
	
	if ( g_bIsStrongMotherZombie[client] || ZR_IsClientNemesis ( client ) )
		Format ( buffer, sizeof ( buffer ), "%s\n\t\t하울링 기술: 비명을 질러 근처의 모든 인간에게 피해를 준다.", buffer );
	
	menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zbuyZombieUpgradeMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( ZR_IsClientHuman ( param1 ) )
				return;
			
			if ( g_iZombieUpgrade[param1] >= ZBUY_ZOMBIE_UPGRADE_MAX ) {
				ZBUY_ZOMBIE_UPGRADE_FULL ( param1 )
				zombieUpgradeMenu ( param1 );
				return;
			}
			
			if ( ZBUY_ZOMBIE_UPGRADE_PRICE > loringlib_GetEntityAccount ( param1 ) ) {
				ZBUY_NEED_MORE_ACCOUNT ( param1 )
				zombieUpgradeMenu ( param1 );
				return;
			}
			
			loringlib_SubEntityAccount ( param1, ZBUY_ZOMBIE_UPGRADE_PRICE );
			g_iZombieUpgrade[param1] ++;
			
			static float pos[3];
			GetClientAbsOrigin ( param1, pos );
			pos[2] += 2.0;
			
			if ( g_iZombieUpgrade[param1] >= ZBUY_ZOMBIE_UPGRADE_MAX ) {
				if ( ZR_IsClientMotherZombie ( param1 ) )
					STRONGZOMBIE_ChangeStrongZombie ( param1 );
				EmitAmbientSound ( ZBUY_ZOMBIE_UPGRADE_SOUND_FINAL, pos );
				loringlib_CreateParticleEx ( param1, 0, pos, NULL_VECTOR, "zombie_upgrade", true, 3.0 );
				loringlib_ShowShakeMessage ( param1, 15.0, 2.0 );
			}
			else {
				EmitSoundToClient ( param1, ZBUY_ZOMBIE_UPGRADE_SOUND );
				loringlib_CreateParticleEx ( param1, 0, pos, NULL_VECTOR, "zombie_up", true, 3.0 );
			}
			
			if ( ZR_IsClientNemesis ( param1 ) ) {
				ZR_SetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ), ZR_GetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ) ) + ZBUY_ZOMBIE_UPGRADE_NEMESIS_KNOCKBACK );
				loringlib_SetEntityHealth ( param1, loringlib_GetEntityHealth ( param1 ) + ZBUY_ZOMBIE_UPGRADE_NEMESIS_HP );
				if ( loringlib_GetEntityMaxHealth ( param1 ) > loringlib_GetEntityHealth ( param1 ) )
					loringlib_SetEntityHealth ( param1, loringlib_GetEntityMaxHealth ( param1 ) );
				return;
			}
			
			static float knockbackScale;
			static int health;
			switch ( ZR_GetClientZombieClass ( param1 ) ) {
				case ZR_ZOMBIE_CLASS_NORMAL: {
					knockbackScale = ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_KNOCKBACK;
					health = ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_HP;
				}
				case ZR_ZOMBIE_CLASS_FAST: {
					knockbackScale = ZBUY_ZOMBIE_UPGRADE_FASTZOMBIE_KNOCKBACK;
					health = ZBUY_ZOMBIE_UPGRADE_FASTZOMBIE_HP;
				}
				case ZR_ZOMBIE_CLASS_HEAVY: {
					knockbackScale = ZBUY_ZOMBIE_UPGRADE_HEAVYZOMBIE_KNOCKBACK;
					health = ZBUY_ZOMBIE_UPGRADE_HEAVYZOMBIE_HP;
				}
				default: {
					knockbackScale = ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_KNOCKBACK;
					health = ZBUY_ZOMBIE_UPGRADE_NORMALZOMBIE_HP;
				}
			}
			
			ZR_SetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ), ZR_GetClientKnockbackScale ( param1, ZR_GetClientZombieType ( param1 ) ) + knockbackScale );
			loringlib_SetEntityHealth ( param1, loringlib_GetEntityHealth ( param1 ) + health );
			//	최대 체력으로 넘어갈 경우 최대 체력이 더 증가한다.
			if ( loringlib_GetEntityMaxHealth ( param1 ) > loringlib_GetEntityHealth ( param1 ) ) {
				loringlib_SetEntityMaxHealth ( param1, loringlib_GetEntityHealth ( param1 ) );
				loringlib_SetEntityHealth ( param1, loringlib_GetEntityMaxHealth ( param1 ) );
			}
				
			zombieUpgradeMenu ( param1 );
		}
	}
}

static char g_strGrenadeList[3][3][64] = {
//	Weapon Index Name, Weapon Display Name, Weapon Price
	{ "weapon_hegrenade", "고폭 수류탄", "3000" }
	, { "weapon_flashbang", "조명탄", "1000" }
	, { "weapon_smokegrenade", "얼음 수류탄", "2500" }
};

/**
 * 투척 무기 구입
 */
public void buyGrenadeMenu ( int client ) {
	if ( !IsPlayerAlive ( client ) || !ZR_IsClientHuman ( client ) )
		return;
	
	Menu menu = new Menu ( zbuyGrenadeMenuHandle );
	menu.SetTitle ( "ZBuy\n\t투척 무기 구입\n　" );
	
	static char buffer[128];
	for ( int i = 0; i < sizeof ( g_strGrenadeList ); i ++ ) {
		Format ( buffer, sizeof ( buffer ), "%s [$%s]", g_strGrenadeList[i][1], g_strGrenadeList[i][2] );
		menu.AddItem ( "", buffer, StringToInt ( g_strGrenadeList[i][2] ) <= loringlib_GetEntityAccount ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zbuyGrenadeMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( StringToInt ( g_strGrenadeList[param2][2] ) > loringlib_GetEntityAccount ( param1 ) ) {
				ZBUY_NEED_MORE_ACCOUNT ( param1 )
				buyGrenadeMenu ( param1 );
				return;
			}
			
			loringlib_SubEntityAccount ( param1, StringToInt ( g_strGrenadeList[param2][2] ) );
			GivePlayerItem ( param1, g_strGrenadeList[param2][0] );
			PrintToChat ( param1, " %s \x01%s 을(를) 구입했습니다. \x07[$%d]", ZBUY_MESSAGE_PREFIX, g_strGrenadeList[param2][1], loringlib_GetEntityAccount ( param1 ) );
			
			buyGrenadeMenu ( param1 );
		}
		
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientCommandZbuyMenu ( param1, 0 );
		}
	}
}

/**
 * 탄창 구입
 */
public void buyAmmoMenu ( int client ) {
	if ( !IsPlayerAlive ( client ) || !ZR_IsClientHuman ( client ) )
		return;
	
	Menu menu = new Menu ( zbuyAmmoMenuHandle );
	menu.SetTitle ( "ZBuy\n\t탄창 보급\n　" );
	static char buffer[256], price[16];
	IntToString ( ZBUY_AMMO_BUY_PRICE, price, sizeof ( price ) );
	Format ( buffer, sizeof ( buffer ), "탄창 보급 [$%d]\n\t\t들고 있는 무기의 탄창을 채웁니다.", ZBUY_AMMO_BUY_PRICE );
	menu.AddItem ( price, buffer, ZBUY_AMMO_BUY_PRICE > loringlib_GetEntityAccount ( client ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int zbuyAmmoMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( !ZR_IsClientHuman ( param1 ) )
				return;
			
			char price_str[16];
			menu.GetItem ( param2, price_str, sizeof ( price_str ) );
			if ( StringToInt ( price_str ) > loringlib_GetEntityAccount ( param1 ) ) {
				ZBUY_NEED_MORE_ACCOUNT ( param1 )
				buyAmmoMenu ( param1 );
				return;
			}
			
			if ( loringlib_HasWeaponGrenade ( param1 ) || loringlib_HasWeaponName ( param1, "weapon_knife" ) ) {
				PrintToChat ( param1, "%s \x01주 무기 및 보조 무기만 사용할 수 있습니다.", ZBUY_MESSAGE_PREFIX );
				return;
			}
		
			int ammo = loringlib_GetWeaponAmmo ( param1 );
			if ( ammo < 0 ) {
				PrintToChat ( param1, " %s \x01ERROR: INVALID WEAPON AMMO AMOUNT INDEX: %d", ZBUY_MESSAGE_PREFIX, ammo );
				return;
			}	
			
			int active = loringlib_GetActiveWeapon ( param1 );
			if ( active < 0 ) {
				PrintToChat ( param1, " %s \x01ERROR: INVALID WEAPON INDEX: %d", ZBUY_MESSAGE_PREFIX, active );
				return;
			}
			
			if ( ammo >= LOLAmmoFix_GetWeaponMaxAmmo ( active ) ) {
				PrintToChat ( param1, " %s \x01그정도 탄창이면 충분합니다.", ZBUY_MESSAGE_PREFIX );
				return;
			}
			
			loringlib_SubEntityAccount ( param1, StringToInt ( price_str ) );

			PrintToChat ( param1, " %s \x01탄창을 채웠습니다.", ZBUY_MESSAGE_PREFIX );
		//	loringlib_SetWeaponAmmo ( param1, ammo + charge_ammo );
			loringlib_SetWeaponAmmoIndex ( param1, active, LOLAmmoFix_GetWeaponMaxAmmo ( active ), _, _, _ );
			return;
		}
	}
}