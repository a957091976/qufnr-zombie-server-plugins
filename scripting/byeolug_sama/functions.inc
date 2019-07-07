/**
 * OnGameFrame 메소드를 클라이언트에게 호출함.
 */
public void OnGameFrameToClient () {
	for ( int clients = 1; clients <= MaxClients; clients ++ ) {
		if ( !loringlib_IsValidClient ( clients ) )
			continue;
			
	//	displayCooldownSkills ( clients );
	}
}

/**
 * Global Timer를 클라이언트에게 호출함.
 */
public void OnGlobalTimerCallToClient () {
	for ( int clients = 1; clients <= MaxClients; clients ++ ) {
		if ( !loringlib_IsValidClient ( clients ) )
			continue;

		SURVIVE_TIME_OnTimeCheckSurvive ( clients );
		Assault_GlobalTimerOnLeadershipPassive ( clients );
	}
}

/**
 * 클라이언트 0.1초 리피트 타이머
 */
public Action timerClientsideTimer ( Handle timer, int client ) {
	displayPlayerState ( client );
	VISUAL_OnLookAtPlayer ( client );
	WeaponLowAmmo_TimerOnAlertLowAmmoMessage ( client );
	ZOMBIESKILL_DisplayCooldown ( client );
}

/**
 * 플레이어 정보 허드 표시
 * @noreturn
 */
stock void displayPlayerState ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( g_iSelectSlotIndex[client] == SLOT_NO_SELECT ) {
		loringlib_ShowFadeUserMessageEx ( client, 500, 100, FFADE_AUTO, { 0, 0, 0, 255 } );
		SetHudTextParamsEx ( -1.0, -1.0, 0.2, { 151, 211, 211, 255}, { 255, 255, 255, 255 }, 2, 0.0, 0.0, 0.0 );
		ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG], "!select 명령어로 캐릭터를 선택해 주세요.\n서버에 처음 접속 하셨거나, 모르는게 있다면 !rule 명령어로 도움을 받으세요." );
		return;
	}
	
	static char ap[16];	static char hp[2][16];
	static char dollars[16];
	loringlib_NumberFormat ( loringlib_GetEntityHealth ( client ), hp[0], 15 );
	loringlib_NumberFormat ( loringlib_GetEntityMaxHealth ( client ), hp[1], 15 );
	loringlib_NumberFormat ( loringlib_GetEntityArmor ( client ), ap, sizeof ( ap ) );
	
	loringlib_NumberFormat ( loringlib_GetEntityAccount ( client ), dollars, sizeof ( dollars ) );
	
	/*
	static char expbar[64];	static char expbarcolor[16];
	qufnrTools_DisplayProgressBarCalcul ( float ( g_iExp[client] ), float ( getClientMaxExp ( client ) ), expbar, sizeof ( expbar ) );
	qufnrTools_GetProgressBarColor ( float ( g_iExp[client] ), float ( getClientMaxExp ( client ) ), expbarcolor, sizeof ( expbarcolor ) );
	Format ( expbar, sizeof ( expbar ), "[<font color='%s'>%s</font>]", expbarcolor, expbar );
	*/
	char contents[3][128];
	char ccname[32];
	if ( g_iClientClass[client] == ClientClass_Normal )
		strcopy ( ccname, sizeof ( ccname ), "병과 없음" );
	else
		getClientClassName ( g_iClientClass[client], ccname, sizeof ( ccname ), false, true );
//	Format ( contents[0], sizeof ( contents[] ), "%d LV %s %d / %d EXP\t[%s]", g_iLevel[client], expbar, g_iExp[client], getClientMaxExp ( client ), ccname );
	Format ( contents[0], sizeof ( contents[] ), "Level.%d %s [%d / %d EXP (%.2f％)]", g_iLevel[client], ccname, g_iExp[client], getClientMaxExp ( client ), loringlib_GetPercentage ( g_iExp[client], getClientMaxExp ( client ) ) );
	if ( ZR_IsGameEnd () || !ZR_IsGameStart () || g_bBossLevel )
		Format ( contents[2], sizeof ( contents[] ), "" );
	else
		Format ( contents[2], sizeof ( contents[] ), "Act Point%s: %d　　Survival Time: %d sec", g_iSurvivePoint[client] > 1 ? "s" : "", g_iSurvivePoint[client], g_iSurviveTime[client] );
	if ( ZR_IsClientHuman ( client ) )
		Format ( contents[1], sizeof ( contents[] ), "[HP %s / %s%s | AP %s %s]", hp[0], hp[1], g_bZombiePoison[client] ? " ☠" : "", ap, loringlib_HasEntityHelmet ( client ) ? "▲" : "△" );
	else
		Format ( contents[1], sizeof ( contents[] ), "[HP %s ☠| AP %s]", hp[0], ap );
	
//	PrintHintText ( client, "<pre>%s\n%s\n%s</pre>", contents[0], contents[1], contents[2] );
	
	int clr[4];
	qufnrTools_GetCSGOHudColor ( client, clr );
	SetHudTextParams ( 0.045, 0.987, 0.2, clr[0], clr[1], clr[2], clr[3], 0, 0.0, 0.0, 0.0 );
	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG], "%s\n%s\n%s", contents[2], contents[0], contents[1] );
	
	displayCooldownSkills ( client );	//	Skill cooldown display.
	
//	SetHudTextParamsEx ( 0.38, 0.825, 1.0, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 0, 0.1, 0.1, 0.01 );
//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG], "%d LV　%s %d / %d Exp\nStat 포인트: %d SP\nHP %s / %s | AP %s\n%s: %s", g_iLevel[client], expbar, g_iExp[client], getClientMaxExp ( client ), g_iStatPoint[client], hp[0], hp[1], ap, zd_format, zdollar );

//	SetHudTextParamsEx ( 0.55, 0.825, 1.0, { 255, 255, 255, 255 }, { 255, 255, 255, 255 }, 0, 0.1, 0.1, 0.01 );
//	ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_PLAYER_STATE_MSG2], "\n플레이어pt %d\n$%s\n쥬엘 %s Jewels", g_iPlayerPoint[client], dollars, jewel );
}

/**
 * 플레이어 바라볼 때
 */
void VISUAL_OnLookAtPlayer ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( ZR_IsClientHuman ( client ) ) {
			int target = loringlib_GetClientAimPlayer ( client );
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				if ( ZR_IsClientHuman ( target ) ) {
					char humanclass[64];
					getClientClassName ( g_iClientClass[target], humanclass, sizeof ( humanclass ), false, true );
					if ( StrEqual ( humanclass, "" ) )
						Format ( humanclass, sizeof ( humanclass ), "[Level.%d]", g_iLevel[target] );
					else
						Format ( humanclass, sizeof ( humanclass ), "[Level.%d | %s]", g_iLevel[target], humanclass );
					SetHudTextParams ( -1.0, 0.566, 0.2, 150,198,252, 255, 0, 0.0, 0.0, 0.0 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOOK_AT_PLAYER], "#%d 인간: %N %s 체력: %d／%d%%", GetClientUserId ( target ), target, humanclass, loringlib_GetEntityHealth ( target ), loringlib_GetEntityMaxHealth ( target ) );
				}
				
				else {
					SetHudTextParams ( -1.0, 0.566, 0.2, 234, 88, 87, 255, 0, 0.0, 0.0, 0.0 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOOK_AT_PLAYER], "#%d 좀비: %N", GetClientUserId ( target ), target );
				}
			}
		}
		
		else {
			int target = loringlib_GetClientAimPlayer ( client );
			if ( loringlib_IsValidClient__PlayGame ( target ) ) {
				if ( ZR_IsClientHuman ( target ) ) {
					SetHudTextParams ( -1.0, 0.566, 0.2, 150,198,252, 255, 0, 0.0, 0.0, 0.0 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOOK_AT_PLAYER], "#%d 인간: %N", GetClientUserId ( target ), target );
				}
				
				else {
					SetHudTextParams ( -1.0, 0.566, 0.2, 234, 88, 87, 255, 0, 0.0, 0.0, 0.0 );
					ShowSyncHudText ( client, g_hHudSyncArray[HUDSYNC_LOOK_AT_PLAYER], "#%d 좀비: %N 체력: %d%%", GetClientUserId ( target ), target, loringlib_GetEntityHealth ( target ) );
				}
			}
		}
	}
}

/**
 * 글로벌 타이머 실행
 * @noreturn
 */
stock void createGlobalTimer () {
	if ( g_hGlobalTimer == null )
		g_hGlobalTimer = CreateTimer ( 1.0, timerGlobalTimer, _, TIMER_REPEAT );
}

/**
 * 클라이언트 데이터를 모두 초기화함.
 * @param client
 * @noreturn
 */
stock void resetAllKeys ( int client ) {
	resetClientClassKeys ( client );
	resetClientActiveKeys ( client );
	resetClientItemKeys ( client );
}

/**
 * 클라이언트의 활동 데이터를 초기화함.
 * @note 활동 데이터: EXP, Level, SP, 스텟 레벨, 골드 등
 *
 * @param client
 * @noreturn
 */
stock void resetClientActiveKeys ( int client ) {
	g_iExp[client] = 0;
	g_iLevel[client] = 1;
	g_iStatPoint[client] = 0;
	g_iPlayerPoint[client] = 0;
	for ( int i = 0; i < MAXSTATS; i ++ )
		g_iStats[i][client] = 0;
}

/**
 * 클라이언트의 스텟을 초기화함.
 *
 * @param client
 * @param reset		리셋 여부, true = 0 false = sp를 돌려 받음.
 * @return 처리에 성공하면 true, 아니면 false.
 */
stock bool resetClientStatKeys ( int client, bool reset = true ) {
	int tot = 0;
	for ( int i = 0; i < MAXSTATS; i ++ ) {
		tot += g_iStats[i][client];
		g_iStats[i][client] = 0;
	}
	
	if ( tot <= 0 )
		return false;
	
	if ( !reset )
		g_iStatPoint[client] += tot;
	else
		g_iStatPoint[client] = 0;
		
	return true;
}

/**
 * 클라이언트의 병과 데이터를 초기화함.
 * @note 병과 데이터: 병과 값, 플레이어pt, 기술 레벨 등
 *
 * @param client
 * @param reset			리셋 여부, true = 0 false = 플레이어pt를 돌려 받음
 * @param reset_class	클래스까지 초기화 할까? true = 초기화, false = 그대로 나둔다.
 * @noreturn
 */
stock void resetClientClassKeys ( int client, bool reset = true, bool reset_class = true ) {
	if ( reset ) {
		//	완벽히 초기화
		for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
			for ( int j = 0; j < g_iDefineClassSkills[i]; j ++ ) {
				g_iSkill[i][j][client] = 0;
				g_bSkillCooldown[i][j][client] = false;
				g_bSkillUse[i][j][client] = false;
			}
		}
		
		if ( reset_class )
			g_iClientClass[client] = ClientClass_Normal;
		
		return;
	}
	
	int playerpt_debug_val = 0;
	
	PrintToConsole ( client, " " );
	PrintToConsole ( client, " " );
	
	for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
		if ( i != view_as<int> ( g_iClientClass[client] ) && i != view_as<int> ( ClientClass_Normal ) )
			continue;
		
		for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ ) {
			//	기술레벨이 0 이상일 경우?
			if ( g_iSkill[i][x][client] > 0 ) {
				//	기술 레벨이 0이 될때 까지 돌린다.
				while ( g_iSkill[i][x][client] > 0 ) {
					PrintToConsole ( client, "SKILL_NAME: %s | SKILL_LEVEL: %d", g_strSkillName[i][x], g_iSkill[i][x][client] );
					g_iSkill[i][x][client] --;	//	기술 레벨을 1씩 뺀다.
					//	뺀 기술 값에 필요 플레이어pt를 대입함.
					playerpt_debug_val += getNeedPlayerPoint ( client, view_as<ClientClass> ( i ), x );
				}
			}
		}
	}
	
	PrintToConsole ( client, "돌려 받은 플레이어pt 값: %d 플레이어pt", playerpt_debug_val );
	
	PrintToConsole ( client, " " );
	PrintToConsole ( client, " " );
	
	if ( reset_class )
		g_iClientClass[client] = ClientClass_Normal;
	
	g_iPlayerPoint[client] += playerpt_debug_val;
		
	//	완벽히 초기화
	for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ ) {
		for ( int j = 0; j < g_iDefineClassSkills[i]; j ++ ) {
			g_iSkill[i][j][client] = 0;
			g_bSkillCooldown[i][j][client] = false;
			g_bSkillUse[i][j][client] = false;
		}
	}
}

/**
 * 클라이언트의 아이템 데이터를 초기화함.
 * @param client
 * @noreturn
 */
stock void resetClientItemKeys ( int client ) {
	for ( int i = 0; i < g_iDefineItems; i ++ )
		g_iItem[i][client] = 0;
}

/**
 * 클라이언트 최대 경험치
 * @param client		클라이언트
 * @return 최대 경험치 값
 */
stock int getClientMaxExp ( int client ) {
	if ( g_iLevel[client] <= 1 )
		return EXP_LEVEL1;
	else
		return ( g_iLevel[client] * MAXEXP_SCALE ) + MAXEXP_SCALE_PLUS;
}

/**
 * 레벨 변경 시 호출 되는 함수
 */
public void onChangeLevelup () {
	for ( int i = 1; i <= MaxClients; i ++ )
		if ( loringlib_IsValidClient ( i ) )
			changeLevelup ( i );
}

/**
 * 클라이언트 레벨 업
 * @param client		클라이언트
 * @noreturn
 */
stock void changeLevelup ( int client ) {
	if ( g_iLevel[client] < 1 ) {
		g_iLevel[client] = 1;
		return;
	}
	
	//	Level up block and max level blocking levelup.
	if ( ( g_iClientClass[client] == ClientClass_Normal && g_iLevel[client] == LEVELUP_BLOCK_LEVEL ) ||
		g_iLevel[client] >= MAXLEVEL ) {
		g_iExp[client] = 0;
		return;
	}
	
	if ( g_iExp[client] >= getClientMaxExp ( client ) ) {
		g_iExp[client] -= getClientMaxExp ( client );
		g_iLevel[client] ++;
		g_iStatPoint[client] += REWARD_LEVELUP_SP;
		g_iPlayerPoint[client] += REWARD_LEVELUP_PLAYERPT;
		
		CLIENT_LEVEL_UP_MSG ( client, g_iLevel[client] )
		PrecacheSound ( SOUND_LEVEL_UP );
		EmitSoundToAll ( SOUND_LEVEL_UP );
		
		if ( g_iLevel[client] == LEVELUP_BLOCK_LEVEL )
			if ( g_iClientClass[client] == ClientClass_Normal )
				CLIENT_LEVELUP_THIS_LVUP_BLOCKED_LV_MSG ( client )
		
		NewbieHelp_LevelupOnAlert ( client );
		
		Call_StartForward ( g_hForwardHandlers[ZD_FORWARD_ON_CHANGE_LEVEL] );
		Call_PushCell ( client );
		Call_PushCell ( g_iLevel[client] - 1 );
		Call_PushCell ( g_iLevel[client] );
		Call_PushCell ( getClientMaxExp ( client ) );
		Call_Finish ();
	}
}

/**
 * 뉴비인지 채크함.
 *
 * @param client
 *
 * @return 뉴비일 경우 true, 아닐경우 false.
 */
stock bool isPlayerNewbie ( int client ) {
	if ( client > 0 ) {
		if ( g_iTotalLevels[client] < LEVEL_NEWBIE )
			return true;
	}
	
	return false;
}

/**
 * 스텟 이름을 가져옴.
 * @param statType
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getStatName ( int statType, char[] buffer, int maxlen ) {
	switch ( statType ) {
		case STAT_STR:	Format ( buffer, maxlen, "공격" );
		case STAT_DEF:	Format ( buffer, maxlen, "기력" );
		case STAT_AGL:	Format ( buffer, maxlen, "민첩" );
		default: Format ( buffer, maxlen, "-1" );
	}
}

/**
 * 클래스 이름을 가져옴.
 * @param clientClass
 * @param buffer
 * @param maxlen
 * @param isKeyvalue		키벨류에서 키 이름
 * @param ko				한국어
 * @return 클래스 값
 */
stock int getClientClassName ( ClientClass clientClass, char[] buffer, int maxlen, bool isKeyvalue = false, bool ko = false ) {
	if ( isKeyvalue ) {
		switch ( clientClass ) {
			case ClientClass_Normal: Format ( buffer, maxlen, "client_normal_skill" );
			case ClientClass_Assault: Format ( buffer, maxlen, "client_assualt_skill" );
			case ClientClass_Sniper: Format ( buffer, maxlen, "client_sniper_skill" );
			case ClientClass_Medic: Format ( buffer, maxlen, "client_medic_skill" );
			case ClientClass_Supporter: Format ( buffer, maxlen, "client_supporter_skill" );
			case ClientClass_Shotgunner: Format ( buffer, maxlen, "client_shotgunner_skill" );
			case ClientClass_Gunslinger: Format ( buffer, maxlen, "client_gunslinger_skill" );
			default: Format ( buffer, maxlen, "-1" );
		}
	}
	
	else {
		if ( !ko ) {
			switch ( clientClass ) {
				case ClientClass_Normal: Format ( buffer, maxlen, "" );
				case ClientClass_Assault: Format ( buffer, maxlen, "Assaulter" );
				case ClientClass_Sniper: Format ( buffer, maxlen, "Sniper" );
				case ClientClass_Medic: Format ( buffer, maxlen, "Medic" );
				case ClientClass_Supporter: Format ( buffer, maxlen, "Supporter" );
				case ClientClass_Shotgunner: Format ( buffer, maxlen, "ShotGunner" );
				case ClientClass_Gunslinger: Format ( buffer, maxlen, "Gunslinger" );
				default: Format ( buffer, maxlen, "-1" );
			}
		}
		
		else {
			switch ( clientClass ) {
				case ClientClass_Normal: Format ( buffer, maxlen, "" );
				case ClientClass_Assault: Format ( buffer, maxlen, "돌격병" );
				case ClientClass_Sniper: Format ( buffer, maxlen, "저격병" );
				case ClientClass_Medic: Format ( buffer, maxlen, "의무병" );
				case ClientClass_Supporter: Format ( buffer, maxlen, "보급병" );
				case ClientClass_Shotgunner: Format ( buffer, maxlen, "전문병" );
				case ClientClass_Gunslinger: Format ( buffer, maxlen, "총잡이" );
				default: Format ( buffer, maxlen, "-1" );
			}
		}
	}
	
	return view_as <int> ( clientClass );
}

/**
 * 클래스 파일 이름을 가져옴.
 * @param clientClass
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getClientClassFilename ( ClientClass clientClass, char[] buffer, int maxlen ) {
	switch ( clientClass ) {
		case ClientClass_Normal: Format ( buffer, maxlen, "normal.txt" );
		case ClientClass_Assault: Format ( buffer, maxlen, "assaulter.txt" );
		case ClientClass_Sniper: Format ( buffer, maxlen, "sniper.txt" );
		case ClientClass_Medic: Format ( buffer, maxlen, "medic.txt" );
		case ClientClass_Supporter: Format ( buffer, maxlen, "supporter.txt" );
		case ClientClass_Shotgunner: Format ( buffer, maxlen, "shotgunner.txt" );
		case ClientClass_Gunslinger: Format ( buffer, maxlen, "gunslinger.txt" );
		default: Format ( buffer, maxlen, "error.txt" );
	}
}


/**
 * 기술 이름을 구함.
 *
 * @param id
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getSkillName ( ClientClass clientClass, int id, char[] buffer, int maxlen ) {
	Format ( buffer, maxlen, g_strSkillName[view_as <int> ( clientClass )][id] );
}

/**
 * 기술에 필요한 플레이어pt 량을 구함.
 * @note 필요 플레이어pt가 10 이상이면 0레벨 부터 그 값 만큼 플레이어pt가 필요함.
 * @param client
 * @param clientClass
 * @param skill_Id
 * @return 필요한 플레이어pt 량
 */
stock int getNeedPlayerPoint ( int client, ClientClass clientClass, int skill_Id ) {
	//	Old logic
/*	if ( g_iSkillNeedPrice[clientClass][skill_Id] >= 10 )
		return g_iSkillNeedPrice[clientClass][skill_Id];
	
	else {
		if ( g_iSkillType[clientClass][skill_Id] != SKILL_TYPE_ACTIVE ) {
			if ( g_iSkill[clientClass][skill_Id][client] < 1 )
				return 1;
			else
				return ( g_iSkill[clientClass][skill_Id][client] * g_iSkillNeedPrice[clientClass][skill_Id] );
		}
		
		else
			return g_iSkillNeedPrice[clientClass][skill_Id];
	}	*/
	return g_iSkillNeedPrice[clientClass][skill_Id];
}

/**
 * 기술이 최대 레벨인지 채크함.
 * @param client
 * @param clientClass
 * @param skill_Id
 * @return 최대 레벨이라면 ture, 아니면 false 반환.
 */
stock bool isSkillMaxLvl ( int client, ClientClass clientClass, int skill_Id ) {
	if ( g_iSkill[clientClass][skill_Id][client] >= g_iSkillMaxlvl[clientClass][skill_Id] )
		return true;
	else
		return false;
}

/**
 * 사용 가능한 액티브 스킬인지 채크함.
 * @param client
 * @param skill_Id
 * @return 사용할 수 있다면 true, 아니면 false 반환.
 */
stock bool isPossibleUseSkill ( int client, ClientClass clientClass, int skill_Id ) {
	int clientClassInt = view_as <int> ( clientClass );
	if ( g_iSkillType[clientClassInt][skill_Id] != SKILL_TYPE_ACTIVE ||
		g_iSkill[clientClassInt][skill_Id][client] < 1 ||
		g_bSkillCooldown[clientClassInt][skill_Id][client] ||
		g_bSkillUse[clientClassInt][skill_Id][client] ||
		g_iSkillConsumeAP[clientClassInt][skill_Id] > loringlib_GetEntityArmor ( client ) ) {
//		PrintToServer ( "Skill Type: %d\nSkill Lvl: %d\nSkill Consume AP: %d | Target AP: %d", g_iSkillType[clientClassInt][skill_Id], g_iSkill[clientClassInt][skill_Id][client], g_iSkillConsumeAP[clientClassInt][skill_Id], loringlib_GetEntityArmor ( client ) );
		return false;
	}
	
	return true;
}

/**
 * 장비 아이템 유형의 이름을 가져옴.
 * @param type
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getEquipmentTypeName ( int type, char[] buffer, int maxlen ) {
	switch ( type ) {
		case ITEM_TYPE_EQ_WEAPON:		Format ( buffer, maxlen, "무기" );
		case ITEM_TYPE_EQ_SOULWEAPON:	Format ( buffer, maxlen, "요정" );
		case ITEM_TYPE_EQ_HEAD:			Format ( buffer, maxlen, "모자" );
		case ITEM_TYPE_EQ_TOP:			Format ( buffer, maxlen, "상의" );
		case ITEM_TYPE_EQ_BOTTOMS:		Format ( buffer, maxlen, "하의" );
		case ITEM_TYPE_EQ_BOOTS:		Format ( buffer, maxlen, "신발" );
	}
}

stock bool isValidItem ( int item_code ) {
	bool result = false;
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( i == item_code ) {
			result = true;
			break;
		}
	}
	
	return result;
}

/**
 * 아이템 이름으로 아이템 번호를 가져옴.
 * @param itemName
 * @return 아이템 번호
 */
stock int getItemNum ( char[] itemName ) {
	static int result = -1;
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( StrEqual ( g_strItemName[i], itemName ) ) {
			result = i;
			break;
		}
	}
	
	return result;
}

/**
 * 아이템 번호로 아이템 이름을 가져옴.
 * @param item
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getItemName ( int item, char[] buffer, int maxlen ) {
	if ( item == -1 || item > MAXITEMS )
		return;
		
	Format ( buffer, maxlen, "%s", g_strItemName[item] );
}

/**
 * 아이템 액션 타입이 퍼센테이지인지 채크.
 * @param itemActionType
 * @return 퍼센테이지 값이라면 true, 아니면 false 반환.
 */
stock bool isPer ( int itemActionType ) {
	static bool result;
	switch ( itemActionType ) {
		case ITEM_ACTION_TYPE_STR_PER, ITEM_ACTION_TYPE_DEF_PER, ITEM_ACTION_TYPE_AGL_PER:	result = true;
		default: result = false;
	}
	
	return result;
}

/**
 * 아이템 희귀도 이름을 가져옴.
 * @param item
 * @param buffer
 * @param maxlen
 * @noreturn
 */
stock void getItemRarityName ( int item, char[] buffer, int maxlen ) {
	if ( item < 0 )
		return;
		
	switch ( g_iItemRarity[item] ) {
		case ITEM_RARITY_NORMAL:	Format ( buffer, maxlen, INTERFACE_ITEM_RARITY_NORMAL );
		case ITEM_RARITY_RARE:		Format ( buffer, maxlen, INTERFACE_ITEM_RARITY_RARE );
		case ITEM_RARITY_SR:		Format ( buffer, maxlen, INTERFACE_ITEM_RARITY_UNIQUE );
		case ITEM_RARITY_SSR:		Format ( buffer, maxlen, INTERFACE_ITEM_RARITY_LEGENDARY );
		case ITEM_RARITY_LEGENDARY:		Format ( buffer, maxlen, INTERFACE_ITEM_RARITY_LEGENDARY );
	}
}

/** 
 * 아이템 희귀도 색상을 구함.
 *
 * @param item			아이템 인덱스 값
 * @param format		포멧
 * @param maxlen		포멧 길이 
 */
stock void Item_GetItemRarityColor ( int item, char[] format, int maxlen ) {
	if ( item >= 0 && item <= g_iDefineItems ) {
		switch ( g_iItemRarity[item] ) {
			case ITEM_RARITY_NORMAL:	Format ( format, maxlen, "\x08" );
			case ITEM_RARITY_RARE:		Format ( format, maxlen, "\x0C" );
			case ITEM_RARITY_SR:		Format ( format, maxlen, "\x0E" );
			case ITEM_RARITY_SSR:		Format ( format, maxlen, "\x10" );
			case ITEM_RARITY_LEGENDARY:		Format ( format, maxlen, "\x02" );
			default:					Format ( format, maxlen, "\x08" );
		}
	}
}

/**
 * 아이템 희귀도 색상을 구함. (RGB)
 *
 * @param item			아이템 인덱스 값
 * @param rgb			RGB 색상 대입 변수
 */
stock void Item_GetItemRarityClrRGB ( int item, int rgb[3] ) {
	if ( item >= 0 && item <= g_iDefineItems ) {
		switch ( g_iItemRarity[item] ) {
			case ITEM_RARITY_NORMAL: {
				rgb[0] = 210;
				rgb[1] = 210;
				rgb[2] = 210;
			}
			case ITEM_RARITY_RARE: {
				rgb[0] = 67;
				rgb[1] = 116;
				rgb[2] = 217;
			}
			case ITEM_RARITY_SR: {
				rgb[0] = 200;
				rgb[1] = 166;
				rgb[2] = 60;
			}
			case ITEM_RARITY_SSR: {
				rgb[0] = 200;
				rgb[1] = 60;
				rgb[2] = 60;
			}
			case ITEM_RARITY_LEGENDARY: {
				rgb[0] = 153;
				rgb[1] = 0;
				rgb[2] = 130;
			}
		}
	}
}

void resetClientKeyData ( int client ) {
	g_iLevel[client] = 0;
	g_iExp[client] = 0;
	g_iStatPoint[client] = 0;
	g_iClientClass[client] = ClientClass_Normal;
	g_iPlayerPoint[client] = 0;
	
	g_iResetReserve[client] = -1;
	
	for ( int i = 0; i < MAXSTATS; i ++ ) {
		g_iStats[i][client] = 0;
		g_iBonusStats[i][client] = 0;
	}

	for ( int i = 0; i < MAX_CLIENT_CLASS; i ++ )
		for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ )
			g_iSkill[i][x][client] = 0;
	
/*	for ( int i = 0; i < g_iDefineClassSkills[view_as <int> ( ClientClass_Normal )]; i ++ ) {
		g_iSkill[view_as <int> ( ClientClass_Normal )][i][client] = 0;
	}
	
	if ( g_iClientClass[client] != ClientClass_Normal ) {
		ClientClass classid = g_iClientClass[client];
		for ( int i = 0; i < g_iDefineClassSkills[view_as <int> ( classid )]; i ++ ) {
			g_iSkill[view_as <int> ( classid )][i][client] = 0;
		}
	}	*/
	
	for ( int i = 0; i < MAX_ITEM_TYPES; i ++ ) {
		strcopy ( g_strEquipItem[i][client], ITEM_NAME_MAX_LENGTH, "" );
	}
}

void resetGlobalVarKeyData ( int client ) {
	g_iTotalLevels[client] = 0;
	g_iUnlockSlots[client] = UNLOCK_SLOTS;
	
	g_iGameJoinEventIndex[client] = 0;
	
	g_iRankLevel[client] = 0;
	g_iSurviveCounts[client] = 0;
	g_iTotalActPoints[client] = 0;
	
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		g_iItem[i][client] = 0;
	}
}

/**
 * 라운드 종료 시 초기화 예약이 있다면 초기화한다.
 * @note round_end에서
 */
void ResetMe_RoundEndOnResetData () {
	for ( int client = 1; client <= MaxClients; client ++ ) {
		if ( loringlib_IsValidClient ( client ) ) {
			//	초기화 예약이 있는 클라이언트만
			if ( g_iResetReserve[client] != -1 ) {
				switch ( g_iResetReserve[client] ) {
					case RESET_TYPE_ALL: {	//	전체 초기화
						resetClientClassKeys ( client, true );
						resetClientActiveKeys ( client );
					}
					
					case RESET_TYPE_STATS: {	//	스텟 초기화
						resetClientStatKeys ( client, false );
					}
					
					case RESET_TYPE_SKILLS: {	//	기술 초기화
						resetClientClassKeys ( client, true, false );
						g_iPlayerPoint[client] = g_iLevel[client] - 1;
					}
					
					case RESET_TYPE_CLIENTCLASS: {	//	병과 초기화
						resetClientClassKeys ( client, true, true );
						g_iPlayerPoint[client] = g_iLevel[client] - 1;
					}
				}

				g_iResetReserve[client] = -1;
				CLIENT_RESET_SUCCESS_MSG ( client )
			}
		}
	}
}

/**
 * 클라이언트 데이터를 KeyValues로 부터 불러옴.
 * @param client		클라이언트
 * @param slotIndex		슬롯 값
 * @return	불러오는데 성공하면 true, 아니면 false를 반환.
 */
stock bool defineClientKeyValues ( int client, int slotIndex ) {
	if ( !loringlib_IsValidClient ( client ) )
		return false;
	
	if ( IsFakeClient ( client ) )
		return false;
	
	if ( slotIndex == SLOT_NO_SELECT || slotIndex > MAX_SELECT_SLOT ) {
		LogMessage ( "%N 클라이언트가 데이터를 불러올 때 잘못된 선택 값을 가져왔습니다.", client );
		return false;
	}
	
	static char temp[256];
	static char steam[32];
	static char keyName[64];
	static char tempKeyName[32];
	static char keyBuff[64];
	
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	if ( slotIndex == 1 )
		Format ( keyName, sizeof ( keyName ), "%s", steam );
	else
		Format ( keyName, sizeof ( keyName ), "%s_slot%d", steam, slotIndex );
		
	BuildPath ( Path_SM, temp, sizeof ( temp ), KEYVALUE_PATH, steam );
	
	resetClientKeyData ( client );
	
	KeyValues kv = new KeyValues ( "data" );
	if ( !kv.ImportFromFile ( temp ) ) {
	//	LOG_DEFINE_FAILURE_DATA_MSG ( client, "client_db.txt" )
		resetClientKeyData ( client );
		delete kv;
		return false;
	}
	
/*	if ( !kv.GotoFirstSubKey () ) {
		delete kv;
		return false;
	}	*/
	
	kv.GotoFirstSubKey ();
	
	do {
		kv.GetSectionName ( tempKeyName, sizeof ( tempKeyName ) );
		if ( StrEqual ( tempKeyName, keyName ) ) {
			g_iLevel[client] = kv.GetNum ( "client_level" );
			g_iExp[client] = kv.GetNum ( "client_exp" );
			g_iStatPoint[client] = kv.GetNum ( "client_stat_point" );
			g_iClientClass[client] = view_as < ClientClass> ( kv.GetNum ( "client_class_id" ) );
			g_iPlayerPoint[client] = kv.GetNum ( "client_playerpt" );
			for ( int i = 0; i < MAXSTATS; i ++ ) {	//	stat
				Format ( keyBuff, sizeof ( keyBuff ), "client_stat_%d", i );
				g_iStats[i][client] = kv.GetNum ( keyBuff );
				g_iBonusStats[i][client] = 0;
			}
			
			//	Normal skills.
			for ( int i = 0; i < g_iDefineClassSkills[view_as <int> ( ClientClass_Normal )]; i ++ ) {
				Format ( keyBuff, sizeof ( keyBuff ), "client_normal_skill_%d", i );
				g_iSkill[view_as <int> ( ClientClass_Normal )][i][client] = kv.GetNum ( keyBuff );
			}
			
			//	Client class skills.
			if ( g_iClientClass[client] != ClientClass_Normal ) {
				static char key[32];
				ClientClass classid = g_iClientClass[client];
				getClientClassName ( g_iClientClass[client], key, sizeof ( key ), true );
				for ( int i = 0; i < g_iDefineClassSkills[view_as <int> ( classid )]; i ++ ) {
					Format ( keyBuff, sizeof ( keyBuff ), "%s_%d", key, i );
					g_iSkill[view_as <int> ( classid )][i][client] = kv.GetNum ( keyBuff );
				}
			}
			
			//	Equipment items
			for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
				Format ( keyBuff, sizeof ( keyBuff ), "equipitem_slot%d", i );
				//	아이템 타입이 장착 아이템이 아니거나, 아이템 인덱스 값이 0이거나 보다 낮을 경우
				//	아이템 값을 -1로 한다.
				if ( kv.GetNum ( keyBuff ) <= 0 ||
					g_iItemType[kv.GetNum ( keyBuff )] != ITEM_TYPE_EQUIPMENT )
					g_iEquipItemIndex[i][client] = -1;
				else {
					g_iEquipItemIndex[i][client] = kv.GetNum ( keyBuff );
					itemActionValueToggle ( client, kv.GetNum ( keyBuff ), g_iItemType[kv.GetNum ( keyBuff )], true, i );
				}
			}
	
			SURVIVE_TIME_DataLoadOnResetSurviveTime ( client );
			
			PrintToChat ( client, "%s \x01귀하의 %d번 슬롯의 데이터를 불러왔습니다.", MESSAGE_PREFIX, slotIndex );
			
			delete kv;
			return true;
		}
	} while ( kv.GotoNextKey () );

	delete kv;
	
	return false;
}

/**
 * 클라이언트 데이터를 KeyValues로 부터 저장함.
 * @param client		클라이언트
 * @param slotIndex		슬롯 값
 * @return	저장하는데 성공하면 true, 아니면 false를 반환.
 */
stock bool inputClientKeyValues ( int client, int slotIndex ) {
	if ( !loringlib_IsValidClient ( client ) )
		return false;
	
	if ( IsFakeClient ( client ) )
		return false;
	
	if ( slotIndex == SLOT_NO_SELECT || slotIndex > MAX_SELECT_SLOT ) {
		LogMessage ( "%N 클라이언트의 데이터 저장 도중 잘못된 선택 값을 가져왔습니다.", client );
		return false;
	}
	
	static char temp[PLATFORM_MAX_PATH];
	static char steam[32];
	static char keyName[64];
	static char keyBuff[64];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	
	if ( slotIndex == 1 )
		Format ( keyName, sizeof ( keyName ), "%s", steam );
	else
		Format ( keyName, sizeof ( keyName ), "%s_slot%d", steam, slotIndex );
	
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, KEYVALUE_PATH, steam );
	KeyValues kv = new KeyValues ( "data" );
	kv.ImportFromFile ( temp );
	
/*	if ( !kv.ImportFromFile ( temp ) ) {
		LOG_INPUT_FAILURE_DATA_MSG ( client, "client_db.txt" )
		delete kv;
		return false;
	}	*/
	
	if ( kv.JumpToKey ( keyName, true ) ) {
		kv.SetNum ( "client_level", g_iLevel[client] );
		kv.SetNum ( "client_exp", g_iExp[client] );
		kv.SetNum ( "client_stat_point", g_iStatPoint[client] );
		kv.SetNum ( "client_class_id", view_as <int> ( g_iClientClass[client] ) );
		kv.SetNum ( "client_playerpt", g_iPlayerPoint[client] );
		
		for ( int i = 0; i < MAXSTATS; i ++ ) {	//	Stat
			Format ( keyBuff, sizeof ( keyBuff ), "client_stat_%d", i );
			kv.SetNum ( keyBuff, g_iStats[i][client] );
		//	PrintToServer ( "KeyValue Save: %s %d", keyBuff, kv.GetNum ( keyBuff ) );
		}
		
		int classid = view_as<int> ( ClientClass_Normal );
		//	Normal skills.
		for ( int i = 0; i < g_iDefineClassSkills[classid]; i ++ ) {
			Format ( keyBuff, sizeof ( keyBuff ), "client_normal_skill_%d", i );
			if ( g_iSkill[classid][i][client] > 0 ) {
				kv.SetNum ( keyBuff, g_iSkill[classid][i][client] );
			//	PrintToServer ( "KeyValue Save: %s %d", keyBuff, kv.GetNum ( keyBuff ) );
			} else
				kv.DeleteKey ( keyBuff );
		}
		
		//	Client class skills.
		if ( g_iClientClass[client] != ClientClass_Normal ) {
			classid = view_as<int> ( g_iClientClass[client] );
			static char key[32];
			for ( int i = 1; i < MAX_CLIENT_CLASS; i ++ ) {
				//	클라이언트의 병과가 아닌 병과 기술이 있을 경우 0으로 만든다.
				if ( i != classid ) {
					getClientClassName ( view_as<ClientClass> ( i ), key, sizeof ( key ), true );
					for ( int x = 0; x < g_iDefineClassSkills[i]; x ++ ) {
						if ( g_iSkill[i][x][client] > 0 ) {
							g_iSkill[i][x][client] = 0;
						//	PrintToServer ( "Client Delete Skill ID: CLASS ID %d | SKILL ID %d", i, x );
						}
					}
				}
			}
			
			getClientClassName ( g_iClientClass[client], key, sizeof ( key ), true );
			for ( int i = 0; i < g_iDefineClassSkills[classid]; i ++ ) {
				Format ( keyBuff, sizeof ( keyBuff ), "%s_%d", key, i );
				if ( g_iSkill[classid][i][client] > 0 ) {
					kv.SetNum ( keyBuff, g_iSkill[classid][i][client] );
				//	PrintToServer ( "KeyValue Save: %s %d", keyBuff, kv.GetNum ( keyBuff ) );
				} else
					kv.DeleteKey ( keyBuff );
			}
		}
		
		//	Equipment items
		for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
			Format ( keyBuff, sizeof ( keyBuff ), "equipitem_slot%d", i );
			if ( g_iEquipItemIndex[i][client] <= 0 ||
				g_iItemType[g_iEquipItemIndex[i][client]] != ITEM_TYPE_EQUIPMENT )
				kv.DeleteKey ( keyBuff );
			else {
				kv.SetNum ( keyBuff, g_iEquipItemIndex[i][client] );
				itemActionValueToggle ( client, g_iEquipItemIndex[i][client], g_iItemType[g_iEquipItemIndex[i][client]], false, i );
			}
		}
		
		PrintToServer ( "%N 클라이언트의 %d번 슬롯 데이터를 저장했습니다.", client, slotIndex );
		g_iSelectSlotIndex[client] = SLOT_NO_SELECT;
		
		kv.Rewind ();
		kv.ExportToFile ( temp );
	}
	
	delete kv;
	
	resetClientKeyData ( client );
	
	return true;
}

/**
 * 글로벌 데이터를 불러옴. 접속 시 join_select.inc에서 불러온다.
 *
 * @param client
 * @return 불러오는데 성공했다면 true.
 *
 * @note 글로벌 데이터: 이벤트 인덱스, 캐릭터 슬롯, 아이템 갯수
 */
stock bool defineClientGlobalKeyValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return false;
	
	if ( IsFakeClient ( client ) )
		return false;
	
	static char temp[256];
	static char steam[32];
	static char tempKeyName[32];
	static char keyBuff[64];
	
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, temp, sizeof ( temp ), KEYVALUE_PATH, steam );
	
	resetGlobalVarKeyData ( client );
	
	KeyValues kv = new KeyValues ( "data" );
	if ( !kv.ImportFromFile ( temp ) ) {
		resetGlobalVarKeyData ( client );
		delete kv;
		return false;
	}
	
/*	if ( !kv.GotoFirstSubKey () ) {
		delete kv;
		return false;
	}	*/
	
	kv.GotoFirstSubKey ();
	
	do {
		kv.GetSectionName ( tempKeyName, sizeof ( tempKeyName ) );
		if ( StrEqual ( tempKeyName, "global_variables" ) ) {
			g_iGameJoinEventIndex[client] = kv.GetNum ( "client_game_join_event_index" );
			g_iUnlockSlots[client] = kv.GetNum ( "client_unlock_slots" );
			if ( g_iUnlockSlots[client] < UNLOCK_SLOTS )
				g_iUnlockSlots[client] = UNLOCK_SLOTS;
			
			g_iRankLevel[client] = kv.GetNum ( "client_rank_level" );
			g_iSurviveCounts[client] = kv.GetNum ( "client_total_survivalcounts" );
			g_iTotalActPoints[client] = kv.GetNum ( "client_total_actpoints" );
			
			//	Inventory items.
			for ( int i = 0; i < g_iDefineItems; i ++ ) {
				Format ( keyBuff, sizeof ( keyBuff ), "item_%d", i );
				g_iItem[i][client] = kv.GetNum ( keyBuff );
			}
			
			JOIN_MODE_EVENT_OnDataLoadToJoinModeEventCheck ( client );
			
			delete kv;
			return true;
		}
	} while ( kv.GotoNextKey () );

	delete kv;
	
	return false;
}

/**
 * 글로벌 데이터를 저장함.
 *
 * @param client
 * @return 저장하는데 성공했다면 true.
 */
stock bool inputClientGlobalKeyValues ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return false;
	
	if ( IsFakeClient ( client ) )
		return false;
	
	static char temp[PLATFORM_MAX_PATH];
	static char steam[32];
//	static char keyName[64];
	static char keyBuff[64];
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, KEYVALUE_PATH, steam );
	
	KeyValues kv = new KeyValues ( "data" );
	kv.ImportFromFile ( temp );
	
	if ( kv.JumpToKey ( "global_variables", true ) ) {
		kv.SetNum ( "client_game_join_event_index", g_iGameJoinEventIndex[client] );
		kv.SetNum ( "client_unlock_slots", g_iUnlockSlots[client] );
		kv.SetNum ( "client_rank_level", g_iRankLevel[client] );
		kv.SetNum ( "client_total_survivalcounts", g_iSurviveCounts[client] );
		kv.SetNum ( "client_total_actpoints", g_iTotalActPoints[client] );
		
		//	Inventory items.
		for ( int i = 0; i < g_iDefineItems; i ++ ) {
			Format ( keyBuff, sizeof ( keyBuff ), "item_%d", i );
			if ( g_iItem[i][client] > 0 ) {
				kv.SetNum ( keyBuff, g_iItem[i][client] );
			//	PrintToServer ( "KeyValue Save: %s %d", keyBuff, kv.GetNum ( keyBuff ) );
			} else
				kv.DeleteKey ( keyBuff );
		}	
		
		kv.Rewind ();
		kv.ExportToFile ( temp );
	}
	
	delete kv;
	
	resetGlobalVarKeyData ( client );
	return true;
}

/**
 * 데미지 허드 싱크 만들기
 * @param create
 * @noreturn
 */
stock void damageHudsync ( bool create ) {
	if ( create ) {
		for ( int i = 0; i < 5; i ++ )
			g_hDamageSyncHud[i] = CreateHudSynchronizer ();
	}
	else {
		for ( int i = 1; i <= MaxClients; i ++ )
			if ( loringlib_IsValidClient ( i ) )
				for ( int x = 0; x < 5; x ++ )
					ClearSyncHud ( i, g_hDamageSyncHud[x] );
	}
}

public int noSelector ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	return;
}

/**
 * 무기 이름이 기본 무기인지 채크
 */
stock bool isDefaultWeapon ( char[] name ) {
	bool result = false;
	for ( int i = 0; i < sizeof ( g_strClassWeapon_Default ); i ++ )
		if ( StrEqual ( name, g_strClassWeapon_Default[i], false ) )
			result = true;
	
	return result;
}

/**
 * 무기 이름이 클래스 무기인지 채크
 */
stock bool isClassWeapon ( ClientClass clientClass, char[] name ) {
	if ( clientClass == ClientClass_Normal )
		return true;
	
	bool result = false;
	int iClientClass = view_as<int> ( clientClass );
	for ( int i = 0; i < sizeof ( g_strClassPickUpAllowWeapons[] ); i ++ )
		if ( StrEqual ( name, g_strClassPickUpAllowWeapons[iClientClass][i], false ) )
			result = true;
			
	return result;
}

/**
 * Set client glow.
 */
stock int setClientSkinGlow ( int client, int glowType = 2, const int colors[4] = { 255, 0, 0, 255 } ) {
	char model[PLATFORM_MAX_PATH];
	GetClientModel ( client, model, sizeof ( model ) );
	
	int skin = CPS_SetSkin ( client, model, CPS_RENDER );
	if ( skin > 0 ) {
		int glowOffs;
		if ( ( glowOffs = GetEntSendPropOffs ( skin, "m_clrGlow" ) ) == -1 )
			return -1;
		
		SetEntProp ( skin, Prop_Send, "m_bShouldGlow", true, true );
		SetEntProp ( skin, Prop_Send, "m_nGlowStyle", glowType );
		SetEntPropFloat ( skin, Prop_Send, "m_flGlowMaxDist", 10000000.0 );
		
		SetEntData ( skin, glowOffs, colors[0], _, true );
		SetEntData ( skin, glowOffs + 1, colors[1], _, true );
		SetEntData ( skin, glowOffs + 2, colors[2], _, true );
		SetEntData ( skin, glowOffs + 3, colors[0], _, true );
		
		return skin;
	}
	
	return -1;
}

/**
 * Remove client glow.
 */
stock bool removeClientSkinGlow ( int client ) {
	if ( loringlib_IsValidClient ( client ) ) {
		int skin = CPS_GetSkin ( client );
		if ( skin != INVALID_ENT_REFERENCE ) {
			SetEntProp ( skin, Prop_Send, "m_bShouldGlow", false, true );
			return true;
		}
	}
	
	return false;
}

stock bool isInParty ( int client ) {
	return ( g_bPartyJoined[client] && loringlib_IsValidClient ( g_iPartyManager[client] ) );
}

stock void printToChatParty ( int client, char[] format, any ... ) {
	char buffer[254];
	int member = -1;

	for ( int i = 0; i < PARTY_MAX_MEMBERS; i ++ ) {
		member = StringToInt ( g_strPartyMembers[client][i][PARTY_MEMBER_INDEX] );
		if ( member == -1 )
			continue;
			
		if ( !loringlib_IsValidClient ( member ) )
			continue;
		
		SetGlobalTransTarget ( member );
		VFormat ( buffer, sizeof ( buffer ), format, 3 );
		
		PrintToChat ( member, "%s", buffer );
	}
	
//	SetGlobalTransTarget ( client );
//	VFormat ( buffer, sizeof ( buffer ), format, 3 );
	
//	PrintToChat ( client, "%s", buffer );
}

/**
 * 캐릭터를 선택했는지 채크함.
 *
 * @return 캐릭터를 고르지 않았다면 false, 골랐다면 true.
 */
stock bool isClientCharacterSelected ( int client ) {
	return ( g_iSelectSlotIndex[client] != SLOT_NO_SELECT );
}

/**
 * 병과 캐릭터의 모든 레벨을 구한다.
 *
 * @return total levels
 */
stock int getClientAccountsTotalLevel ( int client ) {
	if ( IsFakeClient ( client ) )
		return 0;
	
	static char temp[PLATFORM_MAX_PATH];
	static char steam[32];
	
	GetClientAuthId ( client, AuthId_SteamID64, steam, sizeof ( steam ) );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, KEYVALUE_PATH, steam );
	
	KeyValues kv = new KeyValues ( "data" );
	
	if ( !kv.ImportFromFile ( temp ) ) {
		delete kv;
		return 0;
	}
	
	static int tempTotalLevels;
	tempTotalLevels = 0;
	
	static char keyBuffer[64];
	for ( int i = 1; i <= MAX_SELECT_SLOT; i ++ ) {
		if ( i == 1 )
			Format ( keyBuffer, sizeof ( keyBuffer ), "%s", steam );
		else
			Format ( keyBuffer, sizeof ( keyBuffer ), "%s_slot%d", steam, i );
		
		if ( kv.JumpToKey ( keyBuffer ) ) {
			tempTotalLevels += kv.GetNum ( "client_level" );
			kv.Rewind ();
		}
	}
	
	delete kv;
	return tempTotalLevels;
}

/**
 * 사운드 프리캐시
 */
void onPrecacheSoundsAll () {
	loringlib_AddFileToDownloadsTableEx ( "sound/%s", ZOMBIESKILL_USE_KILL_SOUNDEFF );
	loringlib_AddFileToDownloadsTableEx ( "sound/%s", SOUND_USE_POTION );
	loringlib_AddFileToDownloadsTableEx ( "sound/%s", SOUND_SNIPER_LAST_BLOW_EXPLODE );
	loringlib_AddFileToDownloadsTableEx ( "sound/%s", SOUND_STRONGZOMBIE_SPAWN_IDLE );
	
	PrecacheSound ( ZOMBIESKILL_USE_KILL_SOUNDEFF, true );
	PrecacheSound ( SOUND_USE_POTION, true );
	PrecacheSound ( SOUND_SNIPER_LAST_BLOW_EXPLODE, true );
	PrecacheSound ( SOUND_SNIPER_VECTOR_BULLET_HIT, true );
	PrecacheSound ( SOUND_BUY_ITEM, true );
	PrecacheSound ( SOUND_STRONGZOMBIE_SPAWN_IDLE, true );
	
	//	병과 개인 사운드
	clientClass_Supporter_PrecacheSounds ();
	ClientClass_Shotgunner_PrecacheSounds ();
	ClientClass_Gunslinger_PrecacheSounds ();
	
	//	아이템 픽업 사운드
	PrecacheSound ( "qufnr/item/pickup_snd_dollars.mp3", true );
	PrecacheSound ( "qufnr/item/pickup_snd_effect.mp3", true );
	static char temp[256];
	for ( int i = 0; i <= ITEM_RARITY_LEGENDARY; i ++ ) {
		Format ( temp, sizeof ( temp ), "qufnr/item/pickup_snd_0%d.mp3", i );
		PrecacheSound ( temp, true );
	}
}

/**
 * 모델 프리캐시
 */
void onPrecacheModelsAll () {
	PrecacheModel ( SNIPER_LAST_BULLET_MISSILE_MODEL, true );
	PrecacheModel ( MODEL_AMMO_BOX, true );
	PrecacheModel ( MODEL_ARMOR_PIERCING_AMMO_BOX, true );
	
	ITEMDROP_PrecacheDropItemModel ();
}

/**
 * 파티클 프리캐시
 */
void onPrecacheParticlesAll () {
	loringlib_PrecacheEffect ( "ParticleEffect" );
	PrecacheGeneric ( "particles/qufnr/class_skills.pcf", true );
	PrecacheGeneric ( "particles/qufnr/class_skills2.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombie_particles2.pcf", true );
	PrecacheGeneric ( "particles/qufnr/class_particles/gunslinger_skill.pcf", true );
	PrecacheGeneric ( "particles/qufnr/class_particles/medic_pgrenade.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombiebomb.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombie/zombie_jump.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombie/zombie_evol.pcf", true );
	PrecacheGeneric ( "particles/qufnr/rare_shine.pcf", true );
	PrecacheGeneric ( "particles/qufnr/zombie/zombie_speedboost.pcf", true );
	AddFileToDownloadsTable ( "particles/qufnr/class_skills.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/class_skills2.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombie_particles2.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/class_particles/gunslinger_skill.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/class_particles/medic_pgrenade.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombiebomb.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombie/zombie_jump.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombie/zombie_evol.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/rare_shine.pcf" );
	AddFileToDownloadsTable ( "particles/qufnr/zombie/zombie_speedboost.pcf" );
	loringlib_PrecacheParticleName ( "heal_pulse" );
	loringlib_PrecacheParticleName ( "heal_up" );
	loringlib_PrecacheParticleName ( "heal_poisonshot" );
	loringlib_PrecacheParticleName ( "heal_nanopulse" );
	loringlib_PrecacheParticleName ( "heal_nanoboost" );
	loringlib_PrecacheParticleName ( "heal_healscreen" );
	loringlib_PrecacheParticleName ( "assault_breaklimit" );
	loringlib_PrecacheParticleName ( "assault_encourage" );
	loringlib_PrecacheParticleName ( "assault_encourage_recieve" );
	loringlib_PrecacheParticleName ( "assault_leader" );
	loringlib_PrecacheParticleName ( "assault_breaklimit2" );
	loringlib_PrecacheParticleName ( "assault_overhitshot" );
	loringlib_PrecacheParticleName ( "assault_encourage2" );
	loringlib_PrecacheParticleName ( "assault_encourage2_recieve" );
	loringlib_PrecacheParticleName ( "sniper_huntermark" );
	loringlib_PrecacheParticleName ( "sniper_huntermarkshot" );
	loringlib_PrecacheParticleName ( "sniper_huntermark_target" );
	loringlib_PrecacheParticleName ( "sniper_huntermarkshot_explode" );
	loringlib_PrecacheParticleName ( "support_iceshot" );
	loringlib_PrecacheParticleName ( "support_fireshot" );
	loringlib_PrecacheParticleName ( "support_armorshot" );
	loringlib_PrecacheParticleName ( "zombie_howl" );
	loringlib_PrecacheParticleName ( "zombie_infecttrail" );
	loringlib_PrecacheParticleName ( "special_pulse_child1" );
	loringlib_PrecacheParticleName ( "special_mine_explode" );
	loringlib_PrecacheParticleName ( "special_explosiverounds" );
	loringlib_PrecacheParticleName ( "gunslinger_bulletdance" );
	loringlib_PrecacheParticleName ( "gunslinger_bullettime" );
	loringlib_PrecacheParticleName ( "gunslinger_bullettime2" );
	loringlib_PrecacheParticleName ( "gunslinger_mccree_aimstart" );
	loringlib_PrecacheParticleName ( "gunslinger_mccree_impact" );
	loringlib_PrecacheParticleName ( "gunslinger_mccree_himpact" );
	loringlib_PrecacheParticleName ( "gunslinger_flash" );
	loringlib_PrecacheParticleName ( "gunslinger_flash_star" );
	loringlib_PrecacheParticleName ( "gunslinger_bulletdance" );
	loringlib_PrecacheParticleName ( "special_light1" );
	loringlib_PrecacheParticleName ( "special_light2" );
	loringlib_PrecacheParticleName ( "support_guard_mid" );
	loringlib_PrecacheParticleName ( "support_guard_start" );
	loringlib_PrecacheParticleName ( "support_guard_break" );
	loringlib_PrecacheParticleName ( "support_exguard_break" );
	loringlib_PrecacheParticleName ( "support_exguard_mid" );
	loringlib_PrecacheParticleName ( "support_exguard_start" );
	loringlib_PrecacheParticleName ( "gunslinger_bulletdance" );
	loringlib_PrecacheParticleName ( "gunslinger_bulletdance_hit" );
	loringlib_PrecacheParticleName ( "gunslinger_exbulletdance_aura" );
	loringlib_PrecacheParticleName ( "gunslinger_exbulletdance" );
	loringlib_PrecacheParticleName ( "gunslinger_exbulletdance_explode_shot_child6" );
	loringlib_PrecacheParticleName ( "gunslinger_exbulletdance_explode_ready" );
	loringlib_PrecacheParticleName ( "gunslinger_exbulletdance_explode_shot" );
	loringlib_PrecacheParticleName ( "medic_pgrenade" );
	loringlib_PrecacheParticleName ( "zombie_bomb_explode" );
	loringlib_PrecacheParticleName ( "zombie_bomb_trail" );
	loringlib_PrecacheParticleName ( "zombie_jump" );
	loringlib_PrecacheParticleName ( "zombie_up" );
	loringlib_PrecacheParticleName ( "zombie_upgrade" );
	loringlib_PrecacheParticleName ( "shine_normal" );
	loringlib_PrecacheParticleName ( "shine_rare" );
	loringlib_PrecacheParticleName ( "zombie_speedboost" );
	loringlib_PrecacheParticleName ( "zombie_speedboost_screen" );
}

/** 
 * Get world size.
 */
/*void getMaxWorldLength () {
	float minhull[3], maxhull[3];
	GetEntPropVector ( 0, Prop_Send, "m_WorldMins", minhull );
	GetEntPropVector ( 0, Prop_Send, "m_WorldMaxs", maxhull );
	g_fMaxWorldLength = GetVectorDistance ( minhull, maxhull );
}	*/