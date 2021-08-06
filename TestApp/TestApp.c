#include <stdio.h>


//#include "CFILE.H"
#include "TEXT.H"
#include "fix.h"

#define N_COCKPIT_BITMAPS 6
#define N_PLAYER_GUNS 8
#define MAX_OBJ_BITMAPS				600
#define MAX_POLYGON_MODELS 200
#define N_ANIM_STATES	5
#define MAX_ROBOT_JOINTS 1600
#define MAX_GUNS 8		//should be multiple of 4 for ubyte array
#define MAX_TEXTURES		1200
#define VCLIP_MAXNUM			110
#define VCLIP_MAX_FRAMES	30
#define MAX_SOUNDS 	254		//bad to have sound 255!
#define MAX_EFFECTS 110
#define MAX_CLIP_FRAMES			50
#define MAX_WALLS					254	// Maximum number of walls
#define MAX_WALL_ANIMS			60		// Maximum different types of doors
#define	NDL	5		//	Number of difficulty levels.
#define MAX_POWERUP_TYPES		50
#define MAX_SUBMODELS 10			//how many animating sub-objects per model
#define MAX_REACTORS 7
#define MAX_CONTROLCEN_GUNS 8
#define MAX_GAUGE_BMS 100	//	increased from 56 to 80 by a very unhappy MK on 10/24/94.
#define	MAX_ROBOT_TYPES	85			// maximum number of robot types
#define N_D2_ROBOT_TYPES		66
#define N_D2_ROBOT_JOINTS		1145
#define N_D2_POLYGON_MODELS		166
#define N_D2_OBJBITMAPS			422
#define N_D2_OBJBITMAPPTRS		502
#define N_D2_WEAPON_TYPES		62

typedef struct vms_angvec {
	fixang p, b, h;
} vms_angvec;
typedef struct bitmap_index {
	ushort	index;
} bitmap_index;
typedef struct {
	fix				play_time;			//total time (in seconds) of clip
	int				num_frames;
	fix				frame_time;			//time (in seconds) of each frame
	int				flags;
	short				sound_num;
	bitmap_index	frames[VCLIP_MAX_FRAMES];
	fix				light_value;
} vclip;
typedef struct {
	ubyte		flags;				//values defined above
	ubyte		pad[3];				//keep alignment
	fix		lighting;			//how much light this casts
	fix		damage;				//how much damage being against this does (for lava)
	short		eclip_num;			//the eclip that changes this, or -1
	short		destroyed;			//bitmap to show when destroyed, or -1
	short		slide_u, slide_v;	//slide rates of texture, stored in 8:8 fix
#ifdef EDITOR
	char		filename[13];		//used by editor to remap textures
	char		pad2[3];
#endif
} tmap_info;
typedef struct jointpos {
	short jointnum;
	vms_angvec angles;
} jointpos;
typedef struct vms_vector {
	fix x, y, z;
} vms_vector;
typedef struct jointlist {
	short n_joints;
	short offset;
} jointlist;
typedef struct eclip {
	vclip 		vc;				//imbedded vclip
	fix			time_left;		//for sequencing
	int			frame_count;	//for sequencing
	short			changing_wall_texture;			//Which element of Textures array to replace.
	short			changing_object_texture;		//Which element of ObjBitmapPtrs array to replace.
	int			flags;			//see above
	int			crit_clip;		//use this clip instead of above one when mine critical
	int			dest_bm_num;	//use this bitmap when monitor destroyed
	int			dest_vclip;		//what vclip to play when exploding
	int			dest_eclip;		//what eclip to play when exploding
	fix			dest_size;		//3d size of explosion
	int			sound_num;		//what sound this makes
	int			segnum, sidenum;	//what seg & side, for one-shot clips
} eclip;
typedef struct {
	fix				play_time;
	short				num_frames;
	short				frames[MAX_CLIP_FRAMES];
	short				open_sound;
	short				close_sound;
	short				flags;
	char				filename[13];
	char				pad;
} wclip;
typedef struct robot_info {
	int			model_num;							// which polygon model?
	vms_vector	gun_points[MAX_GUNS];			// where each gun model is
	ubyte			gun_submodels[MAX_GUNS];		// which submodel is each gun in?

	short 		exp1_vclip_num;
	short			exp1_sound_num;

	short 		exp2_vclip_num;
	short			exp2_sound_num;

	byte			weapon_type;
	byte			weapon_type2;						//	Secondary weapon number, -1 means none, otherwise gun #0 fires this weapon.
	byte			n_guns;								// how many different gun positions
	byte			contains_id;						//	ID of powerup this robot can contain.

	byte			contains_count;					//	Max number of things this instance can contain.
	byte			contains_prob;						//	Probability that this instance will contain something in N/16
	byte			contains_type;						//	Type of thing contained, robot or powerup, in bitmaps.tbl, !0=robot, 0=powerup
	byte			kamikaze;							//	!0 means commits suicide when hits you, strength thereof. 0 means no.

	short			score_value;						//	Score from this robot.
	byte			badass;								//	Dies with badass explosion, and strength thereof, 0 means NO.
	byte			energy_drain;						//	Points of energy drained at each collision.

	fix			lighting;							// should this be here or with polygon model?
	fix			strength;							// Initial shields of robot

	fix		mass;										// how heavy is this thing?
	fix		drag;										// how much drag does it have?

	fix		field_of_view[NDL];					// compare this value with forward_vector.dot.vector_to_player, if field_of_view <, then robot can see player
	fix		firing_wait[NDL];						//	time in seconds between shots
	fix		firing_wait2[NDL];					//	time in seconds between shots
	fix		turn_time[NDL];						// time in seconds to rotate 360 degrees in a dimension
// -- unused, mk, 05/25/95	fix		fire_power[NDL];						//	damage done by a hit from this robot
// -- unused, mk, 05/25/95	fix		shield[NDL];							//	shield strength of this robot
	fix		max_speed[NDL];						//	maximum speed attainable by this robot
	fix		circle_distance[NDL];				//	distance at which robot circles player

	byte		rapidfire_count[NDL];				//	number of shots fired rapidly
	byte		evade_speed[NDL];						//	rate at which robot can evade shots, 0=none, 4=very fast
	byte		cloak_type;								//	0=never, 1=always, 2=except-when-firing
	byte		attack_type;							//	0=firing, 1=charge (like green guy)

	ubyte		see_sound;								//	sound robot makes when it first sees the player
	ubyte		attack_sound;							//	sound robot makes when it attacks the player
	ubyte		claw_sound;								//	sound robot makes as it claws you (attack_type should be 1)
	ubyte		taunt_sound;							//	sound robot makes after you die

	byte		boss_flag;								//	0 = not boss, 1 = boss.  Is that surprising?
	byte		companion;								//	Companion robot, leads you to things.
	byte		smart_blobs;							//	how many smart blobs are emitted when this guy dies!
	byte		energy_blobs;							//	how many smart blobs are emitted when this guy gets hit by energy weapon!

	byte		thief;									//	!0 means this guy can steal when he collides with you!
	byte		pursuit;									//	!0 means pursues player after he goes around a corner.  4 = 4/2 pursue up to 4/2 seconds after becoming invisible if up to 4 segments away
	byte		lightcast;								//	Amount of light cast. 1 is default.  10 is very large.
	byte		death_roll;								//	0 = dies without death roll. !0 means does death roll, larger = faster and louder

	//boss_flag, companion, thief, & pursuit probably should also be bits in the flags byte.
	ubyte		flags;									// misc properties
	ubyte		pad[3];									// alignment

	ubyte		deathroll_sound;						// if has deathroll, what sound?
	ubyte		glow;										// apply this light to robot itself. stored as 4:4 fixed-point
	ubyte		behavior;								//	Default behavior.
	ubyte		aim;										//	255 = perfect, less = more likely to miss.  0 != random, would look stupid.  0=45 degree spread.  Specify in bitmaps.tbl in range 0.0..1.0

	//animation info
	jointlist anim_states[MAX_GUNS + 1][N_ANIM_STATES];

	int		always_0xabcd;							// debugging

} robot_info;
typedef struct weapon_info {
	byte	render_type;				// How to draw 0=laser, 1=blob, 2=object
	byte	persistent;					//	0 = dies when it hits something, 1 = continues (eg, fusion cannon)
	short	model_num;					// Model num if rendertype==2.
	short	model_num_inner;			// Model num of inner part if rendertype==2.

	byte	flash_vclip;				// What vclip to use for muzzle flash
	byte	robot_hit_vclip;			// What vclip for impact with robot
	short	flash_sound;				// What sound to play when fired

	byte	wall_hit_vclip;			// What vclip for impact with wall
	byte	fire_count;					//	Number of bursts fired from EACH GUN per firing.  For weapons which fire from both sides, 3*fire_count shots will be fired.
	short	robot_hit_sound;			// What sound for impact with robot

	byte	ammo_usage;					//	How many units of ammunition it uses.
	byte	weapon_vclip;				//	Vclip to render for the weapon, itself.
	short	wall_hit_sound;			// What sound for impact with wall

	byte	destroyable;				//	If !0, this weapon can be destroyed by another weapon.
	byte	matter;						//	Flag: set if this object is matter (as opposed to energy)
	byte	bounce;						//	1==always bounces, 2=bounces twice 
	byte	homing_flag;				//	Set if this weapon can home in on a target.

	ubyte	speedvar;					//	allowed variance in speed below average, /128: 64 = 50% meaning if speed = 100, can be 50..100

	ubyte	flags;						// see values above

	byte	flash;						//	Flash effect
	byte	afterburner_size;			//	Size of blobs in F1_0/16 units, specify in bitmaps.tbl as floating point.  Player afterburner size = 2.5.

	byte	children;					//	ID of weapon to drop if this contains children.  -1 means no children.

	fix	energy_usage;				//	How much fuel is consumed to fire this weapon.
	fix	fire_wait;					//	Time until this weapon can be fired again.

	fix	multi_damage_scale;		//	Scale damage by this amount when applying to player in multiplayer.  F1_0 means no change.

	bitmap_index bitmap;				// Pointer to bitmap if rendertype==0 or 1.

	fix	blob_size;					// Size of blob if blob type
	fix	flash_size;					// How big to draw the flash
	fix	impact_size;				// How big of an impact
	fix	strength[NDL];				// How much damage it can inflict
	fix	speed[NDL];					// How fast it can move, difficulty level based.
	fix	mass;							// How much mass it has
	fix	drag;							// How much drag it has
	fix	thrust;						//	How much thrust it has
	fix	po_len_to_width_ratio;	// For polyobjects, the ratio of len/width. (10 maybe?)
	fix	light;						//	Amount of light this weapon casts.
	fix	lifetime;					//	Lifetime in seconds of this weapon.
	fix	damage_radius;				//	Radius of damage caused by weapon, used for missiles (not lasers) to apply to damage to things it did not hit
//-- unused--	fix	damage_force;				//	Force of damage caused by weapon, used for missiles (not lasers) to apply to damage to things it did not hit
// damage_force was a real mess.  Wasn't Difficulty_level based, and was being applied instead of weapon's actual strength.  Now use 2*strength instead. --MK, 01/19/95
	bitmap_index	picture;				// a picture of the weapon for the cockpit
	bitmap_index	hires_picture;		// a hires picture of the above
} weapon_info;
typedef struct powerup_type_info {
	int	vclip_num;
	int	hit_sound;
	fix	size;			//3d size of longest dimension
	fix	light;		//	amount of light cast by this powerup, set in bitmaps.tbl
} powerup_type_info;
typedef struct player_ship {
	int 		model_num;
	int		expl_vclip_num;
	fix		mass, drag;
	fix		max_thrust, reverse_thrust, brakes;		//low_thrust
	fix		wiggle;
	fix		max_rotthrust;
	vms_vector gun_points[N_PLAYER_GUNS];
} player_ship;
typedef struct reactor {
	int	model_num;
	int	n_guns;
	vms_vector gun_points[MAX_CONTROLCEN_GUNS];
	vms_vector gun_dirs[MAX_CONTROLCEN_GUNS];
} reactor;
typedef struct polymodel {
	int n_models;
	int model_data_size;
	ubyte* model_data;
	int submodel_ptrs[MAX_SUBMODELS];
	vms_vector submodel_offsets[MAX_SUBMODELS];
	vms_vector submodel_norms[MAX_SUBMODELS];		//norm for sep plane
	vms_vector submodel_pnts[MAX_SUBMODELS];		//point on sep plane 
	fix submodel_rads[MAX_SUBMODELS];				//radius for each submodel
	ubyte submodel_parents[MAX_SUBMODELS];		//what is parent for each submodel
	vms_vector submodel_mins[MAX_SUBMODELS];
	vms_vector submodel_maxs[MAX_SUBMODELS];
	vms_vector mins, maxs;							//min,max for whole model
	fix rad;
	ubyte		n_textures;
	ushort	first_texture;
	ubyte		simpler_model;		//alternate model with less detail (0 if none, model_num+1 else)
//	vms_vector min,max;
} polymodel;

int main()
{
	load_text();
}

/*
int mainHAM()
{
	reactor Reactors[MAX_REACTORS];
	player_ship only_player_ship, * Player_ship = &only_player_ship;
	bitmap_index Textures[MAX_TEXTURES];	// Array of all texture tmaps.
	jointpos Robot_joints[MAX_ROBOT_JOINTS];
	int Num_vclips;
	vclip Vclip[VCLIP_MAXNUM];
	tmap_info TmapInfo[MAX_TEXTURES];
	ubyte Sounds[MAX_SOUNDS];
	ubyte AltSounds[MAX_SOUNDS];
	eclip Effects[MAX_EFFECTS];
	wclip WallAnims[MAX_WALL_ANIMS];
	robot_info Robot_info[MAX_ROBOT_TYPES];			// Robot info for AI system, loaded from bitmaps.tbl.
	weapon_info Weapon_info[N_D2_WEAPON_TYPES];
	powerup_type_info Powerup_info[MAX_POWERUP_TYPES];
	int Dying_modelnums[MAX_POLYGON_MODELS];
	int Dead_modelnums[MAX_POLYGON_MODELS];
	bitmap_index		cockpit_bitmap[N_COCKPIT_BITMAPS];

	//array of pointers to polygon objects
	polymodel Polygon_models[MAX_POLYGON_MODELS];
	bitmap_index Gauges[MAX_GAUGE_BMS];   // Array of all gauge bitmaps.
	bitmap_index Gauges_hires[MAX_GAUGE_BMS];   // hires gauges

	bitmap_index		ObjBitmaps[MAX_OBJ_BITMAPS];
	ushort				ObjBitmapPtrs[MAX_OBJ_BITMAPS];		// These point back into ObjBitmaps, since some are used twice.


	CFILE* fp = cfopen("descent2.ham", "rb");
	int i, t;
	cfile_read_int(fp);
	cfile_read_int(fp);

	int NumTextures = cfile_read_int(fp);
	cfread(Textures, sizeof(bitmap_index), NumTextures, fp);
	cfread(TmapInfo, sizeof(tmap_info), NumTextures, fp);

	t = cfile_read_int(fp);
	cfread(Sounds, sizeof(ubyte), t, fp);
	cfread(AltSounds, sizeof(ubyte), t, fp);

	Num_vclips = cfile_read_int(fp);
	cfread(Vclip, sizeof(vclip), Num_vclips, fp);

	int Num_effects = cfile_read_int(fp);
	cfread(Effects, sizeof(eclip), Num_effects, fp);

	int Num_wall_anims = cfile_read_int(fp);
	cfread(WallAnims, sizeof(wclip), Num_wall_anims, fp);

	int N_robot_types = cfile_read_int(fp);
	//Robot_info = robot_info[N_robot_types];

	cfread(Robot_info, sizeof(robot_info), N_robot_types, fp);

	int N_robot_joints = cfile_read_int(fp);
	cfread(Robot_joints, sizeof(jointpos), N_robot_joints, fp);

	int N_weapon_types = cfile_read_int(fp);
	//Weapon_info = new weapon_info[N_weapon_types];
	cfread(Weapon_info, sizeof(weapon_info), N_weapon_types, fp);

	int N_powerup_types = cfile_read_int(fp);
	cfread(Powerup_info, sizeof(powerup_type_info), N_powerup_types, fp);

	int N_polygon_models = cfile_read_int(fp);
	//Polygon_models = new polymodel[N_polygon_models];
	cfread(Polygon_models, sizeof(polymodel), N_polygon_models, fp);

	for (i = 0; i < N_polygon_models; i++)
	{
		// Polygon_models[i].model_data = malloc(Polygon_models[i].model_data_size);
		// Assert(Polygon_models[i].model_data != NULL);
		cfread(Polygon_models[i].model_data, sizeof(ubyte), Polygon_models[i].model_data_size, fp);
		//g3_init_polygon_model(Polygon_models[i].model_data);
	}

	cfread(Dying_modelnums, sizeof(int), N_polygon_models, fp);
	cfread(Dead_modelnums, sizeof(int), N_polygon_models, fp);

	t = cfile_read_int(fp);
	cfread(Gauges, sizeof(bitmap_index), t, fp);
	cfread(Gauges_hires, sizeof(bitmap_index), t, fp);

	t = cfile_read_int(fp);
	cfread(ObjBitmaps, sizeof(bitmap_index), t, fp);
	cfread(ObjBitmapPtrs, sizeof(ushort), t, fp);

	cfread(&only_player_ship, sizeof(player_ship), 1, fp);

	int Num_cockpits = cfile_read_int(fp);
	cfread(cockpit_bitmap, sizeof(bitmap_index), Num_cockpits, fp);

	int First_multi_bitmap_num = cfile_read_int(fp);

	int Num_reactors = cfile_read_int(fp);
	cfread(Reactors, sizeof(*Reactors), Num_reactors, fp);

	int Marker_model_num = cfile_read_int(fp);

}
*/