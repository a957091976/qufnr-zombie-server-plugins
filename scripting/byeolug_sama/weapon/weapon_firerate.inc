public void onWeaponFirerateFunctionSDKHooks ( int client, bool hook ) {
	if ( hook ) {
		SDKHook ( client, SDKHook_PostThink, onWeaponFireratePostThink );
		SDKHook ( client, SDKHook_PostThinkPost, onWeaponFireratePostThinkPost );
	}
	
	else {
		SDKUnhook ( client, SDKHook_PostThink, onWeaponFireratePostThink );
		SDKUnhook ( client, SDKHook_PostThinkPost, onWeaponFireratePostThinkPost );
	}
}

public void onWeaponFirerateClearModifyNextAtt ( int client ) {
	g_fModifyNextAtt[client] = 0.0;
}

public void onWeaponFireratePostThink ( int client ) {
	int keyInput = GetClientButtons ( client );
	
	//	입력 키가 공격이 아닐 경우
	if ( !( keyInput & IN_ATTACK ) )
		return;
		
	//	클라이언트가 유효하지 않을 경우
	if ( !loringlib_IsValidClient ( client ) )
		return;
		
	//	클라이언트가 Fake Client(봇)일 경우
	if ( IsFakeClient ( client ) )
		return;
	
	int active = GetEntPropEnt ( client, Prop_Send, "m_hActiveWeapon" );
	//	클라이언트가 무기를 들고 있지 않거나, 들고 있는 무기가 유효하지 않을 경우
	if ( active == INVALID_ENT_REFERENCE )
		return;
		
	//	무기가 발사되지 않았을 경우
	if ( GetGameTime () < GetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack" ) )
		return;
		
	//	총알이 비어있거나, 공격할 수 없을 경우
	if ( GetEntProp ( active, Prop_Send, "m_iClip1" ) <= 0 )
		return;
		
	//	탄창과 탄약이 없는 무기일 경우
	if ( GetEntProp ( active, Prop_Send, "m_iPrimaryAmmoType" ) < 0 )
		return;
	
	//	피닉스 파이어레이트, 파이어 부스트 x
	//	피닉스 파이어레이트, 돌격병의 히어로 연사력, 총잡이 불릿 타임
	g_fModifyNextAtt[client] = 1.0 / ( 1.0 + clientClass_Normal_PhoenixFirerate ( client ) + clientClass_Assault_ActiveSkillHero ( client, 2 ) + getEquipmentOptionFirerateSpd ( client ) /*+ clientClass_Assault_Fireboost ( client ) */ );
	
	if ( g_fModifyNextAtt[client] == 1.0 )
		g_fModifyNextAtt[client] = 0.0;
}

public void onWeaponFireratePostThinkPost ( int client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return;

	if ( g_fModifyNextAtt[client] <= 0.0 )
		return;
		
	if ( !IsPlayerAlive ( client ) ) {
		g_fModifyNextAtt[client] = 0.0;
		return;
	}
	
	static char weapon[32];
	int active = loringlib_GetActiveWeaponName ( client, weapon, sizeof ( weapon ) );
	if ( active == INVALID_ENT_REFERENCE )
		return;
	
	float nextPrimaryAtt = GetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack" );
	
	nextPrimaryAtt -= GetGameTime ();
	//	시간이 줄어들면 다음 총알을 더 빨리 발사함.
	nextPrimaryAtt *= g_fModifyNextAtt[client];
	nextPrimaryAtt += GetGameTime ();
	
//	PrintHintText ( client, "Next Attack: %f", g_fModifyNextAtt[client] );
	
	SetEntPropFloat ( active, Prop_Send, "m_flNextPrimaryAttack", nextPrimaryAtt );
	
	g_fModifyNextAtt[client] = 0.0;
}

stock float getEquipmentOptionFirerateSpd ( int client ) {
	float result = 0.0;
	//	유물 옵션 효과
	for ( int i = 0; i < EQUIPITEM_MAX_SLOTS; i ++ )
		if ( g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_FIRERATE][client] > 0.0 )
			result += g_fEquipBonusStat[i][EQUIPMENT_BONUS_STAT_FIRERATE][client];
	return result;
}