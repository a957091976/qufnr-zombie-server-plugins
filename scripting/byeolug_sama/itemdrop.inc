#define ITEMDROP_ITEM_GET_MESSAGE(%1,%2,%3)	PrintToChat ( %1, " \x04[Item Drop] %s%s\x01 아이템을 획득했습니다!", %3, g_strItemName[%2] );

#define ITEMDROP_PICKUP_DROPPED_ITEM_DIST	100.0		/**< 아이템을 사용키로 주울 수 있는 거리 */

#define DROP_ITEM_MODEL			"models/props/de_nuke/hr_nuke/metal_pipe_001/metal_pipe_001_section_short.mdl"

int g_iDropItemAmounts[ITEM_RARITY_LEGENDARY + 1] = { 0, ... };	/**< 그 희귀도에 드랍되는 아이템 갯수 값이다. */
int g_iDropItems[ITEM_RARITY_LEGENDARY + 1][MAXITEMS + 1];		/**< 그 희귀도에 드랍되는 아이템 아이디의 값이다. */

static int g_iItemDroppedIndex[SOURCE_MAXENTITIES] = { -1, ... };

/**
 * 아이템 데이터 베이스를 다 불러오고 드랍되는 아이템 목록을 찾는다.
 */
void ItemLoadOnDefineDropItems () {
	for ( int i = 0; i < 5; i ++ )
		g_iDropItemAmounts[i] = 0;

	for ( int rarity = 0; rarity < 5; rarity ++ ) {
		PrintToServer ( "## -- Rarity: %d", rarity );
		for ( int item = 0; item < g_iDefineItems; item ++ ) {
			if ( g_iItemRarity[item] == rarity && ( g_iItemDrop[item] >= 1 && g_iItemDrop[item] <= 3 ) ) {
				g_iDropItems[rarity][g_iDropItemAmounts[rarity]] = item;
				PrintToServer ( "# IDX: %d | ITEM: %d | ITEM NAME: %s", g_iDropItemAmounts[rarity], g_iDropItems[rarity][g_iDropItemAmounts[rarity]], g_strItemName[item] );
				g_iDropItemAmounts[rarity] ++;
			}
		}
	}
}

/**
 * 라운드 시작 또는 맵 시작마다 드랍 아이템 인덱스 값을 초기화한다.
 */
public void ITEMDROP_OnClearDroppedItemIndex () {
	for ( int i = 0; i < sizeof ( g_iItemDroppedIndex ); i ++ )
		if ( g_iItemDroppedIndex[i] != -1 )
			g_iItemDroppedIndex[i] = -1;
}

/**
 * 드랍 아이템 모델 프리캐싱
 */
public void ITEMDROP_PrecacheDropItemModel () {
	PrecacheModel ( DROP_ITEM_MODEL, true );
}

/*
//	일반 드랍
static int g_iDropItemList[5] = {
	0, 1, 2, 3, 4
};

//	해드샷 드랍
static int g_iDropItemList_Headshot[2] = {
	3, 32
};

//	숙주 해드샷 드랍
static int g_iDropItemList_MotherHeadshot[3] = {
	3, 32, 33
};

//	숙주 좀비 드랍 (모든 클래스 포함)
static int g_iDropItemList_MotherZombie[2] = {
	3, 4
};

//	깃털 좀비 드랍
static int g_iDropItemList_FastZombie[1] = {
	28
};

//	숙주 깃털 좀비 드랍
static int g_iDropItemList_MotherFastZombie[2] = {
	28, 29
};

//	무거운 좀비 드랍
static int g_iDropItemList_HeavyZombie[1] = {
	30
};

//	숙주 무거운 좀비 드랍
static int g_iDropItemList_MotherHeavyZombie[2] = {
	30, 31
};	*/

public void ITEMDROP_OnZombieKillNormalItemDrop ( int victim, int attacker, bool headshot ) {
	if ( !loringlib_IsValidClient ( victim ) ||
		!loringlib_IsValidClient ( attacker ) )
		return;
	
	if ( ZR_IsClientHuman ( attacker ) && !ZR_IsClientHuman ( victim ) )
		ItemDrop_AddDropItemToClient ( attacker, victim, 5 );
}

/**
 * 아이템 드랍 init
 *
 * @param client
 * @param victim
 * @param amounts		드랍 갯수
 */
stock void ItemDrop_AddDropItemToClient ( int client, int victim, int amounts = 1 ) {
	if ( loringlib_IsValidClient ( client ) && loringlib_IsValidClient ( victim ) ) {
		static float spawnOrigin[3];
		loringlib_GetClientOriginEx ( victim, spawnOrigin );
	//	spawnOrigin[2] += 5.0;
		
		int loop = 0;
		int findValue = 0, item = -1, rarity = -1;
		while ( loop < amounts ) {
			rarity = ItemDrop_GetDropItemRarity ();
			
			if ( rarity == -1 ) {
				loop ++;
				continue;
			}
			
			findValue = GetRandomInt ( 0, g_iDropItemAmounts[rarity] );
			item = g_iDropItems[rarity][findValue];
			
			if ( ( item < 0 || item > g_iDefineItems ) ||
				!loringlib_IsValidClient ( victim ) ) {
				LogError ( "Invalid Drop Data: Target %d | Item Number %d", victim, item );
				return;
			}
			
			if ( item >= 0 &&
				g_iItemRarity[item] == rarity &&
				ItemDrop_IsDrop ( victim, item ) ) {
				ItemDrop_CreateDropItem ( spawnOrigin, item );
			//	g_iItem[item][client] ++;
				
				loop ++;
			}
		}
	}
}

public void ItemDrop_SpawnOnSetModel ( int entity ) {
	if ( IsValidEdict ( entity ) ) {
		DispatchKeyValue ( entity, "model", DROP_ITEM_MODEL );
		SetEntityModel ( entity, DROP_ITEM_MODEL );
		
		loringlib_SetEntityRenderColor ( entity, 255, 255, 255, 0 );
	}
}

/**
 * 드랍된 아이템을 만든다.
 */
public void ItemDrop_CreateDropItem ( float pos[3], int item ) {
//	int entity = CreateEntityByName ( "prop_physics_override" );
	int entity = CreateEntityByName ( "tagrenade_projectile" );
	if ( entity == -1 )
		return;
	
	DispatchKeyValue ( entity, "spawnflags", "256" );
	DispatchKeyValue ( entity, "disableshadows", "1" );
	DispatchKeyValue ( entity, "model", DROP_ITEM_MODEL );
	
	SetEntProp ( entity, Prop_Send, "m_usSolidFlags", 12 );
	SetEntProp ( entity, Prop_Data, "m_nSolidType", 6 );
	SetEntProp ( entity, Prop_Send, "m_CollisionGroup", 1 );
	
	DispatchSpawn ( entity );
	
	static float vectors[3];
	static float angles[3];
	vectors[0] = GetRandomFloat ( -179.0, 179.0 );
	vectors[1] = GetRandomFloat ( -179.0, 179.0 );
	vectors[2] = GetRandomFloat ( 200.0, 380.0 );	//	위로 튀어 오르게 한다.
	angles[0] = -89.0;	//	위를 보게 한다.
	pos[2] += 10.0;		//	땅에 안낑기게
	TeleportEntity ( entity, pos, angles, vectors );
//	SetEntityMoveType ( entity, MOVETYPE_NONE );
	
	g_iItemDroppedIndex[entity] = item;	//	아이템 값을 대입
	
//	static int clr[3];
//	Item_GetItemRarityClrRGB ( item, clr );
//	loringlib_SetEntityRenderColor ( entity, clr[0], clr[1], clr[2], 255 );
	loringlib_SetEntityRenderColor ( entity, 255, 255, 255, 0 );
	
	static float origin[3];	static char eff[64];
	loringlib_GetEntityOriginEx ( entity, origin );
	ItemDrop_GetRarityParticle ( g_iItemRarity[item], eff, sizeof ( eff ) );
	int dropEff = loringlib_CreateParticleEx ( entity, 0, origin, NULL_VECTOR, eff, true, 999.0 );
	
	//	Sound play
	static char sound[256];
	ItemDrop_GetItemPickupSound ( g_iItemRarity[item], sound, sizeof ( sound ) );
	EmitAmbientSound ( sound, origin );
	if ( g_iItemRarity[item] >= ITEM_RARITY_UNIQUE )
		EmitAmbientSound ( "qufnr/item/pickup_snd_effect.mp3", origin );
	
	SDKHook ( entity, SDKHook_Spawn, ItemDrop_SpawnOnSetModel );
	
	SDKHook ( dropEff, SDKHook_SetTransmit, ItemDrop_TransmitOnDroppedItem );
	SDKHook ( entity, SDKHook_StartTouch, ItemDrop_TouchOnDroppedItem );
//	CreateTimer ( 1.0, timerCheckDroppedItemGround, entity, TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE );
}

/** 
 * 좀비는 드랍된 아이템을 안보이게 한다.
 */
public Action ItemDrop_TransmitOnDroppedItem ( int entity, int clients ) {
	static char classname[32];
	GetEdictClassname ( entity, classname, sizeof ( classname ) );
	if ( StrEqual ( classname, "info_particle_system", false ) )
		if ( GetEdictFlags ( entity ) & FL_EDICT_ALWAYS )
			SetEdictFlags ( entity, ( GetEdictFlags ( entity ) ^ FL_EDICT_ALWAYS ) );
	
	if ( loringlib_IsValidClient ( clients ) )
		if ( ZR_IsClientHuman ( clients ) || GetClientTeam ( clients ) == CS_TEAM_SPECTATOR )
			return Plugin_Continue;
	
	return Plugin_Handled;
}

/**
 * 아이템이 어딘가에 닿으면 멈춘다.
 */
public Action ItemDrop_TouchOnDroppedItem ( int entity, int toucher ) {
	if ( IsValidEdict ( entity ) ) {
		if ( toucher == 0 )
			SetEntityMoveType ( entity, MOVETYPE_NONE );
	}
	
	return Plugin_Continue;
}

/**
 * 아이템이 땅에 떨어질 때 멈추게 한다.
 * @note 굴러 간다거나, 총으로 쏠때 밀치는걸 방지하기 위함
 */
/*
public Action timerCheckDroppedItemGround ( Handle timer, any entity ) {
	if ( IsValidEdict ( entity ) ) {
		if ( GetEdictFlags ( entity ) & FL_ONGROUND ) {
			SetEntityMoveType ( entity, MOVETYPE_NONE );
			return Plugin_Stop;
		}
	}
	
	return Plugin_Continue;
}	*/

/**
 * 드랍된 아이템을 보고 E를 눌렀을 때 먹게 한다.
 */
public void ItemDrop_UseOnGetDropItem ( int client ) {
	int target = loringlib_GetClientAimEntity ( client );
	
	if ( !IsValidEdict ( target ) ||
		g_iItemDroppedIndex[target] < 0 )
		return;
	
	if ( !ZR_IsClientHuman ( client ) )
		return;
	
	static float distance[2][3];
	GetClientAbsOrigin ( client, distance[0] );
	loringlib_GetEntityOriginEx ( target, distance[1] );
	
	//	주울 수 있는 거리안에 있을 경우?
	if ( GetVectorDistance ( distance[0], distance[1] ) <= ITEMDROP_PICKUP_DROPPED_ITEM_DIST ) {
		int item = g_iItemDroppedIndex[target];
		static char rarityColor[16];
		Item_GetItemRarityColor ( item, rarityColor, sizeof ( rarityColor ) );
		ITEMDROP_ITEM_GET_MESSAGE ( client, item, rarityColor )
		g_iItem[item][client] ++;
		
		//	Sound play
		static char sound[256];
		ItemDrop_GetItemPickupSound ( g_iItemRarity[item], sound, sizeof ( sound ) );
		static float soundOrigin[3];
		loringlib_GetEntityOriginEx ( target, soundOrigin );
		EmitAmbientSound ( sound, soundOrigin );
		if ( g_iItemRarity[item] >= ITEM_RARITY_UNIQUE )
			EmitAmbientSound ( "qufnr/item/pickup_snd_effect.mp3", soundOrigin );
		
		g_iItemDroppedIndex[target] = -1;
		AcceptEntityInput ( target, "kill" );
	}
}

/**
 * 드랍되는 아이템 희귀도를 구한다.
 *
 * @return 아이템 희귀도
 */
stock int ItemDrop_GetDropItemRarity () {
	static int result;
	result = -1;
	
	while ( true ) {
		if ( loringlib_IsIntegerChance ( ITEM_NORMAL_DROP_CHANCE ) ) {
			result = ITEM_RARITY_NORMAL;
			break;
		}
		else if ( loringlib_IsIntegerChance ( ITEM_RARE_DROP_CHANCE ) ) {
			result = ITEM_RARITY_RARE;
			break;
		}
		else if ( loringlib_IsIntegerChance ( ITEM_SR_DROP_CHANCE ) ) {
			result = ITEM_RARITY_SR;
			break;
		}
		else if ( loringlib_IsIntegerChance ( ITEM_SSR_DROP_CHANCE ) ) {
			result = ITEM_RARITY_SSR;
			break;
		}
		else if ( loringlib_IsIntegerChance ( ITEM_LEGEND_DROP_CHANCE ) ) {
			result = ITEM_RARITY_LEGENDARY;
			break;
		}
		else {
			result = -1;
			break;
		}
	}
	
	return result;
}

/**
 * 드랍 채크
 */
stock bool ItemDrop_IsDrop ( int victim, int itemid ) {
	if ( loringlib_IsValidClient ( victim ) ) {
		if ( ZR_IsClientZombie ( victim ) && !ZR_IsClientMotherZombie ( victim ) )
			return ( g_iItemDrop[itemid] == 1 );
		else if ( ZR_IsClientMotherZombie ( victim ) )
			return ( g_iItemDrop[itemid] >= 1 && g_iItemDrop[itemid] <= 2 );
		else if ( ZR_IsClientNemesis ( victim ) )
			return ( g_iItemDrop[itemid] >= 1 && g_iItemDrop[itemid] <= 3 );
	}
	
	return ( g_iItemDrop[itemid] == 1 );
}

/** 
 * 드랍 상자 희귀도 파티클
 */
stock void ItemDrop_GetRarityParticle ( int rarity, char[] buffer, int maxlen ) {
	switch ( rarity ) {
		case ITEM_RARITY_NORMAL:	Format ( buffer, maxlen, "shine_normal" );
		case ITEM_RARITY_RARE:		Format ( buffer, maxlen, "shine_rare" );
		case ITEM_RARITY_SR:		Format ( buffer, maxlen, "shine_hero" );
		case ITEM_RARITY_SSR:		Format ( buffer, maxlen, "shine_unique" );
		case ITEM_RARITY_LEGENDARY:		Format ( buffer, maxlen, "shine_legend" );
		default: Format ( buffer, maxlen, "shine_normal" );
	}
}

/**
 * 희귀도에 따라 픽업 사운드를 구함.
 */
stock void ItemDrop_GetItemPickupSound ( int rarity, char[] sound, int maxlen ) {
	Format ( sound, maxlen, "qufnr/item/pickup_snd_0%d.mp3", rarity );
}