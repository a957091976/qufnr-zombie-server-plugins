/**
 * Server Start to Load store items.
 */
public void serverStartToDefineItems () {
	clearStoreItem ();
	CreateTimer ( 0.1, timerDefineStoreItemPost );
}

/**
 * Map Start to Load store items.
 */
public void mapStartToDefineItems () {
	clearStoreItem ();
	CreateTimer ( 0.1, timerDefineStoreItemPost );
}

public Action timerDefineStoreItemPost ( Handle timer ) {
	defineStoreItem ( false );
}

/**
 * Load store items.
 */
public void defineStoreItem ( bool debugmode ) {
	bool success = false;
	static char temp[PLATFORM_MAX_PATH];
	BuildPath ( Path_SM, temp, PLATFORM_MAX_PATH, "data/sourcestore/item.txt" );
	
	KeyValues kv = new KeyValues ( "item" );
	if ( kv.ImportFromFile ( temp ) ) {
		success = true;
		g_iDefineStoreItem = 0;
		kv.GotoFirstSubKey ();
		do {
			kv.GetSectionName ( g_strItemName[g_iDefineStoreItem], sizeof ( g_strItemName ) );
			kv.GetString ( "item_path", g_strItemModel[g_iDefineStoreItem], sizeof ( g_strItemModel ) );
			g_iItemType[g_iDefineStoreItem] = kv.GetNum ( "item_type" );
			if ( g_iItemType[g_iDefineStoreItem] == ITEMTYPE_CHARACTER ||
				g_iItemType[g_iDefineStoreItem] == ITEMTYPE_AURA ||
				g_iItemType[g_iDefineStoreItem] == ITEMTYPE_TRAIL ) {
				kv.GetString ( "item_arms_path", g_strItemArmsModel[g_iDefineStoreItem], sizeof ( g_strItemArmsModel ) );
				
				if ( !StrEqual ( g_strItemArmsModel[g_iDefineStoreItem], "" ) )	{	//	"Bad String: " protect.
					if ( g_iItemType[g_iDefineStoreItem] == ITEMTYPE_CHARACTER )
						PrecacheModel ( g_strItemArmsModel[g_iDefineStoreItem], true );
				}
			}
			g_iItemPrice[g_iDefineStoreItem] = kv.GetNum ( "item_price" );
			g_iItemRarity[g_iDefineStoreItem] = kv.GetNum ( "item_rarity" );
			g_iItemClassLevel[g_iDefineStoreItem] = kv.GetNum ( "item_classlevel" );
			if ( g_iItemType[g_iDefineStoreItem] == ITEMTYPE_LASERBULLET ) {
				g_iItemColor[g_iDefineStoreItem][0] = kv.GetNum ( "item_color_red" );
				g_iItemColor[g_iDefineStoreItem][1] = kv.GetNum ( "item_color_green" );
				g_iItemColor[g_iDefineStoreItem][2] = kv.GetNum ( "item_color_blue" );
				g_iItemColor[g_iDefineStoreItem][3] = kv.GetNum ( "item_color_alpha" );
			}
			g_iItemOption[g_iDefineStoreItem] = kv.GetNum ( "item_option" );
			g_iItemRandomDropType[g_iDefineStoreItem] = kv.GetNum ( "item_random_droptype" );
			g_iItemNotSell[g_iDefineStoreItem] = kv.GetNum ( "item_not_sell" );
			kv.GetString ( "item_admin_skin_steamid", g_strItemAdmSkinSteamID[g_iDefineStoreItem], sizeof ( g_strItemAdmSkinSteamID[] ) );
			
			//	is item using info particle system?
			if ( g_iItemType[g_iDefineStoreItem] == ITEMTYPE_AURA ||
				g_iItemType[g_iDefineStoreItem] == ITEMTYPE_TRAIL ) {
				loringlib_PrecacheEffect ( "ParticleEffect" );
				AddFileToDownloadsTable ( g_strItemModel[g_iDefineStoreItem] );
				PrecacheGeneric ( g_strItemModel[g_iDefineStoreItem], true );
				if ( g_strItemArmsModel[g_iDefineStoreItem][0] != EOS )
					loringlib_PrecacheParticleName ( g_strItemArmsModel[g_iDefineStoreItem] );
			}
			
			//	태그 색상 값을 가져옵니다.
			if ( g_iItemType[g_iDefineStoreItem] == ITEMTYPE_TAGTITLE ) {
				//	만약 자유 태그일 경우엔 기본 값을 가져옵니다.
				if ( StrEqual ( g_strItemName[g_iDefineStoreItem], g_strConfFreeTagName ) )
					g_iItemTagColor[g_iDefineStoreItem] = 0;
				
				//	아닐 경우 지정된 태그 색상 값을 가져옵니다.
				else
					g_iItemTagColor[g_iDefineStoreItem] = kv.GetNum ( "item_tag_color" );
			}
			
			g_iDefineStoreItem ++;
		}
		
		while ( kv.GotoNextKey () );
		delete kv;
	}
	
	else
		delete kv;
		
	if ( debugmode ) {
		if ( success ) {
			PrintToChatAll ( "%s \x07DEBUG: \x01모든 아이템을 불러왔습니다.", g_strConfChatPrefix );
			LogMessage ( "DEBUG: All item was defined." );
		}
		
		else {
			PrintToChatAll ( "%s \x07DEBUG: \x01아이템을 불러오지 못했습니다.", g_strConfChatPrefix );
			LogMessage ( "DEBUG: Failed to define items." );
		}
	}
	
	GASYA_ItemLoadOnDefineDropItems ();
}

/**
 * Clear store items.
 */
public void clearStoreItem () {
	if ( g_iDefineStoreItem <= 0 ) {
		PrintToChatAll ( "%s \x07ERROR:\x01 삭제할 데이터가 없습니다.", g_strConfChatPrefix );
		LogMessage ( "ERROR: Nothing data to delete." );
		return;
	}
	
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		g_strItemName[i] = "";
		g_strItemModel[i] = "";
		g_strItemArmsModel[i] = "";
		g_iItemPrice[i] = 0;
		g_iItemClassLevel[i] = 0;
		g_iItemRarity[i] = 0;
		g_iItemType[i] = 0;
		g_iItemColor[i][0] = 0;
		g_iItemColor[i][1] = 0;
		g_iItemColor[i][2] = 0;
		g_iItemColor[i][3] = 0;
		g_iItemNotSell[i] = 0;
		g_iItemRandomDropType[i] = 0;
	}
	
	g_iDefineStoreItem = 0;
	PrintToChatAll ( "%s \x07DEBUG:\x01 모든 아이템 데이터를 삭제했습니다.", g_strConfChatPrefix );
	LogMessage ( "DEBUG: All item was deleted." );
}