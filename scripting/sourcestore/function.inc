/**
 * Timer post equip item.
 */
public Action timerEquipItemPost ( Handle timer, int client ) {
	equipCharModel ( client );
	equipTrail ( client );
	equipLaserBullet ( client );
	equipParticleAura ( client );
}

/**
 * Equip the laserbullet.
 */
stock bool equipLaserBullet ( int client ) {
	bool result = false;
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return result;
		
	g_strLaserBulletData[client][LASERBULLET_MODELPATH] = DEFAULT_LASERBULLET_MODEL;
	int i = getItem ( g_strEquipItemName[client][ITEMTYPE_LASERBULLET] );
//	PrintToChat ( client, "%d", i );
	if ( i == -1 || g_iItemType[i] != ITEMTYPE_LASERBULLET ) {
		g_strLaserBulletData[client][LASERBULLET_EQUIP_CHECK] = "0";
		return result;
	}

	if ( StrEqual ( g_strEquipItemName[client][ITEMTYPE_LASERBULLET], g_strItemName[i], false ) ) {
		g_strLaserBulletData[client][LASERBULLET_MODELPATH] = g_strItemModel[i];
		Format ( g_strLaserBulletData[client][LASERBULLET_COLOR_RED], 255, "%d", g_iItemColor[i][0] );
		Format ( g_strLaserBulletData[client][LASERBULLET_COLOR_GREEN], 255, "%d", g_iItemColor[i][1] );
		Format ( g_strLaserBulletData[client][LASERBULLET_COLOR_BLUE], 255, "%d", g_iItemColor[i][2] );
		Format ( g_strLaserBulletData[client][LASERBULLET_COLOR_ALPHA], 255, "%d", g_iItemColor[i][3] );
		Format ( g_strLaserBulletData[client][LASERBULLET_OPTION], 255, "%d", g_iItemOption[i] );
		g_strLaserBulletData[client][LASERBULLET_EQUIP_CHECK] = "1";
		result = true;
	}
	
	else {
		g_strLaserBulletData[client][LASERBULLET_COLOR_RED] = "0";
		g_strLaserBulletData[client][LASERBULLET_COLOR_GREEN] = "0";
		g_strLaserBulletData[client][LASERBULLET_COLOR_BLUE] = "0";
		g_strLaserBulletData[client][LASERBULLET_COLOR_ALPHA] = "0";
		g_strLaserBulletData[client][LASERBULLET_EQUIP_CHECK] = "0";
		g_strLaserBulletData[client][LASERBULLET_OPTION] = "0";
		result = false;
	}

	return result;
}

/**
 * Fire the laser beam.
 */
/*
public Action timerFireLaserBeam ( Handle timer, DataPack dp ) {
	dp.Reset ();
	int id = GetClientOfUserId ( dp.ReadCell () );
	if ( !loringlib_IsValidClient__PlayGame ( id ) )
		return Plugin_Stop;
	
	PrintToChat ( id, "fired" );
	
	float laser[3];
	laser[0] = dp.ReadFloat ();
	laser[1] = dp.ReadFloat ();
	laser[2] = dp.ReadFloat ();
	
	float pos[3];
	static char weapon[32];
	GetClientWeapon ( id, weapon, sizeof ( weapon ) );
	if ( StrContains ( weapon, "knife" ) != -1
		|| StrEqual ( weapon, "weapon_hegrenade" )
		|| StrEqual ( weapon, "weapon_smokegrenade" )
		|| StrEqual ( weapon, "weapon_flashbang" )
		|| StrEqual ( weapon, "weapon_tagrenade" )
		|| StrEqual ( weapon, "weapon_healthshot" )
		|| StrEqual ( weapon, "weapon_molotov" )
		|| StrEqual ( weapon, "weapon_incgrenade" )
		|| StrEqual ( weapon, "weapon_decoy" )
		|| StrEqual ( weapon, "weapon_c4" ) ) {
	//	PrintToChat ( id, "Test" );
		return Plugin_Stop;
	}
		
	GetClientEyePosition ( id, pos );
	pos[2] -= 15.0;
		
	int color[4];
	color[0] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_RED] );
	color[1] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_GREEN] );
	color[2] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_BLUE] );
	color[3] = StringToInt ( g_strLaserBulletData[id][LASERBULLET_COLOR_ALPHA] );
	TE_SetupBeamPoints ( pos, laser, PrecacheModel ( g_strLaserBulletData[id][LASERBULLET_MODELPATH] ), 0, 0, 0, 0.5, 0.5, 0.5, 10, 0.0, color, 0 );
	TE_SendToAll ();
	
	return Plugin_Stop;
}
*/

/**
 * Equip the character model.
 */
stock bool equipCharModel ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return false;
	
	int i = getItem ( g_strEquipItemName[client][ITEMTYPE_CHARACTER] );
	if ( i == -1 )
		return false;
	
	if ( StrEqual ( g_strEquipItemName[client][ITEMTYPE_CHARACTER], g_strItemName[i], false ) ) {
		if ( StrEqual ( g_strItemModel[i], "" ) ) {
		//	PrintToChat ( client, "%s \x01아이템 모델 경로에 문제가 있습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
			return false;
		}
		
		PrecacheModel ( g_strItemModel[i] );
		//	ZM
		if ( g_iConfZombieMod == 1 ) {
			if ( GetClientTeam ( client ) == CS_TEAM_CT ) {
				SetEntityModel ( client, g_strItemModel[i] );
				//	CS:GO
				if ( GetEngineVersion () == Engine_CSGO ) {
					loringlib_SetEntityArmsModel ( client, "" );
					if ( StrEqual ( g_strItemArmsModel[i], "" ) )
						CreateTimer ( 0.5, timerSetDefaultArms, client );
					else
						loringlib_SetEntityArmsModel ( client, g_strItemArmsModel[i] );
				}
			}
		}
		
		else {
			SetEntityModel ( client, g_strItemModel[i] );
			//	CS:GO
			if ( GetEngineVersion () == Engine_CSGO ) {
				loringlib_SetEntityArmsModel ( client, "" );
				if ( StrEqual ( g_strItemArmsModel[i], "" ) )
					CreateTimer ( 0.5, timerSetDefaultArms, client );
				else
					loringlib_SetEntityArmsModel ( client, g_strItemArmsModel[i] );
			}
		}
		
		return true;
	}
	
	else
		return false;
}

/**
 * Equip the trail sprite.
 */
stock void equipTrail ( int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return;
		
	removeTrailParticles ( client );
		
	int i = getItem ( g_strEquipItemName[client][ITEMTYPE_TRAIL] );
	if ( i == -1 )
		return;
	
	DataPack data;
	CreateDataTimer ( 0.5, timerEquipTrailPost, data, TIMER_FLAG_NO_MAPCHANGE );
	data.WriteCell ( client );
	data.WriteCell ( i );
}

public Action timerEquipTrailPost ( Handle timer, DataPack data ) {
	data.Reset ();
	
	static int client;
	static int i;
	client = data.ReadCell ();
	i = data.ReadCell ();
	if ( StrEqual ( g_strEquipItemName[client][ITEMTYPE_TRAIL], g_strItemName[i], false ) ) {
		if ( StrEqual ( g_strItemModel[i], "" ) ) {
		//	PrintToChat ( client, "%s \x01아이템 모델 경로에 문제가 있습니다. 관리자에게 문의하세요.", g_strConfChatPrefix );
			return Plugin_Stop;
		}
/*		PrecacheModel ( g_strItemModel[i] );
		if ( g_iConfZombieMod ) {
			if ( GetClientTeam ( client ) == CS_TEAM_CT ) {
				if ( g_iTrailEntity[client] != -1 )
					loringlib_RemoveSpriteTrail ( client, g_iTrailEntity[client] );
				
				loringlib_RemoveSpriteTrail ( client, g_iTrailEntity[client] );
				loringlib_SetupSpriteTrail ( client, g_iTrailEntity[client], g_strItemModel[i], { 255, 255, 255, 255 }, 1.3, 12.0, 10.0 );
			}
		}
		
		else {
			if ( g_iTrailEntity[client] != -1 )
				loringlib_RemoveSpriteTrail ( client, g_iTrailEntity[client] );
				
			loringlib_RemoveSpriteTrail ( client, g_iTrailEntity[client] );
			loringlib_SetupSpriteTrail ( client, g_iTrailEntity[client], g_strItemModel[i], { 255, 255, 255, 255 }, 1.3, 12.0, 10.0 );
		}	*/
		
		if ( StrContains ( g_strItemModel[i], "particles", false ) == -1 )
			return Plugin_Stop;
		
		float origin[3];
		GetClientAbsOrigin ( client, origin );
		origin[2] += 2.0;
		int entity = loringlib_CreateParticle ( client, 0, origin, g_strItemArmsModel[i], true, 999.0 );
		g_iTrailEntity[client] = EntIndexToEntRef ( entity );
	}
	
	return Plugin_Stop;
}

/**
 * Equip the particle aura.
 */
stock void equipParticleAura ( int client ) {
	if ( loringlib_IsValidClient__PlayGame ( client ) ) {
		removeAuraParticles ( client );
		
		int itemid = getItem ( g_strEquipItemName[client][ITEMTYPE_AURA] );
		if ( itemid == -1 )
			return;
		
		DataPack data;
		CreateDataTimer ( 0.5, timerEquipAuraPost, data, TIMER_FLAG_NO_MAPCHANGE );
		data.WriteCell ( client );
		data.WriteCell ( itemid );
	}
}

public Action timerEquipAuraPost ( Handle timer, DataPack data ) {
	data.Reset ();
	
	static int client;
	static int itemid;
	client = data.ReadCell ();
	itemid = data.ReadCell ();
	
	//	PrintToChatAll ( "Equip: %s", g_strEquipItemName[client][ITEMTYPE_AURA] );
	//	PrintToChatAll ( "Particle: %s", g_strItemModel[itemid] );
	//	PrintToChatAll ( "Particle Name: %s", g_strItemArmsModel[itemid] );
		
	if ( StrEqual ( g_strEquipItemName[client][ITEMTYPE_AURA], g_strItemName[itemid], false ) ) {
		if ( StrContains ( g_strItemModel[itemid], "particles", false ) == -1 )
			return Plugin_Stop;
		
		float origin[3];
		GetClientAbsOrigin ( client, origin );
		origin[2] += 2.0;
		int entity = loringlib_CreateParticle ( client, 0, origin, g_strItemArmsModel[itemid], true, 999.0 );
		g_iAuraEntity[client] = EntIndexToEntRef ( entity );
	}
	
	return Plugin_Stop;
}

/**
 * Remove particle aura.
 */
void removeAuraParticles ( int client ) {
	if ( g_iAuraEntity[client] == INVALID_ENT_REFERENCE )
		return;
	
	int ent = EntRefToEntIndex ( g_iAuraEntity[client] );
	g_iAuraEntity[client] = INVALID_ENT_REFERENCE;
	
	if ( ent == INVALID_ENT_REFERENCE || !IsEntParticleSystem ( ent ) )
		return;
	
	AcceptEntityInput ( ent, "Kill" );
}

/**
 * Remove trail particle.
 */
void removeTrailParticles ( int client ) {
	if ( g_iTrailEntity[client] == INVALID_ENT_REFERENCE )
		return;
	
	int ent = EntRefToEntIndex ( g_iTrailEntity[client] );
	g_iTrailEntity[client] = INVALID_ENT_REFERENCE;
	
	if ( ent == INVALID_ENT_REFERENCE || !IsEntParticleSystem ( ent ) )
		return;
	
	AcceptEntityInput ( ent, "Kill" );
}

public Action timerSetDefaultArms ( Handle timer, int client ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) )
		return Plugin_Stop;
	
	loringlib_SetEntityArmsModel ( client, DEFAULT_ARMS_MODEL );
	return Plugin_Continue;
}

stock bool isValidInventory ( int client ) {
	int x = 0;
	for ( int i = 0; i < MAXITEMS; i ++ )
		if ( g_iItem[client][i] > 0 )
			x ++;
	if ( x < g_iConfMaxInventoryItems )
		return true;
	else
		return false;
}

stock int getItem ( const char[] itemName ) {
	if ( StrEqual ( itemName, "" ) )
		return -1;
	
	for ( int i = 0; i < MAXITEMS; i ++ )
		if ( StrEqual ( g_strItemName[i], itemName ) )
			return i;
			
	return -1;
}

stock int getClientItems ( int client ) {
	int result = 0;
	for ( int i = 0; i < g_iDefineStoreItem; i ++ )
		result += g_iItem[client][i];
		
	return result;
}

stock bool isValidItem ( int id ) {
	for ( int i = 0; i < g_iDefineStoreItem; i ++ )
		if ( i == id )
			return true;
			
	return false;
}

stock int getItemTypeEx ( int item ) {
	for ( int i = 0; i < MAXITEMS; i ++ )
		if ( item == i )
			return g_iItemType[i];
			
	return -1;
}

stock int getItemType ( const char[] itemName ) {
	for ( int i = 0; i < MAXITEMS; i ++ )
		if ( !StrEqual ( g_strItemName[i], "" ) && StrEqual ( g_strItemName[i], itemName ) )
			return g_iItemType[i];
			
	return -1;
}

stock void getTypeName ( int type, char[] format, int maxlen ) {
	switch ( type ) {
		case ITEMTYPE_CHARACTER:	Format ( format, maxlen, "Character" );
		case ITEMTYPE_TRAIL:		Format ( format, maxlen, "Trail" );
		case ITEMTYPE_TAGTITLE:		Format ( format, maxlen, "Tag Title" );
		case ITEMTYPE_LASERBULLET:	Format ( format, maxlen, "Laser Bullet" );
		case ITEMTYPE_AURA:			Format ( format, maxlen, "Aura" );
		default:					Format ( format, maxlen, "#Unknown_Item_Type" );
	}
}

stock void getItemTypeName ( const char[] itemName, char[] format, int maxlen ) {
	for ( int i = 0; i < MAXITEMS; i ++ ) {
		if ( StrEqual ( g_strItemName[i], itemName ) {
			getTypeName ( g_iItemType[i], format, maxlen );
			return;
		}
	}
}

stock void getItemRareName ( int item, char[] format, int maxlen ) {
	switch ( g_iItemRarity[item] ) {
		case ITEMRARE_NONE, ITEMRARE_NORMAL, ITEMRARE_RARE, ITEMRARE_SR, ITEMRARE_SSR, ITEMRARE_EVENT: Format ( format, maxlen, g_strConfItemRarity[g_iItemRarity[item]] );
		default: Format ( format, maxlen, "#Invalid_Item_Rarity" );
	}
}

/**
 * 아이템 아이디로 아이템 타입 이름을 구합니다.
 *
 * @param item
 * @param format
 * @param maxlen
 * @param kr		한국어 채크 true일 경우 한국어, false일 경우 영어로 format에 담는다.
 */
stock void getItemEquipTypeName ( int item, char[] format, int maxlen, bool kr = false ) {
	switch ( g_iItemType[item] ) {
		case ITEMTYPE_CHARACTER:	Format ( format, maxlen, kr ? "캐릭터" : "Character" );
		case ITEMTYPE_TRAIL:		Format ( format, maxlen, kr ? "트레일" : "Trail" );
		case ITEMTYPE_LASERBULLET:	Format ( format, maxlen, kr ? "레이저 총알" : "Laser Bullet" );
		case ITEMTYPE_TAGTITLE:		Format ( format, maxlen, kr ? "태그" : "Tag Title" );
		case ITEMTYPE_AURA:			Format ( format, maxlen, kr ? "아우라" : "Aura" );
		default:		Format ( format, maxlen, "#Unknown_Item_Type" );
	}
}

stock void getItemClassLevelName ( int item, char[] format, int maxlen ) {
	switch ( g_iItemClassLevel[item] ) {
		case STORELEVEL_NORMAL, 
		STORELEVEL_BRONZE, 
		STORELEVEL_SILVER, 
		STORELEVEL_GOLD, 
		STORELEVEL_DIAMOND,
		STORELEVEL_LEGENDARY,
		STORELEVEL_LEGENDARY_MASTER,
		STORELEVEL_SUPREME,
		STORELEVEL_GLOBAL_ELITE: Format ( format, maxlen, g_strConfClassLevel[g_iItemClassLevel[item]] );
		default: Format ( format, maxlen, "#Invalid_Item_StoreLevel" );
	}
}

stock bool getEquipItemName ( int client, int type, char[] format, int maxlen ) {
	if ( StrEqual ( g_strEquipItemName[client][type], "" ) ) {
		Format ( format, maxlen, ITEM_UNNAMED );
		return false;
	} else {
		Format ( format, maxlen, g_strEquipItemName[client][type] );
		return true;
	}
}

stock void getTagTitleColor ( char[] tagName, char[] buffer, int maxlen ) {
	for ( int i = 0; i < g_iDefineStoreItem; i ++ ) {
		if ( g_iItemType[i] == ITEMTYPE_TAGTITLE ) {
			if ( StrEqual ( g_strItemName[i], tagName ) ) {
				if ( g_iItemTagColor[i] >= 0 && g_iItemTagColor[i] < 16 ) {
					strcopy ( buffer, maxlen, g_strItemTagColorTemp[g_iItemTagColor[i]][0] );
				}
				//	만약 색상값이 잘못되었다면 기본 색상으로 대처한다.
				else
					strcopy ( buffer, maxlen, g_strItemTagColorTemp[0][0] );
			}
		}
	}
}

/**
 * Clear Preview variable
 */
public void clearPreviewVariables ( int client ) {
	g_bUsePreview[client] = false;
}

/**
 * Preview entity spawn on client eye position.
 */
stock void createPreviewEntity ( int client, int itemId ) {
	if ( !loringlib_IsValidClient__PlayGame ( client ) ) {
		PrintToChat ( client, "%s \x01사망한 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
		return;
	}
	
	if ( !( GetEntityFlags ( client ) & FL_ONGROUND ) ) {
		PrintToChat ( client, "%s \x01점프 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
		return;
	}
	
	if ( ( GetEntityFlags ( client ) & FL_DUCKING ) ) {
		PrintToChat ( client, "%s \x01앉은 상태에서는 사용할 수 없습니다.", g_strConfChatPrefix );
		return;
	}
	
	if ( g_iItemType[itemId] != ITEMTYPE_CHARACTER ) {
		PrintToChat ( client, "%s \x01캐릭터 모델을 가져올 수 없었습니다.", g_strConfChatPrefix );
		return;
	}
	
	int entity = CreateEntityByName ( "prop_dynamic" );
	if ( entity < 0 || !IsValidEdict ( entity ) ) {
		PrintToChat ( client, "%s \x01이 엔진은 지원하지 않습니다.", g_strConfChatPrefix );
		return;
	}
	
	if ( g_bUsePreview[client] ) {
		PrintToChat ( client, "%s \x01나중에 다시 시도하세요.", g_strConfChatPrefix );
		return;
	}
	
	g_bUsePreview[client] = true;
	PrintToChat ( client, "%s \x01귀하의 위치에 %s 캐릭터 Preview가 생성되었습니다.", g_strConfChatPrefix, g_strItemName[itemId] );
	
	DispatchKeyValue ( entity, "model", g_strItemModel[itemId] );
//	loringlib_SetEntityNoblock ( entity );

	SetEntityMoveType ( entity, MOVETYPE_NONE );
	AcceptEntityInput ( entity, "DisableCollision" );
	
	DispatchSpawn ( entity );
//	ActivateEntity ( entity );
	
	float pos[3], ang[3];
	GetClientEyeAngles ( client, ang );
	GetClientAbsOrigin ( client, pos );
	ang[0] = 0.0;
	ang[2] = 0.0;
	ang[1] = ang[1] - 180.0;
	pos[2] += 22.0;
	TeleportEntity ( entity, pos, ang, NULL_VECTOR );
	loringlib_SetEntityOwner2 ( entity, client );
	
//	GetEntPropVector ( entity, Prop_Send, "m_vecOrigin", pos );
//	pos[2] -= 10.0;
//	ang[1] = ang[1] + 180.0;
//	int color[4] = { 0, 0, 255, 255 };
//	int worldtext = loringlib_ShowPointWorldText ( client, pos, ang, 16, color, g_fConfPreviewDisplayTime, "PREVIEW" );
	
	SDKHook ( entity, SDKHook_SetTransmit, viewPreviewModel );
//	SDKHook ( worldtext, SDKHook_SetTransmit, viewPreviewModel );
	
	CreateTimer ( g_fConfPreviewDisplayTime, timerKillPreviewModel, entity );
	CreateTimer ( g_fConfPreviewDisplayTime, timerResetPreviewChecker, client );
}

public Action viewPreviewModel ( int target, int entities ) {
	if ( loringlib_GetEntityOwner2 ( target ) == entities )
		return Plugin_Continue;
		
	return Plugin_Handled;
}

public Action timerKillPreviewModel ( Handle timer, any entity ) {
	if ( entity < 0 || !IsValidEdict ( entity ) )
		return Plugin_Stop;
	
	AcceptEntityInput ( entity, "Kill" );
	return Plugin_Stop;
}

public Action timerResetPreviewChecker ( Handle timer, any client ) {
	if ( !loringlib_IsValidClient ( client ) )
		return Plugin_Stop;
	
	g_bUsePreview[client] = false;
	return Plugin_Stop;
}

stock bool IsValidEquipItemName ( int itemType, char[] itemName ) {
	if ( itemName[0] == EOS )
		return true;

	for ( int i = 0; i < g_iDefineStoreItem; i ++ )
		if ( g_iItemType[i] == itemType )
			if ( StrEqual ( g_strItemName[i], itemName ) )
				return true;
	return false;
}

stock bool IsEntParticleSystem ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		char buffer[256];
		GetEdictClassname ( entity, buffer, sizeof ( buffer ) );
		return StrEqual ( buffer, "info_particle_system", false );
	}
	
	return false;
}