public APLRes AskPluginLoad2 ( Handle myself, bool late, char[] err, int errmax ) {
	CreateNative ( "SourceStore_GetMoneyName", Native_GetMoneyName );
	CreateNative ( "SourceStore_GetClientBalance", Native_GetClientBalance );
	CreateNative ( "SourceStore_SetClientBalance", Native_SetClientBalance );
	CreateNative ( "SourceStore_AddClientBalance", Native_AddClientBalance );
	CreateNative ( "SourceStore_SubClientBalance", Native_SubClientBalance );
	CreateNative ( "SourceStore_GetClientTagTitle", Native_GetClientTagTitle );
	CreateNative ( "SourceStore_GetTagTitleColor", Native_GetTagTitleColor );
	CreateNative ( "SourceStore_IsClientUsingTag", Native_IsClientUsingTag );
	CreateNative ( "SourceStore_GetMaxStoreLevel", Native_GetMaxStoreLevel );
	CreateNative ( "SourceStore_IsClientStoreMVP", Native_IsClientStoreMVP );
	CreateNative ( "SourceStore_IsClientStoreMVPPlus", Native_IsClientStoreMVPPlus );
	CreateNative ( "SourceStore_AddPlayerStoreItem", Native_AddPlayerStoreItem );
	CreateNative ( "SourceStore_SetClientStoreMVP", Native_SetClientStoreMVP );
	
	g_hForwardHandlerArray[FORWARD_ON_BUY_ITEM] = CreateGlobalForward ( "SourceStore_OnBuyItem", ET_Ignore, Param_Cell, Param_Cell, Param_String, Param_String, Param_Cell, Param_Cell );
	g_hForwardHandlerArray[FORWARD_ON_DROP_GASYA_ITEM] = CreateGlobalForward ( "SourceStore_OnDropGasyaItem", ET_Ignore, Param_Cell, Param_Cell, Param_String, Param_String, Param_String, Param_String );
	g_hForwardHandlerArray[FORWARD_ON_SELL_ITEM] = CreateGlobalForward ( "SourceStore_OnSellItem", ET_Ignore, Param_Cell, Param_Cell, Param_String, Param_String, Param_Cell, Param_Cell );
	
	return APLRes_Success;
}

public int Native_GetMoneyName ( Handle plugin, int params ) {
	char[] target = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 1, target, PLATFORM_MAX_PATH );
	Format ( target, GetNativeCell ( 2 ), "%s", g_strConfMoneyName );
	SetNativeString ( 1, target, GetNativeCell ( 2 ), true );
	return 0;
}

public int Native_GetClientBalance ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	if ( !loringlib_IsValidClient ( data ) )
		return -1;
	return g_iBalance[data];
}

public int Native_SetClientBalance ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int target = GetNativeCell ( 2 );
	if ( !loringlib_IsValidClient ( data ) )
		return 0;
	g_iBalance[data] = target;
	return 1;
}

public int Native_AddClientBalance ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int target = GetNativeCell ( 2 );
	if ( !loringlib_IsValidClient ( data ) )
		return 0;
	g_iBalance[data] += target;
	return 1;
}

public int Native_SubClientBalance ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int target = GetNativeCell ( 2 );
	if ( !loringlib_IsValidClient ( data ) )
		return 0;
	g_iBalance[data] -= target;
	return 1;
}

public int Native_GetClientTagTitle ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	char[] target = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 2, target, PLATFORM_MAX_PATH );
	
	if ( StrEqual ( g_strEquipItemName[data][ITEMTYPE_TAGTITLE], "" ) ) {
		return 0;
	}
	
	else {
		Format ( target, GetNativeCell ( 3 ), g_strEquipItemName[data][ITEMTYPE_TAGTITLE] );
		SetNativeString ( 2, target, GetNativeCell ( 3 ), true );
		return 1;
	}
}

public int Native_GetTagTitleColor ( Handle plugin, int params ) {
	char[] name = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 1, name, PLATFORM_MAX_PATH );
	
	char[] target = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 2, target, PLATFORM_MAX_PATH );
	
	getTagTitleColor ( name, target, GetNativeCell ( 3 ) );
	SetNativeString ( 2, target, GetNativeCell ( 3 ), true );
	return 1;
}

public int Native_GetMaxStoreLevel ( Handle plugin, int params ) {
	return MAXSTORELEVEL;
}

public int Native_IsClientUsingTag ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bUseTag[data];
}

public int Native_IsClientStoreMVP ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bIsMvp[data];
}

public int Native_IsClientStoreMVPPlus ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bIsMvpPlus[data];
}

public int Native_AddPlayerStoreItem ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int item_id = GetNativeCell ( 2 );
	g_iItem[data][item_id] += GetNativeCell ( 3 );
	return g_iItem[data][item_id];
}

public int Native_SetClientStoreMVP ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int type = GetNativeCell ( 2 );
	bool setvalue = GetNativeCell ( 3 );
	if ( type == 1 ) {
		g_bIsMvp[data] = setvalue;
		g_bIsMvpPlus[data] = false;
	}
	
	else if ( type == 2 ) {
		g_bIsMvp[data] = setvalue;
		g_bIsMvpPlus[data] = setvalue;
	}
	
	else
		return -1;
	
	return data;
}