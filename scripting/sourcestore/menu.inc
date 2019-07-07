/**
 * Premium Store main menu display.
 */
public Action onClientCommandPremiumStore ( int client, int args ) {
	Menu menu = new Menu ( premiumMenuHandler );
	menu.SetTitle ( "%s\n\tPremium Store\n　", g_strConfMenuTitle );
	if ( !g_bIsMvp[client] ) {
		menu.AddItem ( "", "귀하는 볼 수 없습니다.", ITEMDRAW_DISABLED );
		menu.ExitBackButton = true;
		menu.Display ( client, MENU_TIME_FOREVER );
		return Plugin_Handled;
	}
	bool empty = true;
	static char rarity[16];
	static char info[256];
	static char balanceFormat[32];
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItemRarity[i] == ITEMRARE_EVENT ) {
			empty = false;
			getItemRareName ( i, rarity, sizeof ( rarity ) );
			if ( g_iItemPrice[i] > 0 ) {
				loringlib_NumberFormat ( g_iItemPrice[i], balanceFormat, sizeof ( balanceFormat ) );
				Format ( info, sizeof ( info ), "%s | %s [%s]", g_strItemName[i], rarity, balanceFormat );
			} else
				Format ( info, sizeof ( info ), "%s | %s [FREE]", g_strItemName[i], rarity );
			
			menu.AddItem ( g_strItemName[i], info, g_iItemPrice[i] > g_iBalance[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		}
	}
	
	if ( empty ) {
		menu.AddItem ( "", "EMPTY", ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
	return Plugin_Handled;
}

public int premiumMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char info[64];
			int item = -1;
			menu.GetItem ( param2, info, sizeof ( info ) );
			for ( int i = 0; i < MAXITEMS; i ++ ) {
				if ( StrEqual ( g_strItemName[i], info ) ) {
					item = i;
					break;
				}
			}
			
			if ( item != -1 )
				buyItem ( param1, item );
		}
			
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandStore ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Store main menu display.
 */
public Action onClientCommandStore ( int client, int args ) {
	if ( loringlib_IsValidClient ( client ) ) {
		if ( args > 0 )
			return Plugin_Handled;
		
		Menu menu = new Menu ( storeMenuHandler );
		menu.SetTitle ( g_strConfMenuTitle );
		menu.AddItem ( "", "인벤토리" );
		menu.AddItem ( "", "장착 아이템" );
		menu.AddItem ( "", "아이템 구입\n　" );
		menu.AddItem ( "", "뽑기\n　", g_iConfUseRandombox ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
		menu.AddItem ( "", "Settings" );
		menu.Display ( client, MENU_TIME_FOREVER );
	}
	
	return Plugin_Handled;
}

public int storeMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: storeViewMyInventory ( param1 );
				case 1:	storeViewMyEquipments ( param1 );
				case 2: storeBuyItems ( param1 );
				case 3: storeGasyaMenu ( param1 );
				case 4: storeSettingMenu ( param1 );
			}
		}
	}
}

/**
 * Buy menu.
 */
public void storeBuyItems ( int client ) {
	Menu menu = new Menu ( storeBuyItemHandler );
	menu.SetTitle ( "%s\n\t%s\n　", g_strConfMenuTitle, g_strConfBuyItemTitle );
	menu.AddItem ( "1", "캐릭터", g_iConfMenuEnable[MENUTYPE_CHARACTER] == 1 ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
	menu.AddItem ( "2", "트레일", g_iConfMenuEnable[MENUTYPE_TRAIL] == 1 ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
	menu.AddItem ( "4", "레이저 총알", g_iConfMenuEnable[MENUTYPE_LASERBULLET] == 1 ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
	menu.AddItem ( "3", "태그 타이틀", g_iConfMenuEnable[MENUTYPE_TAGTITLE] == 1 ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
	menu.AddItem ( "5", "아우라", g_iConfMenuEnable[MENUTYPE_AURA] == 1 ? ITEMDRAW_DEFAULT : ITEMDRAW_RAWLINE );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int storeBuyItemHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char type[16];
			menu.GetItem ( param2, type, sizeof ( type ) );
			listOfItems ( param1, StringToInt ( type ) );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandStore ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Setting Menu
 */
public void storeSettingMenu ( int client ) {
	Menu menu = new Menu ( storeSettingMenuHandler );
	menu.SetTitle ( "%s\nSettings\n　", g_strConfMenuTitle );
	static char status[MAX_SETTINGS][32];
	Format ( status[SETTINGTYPE_MIRROR], sizeof ( status[] ), "Toggle Mirror: %s", g_iClientSettingToggle[SETTINGTYPE_MIRROR][client] ? "Enabled" : "Disabled" );
	Format ( status[SETTINGTYPE_THIRDPERSON], sizeof ( status[] ), "Toggle Third Person: %s", g_iClientSettingToggle[SETTINGTYPE_THIRDPERSON][client] ? "Enabled" : "Disabled" );
	for ( int i = 0; i < MAX_SETTINGS; i ++ )
		menu.AddItem ( "", status[i] );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int storeSettingMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				case 0: onClientCommandMirror ( param1, 0 );
				case 1: onClientCommandThirdPerson ( param1, 0 );
			}
		
			storeSettingMenu ( param1 );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: onClientCommandStore ( param1, 0 );
			}
		}
	}
}

/**
 * List of items.
 */
public void listOfItems ( int client, int type ) {
	static char typeName[32];
	getTypeName ( type, typeName, sizeof ( typeName ) );
	Menu menu = new Menu ( listOfItemsHandler );
	menu.SetTitle ( "%s\n\t%s\n\t\t%s\n　", g_strConfMenuTitle, g_strConfBuyItemTitle, typeName );
	bool empty = true;
	static char rarity[16];
	static char info[256];
	static char balanceFormat[32];
	
	if ( type == ITEMTYPE_CHARACTER )
		menu.AddItem ( "#PREVIEW", "Preview\n\t\t판매중인 캐릭터를 미리 볼 수 있습니다.\n　" );
	
	for ( int j = 0; j < MAXSTORELEVEL; j ++ ) {	//	store rank level
		for ( int x = 1; x <= 4; x ++ ) {			//	item rarity
			for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {	//	item indexes
				if ( g_iItemRarity[i] != x )
					continue;
				
				if ( g_iItemClassLevel[i] != j )
					continue;
				
				if ( g_iItemNotSell[i] == 1 )
					continue;
				
				if ( g_iItemType[i] == type ) {
					empty = false;
					getItemRareName ( i, rarity, sizeof ( rarity ) );
					if ( g_iItemPrice[i] > 0 ) {
						loringlib_NumberFormat ( g_iItemPrice[i], balanceFormat, sizeof ( balanceFormat ) );
						Format ( info, sizeof ( info ), "%s | %s [%s]", g_strItemName[i], rarity, balanceFormat );
					} else
						Format ( info, sizeof ( info ), "%s | %s [FREE]", g_strItemName[i], rarity );
					
					menu.AddItem ( g_strItemName[i], info, g_iItemPrice[i] > g_iBalance[client] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
				}
			}
		}
	}
	
	if ( empty ) {
		menu.AddItem ( "", "EMPTY", ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int listOfItemsHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char info[64];
			int item = -1;
			menu.GetItem ( param2, info, sizeof ( info ) );
			
			if ( StrEqual ( info, "#PREVIEW", false ) ) {
				listOfItemPreview ( param1 );
				return;
			}
			
			for ( int i = 0; i < MAXITEMS; i ++ ) {
				if ( StrEqual ( g_strItemName[i], info ) ) {
					item = i;
					break;
				}
			}
			
			if ( item != -1 )
				buyItem ( param1, item );
		}
			
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					storeBuyItems ( param1 );
					return;
				}
			}
		}
	}
}

/**
 * 구입 처리
 */
public int buyItem ( int client, int item ) {
	if ( item == -1 ) {
		PrintToChat ( client, "%s \x07ERROR: \x01아이템 값이 잘못되었습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
		return;
	}
	
	//	Free item.
	if ( g_iItemPrice[item] <= 0 ) {
		buyFreeItem ( client, item );
		return;
	}
	
	Menu menu = new Menu ( buyItemHandler );
	static char lore[256];
	static char balanceFormat[32];
	loringlib_NumberFormat ( g_iItemPrice[item], balanceFormat, sizeof ( balanceFormat ) );
	if ( g_iItemRarity[item] == ITEMRARE_ADMIN )
		Format ( lore, sizeof ( lore ), "\t\t아이템 번호: %d\n\t\t아이템 가격: %s %s\n\t\t아이템 등급: NONE", item, balanceFormat, g_strConfMoneyName );
	else
		Format ( lore, sizeof ( lore ), "\t\t아이템 번호: %d\n\t\t아이템 가격: %s %s\n\t\t아이템 등급: %s", item, balanceFormat, g_strConfMoneyName, g_strConfItemRarity[g_iItemRarity[item]] );
	
	if ( g_iItemType[item] == ITEMTYPE_TAGTITLE ) {
		if ( g_iItemTagColor[item] >= 0 && g_iItemTagColor[item] < 16 )
			Format ( lore, sizeof ( lore ), "%s\n\t\t태그 색상: %s", lore, g_strItemTagColorTemp[g_iItemTagColor[item]][1] );
		else
			Format ( lore, sizeof ( lore ), "%s\n\t\t태그 색상: %s", lore, g_strItemTagColorTemp[0][0] );
	}
	
	menu.SetTitle ( "%s\n\t%s 아이템을 구입할까요?\n%s\n　", g_strConfMenuTitle, g_strItemName[item], lore );
	static char xitem[4];
	IntToString ( item, xitem, sizeof ( xitem ) );
	menu.AddItem ( xitem, g_iBalance[client] >= g_iItemPrice[item] ? "Yes" : "Not enough balance.", g_iBalance[client] >= g_iItemPrice[item] ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED );
	menu.AddItem ( "", "Cancel" );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyItemHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char info[4];
			menu.GetItem ( param2, info, sizeof ( info ) );
			int item = StringToInt ( info );
			switch ( param2 ) {
				case 0: {
					g_iItem[param1][item] ++;
					g_iBalance[param1] -= g_iItemPrice[item];
					displayReceipt ( param1, item );
					
					char itemType[32];
					getItemEquipTypeName ( item, itemType, sizeof ( itemType ) );
					
					Call_StartForward ( g_hForwardHandlerArray[FORWARD_ON_BUY_ITEM] );
					Call_PushCell ( param1 );
					Call_PushCell ( item );
					Call_PushString ( g_strItemName[item] );
					Call_PushString ( itemType );
					Call_PushCell ( g_iBalance[param1] + g_iItemPrice[item] );
					Call_PushCell ( g_iBalance[param1] );
					Call_Finish ();
				}
				
				case 1: {
					int type = getItemType ( g_strItemName[item] );
					listOfItems ( param1, type );
				}
			}
		}
	}
}

/**
 * 구입 처리 (무료 아이템)
 */
public void buyFreeItem ( int client, int item ) {
	Menu menu = new Menu ( buyFreeItemHandler );
	static char lore[256];
	if ( g_iItemRarity[item] == ITEMRARE_ADMIN )
		Format ( lore, sizeof ( lore ), "\t\t아이템 번호: %d\n\t\t아이템 등급: NONE\n\t\t아이템 사용 등급: %s", item, g_strConfClassLevel[g_iItemClassLevel[item]] );
	else
		Format ( lore, sizeof ( lore ), "\t\t아이템 번호: %d\n\t\t아이템 등급: %s\n\t\t아이템 사용 등급: %s", item, g_strConfItemRarity[g_iItemRarity[item]], g_strConfClassLevel[g_iItemClassLevel[item]] );
	menu.SetTitle ( "%s\n\t%s 아이템을 구입할까요?\n%s\n　", g_strConfMenuTitle, g_strItemName[item], lore );
	static char xitem[4];
	IntToString ( item, xitem, sizeof ( xitem ) );
	menu.AddItem ( xitem, "Yes" );
	menu.AddItem ( "", "Cancel\n　\n\t\t이 아이템은 무료 입니다. 구입 시 바로 착용됩니다.\n\t\t\t무료 아이템들은 인벤토리로 옮기거나, 판매할 수 없습니다." );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int buyFreeItemHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[4];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int i = StringToInt ( item );
			int type = getItemTypeEx ( i );
			
			switch ( param2 ) {
				case 0: {
					//	아이템을 사용하고 있을 경우
					if ( !StrEqual ( g_strEquipItemName[param1][type], "" ) ) {
						PrintToChat ( param1, "%s \x01아이템을 해제하고 사용하세요. 현재 \x0C%s \x01아이템을 사용하고 있습니다.", g_strConfChatPrefix, g_strEquipItemName[param1][type] );
						storeViewMyEquipments ( param1 );
						return;
					}
					
					g_strEquipItemName[param1][type] = g_strItemName[i];
					switch ( type ) {
					//	case ITEMTYPE_CHARACTER:	equipCharModel ( param1 );
						case ITEMTYPE_CHARACTER:	PrintToChat ( param1, "%s \x01다음 라운드 부터 캐릭터가 적용됩니다.", g_strConfChatPrefix );
						case ITEMTYPE_TRAIL:		equipTrail ( param1 );
						case ITEMTYPE_LASERBULLET:	equipLaserBullet ( param1 );
						case ITEMTYPE_AURA:			equipParticleAura ( param1 );
					}
					
					displayReceipt ( param1, i );
					PrintToChat ( param1, "%s \x01Update success.", g_strConfChatPrefix );
				}
				
				case 1: listOfItems ( param1, type );
			}
		}
	}
}

public void displayReceipt ( int client, int item ) {
	Menu menu = new Menu ( displayReceiptHandler );
	menu.SetTitle ( "%s\n\t%s 아이템을 구입했습니다.\n　", g_strConfMenuTitle, g_strItemName[item] );
	menu.AddItem ( "", g_strConfVMITitle );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int displayReceiptHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( action == MenuAction_Select )
		if ( param2 == 0 )
			storeViewMyInventory ( param1 );
}

/**
 * 인벤토리
 */
public void storeViewMyInventory ( int client ) {
	g_iTempSelector[client] = -1;
	static char itemInfo[4][128];
	static char balanceFormat[32];
	loringlib_NumberFormat ( g_iBalance[client], balanceFormat, sizeof ( balanceFormat ) );
	Menu menu = new Menu ( storeVMIHandler );
	Format ( itemInfo[3], sizeof ( itemInfo[] ), "\n\t\t프리미엄: %s", g_bIsMvpPlus[client] ? "SVIP" : ( g_bIsMvp[client] ? "VIP" : "NONE" ) );
	menu.SetTitle ( "%s\n\t%s\n\t\t지갑: %s %s\n\t\tItems: %d / %d%s\n　", g_strConfMenuTitle, g_strConfVMITitle, balanceFormat, g_strConfMoneyName, getClientItems ( client ), g_iConfMaxInventoryItems, g_iConfUseMvp == 1 ? itemInfo[3] : "" );
	bool validItem = false;
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItem[client][i] > 0 ) {
			validItem = true;
			getItemRareName ( i, itemInfo[0], 127 );
			Format ( itemInfo[1], 127, "%s | %s [%d개]", g_strItemName[i], itemInfo[0], g_iItem[client][i] );
			Format ( itemInfo[2], 127, "%d", i );
			menu.AddItem ( itemInfo[2], itemInfo[1] );
		}
	}
	
	if ( !validItem ) {
		menu.AddItem ( "", "아이템이 없습니다.", ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int storeVMIHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char name[16];
			menu.GetItem ( param2, name, sizeof ( name ) );
			for ( int i = 0; i < MAXITEMS; i ++ ) {
				if ( i == StringToInt ( name ) ) {
					g_iTempSelector[param1] = getItem ( g_strItemName[i] );
					useItem ( param1, g_iTempSelector[param1] );
					
					break;
				}
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandStore ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Use the item.
 */
public void useItem ( int client, int item ) {
	//	Is free tag?
	if ( StrEqual ( g_strItemName[item], g_strConfFreeTagName ) &&
		g_iItemType[item] == ITEMTYPE_TAGTITLE ) {
		g_iTempTagItem[client] = item;
		Menu menu = new Menu ( useTagHandler );
		menu.SetTitle ( "%s\n\t칭호를 설정할까요?\n　", g_strConfMenuTitle );
		menu.AddItem ( "", "Yes" );
		menu.AddItem ( "", "Cancel" );
		menu.ExitButton = false;
		menu.Display ( client, MENU_TIME_FOREVER );
		return;
	}
	
	Menu menu = new Menu ( useItemHandler );
	menu.SetTitle ( "%s\n\t아이템을 어떻게 할까?\n\t\t아이템 명: %s\n\t\t아이템 번호: %d\n　", g_strConfMenuTitle, g_strItemName[item], item );
	menu.AddItem ( "", "Equip" );
	menu.AddItem ( "", "Sell" );
	menu.AddItem ( "", "Drop", ITEMDRAW_DISABLED );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

/**
 * Tag
 */
public int useTagHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			if ( param2 == 0 )
				useTag ( param1 );
		}
	}
}

public void useTag ( int client ) {
	if ( !StrEqual ( g_strEquipItemName[client][ITEMTYPE_TAGTITLE], "" ) ) {
		PrintToChat ( client, "%s \x01칭호 아이템을 장착 해제해 주세요." );
		storeViewMyEquipments ( client );
		return;
	}
	
	g_bUseTag[client] = true;
	Menu menu = new Menu ( useTagHandler );
	menu.SetTitle ( "%s\n\t칭호 사용:", g_strConfMenuTitle );
	menu.AddItem ( "", "설정할 칭호를 채팅창에 입력하세요." );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

/**
 * item
 */
public int useItemHandler ( Menu menu, MenuAction action, int param1, int param2 ) {	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			int selector = g_iTempSelector[param1]; 
			int type = getItemTypeEx ( selector );
			int sellPrice = ( g_iItemPrice[selector] / ITEM_SELL_DIV );
			switch ( param2 ) {
				//	Equip
				case 0: {
					//	인벤토리 공간이 있을 경우
					if ( g_iItem[param1][selector] <= 1 ||
						isValidInventory ( param1 ) ) {
						//	아이템을 사용하고 있을 경우
						if ( !StrEqual ( g_strEquipItemName[param1][type], "" ) ) {
							PrintToChat ( param1, "%s \x01아이템을 해제하고 사용하세요. 현재 \x0C%s \x01아이템을 사용하고 있습니다.", g_strConfChatPrefix, g_strEquipItemName[param1][type] );
							storeViewMyEquipments ( param1 );
							return;
						}
						
						g_strEquipItemName[param1][type] = g_strItemName[selector];
						switch ( type ) {
						//	case ITEMTYPE_CHARACTER:	equipCharModel ( param1 );
							case ITEMTYPE_CHARACTER:	PrintToChat ( param1, "%s \x01다음 라운드 부터 캐릭터가 적용됩니다.", g_strConfChatPrefix );
							case ITEMTYPE_TRAIL:		equipTrail ( param1 );
							case ITEMTYPE_LASERBULLET:	equipLaserBullet ( param1 );
						//	case ITEMTYPE_TAGTITLE:		equipTagTitle ( param1 );
						}
						
						g_iItem[param1][selector] --;	//	아이템을 사용했으니 사용한 아이템 갯수만큼 빼줍니다.
						PrintToChat ( param1, "%s \x01Update success.", g_strConfChatPrefix );
					}
					
					else
						PrintToChat ( param1, "%s \x01Inventory is full!", g_strConfChatPrefix );
				}
				
				//	Sell
				case 1: {
					sellVerifyMenu ( param1, selector, sellPrice );
				}
				
				//	Drop
				case 2: {
					g_iItem[param1][selector] --;
					PrintToChat ( param1, "%s \x01%s 아이템을 버렸습니다.", g_strConfChatPrefix, g_strItemName[selector] );
				}
			}
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandStore ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * 버리기 전에 확인 메뉴
 */
void sellVerifyMenu ( int client, int item, int sell ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	if ( item < 0 || item > MAXITEMS ) {
		PrintToChat ( client, "%s \x01유효하지 않는 아이템 입니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
		return;
	}
	
	Menu menu = new Menu ( sellVerifyMenuHandle );
	menu.SetTitle ( "%s\n\t정말 %s 아이템을 팔까요?\n\t\t판매가: %d %s\n　", g_strConfMenuTitle, g_strItemName[item], sell, g_strConfMoneyName );
	static char info[32];
	static char itemId[16];
	Format ( info, sizeof ( info ), "%d@%d", item, sell );
	IntToString ( item, itemId, sizeof ( itemId ) );
	menu.AddItem ( info, "판매" );
	menu.AddItem ( itemId, "취소" );
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int sellVerifyMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			switch ( param2 ) {
				//	Sell this item.
				case 0: {
					static char itemId[32];
					static char temp[2][16];
					menu.GetItem ( param2, itemId, sizeof ( itemId ) );
					ExplodeString ( itemId, "@", temp, sizeof ( temp ), sizeof ( temp[] ) ); 
					int item = StringToInt ( temp[0] );
					int sellPrice = StringToInt ( temp[1] );
					
					g_iItem[param1][item] --;
					g_iBalance[param1] += sellPrice;
					PrintToChat ( param1, "%s \x01%s 아이템을 판매하고 %d %s 만큼 받았습니다.", g_strConfChatPrefix, g_strItemName[item], sellPrice, g_strConfMoneyName );
				
					static char itemType[32];
					getTypeName ( g_iItemType[item], itemType, sizeof ( itemType ) );
					
					Call_StartForward ( g_hForwardHandlerArray[FORWARD_ON_SELL_ITEM] );
					Call_PushCell ( param1 );
					Call_PushCell ( item );
					Call_PushString ( g_strItemName[item] );
					Call_PushString ( itemType );
					Call_PushCell ( g_iBalance[param1] - sellPrice );
					Call_PushCell ( g_iBalance[param1] );
					Call_Finish ();
				}
				
				case 1: {
					static char itemId[16];
					menu.GetItem ( param2, itemId, sizeof ( itemId ) );
					
					useItem ( param1, StringToInt ( itemId ) );
				}
			}
		}
	}
}

/**
 * View equipments
 */
public void storeViewMyEquipments ( int client ) {
	Menu menu = new Menu ( storeVMEHandler );
	menu.SetTitle ( "%s\n\t%s\n　", g_strConfMenuTitle, g_strConfVMETitle );
	static char item[5][64];
	getEquipItemName ( client, ITEMTYPE_CHARACTER, item[0], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_TRAIL, item[1], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_TAGTITLE, item[2], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_LASERBULLET, item[3], sizeof ( item[] ) );
	getEquipItemName ( client, ITEMTYPE_AURA, item[4], sizeof ( item[] ) );
	static char list[5][128];
	Format ( list[0], sizeof ( list[] ), "캐릭터: %s", item[0] );
	Format ( list[1], sizeof ( list[] ), "트레일: %s", item[1] );
	Format ( list[2], sizeof ( list[] ), "칭호: %s", item[2] );
	Format ( list[3], sizeof ( list[] ), "레이저 총알: %s", item[3] );
	Format ( list[4], sizeof ( list[] ), "아우라: %s", item[4] );
	for ( int i = 0; i < 5; i ++ ) {
		if ( i == ( ITEMTYPE_TAGTITLE - 1 ) ) {
			menu.AddItem ( "#TAGTITLE_ITEM", list[i], g_iConfMenuEnable[MENUTYPE_TAGTITLE] == 1 ? ( !StrEqual ( g_strEquipItemName[client][ITEMTYPE_TAGTITLE], "" ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED ) : ITEMDRAW_SPACER );
			continue;
		}
		
		menu.AddItem ( item[i], list[i], g_iConfMenuEnable[i + 1] == 1 ? ( getEquipItemName ( client, getItemType ( item[i] ) == -1 ? 0 : getItemType ( item[i] ), item[i], 63 ) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED ) : ITEMDRAW_SPACER );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int storeVMEHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			static char item[64];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int type = getItemType ( item );
			
			//	태그 아이템 일 경우
			if ( StrEqual ( item, "#TAGTITLE_ITEM" ) ) {
				moveToInventoryTagTitle ( param1 );
				return;
			}
			
			//	무료 아이템 일 경우
			if ( g_iItemPrice[getItem ( item )] <= 0 ) {
				alertRemoveItem ( param1, getItem ( item ) );
				return;
			}
			
			//	일반 아이템 일 경우
			moveToInventory ( param1, type );
			storeViewMyEquipments ( param1 );
		}
		
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack:{
					onClientCommandStore ( param1, 0 );
					return;
				}
			}
		}
	}
}

/**
 * Move to inventory.
 */
public void moveToInventory ( int client, int type ) {
	static char item[64];
	bool itemMove = false;
	for ( int i = 0; i < MAXITEMS; i ++ ) {
		if ( g_iItemType[i] == ITEMTYPE_TAGTITLE )
			continue;
		
		if ( StrEqual ( g_strEquipItemName[client][type], g_strItemName[i], false ) ) {
			item = g_strItemName[i];
			itemMove = true;
			
			g_iItem[client][i] ++;
			g_strEquipItemName[client][type] = "";
			
			switch ( type ) {
				//	캐릭터
				case ITEMTYPE_CHARACTER:	PrintToChat ( client, "%s \x01다음 라운드 부터 기본 캐릭터로 바뀝니다.", g_strConfChatPrefix );
			/*	case ITEMTYPE_CHARACTER: {
					if ( !IsPlayerAlive ( client ) )
						return;
					
					//	Work in CS:GO
					if ( GetEngineVersion () == Engine_CSGO ) {
						loringlib_SetEntityArmsModel ( client, "" );
					}
					
					if ( g_iConfZombieMod ) {
						if ( GetClientTeam ( client ) == CS_TEAM_CT )
							CS_UpdateClientModel ( client );
					} else
						CS_UpdateClientModel ( client );
				}	*/
				
				//	트레일
				case ITEMTYPE_TRAIL: removeTrailParticles ( client );
				
				//	레이저 총알
				case ITEMTYPE_LASERBULLET: {
					g_strLaserBulletData[client][LASERBULLET_MODELPATH] = "materials/sprites/laser.vmt";
					g_strLaserBulletData[client][LASERBULLET_COLOR_RED] = "0";
					g_strLaserBulletData[client][LASERBULLET_COLOR_GREEN] = "0";
					g_strLaserBulletData[client][LASERBULLET_COLOR_BLUE] = "0";
					g_strLaserBulletData[client][LASERBULLET_COLOR_ALPHA] = "0";
					g_strLaserBulletData[client][LASERBULLET_EQUIP_CHECK] = "0";
				}
				
				//	아우라
				case ITEMTYPE_AURA: removeAuraParticles ( client );
			}
			
			return;
		}
	}
	
	if ( itemMove )
		PrintToChat ( client, "%s \x01%s 아이템이 인벤토리로 옮겨졌습니다.", g_strConfChatPrefix, item );
	else
		PrintToChat ( client, "%s \x01아이템을 인벤토리로 옮기는데 문제가 생겼습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
}

/** 
 * Move to inventory (Tag Title)
 */
public void moveToInventoryTagTitle ( int client ) {
	int type = ITEMTYPE_TAGTITLE;
	
	int i = getItem ( g_strEquipItemName[client][type] );
	//	여기서 -1은 자유 칭호로 설정한 칭호 값입니다.
	if ( i == -1 ) {
		alertRemoveItem ( client, i );
		return;
	}
	
	//	무료 칭호?
	if ( g_iItemPrice[i] <= 0 ) {
		alertRemoveItem ( client, i );
		return;
	}
	
	if ( StrEqual ( g_strItemName[i], g_strEquipItemName[client][type] ) ) {
		g_iItem[client][i] ++;
		g_strEquipItemName[client][type] = "";
		storeViewMyEquipments ( client );
	}
	
	else {
		PrintToChat ( client, "%s \x01ERROR: 잘못된 아이템 데이터 값", g_strConfChatPrefix );
	}
}

/**
 * Remove item alert.
 */
public void alertRemoveItem ( int client, int item ) {
	int type;
	static char xitem[32];
	if ( item == -1 ) {
		type = ITEMTYPE_TAGTITLE;
		Format ( xitem, sizeof ( xitem ), "#TAGTITLE_ITEM" );
	}
	else {
		type = g_iItemType[item];
		IntToString ( item, xitem, sizeof ( xitem ) );
	}
	
	static char typeName[32];
	getTypeName ( type, typeName, sizeof ( typeName ) );
	Menu menu = new Menu ( alertRemoveItemHandler );
	menu.SetTitle ( "%s\n\t%s\n　", g_strConfMenuTitle, typeName );
	menu.AddItem ( "", "이 아이템은 착용 해제 시 삭제되는 아이템입니다!", ITEMDRAW_DISABLED );
	menu.AddItem ( "", "정말로 삭제할까요?\n\t\n　", ITEMDRAW_DISABLED );
	menu.AddItem ( xitem, "Yes" );
	menu.AddItem ( "", "Back" );
	menu.ExitButton = false;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int alertRemoveItemHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_Select: {
			switch ( param2 ) {
				//	Choose "Yes"
				case 2: {
					static char item[32];
					menu.GetItem ( param2, item, sizeof ( item ) );
					if ( StrEqual ( item, "#TAGTITLE_ITEM" ) ) {
						g_strEquipItemName[param1][ITEMTYPE_TAGTITLE] = "";
						PrintToChat ( param1, "%s \x01삭제되었습니다.", g_strConfChatPrefix );
						storeViewMyEquipments ( param1 );
						return;
					}
					
					int i = StringToInt ( item );
					g_strEquipItemName[param1][g_iItemType[i]] = "";
					PrintToChat ( param1, "%s \x01삭제되었습니다.", g_strConfChatPrefix );
					
					switch ( g_iItemType[i] ) {
						//	캐릭터
						case ITEMTYPE_CHARACTER: {
							if ( !IsPlayerAlive ( param1 ) )
								return;
							
							//	Work in CS:GO
							if ( GetEngineVersion () == Engine_CSGO ) {
								loringlib_SetEntityArmsModel ( param1, "" );
							}
							
							if ( g_iConfZombieMod ) {
								if ( GetClientTeam ( param1 ) == CS_TEAM_CT )
									CS_UpdateClientModel ( param1 );
							} else
								CS_UpdateClientModel ( param1 );
						}
						
						//	트레일
						case ITEMTYPE_TRAIL: removeTrailParticles ( param1 );
						
						//	레이저 총알
						case ITEMTYPE_LASERBULLET: {
							g_strLaserBulletData[param1][LASERBULLET_MODELPATH] = "materials/sprites/laser.vmt";
							g_strLaserBulletData[param1][LASERBULLET_COLOR_RED] = "0";
							g_strLaserBulletData[param1][LASERBULLET_COLOR_GREEN] = "0";
							g_strLaserBulletData[param1][LASERBULLET_COLOR_BLUE] = "0";
							g_strLaserBulletData[param1][LASERBULLET_COLOR_ALPHA] = "0";
							g_strLaserBulletData[param1][LASERBULLET_EQUIP_CHECK] = "0";
						}
				
						//	아우라
						case ITEMTYPE_AURA: removeAuraParticles ( param1 );
					}
					
					storeViewMyEquipments ( param1 );
				}
				
				//	Choose "Back"
				case 3: {
					storeViewMyEquipments ( param1 );
				}
			}
		}
	}
}

void listOfItemPreview ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	Menu menu = new Menu ( previewMenuHandler );
	
	static char item[16];
	static char format[256];
	menu.SetTitle ( "%s\n\tPreview\n　", g_strConfMenuTitle );
	for ( int i = 0; i < MAXSTORELEVEL; i ++ ) {
		for ( int x = 0; x < g_iDefineStoreItem; x ++ ) {
			if ( g_iItemRarity[x] == ITEMRARE_ADMIN )
				continue;
			
			if ( g_iItemType[x] != ITEMTYPE_CHARACTER )
				continue;
			
			if ( g_iItemClassLevel[x] != i )
				continue;
			
			if ( g_iItemNotSell[i] == 1 )
				continue;
				
			Format ( item, sizeof ( item ), "%d", x );
			Format ( format, sizeof ( format ), "%s | %s (%s)", g_strItemName[x], g_strConfItemRarity[g_iItemRarity[x]], g_strConfClassLevel[g_iItemClassLevel[x]] );
			menu.AddItem ( item, format );
		}
	}
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int previewMenuHandler ( Menu menu, MenuAction action, int param1, int param2 ) {
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			
			if ( g_iItemType[itemId] != ITEMTYPE_CHARACTER )
				return;
			
			createPreviewEntity ( param1, itemId );
			listOfItemPreview ( param1 );
		}
		case MenuAction_Cancel: {
			if ( param2 == MenuCancel_ExitBack )
				listOfItems ( param1, ITEMTYPE_CHARACTER );
		}
	}
}