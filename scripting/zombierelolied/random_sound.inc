#define SOUND_MAX_PATH		225

/**
 * @section Defines for min/max moaning sound file index.
 */
#define SOUND_MOAN_PATH "qufnr/zombie/zombie_voice_idle%d.mp3" // %d is sound file index
#define SOUND_MOAN_MIN 1
#define SOUND_MOAN_MAX 14
/**
 * @endsection
 */

/**
 * @section Defines for min/max groan sound file index.
 */
#define SOUND_GROAN_PATH "qufnr/zombie/zombie_pain%d.mp3" // %d is sound file index
#define SOUND_GROAN_MIN 1
#define SOUND_GROAN_MAX 6
/**
 * @endsection
 */

/**
 * @section Defines for min/max death sound file index.
 */
#define SOUND_DEATH_PATH "qufnr/zombie/zombie_die%d.mp3" // %d is sound file index
#define SOUND_DEATH_MIN 1
#define SOUND_DEATH_MAX 3

/**
 * @section Defines for min/max death idle sound file index.
 */
#define SOUND_DEATH_IDLE_PATH	"qufnr/zombie/infect%d.mp3"	//	%d is sound file index
#define SOUND_DEATH_IDLE_MIN	1
#define SOUND_DEATH_IDLE_MAX	4

enum zombieSounds {
	idle	/* moan (idle) */
	, pain	/* groan (pain) */
	, death	/* death */
	, death_idle
}

/**
 * Gets a random zombie sound from qufnr/zombie/.
 *
 * @param sample				The randomly picked sound.
 * @param soundType				The type of sound to get. (See enum zombieSounds)
 * @return			True if sound was successfully picked, false otherwise.
 */
stock bool getRandomZombieSounds ( char[] sample, zombieSounds types ) {
	int min, max;
	char path[SOUND_MAX_PATH];
	
	switch ( types ) {
		case idle: {
			min = SOUND_MOAN_MIN;
			max = SOUND_MOAN_MAX;
			strcopy ( path, sizeof ( path ), SOUND_MOAN_PATH );
		}
		
		case pain: {
			min = SOUND_GROAN_MIN;
			max = SOUND_GROAN_MAX;
			strcopy ( path, sizeof ( path ), SOUND_GROAN_PATH );
		}
		
		case death: {
			min = SOUND_DEATH_MIN;
			max = SOUND_DEATH_MAX;
			strcopy ( path, sizeof ( path ), SOUND_DEATH_PATH );
		}
		
		case death_idle: {
			min = SOUND_DEATH_IDLE_MIN;
			max = SOUND_DEATH_IDLE_MAX;
			strcopy ( path, sizeof ( path ), SOUND_DEATH_IDLE_PATH );
		}
		
		default: return false;
	}
	
	int randam = GetRandomInt ( min, max );
	Format ( sample, SOUND_MAX_PATH, path, randam );
	
	return true;
}