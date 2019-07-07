/**
 * RoundStart to reset potion use counts.
 */
void ITEMINIT_RoundStartOnPotionUseCountClear () {
	for ( int i = 0; i < sizeof ( g_iPotionUseCount[] ); i ++ ) {
		g_iPotionUseCount[ITEM_USE_TYPE_AP][i] = 0;
		g_iPotionUseCount[ITEM_USE_TYPE_HP][i] = 0;
	}
}

/**
 * 아이템 사용
 * @param client
 * @param itemId		Item index.
 * @noreturn
 */
public void useItem ( int client, int itemId ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
		
	if ( g_iItem[itemId][client] < 1 )	//	아이템이 없을 경우,
		return;
	
	static char description[256];
	static char equipmentOptionInfo[256];
	static char equipmentTypeInfo[128];
	static char equipmentTypeTargetname[64];
	
	strcopy ( description, sizeof ( description ), g_strItemDescription[itemId] );
	ReplaceString ( description, sizeof ( description ), "{newline}", "\n　　" );
	
	if ( g_iItemType[itemId] == ITEM_TYPE_EQUIPMENT ) {
		static int equipmentTargetClientClass;
		equipmentTargetClientClass = getEquipmentTypeTargetToClientClassIndex ( g_strItemEquipmentTypeTarget[itemId] );
		static char optionInfoArr[2][32];	//	옵션 값 찾기 ex) 100%20.0 에서 100과 20.0%를 찾는다.
		static char optionCalcType[16];		//	계산 방식 찾기 ex) 100%20.0 에서 두 숫자 사이의 %를 찾는다.
		static int optionType;			//	옵션 타입 ex) 100%20.0 에서 100을 옵션 타입값으로 바꿀때 사용 한다.
		static char optionTypename[MAX_EQUIPMENT_BONUS_STAT][32] = {
			"공격",	"기력",	"민첩",	"재장전 속도",	"연사 속도",	"무기 피해"
		};
		Format ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "\n\t옵션:" );
		for ( int i = 1; i <= MAX_EQUIPMENT_OPTIONS; i ++ ) {
		//	PrintToChatAll ( "%d Item Option: %s", itemId, g_strItemEquipmentOptions[itemId][i] );
			
			if ( !StrEqual ( g_strItemEquipmentOptions[itemId][i], "-1" ) ) {
				if ( StrContains ( g_strItemEquipmentOptions[itemId][i], "+" ) != -1 )
					strcopy ( optionCalcType, sizeof ( optionCalcType ), "+" );
				else if ( StrContains ( g_strItemEquipmentOptions[itemId][i], "%" ) != -1 )
					strcopy ( optionCalcType, sizeof ( optionCalcType ), "%" );
				else {
					Format ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "%s\n\t\tERROR", equipmentOptionInfo );
					continue;
				}
				
				ExplodeString ( g_strItemEquipmentOptions[itemId][i], optionCalcType, optionInfoArr, sizeof ( optionInfoArr ), sizeof ( optionInfoArr[] ) );
				optionType = replaceOptionIdxToStatIdx ( StringToInt ( optionInfoArr[0] ) );
				
				//	Skill bonus options
				if ( g_iItemEquipmentType[itemId] == ITEM_EQUIPMENT_TYPE_CLASS ) {
					if ( StringToInt ( optionInfoArr[0] ) >= 0 && StringToInt ( optionInfoArr[0] ) < 100 )
						Format ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "%s\n\t\t%s: +%d", equipmentOptionInfo, g_strSkillName[equipmentTargetClientClass][optionType], StringToInt ( optionInfoArr[1] ) );
				}
				else {
					if ( optionType == EQUIPMENT_BONUS_STAT_STR ||
						optionType == EQUIPMENT_BONUS_STAT_DEF ||
						optionType == EQUIPMENT_BONUS_STAT_AGL )	//	정수 표시
						Format ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "%s\n\t\t%s: +%.0f%s", equipmentOptionInfo, optionTypename[optionType], StringToFloat ( optionInfoArr[1] ), StrEqual ( optionCalcType, "%" ) ? "%" : "" );
					else	//	소숫점 표시
						Format ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "%s\n\t\t%s: +%.2f%s", equipmentOptionInfo, optionTypename[optionType], StringToFloat ( optionInfoArr[1] ), StrEqual ( optionCalcType, "%" ) ? "%" : "" );
				}
			}
		}
		
		if ( equipmentTargetClientClass < 0 ) {	//	병과 유물이 아닐 경우?
			//	무기 유물일 경우?
			if ( StrContains ( g_strItemEquipmentTypeTarget[itemId], "weapon_", false ) != -1 )
				Format ( equipmentTypeInfo, sizeof ( equipmentTypeInfo ), "\n\t타입: 무기" );
			else
				Format ( equipmentTypeInfo, sizeof ( equipmentTypeInfo ), "\n\t타입: 없음" );
		}
		
		//	병과 유물일 경우?
		else {
			getClientClassName ( view_as<ClientClass> ( equipmentTargetClientClass ), equipmentTypeTargetname, sizeof ( equipmentTypeTargetname ), false, true );
			Format ( equipmentTypeInfo, sizeof ( equipmentTypeInfo ), "\n\t타입: %s 전용", equipmentTypeTargetname );
		}
	}
	else {
		strcopy ( equipmentOptionInfo, sizeof ( equipmentOptionInfo ), "" );
		Format ( equipmentTypeInfo, sizeof ( equipmentTypeInfo ), "" );
	}
	
	Menu menu = new Menu ( useItemMenuHandler );
	menu.SetTitle ( "%s\n\t아이템 사용\n　\n\t정보:\n\t\t%s (Index: %d)\n\t\t%s%s%s\n\t￣￣￣￣￣￣￣￣￣￣￣￣￣￣\n\t\t%d개 소유중\n　", MENU_TITLE, g_strItemName[itemId], itemId, description, equipmentOptionInfo, equipmentTypeInfo, g_iItem[itemId][client] );
	static char itemchar[32];
	IntToString ( itemId, itemchar, sizeof ( itemchar ) );
	switch ( g_iItemType[itemId] ) {	//	각 아이템 타입마다 표시
		case ITEM_TYPE_NORMAL: {
			Format ( itemchar, sizeof ( itemchar ), "#DELETE_THIS@%d", itemId );
			menu.AddItem ( itemchar, "버리다." );
			menu.ExitBackButton = true;
			menu.Display ( client, MENU_TIME_FOREVER );
			return;
		}
		case ITEM_TYPE_CONSUME_EXP, 
		ITEM_TYPE_CONSUME_SP, 
		ITEM_TYPE_CONSUME_PLAYERPT, 
		ITEM_TYPE_CONSUME_AP, 
		ITEM_TYPE_CONSUME_HP,
		ITEM_TYPE_CONSUME_RESET_STAT,
		ITEM_TYPE_CONSUME_RESET_CLASS,
		ITEM_TYPE_PACKAGE,
		ITEM_TYPE_CONSUME_RESET_PLAYERPT: menu.AddItem ( itemchar, "사용" );
	//	case ITEM_TYPE_EQ_WEAPON, ITEM_TYPE_EQ_SOULWEAPON, ITEM_TYPE_EQ_HEAD, ITEM_TYPE_EQ_TOP, ITEM_TYPE_EQ_BOTTOMS, ITEM_TYPE_EQ_BOOTS: menu.AddItem ( itemchar, "착용", g_iItemEquipLevel[itemId] > g_iLevel[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		case ITEM_TYPE_EQUIPMENT: menu.AddItem ( itemchar, "착용", g_iItemEquipLevel[itemId] > g_iLevel[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		default: {
			delete menu;
			return;
		}
	}
	
	Format ( itemchar, sizeof ( itemchar ), "#SELL_THIS@%d", itemId );
	menu.AddItem ( itemchar, "팔기" );
	Format ( itemchar, sizeof ( itemchar ), "#DELETE_THIS@%d", itemId );
	menu.AddItem ( itemchar, "버리기" );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int useItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End:	delete menu;
		case MenuAction_Select: {
			int itemId;
			static char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			static char explode[2][4];
			if ( StrContains ( item, "#SELL_THIS" ) != -1 ) {	//	아이템 팔기
				ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
				itemId = StringToInt ( explode[1] );
				if ( g_iItem[itemId][param1] < 1 )
					return;
				
				sellItemMenu ( param1, itemId );
				return;
			}
			
			if ( StrContains ( item, "#DELETE_THIS" ) != -1 ) {	//	아이템 삭제
				ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
				itemId = StringToInt ( explode[1] );
				if ( g_iItem[itemId][param1] < 1 )
					return;
				
				CLIENT_DELETE_THIS_ITEM_MSG ( param1, g_strItemName[itemId] )
				g_iItem[itemId][param1] --;
				return;
			}
			
			else {	//	아이템 사용
				itemId = StringToInt ( item );
				if ( g_iItem[itemId][param1] < 1 )
					return;
				
				switch ( g_iItemType[itemId] ) {
					//	소비
					case ITEM_TYPE_CONSUME_EXP, 
					ITEM_TYPE_CONSUME_SP, 
					ITEM_TYPE_CONSUME_PLAYERPT, 
					ITEM_TYPE_CONSUME_AP,
					ITEM_TYPE_CONSUME_HP,
					ITEM_TYPE_CONSUME_RESET_STAT,
					ITEM_TYPE_CONSUME_RESET_CLASS,
					ITEM_TYPE_PACKAGE,
					ITEM_TYPE_CONSUME_RESET_PLAYERPT: consumeItemUse ( param1, itemId, g_iItemType[itemId] );
					//	장비
				//	case ITEM_TYPE_EQ_WEAPON, ITEM_TYPE_EQ_SOULWEAPON, ITEM_TYPE_EQ_HEAD, ITEM_TYPE_EQ_TOP, ITEM_TYPE_EQ_BOTTOMS, ITEM_TYPE_EQ_BOOTS: equipmentItemUse ( param1, itemId, g_iItemType[itemId] );
					case ITEM_TYPE_EQUIPMENT: equipmentItemUse ( param1, itemId, g_iItemType[itemId] );
				}
			}
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
				//	FakeClientCommand ( param1, "rebuy" );
					clientInventory ( param1 );
					return;
				}
			}
		}
	}
}

/**
 * 아이템 판매
 * @param client
 * @param itemId
 * @noreturn
 */
public void sellItemMenu ( int client, int itemId ) {
	if ( itemId < 0 )
		return;
		
	if ( g_iItem[itemId][client] < 1 )
		return;
	
	int sell = g_iItemPrice[itemId] / ITEM_SELL_PRICE_DIV;
	static char zd[32];
	static char info[64];
	SourceStore_GetMoneyName ( zd, sizeof ( zd ) );
	Menu menu = new Menu ( sellItemMenuHandler );
	if ( g_iItemPrice[itemId] > 0 ) {
		menu.SetTitle ( "%s\n\t아이템 팔기\n　\n\t\t%s\n\t\t판매가: %d %s", MENU_TITLE, g_strItemName[itemId], sell, zd );
		Format ( info, sizeof ( info ), "#SELL_THIS@%d", itemId );
		menu.AddItem ( info, "팔다." );
	} else {
		menu.SetTitle ( "%s\n\t아이템 팔기\n　\n\t\t%s\n\t\t판매가: 없음\n\t\t이 아이템은 판매할 수 없습니다.", MENU_TITLE, g_strItemName[itemId] );
		Format ( info, sizeof ( info ), "#DELETE_THIS@%d", itemId );
		menu.AddItem ( info, "버리다." );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int sellItemMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			int itemId;
			static char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			static char explode[2][4];
			if ( StrContains ( item, "#SELL_THIS" ) != -1 ) {	//	아이템 팔기
				ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
				itemId = StringToInt ( explode[1] );
				if ( g_iItem[itemId][param1] < 1 )
					return;
				
				g_iItem[itemId][param1] --;
				
				int sell = g_iItemPrice[itemId] / ITEM_SELL_PRICE_DIV;
				static char zd[32];
				SourceStore_GetMoneyName ( zd, sizeof ( zd ) );
				SourceStore_SetClientBalance ( param1, SourceStore_GetClientBalance ( param1 ) + sell );
				CLIENT_SELL_THIS_ITEM_MSG ( param1, g_strItemName[itemId], sell, zd )
				return;
			}
			
			if ( StrContains ( item, "#DELETE_THIS" ) != -1 ) {	//	아이템 삭제
				ExplodeString ( item, "@", explode, sizeof ( explode ), sizeof ( explode[] ) );
				itemId = StringToInt ( explode[1] );
				if ( g_iItem[itemId][param1] < 1 )
					return;
				
				CLIENT_DELETE_THIS_ITEM_MSG ( param1, g_strItemName[itemId] )
				g_iItem[itemId][param1] --;
				return;
			}
		}
	}
}

/**
 * 소비 아이템 사용
 * @param client
 * @param itemId
 * @param itemType
 * @noreturn
 */
public void consumeItemUse ( int client, int itemId, int itemType ) {
	if ( g_iItem[itemId][client] < 1 )
		return;

	switch ( itemType ) {
		case ITEM_TYPE_CONSUME_EXP:	{	//	경험치 소비 아이템
			g_iExp[client] += RoundFloat ( g_fItemAction[itemId] );
		}
		case ITEM_TYPE_CONSUME_SP: {	//	Stat Point 소비 아이템
			g_iStatPoint[client] += RoundFloat ( g_fItemAction[itemId] );
		}
		case ITEM_TYPE_CONSUME_PLAYERPT: {	//	Stat Point 소비 아이템
			g_iPlayerPoint[client] += RoundFloat ( g_fItemAction[itemId] );
		}
		case ITEM_TYPE_CONSUME_AP: {	//	AP 소비 아이템
			if ( !ZR_IsClientHuman ( client ) ) {
				CLIENT_USE_ITEM_ON_ZOMBIE_MSG ( client )
				return;
			}
			
			//	사용 횟수 초과
			if ( g_iPotionUseCount[ITEM_USE_TYPE_AP][client] >= ITEM_AP_POSION_USE_COUNT ) {
				CLIENT_USE_AP_OR_HP_USE_COUNT_EXCESS_MSG ( client, "AP" )
				return;
			}
			
			g_iPotionUseCount[ITEM_USE_TYPE_AP][client] ++;
			
			float pos[3];
			GetClientAbsOrigin ( client, pos );
			pos[2] += 10.0;
			EmitAmbientSound ( SOUND_USE_POTION, pos, client );
			loringlib_SetEntityArmor ( client, loringlib_GetEntityArmor ( client ) + RoundFloat ( g_fItemAction[itemId] ) );
			
			g_iItem[itemId][client] --;
			return;
		}
		case ITEM_TYPE_CONSUME_HP: {
			if ( !ZR_IsClientHuman ( client ) ) {
				CLIENT_USE_ITEM_ON_ZOMBIE_MSG ( client )
				return;
			}
			
			//	사용 횟수 초과
			if ( g_iPotionUseCount[ITEM_USE_TYPE_HP][client] >= ITEM_HP_POSION_USE_COUNT ) {
				CLIENT_USE_AP_OR_HP_USE_COUNT_EXCESS_MSG ( client, "HP" )
				return;
			}
			
			if ( loringlib_GetEntityHealth ( client ) >= loringlib_GetEntityMaxHealth ( client ) ) {
				CLIENT_ITEM_USE_MAXHEALTH_FAIL_MSG ( client )
				return;
			}
			
			g_iPotionUseCount[ITEM_USE_TYPE_HP][client] ++;
			
			float pos[3];
			GetClientAbsOrigin ( client, pos );
			pos[2] += 10.0;
			EmitAmbientSound ( SOUND_USE_POTION, pos, client );
			
			loringlib_SetEntityHealth ( client, loringlib_GetEntityHealth ( client ) + RoundFloat ( g_fItemAction[itemId] ) );
			if ( loringlib_GetEntityHealth ( client ) > loringlib_GetEntityMaxHealth ( client ) )
				loringlib_SetEntityHealth ( client, loringlib_GetEntityMaxHealth ( client ) );
			
			CLIENT_ITEM_USE_SUCCESS_MSG ( client, g_strItemName[itemId] )
			g_iItem[itemId][client] --;
			return;
		}
		
		//	스텟 초기화권
		case ITEM_TYPE_CONSUME_RESET_STAT: {
			resetClientStatKeys ( client, false );
		}
		
		//	클래스 초기화권
		case ITEM_TYPE_CONSUME_RESET_CLASS: {
			resetClientClassKeys ( client );
			g_iClientClass[client] = ClientClass_Normal;
			g_iPlayerPoint[client] = ( g_iLevel[client] - 1 );
		}
		
		//	아이템 패키지
		case ITEM_TYPE_PACKAGE: {
			usePackage ( client, itemId );
		}
		
		//	플레이어pt 초기화권
		case ITEM_TYPE_CONSUME_RESET_PLAYERPT: {
			resetClientClassKeys ( client, true, false );
			g_iPlayerPoint[client] = ( g_iLevel[client] - 1 );
		}
		
		//	캐릭터 슬롯 추가 아이템
		case ITEM_TYPE_SLOT_UNLOCK: {
			if ( g_iUnlockSlots[client] >= MAX_SELECT_SLOT ) {
				PrintToChat ( client, " \x04[Character Slot] \x01이미 모든 슬롯을 확장 시켰습니다." );
				return;
			}
			
			g_iUnlockSlots[client] ++;
			PrintToChat ( client, " \x04[Character Slot] \x01캐릭터 슬롯이 한칸 추가되었습니다." );
		}
	}
	
	CLIENT_ITEM_USE_SUCCESS_MSG ( client, g_strItemName[itemId] )
	g_iItem[itemId][client] --;
	
	if ( g_iItem[itemId][client] > 0 )
		useItem ( client, itemId );
	else
		clientInventory ( client );
}

/**
 * 장비 아이템 사용
 * @param client
 * @param itemId
 * @param itemType
 * @noreturn
 */
public void equipmentItemUse ( int client, int itemId, int itemType ) {
	//	아이템 타입을 수시로 채크해준다.
	if ( itemType != ITEM_TYPE_EQUIPMENT )
		return;
	
	
	if ( g_iItemEquipLevel[itemId] > g_iLevel[client] )
		return;
	
	//	같은 타입의 유물을 이미 끼고 있는지 채크한다.
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
		if ( g_iEquipItemIndex[i][client] > 0 ) {
			if ( g_iItemEquipmentType[g_iEquipItemIndex[i][client]] == g_iItemEquipmentType[itemId] ) {
				CLIENT_ITEM_EQUIP_FAILED_STACKED ( client, g_strItemName[g_iEquipItemIndex[i][client]] )
				return;
			}
		}
	}
	
	//	병과 유물일 경우?
	if ( g_iItemEquipmentType[itemId] == ITEM_EQUIPMENT_TYPE_CLASS ) {
		if ( !StrEqual ( g_strItemEquipmentTypeTarget[itemId], "normal", false ) ) {
			int classid = getEquipmentTypeTargetToClientClassIndex ( g_strItemEquipmentTypeTarget[itemId] );
			//	병과가 맞지 않을 경우
			if ( g_iClientClass[client] != view_as<ClientClass> ( classid ) ) {
				CLIENT_ITEM_EQUIP_FAILED_DISMATCH_CLASS ( client )
				return;
			}
		}
	}
	
	//	유물 빈칸을 채크한다.
	int tempSlotIndex = -1;
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
		if ( g_iEquipItemSlotUnlock[i] <= g_iLevel[client] ) {
			if ( g_iEquipItemIndex[i][client] <= 0 ) {
				tempSlotIndex = i;
				break;
			}
		}
	}
	
	//	유물 칸이 없을 경우
	if ( tempSlotIndex == -1 ) {
		CLIENT_ITEM_EQUIP_FAILED_FULL_MSG ( client )
		FakeClientCommand ( client, "rebuy" );
		return;
	}
	
	//	칸이 있을 경우
	g_iEquipItemIndex[tempSlotIndex][client] = itemId;
	g_iItem[itemId][client] --;
	//	아이템 속성 값을 삽입한다.
	itemActionValueToggle ( client, itemId, itemType, true, tempSlotIndex );
	
	CLIENT_ITEM_EQUIP_SUCCESS_MSG ( client, g_strEquipItem[itemType][client] )
	FakeClientCommand ( client, "rebuy" );
	return;
	
	//	착용중인 아이템이 있을 경우
/*	if ( !StrEqual ( g_strEquipItem[itemType][client], "" ) ) {
		CLIENT_ITEM_EQUIP_FAILED_MSG ( client, g_strEquipItem[itemType][client] )
		FakeClientCommand ( client, "rebuy" );
		return;
	}
	
	g_strEquipItem[itemType][client] = g_strItemName[itemId];
	g_iItem[itemId][client] --;
	itemActionValueToggle ( client, itemId, itemType, true );
	CLIENT_ITEM_EQUIP_SUCCESS_MSG ( client, g_strEquipItem[itemType][client] )
	FakeClientCommand ( client, "rebuy" );
	return;	*/
}

/**
 * 페키지 아이템 사용
 */
void usePackage ( int client, int item ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( g_iItemType[item] != ITEM_TYPE_PACKAGE )
			return;
		
		if ( strcmp ( g_strItemName[item], "2019 RECODE SPECIAL THANKS PACKAGE" ) != -1 ) {
			SourceStore_AddClientBalance ( client, 10000 );
			g_iItem[12][client] += 20;
			
			PrintToChat ( client, " \x04[Package] \x01다음 아이템을 획득했습니다:" );
			PrintToChat ( client, "┣ \x0910,000 zdollars" );
			PrintToChat ( client, "┗ \x09Extend 경험치 Elixir III \x01× 20" );
		}
		
		else if ( strcmp ( g_strItemName[item], "2019 RECODE SPECIAL THANKS VIP/SVIP PACKAGE" ) != -1 ) {
			SourceStore_AddClientBalance ( client, 10000 );
			g_iItem[12][client] += 30;

			PrintToChat ( client, " \x04[Package] \x01다음 아이템을 획득했습니다:" );
			PrintToChat ( client, "┣ \x0910,000 zdollars" );
			PrintToChat ( client, "┗ \x09Extend 경험치 Elixir III \x01× 30" );
		}
	}
}

/**
 * 무기 유물을 장착중일 경우 무기를 들게 한다.
 *
 * @param client
 */
public void ITEMINIT_SpawnOnEquipWeapon ( int client ) {
	CreateTimer ( 0.5, ITEMINIT_SpawnOnEquipWeaponPost, GetClientSerial ( client ), TIMER_FLAG_NO_MAPCHANGE );
}

public Action ITEMINIT_SpawnOnEquipWeaponPost ( Handle timer, any serial ) {
	static int client;
	client = GetClientFromSerial ( serial );
	
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	if ( !ZR_IsClientHuman ( client ) )
		return Plugin_Stop;
	
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ ) {
		if ( g_iEquipItemIndex[i][client] <= 0 )
			continue;
		
		//	유물 타입 타겟이 무기일 경우?
		if ( strcmp ( g_strItemEquipmentTypeTarget[g_iEquipItemIndex[i][client]], "weapon_" ) ) {
			if ( strcmp ( g_strItemEquipmentTypeTarget[g_iEquipItemIndex[i][client]], "hkp2000" ) ) {
				loringlib_SetEntityTeamNumber ( client, CS_TEAM_T );
				GivePlayerItem ( client, g_strItemEquipmentTypeTarget[g_iEquipItemIndex[i][client]] );
				loringlib_SetEntityTeamNumber ( client, CS_TEAM_CT );
				return Plugin_Stop;
			}
			GivePlayerItem ( client, g_strItemEquipmentTypeTarget[g_iEquipItemIndex[i][client]] );
			return Plugin_Stop;
		}
	}
	
	return Plugin_Stop;
}

/**
 * 아이템 액션 밸류를 적용 시킴.
 * @param client
 * @param itemId
 * @param itemType
 * @param equip		착용 여부
 */
stock void itemActionValueToggle ( int client, int itemId, int itemType, bool equip, int slot ) {
	if ( itemType != ITEM_TYPE_EQUIPMENT ||
		itemId <= 0 )
		return;
	
	if ( g_iItemEquipmentType[itemId] <= 0 )
		return;
	
	for ( int i = 1; i <= MAX_EQUIPMENT_OPTIONS; i ++ )
		setEquipmentOptionResult ( client, g_strItemEquipmentOptions[itemId][i], slot, g_strItemEquipmentTypeTarget[itemId], equip );
}

/**
 * 유물 옵션 값 대입하기
 * 
 * @param equipToggle			장착 유무, false 경우 윈래 값으로 대돌린다.
 */
stock void setEquipmentOptionResult ( int client, char[] option, int slot, char[] equipmentTypeTarget, bool equipToggle ) {
	//	옵션 값이 없을 경우
	if ( StrEqual ( option, "-1" ) )
		return;
	
	static char temp[PLATFORM_MAX_PATH];
	Format ( temp, sizeof ( temp ), "%s", option );
	
	static char split[16];	//	구분 문자
	static int calc;
	//	플러스 경우 1, 퍼센트 경우 2로 구분한다.
	if ( StrContains ( temp, "+" ) != -1 ) {
		strcopy ( split, sizeof ( split ), "+" );
		calc = 1;
	} else if ( StrContains ( temp, "%" ) != -1 ) {
		strcopy ( split, sizeof ( split ), "%" );
		calc = 2;
	}
	
	static char explode[2][16];
	ExplodeString ( temp, split, explode, sizeof ( explode ), sizeof ( explode[] ) );
	
	//	옵션 타입 값과 대입 값을 가져온다.
	static int optionType;
	static float optionValue;
	optionType = StringToInt ( explode[0] );		//	옵션 타입 값
	optionValue = StringToFloat ( explode[1] );		//	옵션 대입 값
	
	int index = replaceOptionIdxToStatIdx ( optionType );	//	보너스 스텟 변수 배열 값
	int classid = getEquipmentTypeTargetToClientClassIndex ( equipmentTypeTarget );	//	장착 타입 타겟 값
	
	//	Stat option
	if ( equipToggle ) {	//	Equip
		switch ( calc ) {
			case 1: {	//	합산
				if ( optionType >= 100 && optionType < 200 )
					g_fEquipBonusStat[slot][index][client] += optionValue;
				else if ( optionType >= 200 && optionType < 300 )
					g_fEquipBonusStat[slot][index][client] += optionValue;
				else
					if ( classid != -1 )
						g_iEquipBonusSkill[classid][optionType][client] += RoundFloat ( optionValue );
			}
			
			case 2: {	//	퍼센티지				
				float percentage = optionValue / 100;	//	퍼센티지 계산
			//	percentage += 1.0;
				
				if ( optionType >= 100 && optionType < 200 ) {
					int statArrayIndex = replaceOptionIdxToStatArrIdx ( optionType );
					if ( statArrayIndex == -1 )
						return;
					g_fEquipBonusStat[slot][index][client] += ( g_iStats[statArrayIndex][client] * percentage );
				}
				else if ( optionType >= 200 && optionType < 300 )
					g_fEquipBonusStat[slot][index][client] += percentage;
			}
		}
	}
	
	else {		//	Unequipped
		if ( optionType >= 100 && optionType < 300 )
			g_fEquipBonusStat[slot][index][client] = 0.0;
		else
			if ( classid != -1 )
				g_iEquipBonusSkill[classid][optionType][client] -= RoundFloat ( optionValue );
	}
}

/**
 * 착용 타겟이 병과 명일 때 병과 index로 변환한다.
 */
stock int getEquipmentTypeTargetToClientClassIndex ( char[] equipmentTypeTarget ) {
	static int classid;
	if ( StrEqual ( equipmentTypeTarget, "assaulter" ) )
		classid = view_as<int> ( ClientClass_Assault );
	else if ( StrEqual ( equipmentTypeTarget, "medic" ) )
		classid = view_as<int> ( ClientClass_Medic );
	else if ( StrEqual ( equipmentTypeTarget, "sniper" ) )
		classid = view_as<int> ( ClientClass_Sniper );
	else if ( StrEqual ( equipmentTypeTarget, "supporter" ) )
		classid = view_as<int> ( ClientClass_Supporter );
	else if ( StrEqual ( equipmentTypeTarget, "shotgunner" ) )
		classid = view_as<int> ( ClientClass_Shotgunner );
	else if ( StrEqual ( equipmentTypeTarget, "gunslinger" ) )
		classid = view_as<int> ( ClientClass_Gunslinger );
	else if ( StrEqual ( equipmentTypeTarget, "normal" ) )
		classid = view_as<int> ( ClientClass_Normal );
	else
		classid = -1;
	
	return classid;
}

/**
 * 옵션 타입 값에서 보너스 스텟 변수 배열 값으로 바꿈
 * @note 100 이하의 값일 경우 optionIndex 값으로 반환한다.
 *
 * @param itemActionType
 * @return Bonus Stat Array index
 */
stock int replaceOptionIdxToStatIdx ( int optionIndex ) {
	int result;
	switch ( optionIndex ) {
		case ITEM_OPTION_TYPE_STR: result = EQUIPMENT_BONUS_STAT_STR;
		case ITEM_OPTION_TYPE_DEF: result = EQUIPMENT_BONUS_STAT_DEF;
		case ITEM_OPTION_TYPE_AGL: result = EQUIPMENT_BONUS_STAT_AGL;
		case ITEM_OPTION_TYPE_RELOAD:	result = EQUIPMENT_BONUS_STAT_RELOADSPD;
		case ITEM_OPTION_TYPE_FIRERATE: result = EQUIPMENT_BONUS_STAT_FIRERATE;
		case ITEM_OPTION_TYPE_FIRE_DAMAGE:	result = EQUIPMENT_BONUS_STAT_DAMAGE;
		default: result = optionIndex;
	}
	
	return result;
}

/**
 * 옵션 타입 값에서 스텟 값으로 바꿈
 */
stock int replaceOptionIdxToStatArrIdx ( int optionIndex ) {
	int result = -1;
	switch ( optionIndex ) {
		case ITEM_OPTION_TYPE_STR: result = STAT_STR;
		case ITEM_OPTION_TYPE_DEF: result = STAT_DEF;
		case ITEM_OPTION_TYPE_AGL: result = STAT_AGL;
		default: result = -1;
	}
	
	return result;	
}

/**
 * 아이템 데이터를 KeyValue에서 찾아옴.
 */
public void defineItemKeyValue () {
	static char tempKeyname[64];
	
	static char temp[PLATFORM_MAX_PATH];
	static char file[32];
	Format ( file, sizeof ( file ), "item_list.txt" );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, "data/zombierelolied/addons/%s", file );
	KeyValues kv = new KeyValues ( file );
	if ( kv.ImportFromFile ( temp ) ) {
		g_iDefineItems = 0;
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strItemName[g_iDefineItems], ITEM_NAME_MAX_LENGTH );
			kv.GetString ( "description", g_strItemDescription[g_iDefineItems], ITEM_DESCRIPTION_MAX_LENGTH );
			g_iItemType[g_iDefineItems] = kv.GetNum ( "type" );
			g_fItemAction[g_iDefineItems] = kv.GetFloat ( "actionvalue" );
			g_iItemActionType[g_iDefineItems] = kv.GetNum ( "actiontype" );
			g_iItemRarity[g_iDefineItems] = kv.GetNum ( "rarity" );
			g_bItemSell[g_iDefineItems] = ( kv.GetNum ( "issell" ) == 1 ? true : false );
			g_iItemPrice[g_iDefineItems] = kv.GetNum ( "price" );
			g_iItemDrop[g_iDefineItems] = kv.GetNum ( "isdrop" );
			//	장착 아이템일 경우 해당 아이템의 조합 아이템을 불러옴
			if ( g_iItemType[g_iDefineItems] == ITEM_TYPE_EQUIPMENT ) {
				g_iItemEquipLevel[g_iDefineItems] = kv.GetNum ( "equiplevel" );
				kv.GetString ( "craft_item1", g_strItemCraftItems[g_iDefineItems][0], ITEM_CRAFTITEM_CHAR_MAX_LENGTH );
				kv.GetString ( "craft_item2", g_strItemCraftItems[g_iDefineItems][1], ITEM_CRAFTITEM_CHAR_MAX_LENGTH );
				kv.GetString ( "craft_item3", g_strItemCraftItems[g_iDefineItems][2], ITEM_CRAFTITEM_CHAR_MAX_LENGTH );
				kv.GetString ( "craft_item4", g_strItemCraftItems[g_iDefineItems][3], ITEM_CRAFTITEM_CHAR_MAX_LENGTH );
				
				g_iItemEquipmentType[g_iDefineItems] = kv.GetNum ( "equipment_type" );
				
				//	유물 타입이 웨폰이거나, 클래스일 경우?
				if ( g_iItemEquipmentType[g_iDefineItems] == ITEM_EQUIPMENT_TYPE_WEAPON ||
					g_iItemEquipmentType[g_iDefineItems] == ITEM_EQUIPMENT_TYPE_CLASS ) {
					//	타입 타겟을 가져온다. (무기 이름이나, 병과 이름 등)
					kv.GetString ( "equipment_type_target", g_strItemEquipmentTypeTarget[g_iDefineItems], sizeof ( g_strItemEquipmentTypeTarget[] ) );
				}
				
				//	아이템 옵션 값을 가져온다.
				for ( int i = 1; i <= MAX_EQUIPMENT_OPTIONS; i ++ ) {
					Format ( tempKeyname, sizeof ( tempKeyname ), "equipment_option%d", i );
					kv.GetString ( tempKeyname, g_strItemEquipmentOptions[g_iDefineItems][i], sizeof ( g_strItemEquipmentOptions[][] ) );
				}
				
			} else {
				g_iItemEquipLevel[g_iDefineItems] = 0;
				
				g_strItemCraftItems[g_iDefineItems][0] = "-1";
				g_strItemCraftItems[g_iDefineItems][1] = "-1";
				g_strItemCraftItems[g_iDefineItems][2] = "-1";
				g_strItemCraftItems[g_iDefineItems][3] = "-1";
				
				for ( int i = 1; i <= MAX_EQUIPMENT_OPTIONS; i ++ ) {
					g_strItemEquipmentOptions[g_iDefineItems][i] = "-1";
				}
			}
			
			g_iDefineItems ++;
		}
		
		while ( kv.GotoNextKey () );
		delete kv;
	} else	delete kv;
	
	if ( g_iDefineItems > 0 )
		ItemLoadOnDefineDropItems ();
}