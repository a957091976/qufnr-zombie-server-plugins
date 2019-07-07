#define SOUND_CRAFTING_SUCCESS		"items/item_store_add_to_cart.wav"

/**
 * 조합가능한 아이템 표시
 */
public void craftItemMenu ( int client ) {
	Menu menu = new Menu ( craftItemMenuHandle );
	menu.SetTitle ( "%s\n\t제작\n　", MENU_TITLE );
	menu.AddItem ( "유물", "유물" );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int craftItemMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char name[32];
			menu.GetItem ( param2, name, sizeof ( name ) );
			
			switch ( param2 ) {
				case 0:	listOfCraftItemMenu ( param1, ITEM_TYPE_EQUIPMENT, name );
			}
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					clientCommandMenu ( param1, 0 );
				}
			}
		}
	}
}

/**
 * 제작 가능 무기 목록
 */
public void listOfCraftItemMenu ( int client, int type, char[] name ) {
	Menu menu = new Menu ( listOfCraftItemMenuHandle );
	menu.SetTitle ( "%s\n\t제작: %s\n　", MENU_TITLE, name );
	static char contents[64];
	static char itemId[16];
	bool empty = true;
	for ( int i = 0; i < g_iDefineItems; i ++ ) {
		if ( g_iItemType[i] != type )
			continue;
		
		if ( StrEqual ( g_strItemCraftItems[i][0], "-1" ) &&
			StrEqual ( g_strItemCraftItems[i][1], "-1" ) &&
			StrEqual ( g_strItemCraftItems[i][2], "-1" ) &&
			StrEqual ( g_strItemCraftItems[i][3], "-1" ) )
			continue;
		
		IntToString ( i, itemId, sizeof ( itemId ) );
		Format ( contents, sizeof ( contents ), "%s | LV. %d", g_strItemName[i], g_iItemEquipLevel[i] );
		menu.AddItem ( itemId, contents, g_iLevel[client] < g_iItemEquipLevel[i] ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT );
		
		empty = false;
	}
	
	if ( empty )
		menu.AddItem ( "", "데이터가 없습니다.", ITEMDRAW_DISABLED );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int listOfCraftItemMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			craftMenu ( param1, itemId );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					craftItemMenu ( param1 );
				}
			}
		}
	}
}

/**
 * 아이템 제작 메뉴
 */
public void craftMenu ( int client, int itemId ) {
	Menu menu = new Menu ( craftMenuHandle );
	static char itemRarity[32];
	static char discription[256];
	strcopy ( discription, sizeof ( discription ), g_strItemDescription[itemId] );
	ReplaceString ( discription, sizeof ( discription ), "{newline}", "\n\t\t" );
	getItemRarityName ( itemId, itemRarity, sizeof ( itemRarity ) );
	menu.SetTitle ( "%s\n\t제작\n　\n\t\t%s (%s)\n\t설명 및 속성:\n\t\t%s\n　\n\t재료:", MENU_TITLE, g_strItemName[itemId], itemRarity, discription );
	
	int itemData[2];
	static char list[64];
	for ( int i = 0; i < 4; i ++ ) {
		if ( StrContains ( g_strItemCraftItems[itemId][i], "-1" ) != -1 )
			continue;
		
		getCraftItems ( g_strItemCraftItems[itemId][i], itemData );
		Format ( list, sizeof ( list ), "%s [%d개]%s", g_strItemName[itemData[0]], itemData[1], g_iItem[itemData[0]][client] < itemData[1] ? " X" : " V" );
		menu.AddItem ( "#EMPTY", list, ITEMDRAW_DISABLED );
	}
	static char dollarInfo[2][32];
	SourceStore_GetMoneyName ( dollarInfo[0], sizeof ( dollarInfo[] ) );
	loringlib_NumberFormat ( g_iItemPrice[itemId], dollarInfo[1], sizeof ( dollarInfo ) );
	Format ( list, sizeof ( list ), "제작 비용: %s %s\n　", dollarInfo[1], dollarInfo[0] );
	menu.AddItem ( "#EMPTY", list, ITEMDRAW_DISABLED );
	
	IntToString ( itemId, list, sizeof ( list ) );
	menu.AddItem ( list, "제작" );
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int craftMenuHandle ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[16];
			menu.GetItem ( param2, item, sizeof ( item ) );
			int itemId = StringToInt ( item );
			if ( StrEqual ( item, "#EMPTY" ) )
				return;
				
			craftItem ( param1, itemId );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: {
					craftItemMenu ( param1 );
				}
			}
		}
	}
}

/**
 * 아이템 제작
 */
public void craftItem ( int client, int itemId ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;
	
	int itemData[2];
	bool a = true;
	for ( int i = 0; i < 4; i ++ ) {
		if ( StrContains ( g_strItemCraftItems[itemId][i], "-1" ) != -1 )
			continue;
		
		getCraftItems ( g_strItemCraftItems[itemId][i], itemData );
		//	재료가 부족할 경우
		if ( g_iItem[itemData[0]][client] < itemData[1] )
			a = false;
	}
	
	//	재료 중 하나가 부족할 경우
	if ( !a ) {
		craftMenu ( client, itemId );
		CLIENT_CRAFT_NEED_MORE_ITEMS ( client )
		return;
	}
	
	//	자금 여부
	if ( g_iItemPrice[itemId] > SourceStore_GetClientBalance ( client ) ) {
		craftMenu ( client, itemId );
		CLIENT_NEED_MORE_STORE_DOLLAR ( client )
		return;
	}
	
	//	재료 아이템 삭제
	for ( int i = 0; i < 4; i ++ ) {
		if ( StrContains ( g_strItemCraftItems[itemId][i], "-1" ) != -1 )
			continue;
			
		getCraftItems ( g_strItemCraftItems[itemId][i], itemData );
		g_iItem[itemData[0]][client] -= itemData[1];
	}
	
	//	자금 차감
	SourceStore_SetClientBalance ( client, g_iItemPrice[itemId] );
	
	g_iItem[itemId][client] ++;
	CLIENT_CRAFT_ITEM_CREATED_MSG ( client, g_strItemName[itemId] )
	PrecacheSound ( SOUND_CRAFTING_SUCCESS );
	EmitSoundToClient ( client, SOUND_CRAFTING_SUCCESS );
}
 
/**
 * Get of the craft items
 *
 * @param values		키벨류의 벨류 값
 * @param data			2차원 배열 0 - 아이템 아이디, 1 - 아이템 갯수
 */
stock void getCraftItems ( char[] values, int data[2] ) {
	if ( StrContains ( values, "@" ) == -1 )
		return;
		
	static char data_target[2][16];
	ExplodeString ( values, "@", data_target, sizeof ( data_target ), sizeof ( data_target[] ) );
	data[0] = StringToInt ( data_target[0] );
	data[1] = StringToInt ( data_target[1] );
}