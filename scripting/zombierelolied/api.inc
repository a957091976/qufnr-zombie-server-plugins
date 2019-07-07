public APLRes AskPluginLoad2 ( Handle myself, bool late, char[] err, int errmax ) {
	CreateNative ( "ZR_IsClientZombie", Native_IsClientZombie );
	CreateNative ( "ZR_IsClientMotherZombie", Native_IsClientMotherZombie );
	CreateNative ( "ZR_IsClientHuman", Native_IsClientHuman );
	CreateNative ( "ZR_IsClientSafeHuman", Native_IsClientSafeHuman );
	CreateNative ( "ZR_GetClientZombieType", Native_GetClientZombieType );
	CreateNative ( "ZR_GetClientZombieClass", Native_GetClientZombieClass );
	CreateNative ( "ZR_GetClientKnockbackScale", Native_GetClientKnockbackScale );
	CreateNative ( "ZR_SetClientKnockbackScale", Native_SetClientKnockbackScale );
	CreateNative ( "ZR_IsGameStart", Native_IsGameStart );
	CreateNative ( "ZR_IsGameEnd", Native_IsGameEnd );
	CreateNative ( "ZR_IsGameWait", Native_IsGameWait );
	CreateNative ( "ZR_IsNemesisRound", Native_IsNemesisRound );
	CreateNative ( "ZR_IsClientNemesis", Native_IsClientNemesis );
	CreateNative ( "ZR_SetGameCheckOptions", Native_SetGameCheckOptions );
	
	//	포워드 함수 생성
	g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_ZOMBIE] = CreateGlobalForward ( "ZR_OnChangeZombie", ET_Ignore, Param_Cell, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_MOTHER_ZOMBIE] = CreateGlobalForward ( "ZR_OnChangeMotherZombie", ET_Ignore, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_HUMAN] = CreateGlobalForward ( "ZR_OnChangeHuman", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_ZOMBIE_DEFINE_TICKS] = CreateGlobalForward ( "ZR_OnZombieInfectStartTicks", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_ZOMBIE_INFECT_STARTED] = CreateGlobalForward ( "ZR_OnZombieInfectStarted", ET_Ignore );
	g_hForwardHandlers[ZR_FORWARD_ON_CHOOSE_NEMESIS] = CreateGlobalForward ( "ZR_OnChooseNemesis", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_CHANGE_NEMESIS] = CreateGlobalForward ( "ZR_OnChangeNemesis", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_CLIENT_ZTELE_STARTED] = CreateGlobalForward ( "ZR_OnClientZteleStarted", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_CLIENT_ZTELE_TELEPORTED] = CreateGlobalForward ( "ZR_OnClientZteleTeleported", ET_Ignore, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_LIGHT_GRENADE_EXPLODE] = CreateGlobalForward ( "ZR_OnLightGrenadeExplode", ET_Ignore, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_FREEZE_GRENADE_EXPLODE] = CreateGlobalForward ( "ZR_OnFreezeGrenadeExplode", ET_Ignore, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_THROW_LIGHT_GRENADE] = CreateGlobalForward ( "ZR_OnThrowLightGrenade", ET_Ignore, Param_Cell, Param_Cell );
	g_hForwardHandlers[ZR_FORWARD_ON_THROW_FREEZE_GRENADE] = CreateGlobalForward ( "ZR_OnThrowFreezeGrenade", ET_Ignore, Param_Cell, Param_Cell );
	
	return APLRes_Success;
}

public int Native_IsClientZombie ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bZombie[data];
}

public int Native_IsClientMotherZombie ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bZombie[data] && g_bMotherZombie[data] ? 1 : 0;
}

public int Native_IsClientHuman ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return !g_bZombie[data] && !g_bMotherZombie[data] ? 1 : 0;
}

public int Native_IsClientSafeHuman ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bSafeHuman[data];
}

public int Native_GetClientZombieType ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_iZombieType[data];
}

public int Native_GetClientZombieClass ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_iClientZombieClass[data];
}

public int Native_GetClientKnockbackScale ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int type = GetNativeCell ( 2 );
	return view_as <int> ( g_fZombieKnockbackScale[type][data] );
}


public int Native_SetClientKnockbackScale ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	int type = GetNativeCell ( 2 );
	float amount = GetNativeCell ( 3 );
	g_fZombieKnockbackScale[type][data] = amount;
	return 1;
}

public int Native_IsGameStart ( Handle plugin, int params ) {
	return g_bGameStart;
}

public int Native_IsGameEnd ( Handle plugin, int params ) {
	return g_bGameEnd;
}

public int Native_IsGameWait ( Handle plugin, int params ) {
	return g_bGameWaitPlayer;
}

public int Native_IsClientNemesis ( Handle plugin, int params ) {
	int data = GetNativeCell ( 1 );
	return g_bNemesis[data];
}

public int Native_IsNemesisRound ( Handle plugin, int params ) {
	return g_bNemesisRound;
}

public int Native_SetGameCheckOptions ( Handle plugin, int params ) {
	if ( g_bGameWaitPlayer )
		return 0;

	int option = GetNativeCell ( 1 );
	bool optionValue = GetNativeCell ( 2 );
	
	switch ( option ) {
		case 0:		g_bGameStart = optionValue;
		case 1:		g_bGameEnd = optionValue;
		case 2:		g_bNemesisRound = optionValue;
	}
	
	return 1;
}