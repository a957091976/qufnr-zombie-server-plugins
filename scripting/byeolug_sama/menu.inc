static bool g_bSkillMenuSelectPosition[MAXPLAYERS + 1] = { true, ... };
 
/**
 * 액티브 스킬 메뉴
 */
public void clientCommandASKMenu ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( !IsPlayerAlive ( client ) )
		return;
		
	if ( !ZR_IsClientHuman ( client ) )
		return;
	
	Menu menu = new Menu ( commandASKMenuHandler );
	menu.SetTitle ( "%s\n\t무엇을 할까?\n　", MENU_TITLE );
	menu.AddItem ( "", "일반 기술", !ZR_IsGameStart () ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	if ( g_iClientClass[client] != ClientClass_Normal )
		menu.AddItem ( "", "병과 기술\n　", !ZR_IsGameStart () ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	else
		menu.AddItem ( "", "!\n　", ITEMDRAW_SPACER );
	menu.AddItem ( "", "소비 아이템 사용" );
	menu.AddItem ( "", "!", ITEMDRAW_SPACER );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int commandASKMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: openASKList ( param1, ClientClass_Normal );
				case 1: openASKList ( param1, g_iClientClass[param1] );
				case 2: openConsumeItemMenu ( param1 );
			}
		}
	}
}

/**
 * 소비 아이템 목록
 */
public void openConsumeItemMenu ( int client ) {
	Menu menu = new Menu ( openConsumeItemMenuHandler );
	menu.SetTitle ( "%s\n\t소비 아이템 사용:\n　\n\t\t* 선택 시 바로 사용 됩니다.\n　", MENU_TITLE );
	
	static char itemId[4];
	static char format[64];
	bool empty = true;
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( g_iItemType[i] == ITEM_TYPE_CONSUME_AP ||
			g_iItemType[i] == ITEM_TYPE_CONSUME_HP ) {
			if ( g_iItem[i][client] > 0 ) {
				empty = false;
				
				IntToString ( i, itemId, sizeof ( itemId ) );
				Format ( format, sizeof ( format ), "%s [%d개]", g_strItemName[i], g_iItem[i][client] );
				
				menu.AddItem ( itemId, format );
			}
		}
	}
	
	if ( empty )
		menu.AddItem ( "", "비어있음", ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int openConsumeItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( ZR_IsClientZombie ( param1 ) )
				return;
			
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			
			consumeItemUse ( param1, itemId, g_iItemType[itemId] );
			openConsumeItemMenu ( param1 );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientCommandASKMenu ( param1 );
				}
			}
		}
	}
}

/**
 * 액티브 스킬 목록
 */
public void openASKList ( int client, ClientClass clientClass ) {
	int clientClassInt = view_as <int> ( clientClass );
	static char name[32];
	getClientClassName ( clientClass, name, sizeof ( name ) );
	bool empty = false;
	Menu menu = new Menu ( openASKListMenuHandler );
	static char status[16];
	static char format[256];
	static char cooldown[256];
	if ( clientClass == ClientClass_Normal )
		menu.SetTitle ( "%s\n\t액티브 기술:\n　", MENU_TITLE );
	else
		menu.SetTitle ( "%s\n\t%s 병과 액티브 기술:\n　", MENU_TITLE, name );
		
	if ( !ZR_IsGameStart () ) {
		menu.AddItem ( "", "지금은 사용할 수 없습니다.", ITEMDRAW_DISABLED );
		menu.Display ( client, MENU_TIME_FOREVER );
		return;
	}
		
	for ( int i = 0; i < g_iDefineClassSkills[clientClassInt]; i ++ ) {
		if ( g_iSkillType[clientClassInt][i] == SKILL_TYPE_ACTIVE ) {
			if ( g_iSkill[clientClassInt][i][client] > 0 ) {
				if ( g_bSkillCooldown[clientClassInt][i][client] )
					Format ( cooldown, sizeof ( cooldown ), "\n\t\t재사용 대기시간: %d Second%s", g_iSkillCooldownTicks[clientClassInt][i][client], g_iSkillCooldownTicks[clientClassInt][i][client] > 1 ? "s" : "" );
				else
					Format ( cooldown, sizeof ( cooldown ), "" );
				Format ( status, sizeof ( status ), "%d@%d", clientClassInt, i );
				Format ( format, sizeof ( format ), "%s (%d LEVEL)\n\t\t소모 AP: %d%s\n　", g_strSkillName[clientClassInt][i], g_iSkill[clientClassInt][i][client], g_iSkillConsumeAP[clientClassInt][i], cooldown );
				menu.AddItem ( status, format, isPossibleUseSkill ( client, clientClass, i ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
				empty = true;
			}
		}
	}
	
	if ( !empty )
		menu.AddItem ( "", "비어있음", ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int openASKListMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			if ( !ZR_IsGameStart () )
				return;
			
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			static char explode[2][4];
			ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
			int clientClassInt = StringToInt ( explode[0] );
			int skill_Id = StringToInt ( explode[1] );
			useClassActiveSkill ( param1, view_as <ClientClass> ( clientClassInt ), skill_Id );	//	'use_skill_function.inc' include function.
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientCommandASKMenu ( param1 );
					return;
				}
			}
		}
	}
}

/**
 * 메인 메뉴
 */
public Action clientCommandMenu ( int client, int args ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Continue;
	
	Menu menu = new Menu ( commandMenuHandler );
	menu.SetTitle ( "%s\n　", MENU_TITLE );
	if ( !isClientCharacterSelected ( client ) ) {
		menu.AddItem ( "", "캐릭터를 선택하세요.", ITEMDRAW_DISABLED );
		menu.Display ( client, MENU_TIME_FOREVER );
		return Plugin_Handled;
	}
	menu.AddItem ( "0", "정보" );
	
	menu.AddItem ( "1", "장비" );
	
	menu.AddItem ( "2", "인벤토리" );
	
	menu.AddItem ( "3", "육성\n￣￣￣￣￣￣￣￣￣￣￣￣" );
	
	menu.AddItem ( "4", "상점" );
	menu.AddItem ( "5", "제작\n￣￣￣￣￣￣￣￣￣￣￣￣" );
	
	if ( g_iClientClass[client] == ClientClass_Normal )
		menu.AddItem ( "6", "병과" );

	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int commandMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			
			static int num;
			num = StringToInt ( item );
			
			switch ( num ) {
				case 0: clientCommandInfo ( param1, 0 );
				case 1: clientEquipments ( param1 );
				case 2: clientInventory ( param1 );
				case 3: clientClassStatMenu ( param1 );
				case 4: clientShopMenu ( param1 );
				case 5:	craftItemMenu ( param1 );
				case 6: clientChooseClassMenu ( param1 );
			}
		}
	}
}

/**
 * 육성 메뉴 
 */
public void clientClassStatMenu ( int client ) {
	Menu menu = new Menu ( clientClassStatMenuHandle );
	menu.SetTitle ( "%s\n\t육성\n　", MENU_TITLE );
	menu.AddItem ( "", "스텟" );
	menu.AddItem ( "", "기술" );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientClassStatMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0:		clientCommandStat ( param1, 0 );
				case 1:		clientClassMenu ( param1 );
			}
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientCommandMenu ( param1, 0 );
		}
	}
}

/**
 * 인벤토리
 */
public void clientInventory ( int client ) {
	static char moneyname[32];
	SourceStore_GetMoneyName ( moneyname, sizeof ( moneyname ) );
	Menu menu = new Menu ( clientInventoryMenuHandler );
	menu.SetTitle ( "%s\n\t인벤토리\n\t\t지갑: %d %s%s\n　", MENU_TITLE, SourceStore_GetClientBalance ( client ), moneyname, SourceStore_GetClientBalance ( client ) > 0 ? "(s)" : "" );
//	menu.AddItem ( "#GO_TO_EQUIPMENT_MENU", "장비\n　" );
	bool empty = true;
	static char buffer[256];
	static char itemId[4];
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( g_iItem[i][client] > 0 ) {
			empty = false;
			Format ( buffer, sizeof ( buffer ), "%s (%d개)", g_strItemName[i], g_iItem[i][client] );
			IntToString ( i, itemId, sizeof ( itemId ) );
			menu.AddItem ( itemId, buffer );
		}
	}
	if ( empty )
		menu.AddItem ( "", "비어있음", ITEMDRAW_DISABLED );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientInventoryMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			static char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			if ( StrEqual ( item, "#GO_TO_EQUIPMENT_MENU" ) ) {
				clientEquipments ( param1 );
				return;
			}
			
			int itemId = StringToInt ( item );
			useItem ( param1, itemId );
//			PrintToChat ( param1, "ITEM ID: %d", itemId );
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

public void clientEquipments ( int client ) {
	Menu menu = new Menu ( clientEquipmentMenuHandler );
	menu.SetTitle ( "%s\n\t장비\n　", MENU_TITLE );
	static char buffer[256];
	static char data[32];
/*	static char typename[32];
	for ( int i = ITEM_TYPE_EQ_WEAPON; i <= ITEM_TYPE_EQ_SOULWEAPON; i ++ ) {
		getEquipmentTypeName ( i, typename, sizeof ( typename ) );
		Format ( data, sizeof ( data ), "%s@%d", StrEqual ( g_strEquipItem[i][client], "" ) ? "null" : g_strEquipItem[i][client], i );
		Format ( buffer, sizeof ( buffer ), "%s: %s", typename, StrEqual ( g_strEquipItem[i][client], "" ) ? INTERFACE_NO_EQUIPMENT_ITEM : g_strEquipItem[i][client] );
		menu.AddItem ( data, buffer, StrEqual ( g_strEquipItem[i][client], "" ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	}	*/

	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
		if ( g_iEquipItemSlotUnlock[i] > g_iLevel[client] ) {
			Format ( buffer, sizeof ( buffer ), "유물: [잠김: 레벨 %d 필요]", g_iEquipItemSlotUnlock[i] );
			menu.AddItem ( "", buffer, ITEMDRAW_DISABLED );
		}
		else {
			if ( g_iEquipItemIndex[i][client] <= 0 ||
				g_iItemType[g_iEquipItemIndex[i][client]] != ITEM_TYPE_EQUIPMENT ) {
				Format ( buffer, sizeof ( buffer ), "유물: [%s]", INTERFACE_NO_EQUIPMENT_ITEM );
				Format ( data, sizeof ( data ), "#IS_EMPTY_SLOT" );
			}
			else {
				Format ( buffer, sizeof ( buffer ), "유물: %s", g_strItemName[g_iEquipItemIndex[i][client]] );
				Format ( data, sizeof ( data ), "%d@%d", i, g_iEquipItemIndex[i][client] );
			}
			
			menu.AddItem ( data, buffer, StrEqual ( data, "#IS_EMPTY_SLOT" ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		}
	}
	
/*
	#define		EQUIPMENT_BONUS_STAT_STR		0
	#define		EQUIPMENT_BONUS_STAT_AGL		1
	#define		EQUIPMENT_BONUS_STAT_DEF		2
	#define		EQUIPMENT_BONUS_STAT_RELOADSPD	3
	#define		EQUIPMENT_BONUS_STAT_FIRERATE	4
	#define		EQUIPMENT_BONUS_STAT_DAMAGE		5
*/
	
	static char optionInfo[256];
	static char optionTypename[MAX_EQUIPMENT_BONUS_STAT][32] = {
		"공격",	"기력",	"민첩",	"재장전 속도",	"연사 속도",	"무기 피해"
	};
	
	float tempTotalOption = 0.0;
	Format ( optionInfo, sizeof ( optionInfo ), "유물 전체 옵션:" );
	for ( int i = 0; i < MAX_EQUIPMENT_BONUS_STAT; i ++ ) {
		tempTotalOption = 0.0;
		for ( int x = 0; x < EQUIPITEM_MAX_SLOTS; x ++ ) {
			tempTotalOption += g_fEquipBonusStat[x][i][client];
		}
		
		
		if ( tempTotalOption > 0.0001 ) {
			if ( i == EQUIPMENT_BONUS_STAT_STR ||
				i == EQUIPMENT_BONUS_STAT_DEF ||
				i == EQUIPMENT_BONUS_STAT_AGL )
				Format ( optionInfo, sizeof ( optionInfo ), "%s\n\t\t%s: +%d", optionInfo, optionTypename[i], RoundFloat ( tempTotalOption ) );
			else
				Format ( optionInfo, sizeof ( optionInfo ), "%s\n\t\t%s: +%.3f", optionInfo, optionTypename[i], tempTotalOption );
		}
		else
			Format ( optionInfo, sizeof ( optionInfo ), "%s\n\t\t%s: ---", optionInfo, optionTypename[i] );
	}
	
	menu.AddItem ( "", "space", ITEMDRAW_SPACER );
	menu.AddItem ( "", optionInfo, ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientEquipmentMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			static char item[32];
			static char explode[2][32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			
			if ( item[0] == EOS || StrEqual ( item, "#IS_EMPTY_SLOT" ) )
				return;

			
			//	슬롯과 아이템 인덱스 값을 Explode String하여 가져온다.
			ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
			//	각각 변수에 삽입
			static int itemSlot;
			static int itemIndex;
			itemSlot = StringToInt ( explode[0] );
			itemIndex = StringToInt ( explode[1] );
			
			if ( itemIndex <= 0 ||
				g_iItemType[itemIndex] != ITEM_TYPE_EQUIPMENT ) {
				CLIENT_ITEM_UNEQUIP_FAILED1_MSG ( param1, "" )
				g_iEquipItemIndex[itemSlot][param1] = -1;
				clientEquipments ( param1 );
				return;
			}
			
			CLIENT_ITEM_UNEQUIP_SUCCESS_MSG ( param1, g_strItemName[itemIndex] )
			
			itemActionValueToggle ( param1, itemIndex, g_iItemType[itemIndex], false, itemSlot );	//	Unequipped
			g_iItem[itemIndex][param1] ++;
			g_iEquipItemIndex[itemSlot][param1] = -1;
			clientEquipments ( param1 );
			return;
			
			/*
			ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
			int itemType = StringToInt ( explode[1] );
			if ( StrEqual ( explode[0], "null" ) )
				return;
				
			for ( int i = 0; i < g_iDefineItems; i ++ ) {
				if ( StrEqual ( explode[0], g_strItemName[i] ) ) {
					itemActionValueToggle ( param1, i, itemType, false );
					g_iItem[i][param1] ++;
					g_strEquipItem[itemType][param1] = "";
					CLIENT_ITEM_UNEQUIP_SUCCESS_MSG ( param1, explode[0] )
					clientEquipments ( param1 );
					return;
				}
				
				else {
					//	없는 아이템일 경우
					if ( i == ( g_iDefineItems - 1 ) ) {
						CLIENT_ITEM_UNEQUIP_FAILED1_MSG ( param1, explode[0] )
						g_strEquipItem[itemType][param1] = "";
						clientEquipments ( param1 );
						return;
					}
				}
			}
			*/
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
 * 상점 메뉴
 */
public void clientShopMenu ( int client ) {
	Menu menu = new Menu ( shopMenuHandler );
	menu.SetTitle ( "%s\n\t상점\n　", MENU_TITLE );
	bool empty = true;
	static char item[4];
	static char buffer[256];
	/*
	menu.AddItem ( "#ZDOLLAR_GACHA_ONE_TIME", "일반 뽑기 1회" );
	menu.AddItem ( "#ZDOLLAR_GACHA_TEN_TIMES", "일반 뽑기 10회" );
	menu.AddItem ( "#JEWEL_GACHA_ONE_TIME", "플레티넘 뽑기 1회", ITEMDRAW_DISABLED );
	menu.AddItem ( "#JEWEL_GACHA_TEN_TIMES", "플레티넘 뽑기 10회\n　\n\t--- 판매 아이템 목록 ---", ITEMDRAW_DISABLED );
	*/

	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( g_bItemSell[i] ) {
			empty = false;
			IntToString ( i, item, sizeof ( item ) );
			Format ( buffer, sizeof ( buffer ), "%s [%d]", g_strItemName[i], g_iItemPrice[i] );
			menu.AddItem ( item, buffer );
		}
	}
	
	if ( empty )
		menu.AddItem ( "", "판매중인 아이템이 없습니다.", ITEMDRAW_DISABLED );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int shopMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			static char item[64];
			menu.GetItem ( param2, item, sizeof ( item ) );
			/*
			if ( StrEqual ( item, "#ZDOLLAR_GACHA_ONE_TIME" ) ) {
				menuNormalGasyaMenu ( param1, 1 );
				return;
			}
			
			if ( StrEqual ( item, "#ZDOLLAR_GACHA_TEN_TIMES" ) ) {
				menuNormalGasyaMenu ( param1, 10 );
				return;
			}*/
			
			buyItem ( param1, StringToInt ( item ) );
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
 * 아이템 구입 메뉴
 */
public void buyItem ( int client, int itemId ) {
	bool accept = ( SourceStore_GetClientBalance ( client ) >= g_iItemPrice[itemId] ? true : false );
	static char moneyname[32];
	static char item[4];
	char description[256];
	strcopy ( description, sizeof ( description ), g_strItemDescription[itemId] );
	ReplaceString ( description, sizeof ( description ), "{newline}", "\n　　" );
	SourceStore_GetMoneyName ( moneyname, sizeof ( moneyname ) );
	IntToString ( itemId, item, sizeof ( item ) );
	Menu menu = new Menu ( buyItemMenuHandler );
	menu.SetTitle ( "%s\n\t상점 (아이템 구입)\n　\n\t\t%s (Index: %d)\n\t\t%s\n\t\t아이템 가격: %d %s\n　", MENU_TITLE, g_strItemName[itemId], itemId, description, g_iItemPrice[itemId], moneyname );
	menu.AddItem ( item, accept ? "구입" : "구입 (자금 부족)", accept ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "#CANCEL", "취소" );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			static char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			if ( StrEqual ( item, "#CANCEL" ) ) {
				clientShopMenu ( param1 );
				return;
			}
			
			int itemId = StringToInt ( item );
			if ( SourceStore_GetClientBalance ( param1 ) < g_iItemPrice[itemId] ) {
				CLIENT_ITEM_BUY_FAILED_MSG ( param1 )
				return;
			}
			
			SourceStore_SubClientBalance ( param1, g_iItemPrice[itemId] );
			g_iItem[itemId][param1] ++;
			CLIENT_ITEM_BUY_SUCCESS_MSG ( param1, g_strItemName[itemId] )
			EmitSoundToClient ( param1, SOUND_BUY_ITEM );
			buyItem ( param1, itemId );
			
			Call_StartForward ( g_hForwardHandlers[ZD_FORWARD_ON_BUY_ITEM] );
			Call_PushCell ( param1 );
			Call_PushString ( g_strItemName[itemId] );
			Call_Finish ();
		}
	}
}

/**
 * 병과 선택 메뉴
 */
public void clientChooseClassMenu ( int client ) {
	Menu menu = new Menu ( chooseClassMainMenuHandler );
	menu.SetTitle ( "%s\n\t병과 선택:\n　", MENU_TITLE );
	for ( int i = 0; i < g_iDefineClass; i ++ ) {
		if ( i == 0 ) {
			menu.AddItem ( "", "", ITEMDRAW_IGNORE );
			continue;
		}
		
		menu.AddItem ( "", g_strClientClassName[i], g_iClientClassOnlyAdmin[i] ? ( loringlib_IsClientAdmin ( client ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED ) : ITEMDRAW_DEFAULT );
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int chooseClassMainMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			viewClassInfo ( param1, view_as <ClientClass> ( param2 ) );
			return;
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
 * 병과 정보 메뉴
 */
public void viewClassInfo ( int client, ClientClass thisChoose ) {
	if ( g_iClientClass[client] != ClientClass_Normal )
		return;
	
	int choose = view_as <int> ( thisChoose );
	static char classid[4];
	IntToString ( choose, classid, sizeof ( classid ) );
	static char name[64];
	getClientClassName ( thisChoose, name, sizeof ( name ) );
	Menu menu = new Menu ( classInfoMenuHandler );
	menu.SetTitle ( "%s\n\t병과 선택:\n\t\t%s\n\t\t필요 레벨: %d PVL\n　", MENU_TITLE, name, g_iClientClassNeedlvl[choose] );
	menu.AddItem ( classid, "병과 설명" );
	menu.AddItem ( classid, "선택", g_iClientClassNeedlvl[choose] > g_iLevel[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int classInfoMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int classid = StringToInt ( item );
			
			if ( param2 == 0 ) {
				viewClassDescription ( param1, classid );
				return;
			}
			
			else if ( param2 == 1 ) {
				CLIENT_CHOOSE_CLASS_BROADCAST_MSG ( param1, g_strClientClassName[classid] )
				g_iClientClass[param1] = view_as <ClientClass> ( classid );
				return;
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
 * 병과 설명 메뉴
 */
public void viewClassDescription ( int client, int classid ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	char name[32];
	getClientClassName ( view_as<ClientClass> ( classid ), name, sizeof ( name ) );
	Menu menu = new Menu ( classDescriptionMenuHandler );
	menu.SetTitle ( "%s\n\t병과 선택:\n\t%s\n　", MENU_TITLE, name );
	
	char description[512];
	strcopy ( description, sizeof ( description ), g_strClientClassDescription[classid] );
	ReplaceString ( description, sizeof ( description ), "{newline}", "\n\t\t" );
	Format ( description, sizeof ( description ), "병과 설명:\n\t\t%s", description );
	menu.AddItem ( "", description, ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
	return;
}

public int classDescriptionMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientChooseClassMenu ( param1 );
		}
	}
} 

/**
 * 병과 메인 메뉴
 */
public void clientClassMenu ( int client ) {
	static char name[32];
	getClientClassName ( g_iClientClass[client], name, sizeof ( name ) );
	Menu menu = new Menu ( classMainMenuHandler );
	menu.SetTitle ( "%s\n\t병과\n\t\t병과: %s\n\t\t플레이어pt: %d\n　", MENU_TITLE, name, g_iPlayerPoint[client] );
	menu.AddItem ( "", "기본 기술" );
	if ( g_iClientClass[client] != ClientClass_Normal ) {
		static char buff[64];
		Format ( buff, sizeof ( buff ), "%s 기술", name );
		menu.AddItem ( "", buff );
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int classMainMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: viewClientClassSkills ( param1, true );
				case 1: viewClientClassSkills ( param1, false );
			}
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientClassStatMenu ( param1 );
					return;
				}
			}
		}
	}
}

/**
 * 기술 목록 2019ver.
 */
public void viewClientClassSkills ( int client, bool normalClass ) {
	ClientClass classIndex;
	if ( normalClass )	classIndex = ClientClass_Normal;
	else				classIndex = g_iClientClass[client];
	
	g_bSkillMenuSelectPosition[client] = normalClass;
	
	int clientClass = view_as<int> ( classIndex );
	
	Menu menu = new Menu ( clientClassSkillMenuHandler );
	menu.SetTitle ( "%s\n\t어떤 기술을 익힐까?\n\t\t플레이어pt: %d\n　", MENU_TITLE, g_iPlayerPoint[client] );
	
	char target[16];
	char format[256];
	
	int block_skill = -1, activity_skill = -1, activity_skill_maxlvl = -1;
	
	//	클라이언트 병과에 맞는 기술들을 나열한다.
	for ( int i = 0; i < g_iDefineClassSkills[clientClass]; i ++ ) {
		//	블록 기술과 활성화 기술을 구해온다.
		block_skill = g_iSkillBlockSkill_Id[clientClass][i];
		activity_skill = g_iSkillActivitySkill_Id[clientClass][i];
		activity_skill_maxlvl = g_iSkillActivitySkillMaxlvl_Id[clientClass][i];
		
		//	메뉴 아이템 옵션 포멧
		Format ( target, sizeof ( target ), "%d@%d", clientClass, i );
		
		//	블록 기술을 이미 배웠을 경우
		if ( block_skill != -1 && g_iSkill[clientClass][block_skill][client] > 0 ) {
			Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술로 인해 잠김]", g_strSkillName[clientClass][i], g_iSkillMaxlvl[clientClass][i], g_strSkillName[clientClass][block_skill] );
			menu.AddItem ( target, format );
		}
		
		//	블록 기술과 블록 상대 기술 모두 안배웠을 경우
		else if ( block_skill != -1 &&
			g_iSkill[clientClass][block_skill][client] <= 0 && g_iSkill[clientClass][i][client] <= 0 ) {
			Format ( format, sizeof ( format ), "%s [플레이어pt: %d]\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 익힐 시 잠김]", g_strSkillName[clientClass][i], getNeedPlayerPoint ( client, classIndex, i ), g_iSkillMaxlvl[clientClass][i], g_strSkillName[clientClass][block_skill] );
			menu.AddItem ( target, format );
		}
		
		//	엑티비티 기술을 배우지 않았을 경우
		else if ( activity_skill != -1 && g_iSkill[clientClass][activity_skill][client] < 1 ) {
			Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 익혀야 함]", g_strSkillName[clientClass][i], g_iSkillMaxlvl[clientClass][i], g_strSkillName[clientClass][activity_skill] );
			menu.AddItem ( target, format );
		}
		
		//	엑티비티 기술이 만렙이 아닐 경우
		else if ( activity_skill_maxlvl != -1 && !isSkillMaxLvl ( client, classIndex, activity_skill_maxlvl ) ) {
			Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 전부 익혀야 함]\n　", g_strSkillName[clientClass][i], g_iSkillMaxlvl[clientClass][i], g_strSkillName[clientClass][activity_skill_maxlvl] );
			menu.AddItem ( target, format );
		}
		
		//	아무 조건도 필요 없을 경우
		else {
			//	기술이 최대레벨이 아닐 경우
			if ( !isSkillMaxLvl ( client, classIndex, i ) )
				Format ( format, sizeof ( format ), "%s [플레이어pt: %d]\n\t\tLEVEL: %d / %d", g_strSkillName[clientClass][i], getNeedPlayerPoint ( client, classIndex, i ), g_iSkill[clientClass][i][client], g_iSkillMaxlvl[clientClass][i] );
			//	최대레벨?
			else
				Format ( format, sizeof ( format ), "%s\n\t\tLEVEL: MAX", g_strSkillName[clientClass][i] );
			
			menu.AddItem ( target, format );
		}
		menu.AddItem ( "", "#SPACE_ITEM", ITEMDRAW_SPACER );
	//	menu.AddItem ( "", "#SPACE_ITEM", ITEMDRAW_SPACER );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientClassSkillMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char contents[16];
			menu.GetItem ( param2, contents, sizeof ( contents ) );
			char status[2][8];
			ExplodeString ( contents, "@", status, sizeof ( status ), sizeof ( status[] ) );
			
			viewSkillDetails ( param1, StringToInt ( status[0] ), StringToInt ( status[1] ) );
		}
		
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				clientClassMenu ( param1 );
		}
	}
}

/**
 * 스킬 자세히 보기
 */
void viewSkillDetails ( int client, int clientClass, int skill ) {
	ClientClass classIndex = view_as<ClientClass> ( clientClass );

	Menu menu = new Menu ( skillDetailsMenuHandler );
	menu.SetTitle ( "%s\n\t%s 기술\n\t\t플레이어pt: %d\n　", MENU_TITLE, g_strSkillName[clientClass][skill], g_iPlayerPoint[client] );
	
	char target[16];
	char format[256];
	
	int block_skill = -1, activity_skill = -1, activity_skill_maxlvl = -1;
	
	//	블록 기술과 활성화 기술을 구해온다.
	block_skill = g_iSkillBlockSkill_Id[clientClass][skill];
	activity_skill = g_iSkillActivitySkill_Id[clientClass][skill];
	activity_skill_maxlvl = g_iSkillActivitySkillMaxlvl_Id[clientClass][skill];
	
	//	메뉴 아이템 옵션 포멧
	Format ( target, sizeof ( target ), "%d@%d", clientClass, skill );
	
	//	블록 기술을 이미 배웠을 경우
	if ( block_skill != -1 && g_iSkill[clientClass][block_skill][client] > 0 ) {
		Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술로 인해 잠김]\n　", g_strSkillName[clientClass][skill], g_iSkillMaxlvl[clientClass][skill], g_strSkillName[clientClass][block_skill] );
		menu.AddItem ( target, format, ITEMDRAW_DISABLED );
	}
	
	//	블록 기술과 블록 상대 기술 모두 안배웠을 경우
	else if ( block_skill != -1 &&
		g_iSkill[clientClass][block_skill][client] <= 0 && g_iSkill[clientClass][skill][client] <= 0 ) {
		Format ( format, sizeof ( format ), "%s [플레이어pt: %d]\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 익힐 시 잠김]\n　", g_strSkillName[clientClass][skill], getNeedPlayerPoint ( client, classIndex, skill ), g_iSkillMaxlvl[clientClass][skill], g_strSkillName[clientClass][block_skill] );
		menu.AddItem ( target, format, getNeedPlayerPoint ( client, classIndex, skill ) <= g_iPlayerPoint[client] ? ( isSkillMaxLvl ( client, classIndex, skill ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT ) : ITEMDRAW_DISABLED );
	}
	
	//	엑티비티 기술을 배우지 않았을 경우
	else if ( activity_skill != -1 && g_iSkill[clientClass][activity_skill][client] < 1 ) {
		Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 익혀야 함]\n　", g_strSkillName[clientClass][skill], g_iSkillMaxlvl[clientClass][skill], g_strSkillName[clientClass][activity_skill] );
		menu.AddItem ( target, format, ITEMDRAW_DISABLED );
	}
	
	//	엑티비티 기술이 만렙이 아닐 경우
	else if ( activity_skill_maxlvl != -1 && !isSkillMaxLvl ( client, classIndex, activity_skill_maxlvl ) ) {
		Format ( format, sizeof ( format ), "%s\n\t\tMAX LEVEL: %d\n\t\t[%s 기술을 전부 익혀야 함]\n　", g_strSkillName[clientClass][skill], g_iSkillMaxlvl[clientClass][skill], g_strSkillName[clientClass][activity_skill_maxlvl] );
		menu.AddItem ( target, format, ITEMDRAW_DISABLED );
	}
	
	//	아무 조건도 필요 없을 경우
	else {
		//	기술이 최대레벨이 아닐 경우
		if ( !isSkillMaxLvl ( client, classIndex, skill ) )
			Format ( format, sizeof ( format ), "%s [플레이어pt: %d]\n\t\tLEVEL: %d / %d\n　", g_strSkillName[clientClass][skill], getNeedPlayerPoint ( client, classIndex, skill ), g_iSkill[clientClass][skill][client], g_iSkillMaxlvl[clientClass][skill] );
		//	최대레벨?
		else
			Format ( format, sizeof ( format ), "%s\n\t\tLEVEL: MAX\n　", g_strSkillName[clientClass][skill] );
		
		menu.AddItem ( target, format, getNeedPlayerPoint ( client, classIndex, skill ) <= g_iPlayerPoint[client] ? ( isSkillMaxLvl ( client, classIndex, skill ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT ) : ITEMDRAW_DISABLED );
	}
	
	char description[512];
	Format ( description, sizeof ( description ), g_strSkillDescription[clientClass][skill] );
	ReplaceString ( description, sizeof ( description ), "{newline}", "\n\t\t" );
	
	Format ( description, sizeof ( description ), "기술 설명:\n\t\t%s\n　", description );
	menu.AddItem ( "#SPACE_ITEM", description, ITEMDRAW_DISABLED );
	
	char etc[128];
	if ( g_iSkillType[clientClass][skill] == SKILL_TYPE_ACTIVE ) {
		Format ( etc, sizeof ( etc ), "소모 AP: %d\n　", g_iSkillConsumeAP[clientClass][skill] );
		menu.AddItem ( "#SPACE_ITEM", etc, ITEMDRAW_DISABLED );
		
		Format ( etc, sizeof ( etc ), "사용 대기시간: %d초\n　", g_iSkillCooldown[clientClass][skill] );
		menu.AddItem ( "#SPACE_ITEM", etc, ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int skillDetailsMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			char contents[16];
			menu.GetItem ( param2, contents, sizeof ( contents ) );
			if ( StrEqual ( contents, "#SPACE_ITEM" ) )
				return;
			
			char status[2][8];
			ExplodeString ( contents, "@", status, sizeof ( status ), sizeof ( status[] ) );
			int clientClass = StringToInt ( status[0] );
			int skill_Id = StringToInt ( status[1] );
			if ( isSkillMaxLvl ( param1, view_as <ClientClass> ( clientClass ), skill_Id ) )
				return;
			
			if ( getNeedPlayerPoint ( param1, view_as <ClientClass> ( clientClass ), skill_Id ) > g_iPlayerPoint[param1] )
				return;
			
			g_iPlayerPoint[param1] -= getNeedPlayerPoint ( param1, view_as <ClientClass> ( clientClass ), skill_Id );
			g_iSkill[clientClass][skill_Id][param1] ++;
			
			viewSkillDetails ( param1, clientClass, skill_Id );
			
			return;
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					if ( g_iClientClass[param1] == ClientClass_Normal )
						viewClientClassSkills ( param1, true );
					else
						viewClientClassSkills ( param1, g_bSkillMenuSelectPosition[param1] );
					return;
				}
			}
		}
	}
}

/**
 * 기본 기술 목록 메뉴
 */
public void viewClientClassSkillsOld ( int client, bool isNormal ) {
	ClientClass clientClassId;
	int clientClassInt;
	if ( isNormal )		clientClassId = ClientClass_Normal;
	else				clientClassId = g_iClientClass[client];
	static char blockSkill[64];
	static char activitySkill[64];
	static char skillDescription[SKILL_DESCRIPTION_MAX_LENGTH];
	clientClassInt = view_as <int> ( clientClassId );
	static char target[32];
	int blockskill_Id = -1;
	int activityskill_Id = -1;
//	IntToString ( clientClassInt, target, sizeof ( target ) );
	Menu menu = new Menu ( clientClassSkillMenuHandlerOld );
	menu.SetTitle ( "%s\n\t기술\n\t\t플레이어pt: %d\n　", MENU_TITLE, g_iPlayerPoint[client] );
	static char format[512];
	for ( int i = 0; i < g_iDefineClassSkills[clientClassInt]; i ++ ) {
		blockskill_Id = g_iSkillBlockSkill_Id[clientClassInt][i];
		activityskill_Id = g_iSkillActivitySkill_Id[clientClassInt][i];
		
		Format ( skillDescription, SKILL_DESCRIPTION_MAX_LENGTH, g_strSkillDescription[clientClassInt][i] );
		ReplaceString ( skillDescription, SKILL_DESCRIPTION_MAX_LENGTH, "{newline}", "\n\t\t", true );
		
//		PrintToServer ( "MAXLEVEL: %d, NEED_PRICE: %d", g_iSkillMaxlvl[clientClassInt][i], g_iSkillNeedPrice[clientClassInt][i] );
		Format ( target, sizeof ( target ), "%d@%d", clientClassInt, i );
		//	블록 기술을 이미 배웠을 경우
		if ( blockskill_Id != -1 && g_iSkill[clientClassInt][blockskill_Id][client] > 0 ) {
			getSkillName ( clientClassId, blockskill_Id, blockSkill, sizeof ( blockSkill ) );
			
			Format ( format, sizeof ( format ), "%s\n\t\t%s\n\t\t[%s 기술로 인해 잠김]", g_strSkillName[clientClassInt][i], skillDescription, blockSkill );
			menu.AddItem ( target, format, ITEMDRAW_DISABLED );
		}
		
		//	블록 기술과 상대 기술 모두 안배웠을 경우
		else if ( blockskill_Id != -1 && ( g_iSkill[clientClassInt][blockskill_Id][client] <= 0 && g_iSkill[clientClassInt][i][client] <= 0 ) ) {
			getSkillName ( clientClassId, blockskill_Id, blockSkill, sizeof ( blockSkill ) );
			
			Format ( format, sizeof ( format ), "%s\n\t\t%s\n\t\t[%s 기술을 배울 시 잠김]", g_strSkillName[clientClassInt][i], skillDescription, blockSkill );
			menu.AddItem ( target, format );
		}
		
		//	엑티비티 기술이 있는 기술일 경우
		else if ( activityskill_Id != -1 && g_iSkill[clientClassInt][activityskill_Id][client] < 1 ) {
			getSkillName ( clientClassId, activityskill_Id, activitySkill, sizeof ( activitySkill ) );
			Format ( format, sizeof ( format ), "%s\n\t\t%s\n\t\t[%s 기술 필요]", g_strSkillName[clientClassInt][i], skillDescription, activitySkill );
			menu.AddItem ( target, format, ITEMDRAW_DISABLED );
		}
		
		else {
			if ( !isSkillMaxLvl ( client, clientClassId, i ) )
				Format ( format, sizeof ( format ), "%s (필요 플레이어pt: %d)\n\n\t\t%s\n\t\t%d LEVEL", g_strSkillName[clientClassInt][i], getNeedPlayerPoint ( client, clientClassId, i ), skillDescription, g_iSkill[clientClassInt][i][client] );
			else
				Format ( format, sizeof ( format ), "%s (LEVEL MAX)\n\n\t\t%s\n\t\t%d LEVEL", g_strSkillName[clientClassInt][i], skillDescription, g_iSkill[clientClassInt][i][client] );
				
			menu.AddItem ( target, format, getNeedPlayerPoint ( client, clientClassId, i ) <= g_iPlayerPoint[client] ? ( isSkillMaxLvl ( client, clientClassId, i ) ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT ) : ITEMDRAW_DISABLED );
		}
		menu.AddItem ( "", "#SPACE_ITEM", ITEMDRAW_SPACER );
		menu.AddItem ( "", "#SPACE_ITEM", ITEMDRAW_SPACER );
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int clientClassSkillMenuHandlerOld ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[32];
			static char explode[2][4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
			int clientClassId = StringToInt ( explode[0] );
			int skill_Id = StringToInt ( explode[1] );
			if ( isSkillMaxLvl ( param1, view_as <ClientClass> ( clientClassId ), skill_Id ) )
				return;
			
			if ( getNeedPlayerPoint ( param1, view_as <ClientClass> ( clientClassId ), skill_Id ) > g_iPlayerPoint[param1] )
				return;
			
//			viewClientChooseSkill ( param1, clientClassId, skill_Id );
			g_iPlayerPoint[param1] -= getNeedPlayerPoint ( param1, view_as <ClientClass> ( clientClassId ), skill_Id );
			g_iSkill[clientClassId][skill_Id][param1] ++;
//			CLIENT_GET_SKILL_MSG ( param1, g_strSkillName[clientClassId][skill_Id] )
			if ( view_as <ClientClass> ( clientClassId ) == ClientClass_Normal )
				viewClientClassSkills ( param1, true );
			else
				viewClientClassSkills ( param1, false );
			return;
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
 * 아이템 스팩
 */
public void itemSpec ( int client, int itemId, bool isAdmin ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	static char money[32];
	SourceStore_GetMoneyName ( money, sizeof ( money ) );
	Menu menu = new Menu ( itemSpecMenuHandler );
	if ( g_bItemSell[itemId] )
		menu.SetTitle ( "%s\n\t아이템 상세 정보\n　\n\t\t%s (Index: %d)\n\t\t%s\n\t\t가격: %d %s\n　", MENU_TITLE, g_strItemName[itemId], itemId, g_strItemDescription[itemId], g_iItemPrice[itemId], money );
	else
		menu.SetTitle ( "%s\n\t아이템 상세 정보\n　\n\t\t%s (Index: %d)\n\t\t%s\n\t\t[판매하지 않음]\n　", MENU_TITLE, g_strItemName[itemId], itemId, g_strItemDescription[itemId] );
	menu.AddItem ( "", "뒤로", isAdmin ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int itemSpecMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			if ( loringlib_IsClientAdmin ( param1 ) )
				clientCommandItemList ( param1, 0 );
		}
	}
}