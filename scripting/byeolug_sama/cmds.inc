public void registerCommands () {
	//	Users
	RegConsoleCmd ( "sm_menu", clientCommandMenu );				//	메인 메뉴
	RegConsoleCmd ( "rebuy", clientCommandMenu );				//	f3
	RegConsoleCmd ( "sm_info", clientCommandInfo );		//	정보 보기
	RegConsoleCmd ( "sm_hclass", clientCommandHumanClass );		//	병과 선택 메뉴
	RegConsoleCmd ( "sm_stat", clientCommandStat );				//	스텟
	RegConsoleCmd ( "sm_party", clientCommandPartyMenu );		//	파티
	RegConsoleCmd ( "sm_resetme", clientCommandResetMe );		//	리셋 명령어
	RegConsoleCmd ( "sm_resetcancel", clientCommandResetCancel );	//	리셋 취소 명령어
	RegConsoleCmd ( "sm_rank", clientCommandRankLevel );			//	랭크 보기
	RegConsoleCmd ( "sm_round", clientCommandRound );			//	라운드 상황 보기
	
	//	Admins
	RegAdminCmd ( "sm_zdadmin", clientCommandAdminCmd, ADMFLAG_ROOT );
	RegAdminCmd ( "sm_zdgiveitem", clientCommandGiveItem, ADMFLAG_ROOT );	//	아이템 지급 명령어
	RegConsoleCmd ( "sm_resetitem", clientCommandResetItemMenu );
	RegConsoleCmd ( "@itemlist", clientCommandItemList );
	
	RegServerCmd ( "itemlist_print", serversideCommandItemList );
	
	AddCommandListener ( onCommandDrop, "drop" );
	
	registerCmdsOnRegPartyCmds ();
}

public Action OnPlayerRunCmd ( int client, int& key, int& impulse, float vel[3], float angles[3], int& weapon, int& subtype, int& cmdnum, int& tickcount, int& seed, int mouse[2] ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	clientClass_Supporter_UseAmmobox ( client, key );	//	Supporter ammo box
	
	static bool clientReloading[MAXPLAYERS + 1];
	static char weaponname[32];
	int active = loringlib_GetActiveWeaponName ( client, weaponname, sizeof ( weaponname ) );
	if ( active == INVALID_ENT_REFERENCE )
		return Plugin_Continue;	

	//	메디샷 왼쪽 막기
//	if ( StrContains ( weaponname, "healthshot", false ) != -1 )
//		SetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack", GetGameTime() + 9999.0 );
	
	//	Press E(USE).
	if ( key & IN_USE ) {
		if ( !g_bPressKey[KEY_IN_USE][client] ) {
			ItemDrop_UseOnGetDropItem ( client );
			g_bPressKey[KEY_IN_USE][client] = true;
		}
	}	else	g_bPressKey[KEY_IN_USE][client] = false;
	
	//	Press Shift(WALKING).
	if ( key & IN_SPEED ) {
		if ( !g_bPressKey[KEY_IN_SPEED][client] ) {
			clientCommandASKMenu ( client );
			g_bPressKey[KEY_IN_SPEED][client] = true;
		}
		key &= ~IN_SPEED;
		return Plugin_Changed;
	}	else	g_bPressKey[KEY_IN_SPEED][client] = false;
	
	//	Press R(Reload).
	if ( key & IN_RELOAD ) {
		if ( !g_bPressKey[KEY_IN_RELOAD][client] ) {
			ZOMBIESKILL_RunCmdOnUseZombieSkill ( client );
			clientClass_Supporter_ChangeBarricadeGrenade ( client );
			g_bPressKey[KEY_IN_RELOAD][client] = true;
		}
	}	else	g_bPressKey[KEY_IN_RELOAD][client] = false;
	
	//	Press 'attack(Mouse1)' key
	if ( key & IN_ATTACK ) {
		if ( !g_bPressKey[KEY_IN_ATTACK][client] ) {
			g_bPressKey[KEY_IN_ATTACK][client] = true;
	//		onUseMedishot ( client );
			clientClass_Supporter_RunCmdOnBarricadeBreak ( client );
		}
	}	else	g_bPressKey[KEY_IN_ATTACK][client] = false;
	
	//	Press 'secondary attack(Mouse2)' key
	if ( key & IN_ATTACK2 ) {
		if ( !g_bPressKey[KEY_IN_ATTACK2][client] ) {
			g_bPressKey[KEY_IN_ATTACK2][client] = true;
			onUseMedishotTarget ( client );
		}
	}	else	g_bPressKey[KEY_IN_ATTACK2][client] = false;
	
	//	Weapon reloading
	bool reloading = loringlib_IsWeaponReloading ( active );
	if ( !reloading &&
		HasEntProp ( active, Prop_Send, "m_reloadState" ) &&
		GetEntProp ( active, Prop_Send, "m_reloadState" ) > 0 )
		reloading = true;
	
	if ( reloading && !clientReloading[client] )
		changeWeaponReloadSpeed ( client );
	
	clientReloading[client] = reloading;
	
	return Plugin_Continue;
}

public Action clientCommandResetCancel ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iResetReserve[client] != -1 ) {
			PrintToChat ( client, " \x04[Reset] \x01Reset options are canceled." );
			g_iResetReserve[client] = -1;
		}
	}
	
	return Plugin_Handled;
}

public Action clientCommandResetMe ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( !isClientCharacterSelected ( client ) ) {
			CLIENT_YOU_NO_SELECTE_CHARACTER_MSG ( client )
			return Plugin_Handled;
		}
		Menu menu = new Menu ( resetMenuHandler );
		menu.SetTitle ( "%s\n\t무엇을 초기화 할까요?\n　", MENU_TITLE );
		menu.AddItem ( "0", "전체 초기화\n\t\t레벨 1로 돌아갑니다..." );
		menu.AddItem ( "1", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "2", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "3", "스텟 초기화\n\t\t말 그대로 스텟만 초기화합니다.\n\t\t기술과 병과에는 아무 지장 없습니다." );
		menu.AddItem ( "4", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "5", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "6", "기술 초기화\n\t\t기술만 초기화합니다.\n\t\t단, 플레이어pt는 레벨에 맞게 조정됩니다." );
		menu.AddItem ( "7", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "8", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "9", "병과 초기화\n\t\t병과와 기술이 모두 초기화됩니다.\n\t\t플레이어pt는 기술 초기화와 동일합니다.\n　" );
		menu.AddItem ( "10", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "11", "#SPACE_ITEM", ITEMDRAW_SPACER );
		
		menu.AddItem ( "", "한번 초기화 하시면 다시는 복구할 수 없습니다.", ITEMDRAW_DISABLED );
		
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	return Plugin_Handled;
}

public int resetMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: ResetMe_All ( param1 );
				case 3: ResetMe_ResetStats ( param1 );
				case 6: ResetMe_ResetSkills ( param1 );
				case 9: ResetMe_ResetClientClass ( param1 );
			}
		}
	}
}

/**
 * 전체 초기화 메뉴
 */
void ResetMe_All ( int client ) {
	Menu menu = new Menu ( resetmeAllMenuHandler );
	menu.SetTitle ( "%s\n\t전체 초기화\n　", MENU_TITLE );
	menu.AddItem ( "0", "정말 초기화를 하고싶나요?\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "1", "설마 지겨워서 접으려고 초기화 하시는건가요?\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "2", "조금만 더 힘내시면 강해질 수 있을거에요. 'Cancel'을 눌러 뒤로가세요.\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "3", "다른 이유인가요? 그러면 지금 'Yes'를 눌러 시작하세요. 귀하의 새로운 삶을...\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "4", "Yes" );
	menu.AddItem ( "5", "Cancel" );
	menu.ExitButton = false;
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int resetmeAllMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 4: {
					g_iResetReserve[param1] = RESET_TYPE_ALL;	
					CLIENT_SELECT_RESET_MSG ( param1 )
				}
				
				case 5: clientCommandResetMe ( param1, 0 );
			}
		}
	}
}

/** 
 * 스텟 초기화 메뉴
 */
void ResetMe_ResetStats ( int client ) {
	Menu menu = new Menu ( resetmeStatsMenuHandler );
	menu.SetTitle ( "%s\n\t스텟 초기화\n　", MENU_TITLE );
	menu.AddItem ( "0", "가벼운 마음으로...\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "1", "?", ITEMDRAW_NOTEXT );
	menu.AddItem ( "2", "??", ITEMDRAW_NOTEXT );
	char info[256];
	Format ( info, sizeof ( info ), "\t\t\t사용 제한: %d ~ %d Level\n　", RESET_STATS_ALLOW_MIN_LEVEL, RESET_STATS_ALLOW_MAX_LEVEL );
	menu.AddItem ( "3", info, ITEMDRAW_DISABLED );
	menu.AddItem ( "4", RESET_STATS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_STATS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? "Yes" : "LOCKED", RESET_STATS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_STATS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "5", "Cancel" );
	menu.ExitButton = false;
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int resetmeStatsMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 4: {
					if ( RESET_STATS_ALLOW_MIN_LEVEL <= g_iLevel[param1] && RESET_STATS_ALLOW_MAX_LEVEL >= g_iLevel[param1] ) {
						g_iResetReserve[param1] = RESET_TYPE_STATS;	
						CLIENT_SELECT_RESET_MSG ( param1 )
					}
				}
				
				case 5: clientCommandResetMe ( param1, 0 );
			}
		}
	}
}

/**
 * 기술 초기화 메뉴
 */
void ResetMe_ResetSkills ( int client ) {
	Menu menu = new Menu ( resetSkillsMenuHandler );
	menu.SetTitle ( "%s\n\t기술 초기화\n　", MENU_TITLE );
	menu.AddItem ( "0", "어떤 기술이 마음에 안드나요?\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "1", "기술이 마음에 안들거나, 안좋다고 생각하시면 디스코드 #feedback-suggestion 에서 피드백 바랍니다!\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "2", ".", ITEMDRAW_NOTEXT );
	char info[256];
	Format ( info, sizeof ( info ), "\t\t\t사용 제한: %d ~ %d Level\n　", RESET_SKILLS_ALLOW_MIN_LEVEL, RESET_SKILLS_ALLOW_MAX_LEVEL );
	menu.AddItem ( "3", info, ITEMDRAW_DISABLED );
	menu.AddItem ( "4", RESET_SKILLS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_SKILLS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? "Yes" : "LOCKED", RESET_SKILLS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_SKILLS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "5", "Cancel" );
	menu.ExitButton = false;
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int resetSkillsMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 4: {
					if ( RESET_SKILLS_ALLOW_MIN_LEVEL <= g_iLevel[param1] && RESET_SKILLS_ALLOW_MAX_LEVEL >= g_iLevel[param1] ) {
						g_iResetReserve[param1] = RESET_TYPE_SKILLS;	
						CLIENT_SELECT_RESET_MSG ( param1 )
					}
				}
				
				case 5: clientCommandResetMe ( param1, 0 );
			}
		}
	}
}

/** 
 * 병과 초기화 메뉴
 */
void ResetMe_ResetClientClass ( int client ) {
	Menu menu = new Menu ( resetClientClassMenuHandler );
	menu.SetTitle ( "%s\n\t병과 초기화\n　", MENU_TITLE );
	menu.AddItem ( "0", "아니... 그 병과가 OP란 말이에요.\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "1", "안좋다고 생각하신다구요? 그럼 디스코드 #feedback-suggestion 에서 피드백 바랍니다. :)\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( "2", "..", ITEMDRAW_NOTEXT );
	char info[256];
	Format ( info, sizeof ( info ), "\t\t\t사용 제한: %d ~ %d Level\n　", RESET_CLIENTCLASS_ALLOW_MIN_LEVEL, RESET_CLIENTCLASS_ALLOW_MAX_LEVEL );
	menu.AddItem ( "3", info, ITEMDRAW_DISABLED );
	menu.AddItem ( "4", RESET_CLIENTCLASS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_CLIENTCLASS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? "Yes" : "LOCKED", RESET_CLIENTCLASS_ALLOW_MIN_LEVEL <= g_iLevel[client] && RESET_CLIENTCLASS_ALLOW_MAX_LEVEL >= g_iLevel[client] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "5", "Cancel" );
	menu.ExitButton = false;
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int resetClientClassMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 4: {
					if ( RESET_CLIENTCLASS_ALLOW_MIN_LEVEL <= g_iLevel[param1] && RESET_CLIENTCLASS_ALLOW_MAX_LEVEL >= g_iLevel[param1] ) {
						g_iResetReserve[param1] = RESET_TYPE_CLIENTCLASS;	
						CLIENT_SELECT_RESET_MSG ( param1 )
					}
				}
				
				case 5: clientCommandResetMe ( param1, 0 );
			}
		}
	}
}

/**
 * 병과 선택
 */
public Action clientCommandHumanClass ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	if ( !isClientCharacterSelected ( client ) ) {
		CLIENT_YOU_NO_SELECTE_CHARACTER_MSG ( client )
		return Plugin_Handled;
	}
	
	if ( g_iClientClass[client] != ClientClass_Normal ) {
		CLIENT_CHOOSE_CLASS_FAILED_MSG ( client )
		return Plugin_Handled;
	}
	
	clientChooseClassMenu ( client );
	
	return Plugin_Handled;
}

/**
 * 정보 보기
 */
public Action clientCommandInfo ( int client, int args ) {
	if ( !isClientCharacterSelected ( client ) ) {
		CLIENT_YOU_NO_SELECTE_CHARACTER_MSG ( client )
		return Plugin_Handled;
	}

	if ( args < 1 ) {
		clientInfo ( client, -1 );
		return Plugin_Handled;
	}
	
	static char argname[32];
	GetCmdArg ( 1, argname, sizeof ( argname ) );
	
	static int target;
	target = loringlib_FindTarget ( argname );
	
	if ( target == -1 ) {
		ReplyToCommand ( client, " \x04[Info] \x01%s은(는) 유효하지 않는 클라이언트 명 입니다.", argname );
		return Plugin_Handled;
	}
	
	clientInfo ( client, target );
	return Plugin_Handled;
}

/**
 * 클라이언트 정보 init
 */
public void clientInfo ( int client, int target ) {
	Menu menu = new Menu ( clientInfoMenuHandler );
	static char title[128];
	static char info[5][256];
	if ( target == -1 ) {
		Format ( title, sizeof ( title ), "%s\n\t정보\n　", MENU_TITLE );
		Format ( info[0], sizeof ( info[] ), "\n\t\t병과: %s", g_strClientClassName[view_as <int> ( g_iClientClass[client] )] );
		Format ( info[1], sizeof ( info[] ), "\n\t\tPLV: %d | EXP: %d / %d", g_iLevel[client], g_iExp[client], getClientMaxExp ( client ) );
		Format ( info[2], sizeof ( info[] ), "\n\t\t스탯pt: %d", g_iStatPoint[client] );
		Format ( info[3], sizeof ( info[] ), "\n\t\t플레이어pt: %d", g_iPlayerPoint[client] );
		Format ( info[4], sizeof ( info[] ), "\n　\n\t\t누적 생존 수: %d 회\n\t\t누적 Act Points: %d", g_iSurviveCounts[client], g_iTotalActPoints[client] );
	}
	
	else {
		if ( !loringlib_IsValidClient ( target ) )
			return;
		
		Format ( title, sizeof ( title ), "%s\n\t%N님의 정보\n　", MENU_TITLE, target );
		Format ( info[0], sizeof ( info[] ), "\n\t\t병과: %s", g_strClientClassName[view_as <int> ( g_iClientClass[target] )] );
		Format ( info[1], sizeof ( info[] ), "\n\t\tPLV: %d | EXP: %d / %d", g_iLevel[target], g_iExp[target], getClientMaxExp ( target ) );
		Format ( info[2], sizeof ( info[] ), "\n\t\t스탯pt: %d", g_iStatPoint[target] );
		Format ( info[3], sizeof ( info[] ), "\n\t\t플레이어pt: %d", g_iPlayerPoint[target] );
		Format ( info[4], sizeof ( info[] ), "\n　\n\t\t누적 생존 수: %d 회\n\t\t누적 Act Points: %d", g_iSurviveCounts[target], g_iTotalActPoints[target] );
	}
	
	menu.SetTitle ( "%s%s%s%s%s%s\n　", title, info[0], info[1], info[2], info[3], info[4] );
	if ( target != -1 ) {
		static char targetId[16];
		IntToString ( target, targetId, sizeof ( targetId ) );
		menu.AddItem ( targetId, "장비" );
		menu.AddItem ( targetId, "인벤토리" );
	}
	
	menu.AddItem ( "", "", ITEMDRAW_NOTEXT );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientInfoMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			
			static int target;
			target = StringToInt ( item );
			
			if ( !loringlib_IsValidClient ( target ) )
				return;
			
			switch ( param2 ) {
				case 0:		targetViewEquipments ( param1, target );
				case 1:		targetViewInventory ( param1, target );
				default: return;
			}
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientCommandMenu ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * 타겟 착용 장비 보기
 */
void targetViewEquipments ( int client, int target ) {
	if ( !loringlib_IsValidClient ( target ) )
		return;
	
	Menu menu = new Menu ( targetEquipmentMenuHandle );
	menu.SetTitle ( "%s\n\t%N님의 장비\n　", MENU_TITLE, target );
	
	static char buffer[256];
//	static char typename[32];
	
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
		if ( g_iEquipItemSlotUnlock[i] > g_iLevel[target] ) {
			Format ( buffer, sizeof ( buffer ), "유물: [잠김]", g_iEquipItemSlotUnlock[i] );
			menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
		}
		
		else {
			if ( g_iEquipItemIndex[i][target] <= 0 ||
				g_iItemType[g_iEquipItemIndex[i][target]] != ITEM_TYPE_EQUIPMENT ) {
				Format ( buffer, sizeof ( buffer ), "유물: [%s]", INTERFACE_NO_EQUIPMENT_ITEM );
			}
			else {
				Format ( buffer, sizeof ( buffer ), "유물: %s", g_strItemName[g_iEquipItemIndex[i][target]] );
			}
			
			menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
		}
	}
	
/*	for ( int i = ITEM_TYPE_EQ_WEAPON; i <= ITEM_TYPE_EQ_SOULWEAPON; i ++ ) {
		getEquipmentTypeName ( i, typename, sizeof ( typename ) );
		Format ( buffer, sizeof ( buffer ), "%s: %s", typename, StrEqual ( g_strEquipItem[i][client], "" ) ? INTERFACE_NO_EQUIPMENT_ITEM : g_strEquipItem[i][client] );
		menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
	}	*/
	
	menu.AddItem ( "", " ", ITEMDRAW_SPACER );
	
	static char backButton[32];
	Format ( backButton, sizeof ( backButton ), "#BACK_BUTTON@%d", target );
	menu.AddItem ( backButton, "뒤로" );
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int targetEquipmentMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char option[32];
			menu.GetItem ( param2, option, sizeof ( option ) );
			if ( StrContains ( option, "#BACK_BUTTON", false ) != -1 ) {
				static char temp[2][16];
				ExplodeString ( option, "@", temp, sizeof ( temp ), sizeof ( temp[] ) );
				static int targetId;
				targetId = StringToInt ( temp[1] );
				
				if ( !loringlib_IsValidClient ( targetId ) )
					clientCommandMenu ( param1, 0 );
				else
					clientInfo ( param1, targetId );
			}
		}
	}
}

/**
 * 타겟 인벤토리 보기
 */
void targetViewInventory ( int client, int target ) {
	if ( !loringlib_IsValidClient ( target ) )
		return;
	
	Menu menu = new Menu ( targetInventoryMenuHandle );
	menu.SetTitle ( "%s\n\t%N님의 인벤토리\n　", MENU_TITLE, target );
	
	static bool hasItems;
	hasItems = false;
	static char buffer[256];
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( g_iItem[i][target] > 0 ) {
			hasItems = true;
			Format ( buffer, sizeof ( buffer ), "%s [%d개]", g_strItemName[i], g_iItem[i][target] );
			menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
		}
	}
	
	if ( !hasItems )
		menu.AddItem ( "", "비어있음", ITEMDRAW_DISABLED );
	
	menu.AddItem ( "", " ", ITEMDRAW_SPACER );
	
	static char backButton[32];
	Format ( backButton, sizeof ( backButton ), "#BACK_BUTTON@%d", target );
	menu.AddItem ( backButton, "뒤로" );
	menu.ExitBackButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int targetInventoryMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char option[32];
			menu.GetItem ( param2, option, sizeof ( option ) );
			if ( StrContains ( option, "#BACK_BUTTON", false ) != -1 ) {
				static char temp[2][16];
				ExplodeString ( option, "@", temp, sizeof ( temp ), sizeof ( temp[] ) );
				static int targetId;
				targetId = StringToInt ( temp[1] );
				
				if ( !loringlib_IsValidClient ( targetId ) )
					clientCommandMenu ( param1, 0 );
				else
					clientInfo ( param1, targetId );
			}
		}
	}
}

/**
 * 아이템 목록
 */
public Action clientCommandItemList ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Handled;
	
	if ( !loringlib_IsClientAdmin ( client ) )
		return Plugin_Handled;
	
	Menu menu = new Menu ( itemListMenuHandler );
	menu.SetTitle ( "%s\n\tItem List\n　", MENU_TITLE );
	static char buff[256];
	static char item[4];
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		IntToString ( i, item, sizeof ( item ) );
		Format ( buff, sizeof ( buff ), "[%d] %s%s", i, g_strItemName[i], g_bItemSell[i] ? " [SELL]" : "" );
		menu.AddItem ( item, buff );
	}
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int itemListMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			itemSpec ( param1, itemId, true );
		}
	}
}

/**
 * 서버에 아이템 출력
 **/
public Action serversideCommandItemList ( int args ) {
	PrintToServer ( "ITEM INDEX | ITEM NAME | ITEM PRICE | ITEM STATUS" );
	for ( int i = 0; i < g_iDefineItems; i ++ )
		PrintToServer ( "%d | %s | %d | %s", i, g_strItemName[i], g_iItemPrice[i], g_bItemSell[i] ? " [IN SHOP]" : "[PRIVATE]" );
	return Plugin_Handled;
}


public Action onCommandDrop ( int client, const char[] cmds, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		FakeClientCommand ( client, "sm_zbuy" );
	}
	
	return Plugin_Handled;
}