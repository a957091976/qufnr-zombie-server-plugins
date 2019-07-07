#define		NEWBIE_HELP_ALERT_LOWHP_HUMAN		2.5	/**< 인간 체력 적을 때 알려주는 값 */
//#define		NEWBIE_HELP_ALERT_LOWHP_ZOMBIE		4.0	/**< 좀비 ... */

static bool g_bLowHPCheck[MAXPLAYERS + 1] = { false, ... };

/**
 * 일정 체력이 깎이면 알려준다.
 * @note player_hurt
 */
void NewbieHelp_HurtOnAlertHealth ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		static int lowHealth;
		static int color[3];
		color = { 255, 0, 0 };
		//	인간일 경우
		if ( ZR_IsClientHuman ( client ) ) {
			lowHealth = RoundFloat ( loringlib_GetEntityMaxHealth ( client ) / NEWBIE_HELP_ALERT_LOWHP_HUMAN );
			if ( loringlib_GetEntityHealth ( client ) <= lowHealth ) {
				if ( !g_bLowHPCheck[client] )
					g_bLowHPCheck[client] = true;
				else
					return;
			} else
				g_bLowHPCheck[client] = false;
			
			if ( g_bLowHPCheck[client] ) {
				loringlib_PrintInstructorMessage ( client, 5.0, 150.0, 1.0, HINT_STATIC_ON_HUD, true, ICON_USE_BINDING, "", "speed", true, color, "체력이 얼마 남지 않았습니다, 걷기 키를 이용해 물약을 사용 하세요!" );
				PrintToChat ( client, " %s \x01체력이 얼마 남지 않았습니다, 걷기 키를 이용해 물약을 사용 하세요!", MESSAGE_PREFIX );
			}
		}
	}
}

/**
 * 레벨업 시 알려준다.
 * @note onChangeLevel
 */
void NewbieHelp_LevelupOnAlert ( int client ) {
	if ( isPlayerNewbie ( client ) ) {
		static int color[3];
		color = { 204, 255, 51 };
		loringlib_PrintInstructorMessage ( client, 10.0, 250.0, 1.0, HINT_STATIC_ON_HUD, true, ICON_USE_BINDING, "", "rebuy", true, color, "LEVEL UP! F4 키를 이용하거나, !menu 명령어로 스텟과 기술을 배분하세요!" );
		PrintToChat ( client, " %s \x01레벨업 하였습니다! F4 키를 이용하거나, !menu 명령어를 사용해 스텟과 기술을 배분하세요!", MESSAGE_PREFIX );
	}
}

