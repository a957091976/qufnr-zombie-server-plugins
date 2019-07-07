char g_strClassWeapon_Default[20][32] = {
	"weapon_knife"
	, "weapon_hegrenade"
	, "weapon_flashbang"
	, "weapon_smokegrenade"
	, "weapon_tagrenade"
	, "weapon_decoy"
	, "weapon_molotov"
	, "weapon_incgrenade"
	, "weapon_healthshot"
	, "weapon_c4"
	//	Pistols
	, "weapon_glock"
	, "weapon_usp_silencer"
	, "weapon_hkp2000"
	, "weapon_p250"
	, "weapon_fiveseven"
	, "weapon_deagle"
	, "weapon_cz75a"
	, "weapon_elite"
	, "weapon_revolver"
	, "weapon_tec9"
};

/**
 * Pickup 할 수 있는 무기 배열
 * 권총은 제외됨.
 */
char g_strClassPickUpAllowWeapons[MAX_CLIENT_CLASS][10][32] = {
	{ "", "", "", "", "", "", "", "", "", "" }
	, {	"weapon_famas", "weapon_galilar", "weapon_m4a1", "weapon_m4a1_silencer", "weapon_ak47", "weapon_sg556", "weapon_aug", "", "", "" }
	, { "weapon_ssg08", "weapon_awp", "weapon_scar20", "weapon_g3sg1", "weapon_p250", "", "", "", "", "" }
	, { "weapon_ssg08", "weapon_mp5sd", "", "", "", "", "", "", "", "" }
	, { "weapon_mp9", "weapon_mp7", "weapon_mp5sd", "weapon_ump45", "weapon_p90", "weapon_mac10", "weapon_bizon", "", "", "" }
	, { "weapon_nova", "weapon_xm1014", "weapon_sawedoff", "weapon_mag7", "", "", "", "", "", "" }
	, { "weapon_glock", "weapon_hkp2000", "weapon_usp_silencer", "weapon_p250", "weapon_fiveseven", "weapon_deagle", "weapon_revolver", "weapon_elite", "weapon_cz75a", "weapon_tec9" }
};

/**
 * 돌격병 병과 무기
 */
char g_strClassWeapon_Assault[7][32] = {
	"weapon_famas"
	,	"weapon_galilar"
	,	"weapon_m4a1"
	,	"weapon_m4a1_silencer"
	,	"weapon_ak47"
	,	"weapon_sg556"
	,	"weapon_aug"
};

/**
 * 저격병 병과 무기
 */
char g_strClassWeapon_Sniper[4][32] = {
	"weapon_ssg08"
	,	"weapon_awp"
	,	"weapon_scar20"
	,	"weapon_g3sg1"
};

/**
 * 의무병 병과 무기
 */
char g_strClassWeapon_Medic[6][32] = {
	"weapon_elite"
	,	"weapon_tec9"
	,	"weapon_cz75a"
	,	"weapon_p250"
	,	"weapon_ssg08"
	,	"weapon_mp5sd"
};

/**
 * 보급병 병과 무기
 */
char g_strClassWeapon_Supporter[7][32] = {
	"weapon_mp9"
	,	"weapon_mp7"
	,	"weapon_mp5sd"
	,	"weapon_ump45"
	,	"weapon_p90"
	,	"weapon_mac10"
	,	"weapon_bizon"
};

/**
 * 전문병 병과 무기
 */
char g_strClassWeapon_Shotgunner[4][32] = {
	"weapon_nova"
	, "weapon_xm1014"
	, "weapon_mag7"
	, "weapon_sawedoff"
};

/**
 * 총잡이 병과 무기
 */
char g_strClassWeapon_Gunslinger[10][32] = {
	"weapon_glock"
	, "weapon_usp_silencer"
	, "weapon_hkp2000"
	, "weapon_p250"
	, "weapon_fiveseven"
	, "weapon_elite"
	, "weapon_deagle"
	, "weapon_revolver"
	, "weapon_tec9"
	, "weapon_cz75a"
};

/**
 * 초기화 관련
 */
#define	RESET_STATS_ALLOW_MIN_LEVEL		5
#define RESET_STATS_ALLOW_MAX_LEVEL		15
#define RESET_SKILLS_ALLOW_MIN_LEVEL	10
#define RESET_SKILLS_ALLOW_MAX_LEVEL	20
#define RESET_CLIENTCLASS_ALLOW_MIN_LEVEL	10
#define RESET_CLIENTCLASS_ALLOW_MAX_LEVEL	15
#define LEVELUP_BLOCK_LEVEL				10
#define RESET_TYPE_ALL					1
#define RESET_TYPE_STATS				2
#define RESET_TYPE_SKILLS				3
#define RESET_TYPE_CLIENTCLASS			4

/**
 * 플러그인
 */
#define KEYVALUE_PATH	"data/zombierelolied/addons/playerdb/%s.txt"
#define KEYVALUE_PATH_CL_ITEM	"data/zombierelolied/addons/client_item_db.txt"

/**
 * 점수 제도 관련
 */
#define SURVIVE_POINT_EXCHANGE_EXP_SCALE	1.2	/**< 점수를 경험치로 바꿀 때 곱하는 량 */
#define SURVIVE_POINT_CUT_LINE		20		/**< 점수 커트라인 */
#define SURVIVE_POINT_DAMAGE_SCALE	2000	/**< 점수를 받는 데미지 량 */
#define SURVIVE_POINT_GIVEN_DAMAGES	5		/**< SURVIVE_POINT_DAMAGE_SCALE 만큼 피해를 줬을 때 받는 점수 */
#define SURVIVE_POINT_USE_SKILLS	3		/**< 기술 사용 시 받는 점수 */
#define SURVIVE_POINT_ASSIST_TEAMMATE	2	/**< 팀에게 이로운 행동을 가했을 때 받는 점수 */
#define SURVIVE_POINT_MEDIC_HEALTH_HEAL	1	/**< 메딕 힐 해줄 때 받는 점수 */
#define SURVIVE_POINT_MEDIC_HEALGAS_HEAL	3	/**< 메딕 치유가스로 힐 해줄 때 받는 점수 */
#define SURVIVE_POINT_MEDIC_NANO_MACHINE_GAS_POISON_HEAL	5	/**< 나노머신가스로 독을 없앨 때 받는 점수 */
#define SURVIVE_POINT_SUPPORT_AMMOBOX					2	/**< 타 인이 보급상자를 사용 시 받는 점수 */
#define SURVIVE_POINT_SUPPORT_FREEZE_BULLET				1	/**< 얼음탄 발사 시 받는 점수 */
#define SURVIVE_POINT_MEDIC_MEDIC_BULLET_HEAL			10	/**< 치료탄으로 독을 없앨 때 받는 점수 */
#define SURVIVE_POINT_MEDIC_BOOST_TEAMMATE_DAMAGE		5	/**< 나노강화치유탄 맞은 타겟이 일정 피해만큼 쌓으면 주인이 받는 점수 */
#define	SURVIVE_POINT_SHOTGUNNER_PULSESHOT_ATTACK		1	/**< 전문병 펄스샷 사용 시 맞은 적 수 만큼 받는 점수 */

/**
 * 레벨 관련
 */
#define	MAXLEVEL			150		/**< 최대 레벨 */

/**
 * 경험치 관련
 */
#define	EXP_LEVEL1			200		/**< 레벨 1때 최대 경험치 */
#define MAXEXP_SCALE		150		/**< 레벨마다 올라가는 최대 경험치 량 */
#define	MAXEXP_SCALE_PLUS	100		/**< 추가 최대 경험치 량 */
#define DAMAGE_REWARD_CALCULATION	95.5		/**< 전체 피해량 경험치 지급 계산 */
#define MEDIC_TEAMMATE_HEAL_EXP_MIN			9		/**< 메딕 아군 치료 지급 경험치 */
#define MEDIC_TEAMMATE_HEAL_EXP_MAX			13
#define	SURVIVE_REWARD_ALLOW_PLAYERS_MIN	3		/**< 생존 보상 받을 때 필요한 플레이어 수 */
#define SURVIVE_POINT_MAX_REWARD_EXPS		200		/**< 점수로 받은 최대 경험치 량 */
#define SURVIVE_TIME_EXP_DIVISION			2.5		/**< 생존 시간 경험치 나누기 */

/**
 * 이벤트 관련
 */
#define JOIN_MODE_EVENT_ENABLE		0
#define JOIN_MODE_EVENT_SET_INDEX	JOIN_MODE_EVENT_2019_CHARACTER_SLOT_EVENT	/**< c#.sp에 있음 */
#define EXP_BOOST_SCALE				1.0		/**< 경험치 배율 */
#define EXP_NEWBIE_BOOST_SCALE		1.5		/**< 초보자 경험치 배율 */
#define LEVEL_NEWBIE				10		/**< 뉴비 레벨 커트라인 (전체 레벨로 따진다.) */

/**
 * 병과 관련
 */
#define 	SUPPORTER_BARRICADE_HEALTH_MIN	400
#define 	SUPPORTER_BARRICADE_HEALTH_MAX	500
#define		MODEL_AMMO_BOX	"models/props_fairgrounds/giraffe.mdl"	/**< 서포터 보급 상자 모델 */
#define		MODEL_ARMOR_PIERCING_AMMO_BOX		"models/props_fairgrounds/elephant.mdl"	/**< 서포터 철갑탄 상자 모델 */
#define		SOUND_AMMO_BOX_USE	"items/battery_pickup.wav"		/**< 탄창 상자 사용 시 소리 */
#define		SOUND_AMMO_BOX_EMPTY	"weapons/auto_semiauto_switch.wav"	/**< 탄창 상자 모두 사용 시 소리 */
#define		AMMO_BOX_SPAWN_DISTANCE		250.0	/**< 서포트 탄창 상자 스폰할 수 있는 거리 */
#define		AMMO_BOX_USE_DISTANCE		110.0	/**< 탄창 상자 사용할 수 있는 거리 */
#define		AMMO_BOX_USE_COOLDOWN		1.0		/**< 탄창 상자 사용 쿨타임 */
#define		AMMO_BOX_MAX_AMMO					/**< . */
#define		EXPLODE_BULLET_GAYGUN_COUNTS	10	/**< 딱딱이가 발사할 수 있는 폭발탄 수 */
#define		MEDISHIOT_NORMAL_HEAL_SCALE		20	/**< 의료주사기 체력 회복 량 */

/**
 * 보상 관련
 */
#define REWARD_ROUND_END_SURVIVE_MIN	120	/**< 생존 시 받는 경험치 량 */
#define REWARD_ROUND_END_SURVIVE_MAX	160
#define REWARD_EXP_MIN		25		/**< 적 처치 시 받는 최소 경험치 량 */
#define REWARD_EXP_MAX		30		/**< 적 처치 시 받는 최대 경험치 량 */
#define REWARD_EXP_ZOMBIE_MIN	10	/**< 좀비가 인간 처치 시 받는 최소 경험치 량 */
#define REWARD_EXP_ZOMBIE_MAX	15	/**< 좀비가 인간 처치 시 받는 최대 경험치 량 */
#define REWARD_ASSIST_EXP_MIN	20	/**< 적 처치 도움 시 받는 최소 경험치 량 */
#define REWARD_ASSIST_EXP_MAX	25	/**< 적 처치 도움 시 받는 최대 경험치 량 */
#define REWARD_PARTY_EXP_MIN	8	/**< 적 처치 시 받는 파티 최소 경험치 량 */
#define REWARD_PARTY_EXP_MAX	13	/**< 적 처치 시 받는 파티 최대 경험치 량 */
#define REWARD_LEVELUP_SP		1	/**< 레벨업 시 받는 SP 량 */
#define REWARD_LEVELUP_PLAYERPT	1	/**< 레벨업 시 받는 플레이어pt 량 */
#define REWARD_SELF_MEDISHOT_ATTACKED		120	/**< 주인이 없는 의료주사기를 사용했을 때 주는 경험치 량 */
#define REWARD_NEMESIS_ENDROUND_ALIVE_MIN	30		/**< 네메시스 라운드 생존 시 받는 경험치 량 */
#define REWARD_NEMESIS_ENDROUND_ALIVE_MAX	50
#define REWARD_NEMESIS_ENDROUND_ZDOLLAR_ALIVE_MIN	10	/**< 네메시스 라운드 생존 시 받는 zdollar 량 */
#define REWARD_NEMESIS_ENDROUND_ZDOLLAR_ALIVE_MAX	20
#define REWARD_NEMESIS_KILL_EXP_MIN			150		/**< 네메시스 처치 시 받는 경험치 량 */
#define REWARD_NEMESIS_KILL_EXP_MAX			160
#define REWARD_NEMESIS_ASSIST_EXP_MIN		150		/**< 네메시스 처치 도움 시 받는 경험치 량 */
#define REWARD_NEMESIS_ASSIST_EXP_MAX		200

/**
 * 스탯 관련
 */
#define STAT_STR_VALUE	0.003		/**< 공격력 증가 값 (퍼센트) */
#define STAT_DEF_VALUE	2			/**< 방어력 증가 값 */
#define STAT_DEF_VALUE_BY_ZOMBIE	150		/**< 방어력 증가 값 (좀비) */
#define STAT_AGL_VALUE	0.004		/**< 민첩성 증가 값 */
//	Do not change this.
#define STAT_NEED_SP_STR	1		/**< 공격력 스탯 필요 SP */
#define STAT_NEED_SP_DEF	1		/**< 방어력 스탯 필요 SP */
#define STAT_NEED_SP_AGL	1		/**< 민첩성 스탯 필요 SP */
//
#define STAT_MAXLVL_STR		100		/**< 공격 최대 레벨 */
#define STAT_MAXLVL_DEF		50		/**< 기력 최대 레벨 */
#define STAT_MAXLVL_AGL		50		/**< 민첩 최대 레벨 */

/**
 * 아이템 관련
 */
#define ITEM_SELL_PRICE_DIV		4
#define	ITEM_AP_POSION_USE_COUNT	1	/**< AP 포션 사용 가능 횟수 */
#define ITEM_HP_POSION_USE_COUNT	2	/**< HP 포션 사용 가능 횟수 */

/**
 * 아이템 드랍 관련
 */
#define ITEM_NORMAL_DROP_CHANCE		50	/**< 노말 아이템 드랍 확률 def 25 */
#define ITEM_RARE_DROP_CHANCE		50	/**< 레어 아이템 드랍 확률 def 10 */
#define ITEM_SR_DROP_CHANCE			50	/**< SR 아이템 드랍 확률 def 5 */
#define ITEM_SSR_DROP_CHANCE		50	/**< SSR 아이템 드랍 확률 def 3 */
#define ITEM_LEGEND_DROP_CHANCE		50	/**< 레전드 아이템 드랍 확률 def 1 */

/**
 * 데미지 & 체력 관련
 */
#define ROUND_DMG_PERCENTAGE	1.0
#define GRENADE_DAMAGE		2.5		/**< 수류탄 데미지 (곱) */
#define MEDISHOT_SELF_ATTACKED_HEALTH	10	/**< 주인이 없는 의료주사기를 사용했을 때 올라가는 체력 */

/**
 * 무기 관련
 */
#define RELOAD_NORMAL_SPEED		0.015

/**
 * 메뉴 관련
 */
#define MENU_TITLE			"ZOMBIE DEPARTMENT"
#define MENU_STAT_TITLE		"Stat"

/**
 * 인터페이스
 */
#define INTERFACE_NO_EQUIPMENT_ITEM		"없음"
#define INTERFACE_MONEY_NAME_DISPLAY	"골드"
#define INTERFACE_MONEY_NAME			"Gold"

#define INTERFACE_ITEM_RARITY_NORMAL	"노말"
#define INTERFACE_ITEM_RARITY_RARE		"레어"
#define INTERFACE_ITEM_RARITY_HERO		"히어로"
#define INTERFACE_ITEM_RARITY_UNIQUE	"유니크"
#define INTERFACE_ITEM_RARITY_LEGENDARY		"레전더리"

/**
 * 소리 관련
 */
#define SOUND_LEVEL_UP						"ui/xp_levelup.wav"
#define SOUND_MEDIC_MEDISHOT_SUCCESS		"items/healthshot_success_01.wav"
#define SOUND_USE_POTION					"qufnr/zombie_department/use_potion.mp3"
#define	SOUND_BUY_ITEM						"qufnr/zombie_department/buy_item.mp3"	/**< 아이템 구입 소리 */
#define		SOUND_SNIPER_LAST_BLOW_EXPLODE		"qufnr/sniper/sniper_lastblow_explode.mp3"
#define		SOUND_SNIPER_VECTOR_BULLET_HIT		"qufnr/sniper/sniper_hitmark_bullet_hit.mp3"
#define		SOUND_STRONGZOMBIE_SPAWN_IDLE		"qufnr/zombie_department/strong_zombie_spawn.mp3"

/**
 * 모델 관련
 */
#define SNIPER_LAST_BULLET_MISSILE_MODEL	"models/weapons/eminem/bms_rpg/w_rpg_projectile.mdl"

/**
 * 좀비 기술 관련
 */
#define ZOMBIESKILL_BLOODYFIELD_RADIUS	700.0		/**< 블러디 필드 거리 */
#define ZOMBIESKILL_ARMORBREAKER_RADIUS	400.0		/**< 아머 브레이커 거리 */
#define ZOMBIESKILL_USE_KILL_SOUNDEFF	"qufnr/zombie_department/bloody_field_fix.mp3"
#define ZOMBIESKILL_GLOBAL_COOLDOWN		15

/**
 * 보스 관련
 */
#define BOSS_DEFINE_TIME			10			/**< 보스 등장 시간 */

/**
 * 메세지 관련
 */
#define MESSAGE_PREFIX							" \x04[Zombie Department]"
#define CLIENT_LEVEL_UP_MSG(%1,%2)				PrintToChatAll ( " \x03[Level Up] \x01%N님이 %d (으)로 레벨업 했습니다!", %1, %2 );
#define CLIENT_DEFINE_FAILURE_DATA_MSG(%1)		PrintToChat ( %1, "%s \x01귀하의 데이터를 불러오는데 실패했습니다. 관리자에게 문의하세요.", MESSAGE_PREFIX );
#define LOG_DEFINE_FAILURE_DATA_MSG(%1,%2)			LogError ( "Failed to define data(%s) on %N.", %2, %1 );
#define LOG_INPUT_FAILURE_DATA_MSG(%1,%2)			LogError ( "Failed to input data(%s) on %N.", %2, %1 );
#define CLIENT_CMDHELP_INFO_ARG1(%1)			ReplyToCommand ( %1, "%s \x01'sm_classinfo other' 명령어로 다른 플레이어의 정보를 확인할 수 있습니다.", MESSAGE_PREFIX );
#define CLIENT_CMDHELP_INFO_ARG_DISMATCH(%1)	ReplyToCommand ( %1, "%s \x01Usage: sm_classinfo <other>", MESSAGE_PREFIX );
#define CLIENT_THIS_PLAYER_IS_INVALID_MSG(%1)	ReplyToCommand ( %1, "%s \x01유효하지 않는 클라이언트 입니다.", MESSAGE_PREFIX );
#define CLIENT_NEED_MORE_STATPOINTS(%1)			PrintToChat ( %1, " \x04[Stat] \x01SP가 부족합니다." );
#define CLIENT_LEVELUP_BLOCKED_MSG(%1,%2)		PrintToChat ( %1, " \x04[EXP Manager] \x01%d 레벨 부터는 병과가 없는 상태에서 경험치를 모을 수 없습니다. \x09!hclass\x01명령어로 병과를 선택하세요.", %2 );
#define CLIENT_LEVELUP_THIS_LVUP_BLOCKED_LV_MSG(%1)	PrintToChat ( %1, " \x04[EXP Manager] \x01현재 레벨 부터 병과가 없을 경우 경험치를 모을 수 없습니다. \x09!hclass\x01명령어로 병과를 선택하세요." );
#define CLIENT_GET_EXP_MSG(%1,%2)				PrintToChat ( %1, " \x04[Reward] \x01처치 보상 획득 \x06+%d EXP %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define	CLIENT_ASSIST_GET_EXP_MSG(%1,%2)		PrintToChat ( %1, " \x04[Reward] \x01도움 보상 획득 \x06+%d EXP %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_PERMISSION_MSG(%1)				PrintToChat ( %1, "%s \x01귀하는 이 명령에 접근할 권한이 없습니다.", MESSAGE_PREFIX );
#define CLIENT_CHOOSE_CLASS_BROADCAST_MSG(%1,%2)	PrintToChatAll ( " \x04[Choose Class] \x01%N님이 %s 병과를 선택했습니다.", %1, %2 );
#define CLIENT_CHOOSE_CLASS_FAILED_MSG(%1)			PrintToChat ( %1, " \x04[Choose Class] \x01귀하는 병과를 선택할 수 없습니다." );
#define CLIENT_SKILL_NOT_AVAILABLE_NEED_AP_MSG(%1)	PrintToChat ( %1, " \x04[Skill] \x01AP가 부족합니다." );
#define CLIENT_SKILL_NOT_AVAILABLE_COOLDOWN_MSG(%1)	PrintToChat ( %1, " \x04[Skill] \x01기술 재사용 대기시간이 남았습니다." );
#define CLIENT_SKILL_USE_MSG(%1,%2)				PrintToChat ( %1, " \x04[Skill] \x01%s 을(를) 사용했습니다.", %2 );
#define CLIENT_SKILL_COOLDOWN_END_MSG(%1,%2)	PrintToChat ( %1, " \x04[Skill] \x01%s 기술의 재사용 대기시간이 끝났습니다.", %2 );
#define CLIENT_SKILL_NOT_AVAILABLE_NOW_USING_MSG(%1)	PrintToChat ( %1, " \x04[Skill] \x01이미 사용중 입니다." );
#define CLIENT_RESET_DATA_BY_TARGET_MSG(%1)		PrintToChat ( %1, "%s \x01귀하의 데이터가 초기화 되었습니다.", MESSAGE_PREFIX );
#define CLIENT_RESET_DATA_BY_ADMIN_MSG(%1,%2)	PrintToChat ( %1, "%s \x01%N님의 데이터를 초기화 했습니다.", MESSAGE_PREFIX, %2 );
#define CLIENT_DELETE_THIS_ITEM_MSG(%1,%2)		PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 버렸습니다.", %2 );
#define CLIENT_SELL_THIS_ITEM_MSG(%1,%2,%3,%4)	PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 팔았습니다. \x05+%d %s", %2, %3, %4 );
#define CLIENT_ITEM_BUY_FAILED_MSG(%1)			PrintToChat ( %1, " \x04[Item] \x01%s 아이템 구입에 실패했습니다, 다시 시도 해주세요." );
#define CLIENT_ITEM_BUY_SUCCESS_MSG(%1,%2)		PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 구입했습니다.", %2 );
#define CLIENT_ITEM_USE_SUCCESS_MSG(%1,%2)		PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 사용했습니다.", %2 );
#define CLIENT_ITEM_USE_MAXHEALTH_FAIL_MSG(%1)	PrintToChat ( %1, " \x04[Item] \x01이미 체력이 가득 차 있습니다." );
#define CLIENT_ITEM_UNEQUIP_SUCCESS_MSG(%1,%2)	PrintToChat ( %1, " \x04[Item] \x01%s 아이템이 인벤토리로 옮겨졌습니다.", %2 );
#define CLIENT_ITEM_UNEQUIP_FAILED1_MSG(%1,%2)	PrintToChat ( %1, " \x04[Item] \x01%s 아이템은 존재하지 않는 아이템이므로 삭제되었습니다.", %2 );
#define CLIENT_ITEM_EQUIP_SUCCESS_MSG(%1,%2)	PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 착용 했습니다.", %2 );
#define CLIENT_ITEM_EQUIP_FAILED_MSG(%1,%2)		PrintToChat ( %1, " \x04[Item] \x01%s 현재 착용 중인 아이템을 착용 해제 후 사용해 주세요.", %2 );
#define CLIENT_ITEM_EQUIP_FAILED_STACKED(%1,%2)	PrintToChat ( %1, " \x04[Item] \x01%s 유물로 인해 더 이상 착용할 수 없습니다.", %2 );
#define CLIENT_ITEM_EQUIP_FAILED_DISMATCH_CLASS(%1)	PrintToChat ( %1, " \x04[Item] \x01귀하의 병과는 해당 아이템을 착용할 수 없습니다." );
#define CLIENT_ITEM_EQUIP_FAILED_FULL_MSG(%1)	PrintToChat ( %1, " \x04[Item] \x01유물을 착용할 수 있는 칸이 부족합니다." );
#define CLIENT_MEDIC_CLASS_MEDIC_HEAL_TARGET_FULL_HEALTH(%1,%2)	PrintToChat ( %1, " \x04[Medic] \x01%N님의 체력은 이미 가득 차 있습니다.", %2 );
#define CLIENT_MEDIC_CLASS_MEDIC_HEAL(%1,%2,%3)					PrintToChat ( %1, " \x04[Medic] \x01%N님을 치료 했습니다. \x07+%d HP", %2, %3 );
#define CLIENT_MEDIC_CLASS_MEDIC_HEAL_TARGET(%1,%2,%3)			PrintToChat ( %1, " \x04[Medic] \x01%N님이 귀하를 치료 했습니다. \x07+%d HP", %2, %3 );
#define CLIENT_MEDIC_CLASS_MEDIC_PISTOL_GIVEN_HEALTHSHOT(%1)		PrintToChat ( %1, " \x04[Medic] \x01의료 주사기를 획득했습니다." );
#define CLIENT_MEDIC_MEDISHOT_USE_IS_FULL_HP_MSG(%1)		PrintCenterText ( %1, "\x01귀하는 이미 체력이 꽉 찼습니다." );
#define CLIENT_MEDIC_MEDISHOT_IS_NOT_END_COOLDOWN_MSG(%1)	PrintToChat ( %1, " \x04[Medic] \x01의료주사기의 재사용 대기시간이 끝나지 않았습니다." );
#define CLIENT_MEDIC_MEDISHOT_USE_OWNERSELF_HEAL_MSG(%1,%2)	PrintToChat ( %1, " \x04[Medic] \x01의료주사기로 자신을 치료 했습니다. \x06+%d EXP %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_MEDIC_MEDISHOT_USE_TEAMSELF_HEAL_MSG(%1,%2,%3)	PrintToChat ( %1, " \x04[Medic] \x01%N님이 귀하의 의료주사기를 사용 했습니다. \x06+%d EXP %s", %2, %3, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_MEDIC_MEDISHOT_USE_HEAL_MSG(%1,%2,%3)			PrintToChat ( %1, " \x04[Medic] \x01%N님의 의료주사기를 사용 했습니다. \x07+%d HP", %2, %3 );
#define CLIENT_MEDIC_MEDISHOT_IS_COOLDOWN_ENDED_MSG(%1)		PrintToChat ( %1, " \x04[Medic] \x01이제 의료주사기를 사용할 수 있습니다." );
#define CLIENT_NEED_MORE_STORE_DOLLAR(%1)			PrintToChat ( %1, " \x04[Buy Item] \x01자금이 충분하지 않습니다." );
#define CLIENT_CRAFT_NEED_MORE_ITEMS(%1)			PrintToChat ( %1, " \x04[Item] \x01아이템이 부족합니다." );
#define CLIENT_CRAFT_ITEM_CREATED_MSG(%1,%2)		PrintToChat ( %1, " \x04[Item] \x01%s 아이템을 생산 했습니다.", %2 );
#define CLIENT_ROUNDEND_REWARD_ZERO_MSG(%1)			PrintToChat ( %1, " \x04[Reward] \x01경험치를 받지 못했습니다." );
#define CLIENT_ROUNDEND_REWARD_MSG(%1,%2)			PrintToChat ( %1, " \x04[Reward] \x01경험치를 획득했습니다. \x06+%d EXP %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_SUPPORTER_CLASS_USE_AMMOBOX(%1,%2)	PrintToChat ( %1, " \x04[Support] \x01%N님이 귀하의 탄알 상자를 사용 했습니다.", %2 );
#define CLIENT_SUPPORTER_CLASS_AMMO_BOX_USE_IS_FULL_MSG(%1)	PrintToChat ( %1, " \x04[Support] \x01더 이상 사용할 수 없습니다." );
#define CLIENT_SUPPORTER_CLASS_AMMO_BOX_USE_IS_EXIST_AMMO_MSG(%1)	PrintToChat ( %1, " \x04[Support] \x01그 정도의 총알이면 충분합니다!" );
#define CLIENT_NEMESIS_ROUND_ALIVE_REWARD_MSG(%1,%2,%3,%4)		PrintToChat ( %1, " \x04[Reward] \x01네메시스로 부터 생존하여 보상을 받습니다. \x05+%d EXP %s \x10+%d %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "", %3, %4 );
#define CLIENT_NEED_MORE_DOLLAR(%1)		PrintToChat ( %1, " \x04[Item] \x01자금이 부족합니다." );
#define CLIENT_USE_ZOMBIE_SKILL(%1,%2)		PrintToChatAll ( " \x02[Zombie Skill] \x01숙주좀비 \x0E%N\x01님이 \x02%s\x01을(를) 사용했습니다!", %1, g_bIsStrongMotherZombie[%1] ? g_strStrongZombieActiveSkillList[%2][0] : g_strZombieActiveSkillList[%2][0] );
#define CLIENT_MOTHER_ZOMBIE_SKILL_MSG(%1)	PrintToChat ( %1, " \x02[Zombie Skill] \x01숙주좀비 기술을 사용할 수 있습니다, R키를 눌러서 사용하세요." );
#define CLIENT_MOTHER_ZOMBIE_SKILL_COOL_COUNTDOWN_MSG(%1,%2)	PrintToChat ( %1, " \x02[Zombie Skill] \x01%d 초 후 숙주좀비 기술을 사용할 수 있습니다.", %2 );
#define CLIENT_MOTHER_ZOMBIE_SKILL_COOL_END_MSG(%1)	PrintToChat ( %1, " \x02[Zombie Skill] \x01숙주좀비 기술을 사용할 수 있습니다." );
#define CLIENT_SELECT_RESET_MSG(%1)	PrintToChat ( %1, " \x04[Reset] \x01라운드가 끝나면 초기화 됩니다. 초기화를 취소하려면 \x07!resetcancel\x01을 입력하세요." );
#define CLIENT_RESET_SUCCESS_MSG(%1)	PrintToChat ( %1, " \x04[Reset] \x01초기화 되었습니다." );
#define CLIENT_USE_ITEM_ON_ZOMBIE_MSG(%1)	PrintToChat ( %1, " \x04[Item] \x01인간만 사용할 수 있습니다." );
#define CLIENT_ROUNDEND_SURVIVE_REWARD_MSG(%1,%2)	PrintToChat ( %1, " \x04[Reward] \x01생존 보상을 받았습니다. \x05+%d EXP %s", %2, EXP_BOOST_SCALE > 1.0 ? "\x0EEvent↑" : "" );
#define CLIENT_ROUNDEND_SURVIVE_NOT_ALLOW_PLAYERS_MIN_MSG(%1,%2)	PrintToChat ( %1, " \x04[Reward] \x07서버에 플레이어가 적어서 생존 보상을 받을 수 없습니다. 최소한 %d명 이상의 플레이어가 있어야합니다.", %2 );
#define CLIENT_USE_AP_OR_HP_USE_COUNT_EXCESS_MSG(%1,%2)			PrintToChat ( %1, " \x04[Use Potion] \x01더 이상 %s 포션을 사용할 수 없습니다.", %2 );
#define CLIENT_YOU_NO_SELECTE_CHARACTER_MSG(%1)		PrintToChat ( %1, "%s \x01\x07!select\x01명령어로 캐릭터를 선택하세요.", MESSAGE_PREFIX );

/**
 * 콘바 설정
 */
public void OnConfigsExecuted () {
	ServerCommand ( "ammo_item_limit_healthshot 256" );
	
	ServerCommand ( "sm_cvar healthshot_health %d", MEDISHIOT_NORMAL_HEAL_SCALE );
	ServerCommand ( "sv_health_approach_enabled 1" );	//	healthshot 사용 시 피가 1씩 올라감
	ServerCommand ( "healthshot_healthboost_time 2.5" );	//	속도 올라가는 시간
	ServerCommand ( "healthshot_healthboost_damage_multiplier 0.92" );	//	피해 값
	ServerCommand ( "healthshot_healthboost_speed_multiplier 1.1");	//	속도 값
}