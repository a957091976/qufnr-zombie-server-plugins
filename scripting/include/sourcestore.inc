#if defined _sourcestore_included_
	#endinput
#endif
#define _sourcestore_included_

#define SOURCESTORE_STORELEVEL_NORMAL	0	/**< Players store level values. */
#define SOURCESTORE_STORELEVEL_BRONZE	1
#define SOURCESTORE_STORELEVEL_SILVER	2
#define SOURCESTORE_STORELEVEL_GOLD		3
#define SOURCESTORE_STORELEVEL_DIAMOND	4
#define SOURCESTORE_STORELEVEL_LEGENDARY			5
#define SOURCESTORE_STORELEVEL_LEGENDARY_MASTER		6
#define SOURCESTORE_STORELEVEL_SUPREME				7
#define SOURCESTORE_STORELEVEL_GLOBAL_ELITE			8

#define SOURCESTORE_STORE_MVP		1
#define SOURCESTORE_STORE_MVPPLUS	2

/**
 * 회폐 명을 가져옵니다.
 * 
 * @param buffer		담을 char 변수에
 * @param maxlen		char 크기
 * @return 0
 */
native int SourceStore_GetMoneyName ( char[] buffer, int maxlen );

/**
 * 클라이언트의 Balance를 반환합니다.
 *
 * @param data			클라이언트 인덱스
 * @return		클라이언트의 소지 Balance 량
 * @error		클라이언트가 유효하지 않을 경우 -1을 반환합니다.
 */
native int SourceStore_GetClientBalance ( int data );

/**
 * 클라이언트의 Balance를 Set(설정)합니다.
 *
 * @param data				클라이언트 인덱스
 * @param target			설정할 Balance 량
 * @return			설정에 성공했으면 true, 아니면 false를 반환합니다.
 */
native bool SourceStore_SetClientBalance ( int data, int target );

/**
 * 클라이언트의 Balance를 Add(추가)합니다.
 *
 * @param data				클라이언트 인덱스
 * @param target			추가할 Balance 량
 * @return			설정에 성공했으면 true, 아니면 false를 반환합니다.
 */
native bool SourceStore_AddClientBalance ( int data, int target );

/**
 * 클라이언트의 Balance를 Sub(삭감)합니다.
 *
 * @param data				클라이언트 인덱스
 * @param target			삭감할 Balance 량
 * @return			설정에 성공했으면 true, 아니면 false를 반환합니다.
 */
native bool SourceStore_SubClientBalance ( int data, int target );

/**
 * 클라이언트의 태그를 구합니다.
 *
 * @param data				클라이언트 인덱스
 * @param target			문자열 버퍼
 * @param target_maxlen		문자열 길이
 * @return 			태그를 구했다면 true, 아니면 false를 반환합니다.
 */
native bool SourceStore_GetClientTagTitle ( int data, const char[] target, int target_maxlen );

/**
 * 태그의 색상을 구합니다.
 *
 * @param name				태그 이름
 * @param target			문자열 버퍼
 * @param target_maxlen		문자열 길이
 */
native void SourceStore_GetTagTitleColor ( char[] name, const char[] target, int target_maxlen );

/**
 * 클라이언트의 태그 입력 모드 여부를 구합니다.
 *
 * @param data				클라이언트 인덱스
 * @return				클라이언트 개인 변수에 따라 반환합니다.
 * @error			클라이언트가 유효하지 않으면, false를 반환합니다.
 */
native bool SourceStore_IsClientUsingTag ( int data );

/**
 * 클라이언트의 Store Level의 이름을 구합니다.
 *
 * @param data				클라이언트 인덱스
 * @param target			문자열 버퍼
 * @param target_maxlen		문자열 길이
 * @return 			클라이언트 개인 변수에 따라 반환합니다.
 * @error			클라이언트가 유효하지 않으면, -1을 반환합니다.
 */
native int SourceStore_GetClientStoreLevel ( int data, const char[] target, int target_maxlen );

/**
 * 클라이언트의 Store Level을 구합니다.
 *
 * @param data				클라이언트 인덱스
 * @return 			클라이언트 개인 변수에 따라 반환합니다.
 * @error			클라이언트가 유효하지 않으면, -1을 반환합니다.
 */
native int SourceStore_GetClientStoreLevelIndex ( int data );

/**
 * 클라이언트의 Store Level을 설정합니다.
 *
 * @param data				클라이언트 인덱스
 * @param amount			갯수
 * @noreturn
 */
native int SourceStore_SetClientStoreLevelIndex ( int data, int amount );

/**
 * Returns store max levels.
 * @return		MAXSTORELEVEL
 */
native int SourceStore_GetMaxStoreLevel ();

/**
 * 클라이언트가 MVP인지 구합니다.
 *
 * @param data			
 * @return 클라이언트 MVP 여부
 */
native bool SourceStore_IsClientStoreMVP ( int data );

/**
 * 클라이언트가 MVP+인지 구합니다.
 *
 * @param data			
 * @return 클라이언트 MVP+ 여부
 */
native bool SourceStore_IsClientStoreMVPPlus ( int data );

/**
 * 클라이언트의 인벤토리에 아이템을 추가합니다.
 *
 * @param data
 * @param item_id
 * @param amounts
 * @return 아이템 갯수
 */
native int SourceStore_AddPlayerStoreItem ( int data, int item_id, int amounts );

/**
 * 클라이언트를 mvp 또는 mvp plus로 설정합니다.
 *
 * @param data
 * @param type		List of types: SOURCESTORE_STORE_MVP, SOURCESTORE_STORE_MVPPLUS
 * @param setvalue
 * @return	설정에 성공 했다면 data, 아니면 -1을 반환합니다.
 */
native int SourceStore_SetClientStoreMVP ( int data, int type, bool setvalue );

/**
 * 아이템을 구입할 때 호출 합니다.
 *
 * @param client
 * @param itemId		아이템 번호
 * @param itemName		아이템 이름
 * @param itemType		아이템 타입 명
 * @param oldBalance	클라이언트의 구입 전 소지금
 * @param newBalance	클라이언트의 구입 후 소지금
 */
forward void SourceStore_OnBuyItem ( int client, int itemId, char[] itemName, char[] itemType, int oldBalance, int newBalance );

/**
 * 아이템을 판매할 때 호출 합니다.
 *
 * @param client
 * @param itemId		아이템 번호
 * @param itemName		아이템 이름
 * @param itemType		아이템 타입 명
 * @param oldBalance	클라이언트의 판매 전 소지금
 * @param newBalance	클라이언트의 판매 후 소지금
 */
forward void SourceStore_OnSellItem ( int client, int itemId, char[] itemName, char[] itemType, int oldBalance, int newBalance );

/**
 * 가챠에서 아이템을 뽑을 때 호출 합니다.
 *
 * @param client
 * @param itemId		아이템 번호
 * @param gasyaName		가챠 이름
 * @param itemName		아이템 이름
 * @param itemRarity	아이템 희귀도 명
 * @param itemType		아이템 타입 명
 */
forward void SourceStore_OnDropGasyaItem ( int client, int itemId, char[] gasyaName, char[] itemName, char[] itemRarity, char[] itemType );