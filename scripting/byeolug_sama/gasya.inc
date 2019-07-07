#define NORMAL_GASYA_ONE_TIME_PRICE		1000
#define NORMAL_GASYA_TEN_TIMES_PRICE	9000
#define PLATINUM_GASYA_ONE_TIME_PRICE	500
#define PLATINUM_GASYA_TEN_TIMES_PRICE	4500

#define GASYA_ITEM_KEY_VALUE_PATH	"data/zombierelolied/addons/gasya/"
#define GASYA_NORMAL_GASYA_KVFILE	"gasya_item_normal.txt"

int g_iNormalGasyaItems = 0;	/**< 노말 뽑기 아이템 갯수 */
char g_strNormalGasyaItemName[MAXITEMS][ITEM_NAME_MAX_LENGTH];	/**< 노말 뽑기 아이템 이름 */
float g_fNormalGasyaItemChance[MAXITEMS];	/**< 노말 뽑기 아이템 확률 */

/**
 * 일반 뽑기 아이템 불러오기
 */
public void defineNormalGasyaItems () {
	static char temp[PLATFORM_MAX_PATH];
	static char filePath[256];
	Format ( filePath, sizeof ( filePath ), "%s%s", GASYA_ITEM_KEY_VALUE_PATH, GASYA_NORMAL_GASYA_KVFILE );
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, filePath );
	
	KeyValues kv = new KeyValues ( GASYA_NORMAL_GASYA_KVFILE );
	if ( kv.ImportFromFile ( temp ) ) {
		g_iNormalGasyaItems = 0;
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strNormalGasyaItemName[g_iNormalGasyaItems], sizeof ( g_strNormalGasyaItemName[] ) );
			g_fNormalGasyaItemChance[g_iNormalGasyaItems] = kv.GetFloat ( "chance" );

			g_iNormalGasyaItems ++;
		}
		
		while ( kv.GotoNextKey () );
		delete kv;
		
	} else	delete kv;
}

/**
 * 아이템 명 및 확률 표시
 *
 * @param client
 */
public void menuNormalGasyaItemListMenu ( int client ) {
	Menu menu = new Menu ( MENU_HANDLE_NormalGasyaItemMenu );
	menu.SetTitle ( "%s\n\t일반 뽑기 정보:\n　", MENU_TITLE );
	
	static char info[64];
	for ( int i = 0; i < g_iNormalGasyaItems; i ++ ) {
		Format ( info, sizeof ( info ), "%s (%.2f%%)", g_strNormalGasyaItemName[i], g_fNormalGasyaItemChance[i] );
		menu.AddItem ( "", info, ITEMDRAW_DISABLED );
	}
	
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int MENU_HANDLE_NormalGasyaItemMenu ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: clientShopMenu ( param1 );
			}
		}
	}
}

/**
 * 일반 가챠 메뉴
 *
 * @param client
 * @param loop		연차 횟수
 */
public void menuNormalGasyaMenu ( int client, int loop ) {
	Menu menu = new Menu ( MENU_HANDLE_NormalGasyaMenu );
	menu.SetTitle ( "%s\n\t일반 뽑기 %d회\n　", MENU_TITLE, loop );
	static char price[2][16];
	if ( loop == 1 )
		loringlib_NumberFormat ( NORMAL_GASYA_ONE_TIME_PRICE, price[0], 15 );
	else if ( loop == 10 )
		loringlib_NumberFormat ( NORMAL_GASYA_TEN_TIMES_PRICE, price[0], 15 );
	else
		return;
	
	SourceStore_GetMoneyName ( price[1], 15 );
	static char notice[2][64];
	Format ( notice[0], 63, "가격: %s %s", price[0], price[1] );
	Format ( notice[1], 63, "출현 아이템: 노말, 레어" );
	menu.AddItem ( "", notice[0], ITEMDRAW_DISABLED );
	menu.AddItem ( "", notice[1], ITEMDRAW_DISABLED );
	static char str[16];
	IntToString ( loop, str, sizeof ( str ) );
	menu.AddItem ( str, "뽑기" );
	menu.AddItem ( "#VIEW_ITEMS",  "출현 아이템 및 확률 보기" );
	menu.ExitBackButton = true;
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int MENU_HANDLE_NormalGasyaMenu ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
	
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: {
			static char item[32];
			menu.GetItem ( param2, item, sizeof ( item ) );
			
			if ( StrEqual ( item, "#VIEW_ITEMS" ) ) {
				menuNormalGasyaItemListMenu ( param1 );
				return;
			}
			
			int loop = StringToInt ( item );
			int price;
			if ( loop == 1 )
				price = NORMAL_GASYA_ONE_TIME_PRICE;
			else if ( loop == 10 )
				price = NORMAL_GASYA_TEN_TIMES_PRICE;
			else {
				delete menu;
				return;
			}
			
			//	자금이 없을 경우
			if ( price > SourceStore_GetClientBalance ( param1 ) ) {
				CLIENT_NEED_MORE_STORE_DOLLAR ( param1 )
				delete menu;
				return;
			}
			
			SourceStore_SetClientBalance ( param1, SourceStore_GetClientBalance ( param1 ) - price );
			runNormalGasya ( param1, loop );
		}
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_ExitBack: clientShopMenu ( param1 );
			}
		}
	}
}

/**
 * 노말 뽑기 돌리기
 */
public void runNormalGasya ( int client, int loop ) {
	int[] result = new int[loop];
	
	int item = -1;
	int random;
	
	int x = 0;
	while ( x < loop ) {
		random = GetRandomInt ( 0, g_iNormalGasyaItems - 1 );
		item = getItemNum ( g_strNormalGasyaItemName[random] );
		
		//	아이템이 없을 경우
		if ( item == -1 )
			continue;
		
		//	아이템 확률에 들었을 경우
		if ( !loringlib_IsFloatProbability ( g_fNormalGasyaItemChance[random] ) )
			continue;
		
		result[x] = item;
		
		x ++;
	}
	
	//	다 뽑았을 경우
	Menu menu = new Menu ( MENU_HANDLE_ResultOfGasya );
	menu.SetTitle ( "%s\n\t일반 뽑기 결과:\n　", MENU_TITLE );
	static char info[3][64];
	
	PrintToChat ( client, " " );
	PrintToChat ( client, "------- \x0E뽑기 결과\x01 -------" );
	for ( int i = 0; i < loop; i ++ ) {
		getItemName ( result[i], info[0], sizeof ( info[] ) );
		getItemRarityName ( result[i], info[1], sizeof ( info[] ) );
		Format ( info[2], sizeof ( info[] ), "%s [%s]", info[0], info[1] );
		PrintToChat ( client, "%d. %s", i + 1, info[2] );
		menu.AddItem ( "", info[2], ITEMDRAW_DISABLED );
		
		g_iItem[result[i]][client] ++;
	}
	PrintToChat ( client, " " );
	
	menu.Display ( client, MENU_TIME_FOREVER );
}

public int MENU_HANDLE_ResultOfGasya ( Menu menu, MenuAction action, int param1, int param2 ) {
	if ( !loringlib_IsValidClient ( param1 ) )
		delete menu;
		
	switch ( action ) {
		case MenuAction_End: delete menu;
		case MenuAction_Select: delete menu;
		case MenuAction_Cancel: {
			switch ( param2 ) {
				case MenuCancel_Exit: {
					clientShopMenu ( param1 );
					return;
				}
			}
		}
	}
}