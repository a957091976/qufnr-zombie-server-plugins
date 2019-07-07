public APLRes AskPluginLoad2 ( Handle myself, bool late, char[] err, int err_max ) {
	CreateNative ( "ZD_GetClientHumanClassNum", Native_GetClientHumanClassNum );
	CreateNative ( "ZD_GetClientHumanClassName", Native_GetClientHumanClassName );
	CreateNative ( "ZD_GetClientLevel", Native_GetClientLevel );
	CreateNative ( "ZD_GetClientExp", Native_GetClientExp );
	CreateNative ( "ZD_GetClientMaxExp", Native_GetClientMaxExp );
	CreateNative ( "ZD_GetClientRank", Native_GetClientRank );
	CreateNative ( "ZD_GetClientRankName", Native_GetClientRankName );
	
	g_hForwardHandlers[ZD_FORWARD_ON_CHANGE_LEVEL] = CreateGlobalForward ( "ZD_OnChangeLevel", ET_Ignore, Param_Cell, Param_Cell, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZD_FORWARD_ON_BUY_ITEM] = CreateGlobalForward ( "ZD_OnBuyItem", ET_Ignore, Param_Cell, Param_String );
}

public int Native_GetClientHumanClassNum ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return view_as<int> ( g_iClientClass[data] );
}

public int Native_GetClientHumanClassName ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	char[] format = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 2, format, PLATFORM_MAX_PATH );
	
	int result = getClientClassName ( g_iClientClass[data], format, GetNativeCell ( 3 ), GetNativeCell ( 4 ), GetNativeCell ( 5 ) );
	
	SetNativeString ( 2, format, GetNativeCell ( 3 ), true );
	return result;
}

public int Native_GetClientLevel ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_iLevel[data];
}

public int Native_GetClientExp ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_iExp[data];
}

public int Native_GetClientMaxExp ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return getClientMaxExp ( data );
}

public int Native_GetClientRank ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_iRankLevel[data];
}

public int Native_GetClientRankName ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	
	char[] target = new char[PLATFORM_MAX_PATH];
	GetNativeString ( 2, target, PLATFORM_MAX_PATH );
	
	Format ( target, GetNativeCell ( 3 ), "%s", g_strRankName[g_iRankLevel[data]] );
	SetNativeString ( 2, target, GetNativeCell ( 3 ), true );
	return 1;
}