
#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include <dirent.h>

struct game
{

	struct button
	{
		char label[64];				// label
		int selected;					// button selected?
		int x;								// x position
		int y;								// y position
		int w;								// width
		int h;								// height
		int menu;							// menu that the button belongs to
	} button[2048], buttonlist[2048];
	
	struct input
	{
		int button;						// pressed mousebutton
		int click;						// button recently clicked?
		int x;								// x position (map)
		int y;								// y position (map)
		int x_input;					// x position (screen)
		int y_input;					// y position (screen)
		int x_prev;						// previous x position (screen)
		int y_prev;						// previous y position (screen)
		int x_offset;					// x offset to dragged objects
		int y_offset;					// y offset to dragged objects
		//int drag;							// mouse is dragging something?
		int point_dir;				// position relativ to player
		SDL_Rect clip[5];			// cursor positions on the sprite sheet
		SDL_Surface *sprite;	// sprite sheet with cursor images
	} mouse;
	
	struct object
	{
		double	x;						// x position
		double	x_Orig;				// x position on gamestart
		double	x_Prev;				// previous x position
		int			x_Unit;				// x position of current maptile
		int			x_Next_Unit;	// x position of next maptile
		double	y;						// y position
		double	y_Orig;				// y position on gamestart
		double	y_Prev;				// previous y position
		int			y_Unit;				// y position of current maptile
		int			y_Next_Unit;	// y position of next maptile
	
		int			direction;		// move direction
		int			direction_Prev;		// previous move direction
		int			w;						// width
		int			h;						// height
		char		type[128];		// type of object, e.g. player or box
		int			dragged;			// dragged by mouse?
		int			visible;			// visible
		int			passable;			// object can be passed by other objects
		SDL_Surface	*texture;	// object-texture
	} object[3000];
	
	struct tool_object
	{
		int			x;
		int			y;
		int			w;
		int			h;
		int			visible;
		int 		dragged;
		char		type[128];
		SDL_Surface *texture;
	} tool_object[2048];
	
	double timer_delta;			// time needed to render last frame
	int editor_copy;				// copy objects instead of moving?
	int enter_mapname;			// show menu to enter mapname (editor)
	int keys;								// number of collected keys
	int moves;							// time the player moved
	int num_tool_objects;		// numbers of objects to show in toolbar
	int num_buttons;				// number of buttons
	int num_buttonlist;			// number of buttonlist
	int num_objects;				// number of objects
	int paused;							// game paused?
	int pause_screen;				// pause screen visible?
	int quit;								// quit game
	int update;							// render new frame
	int screen_width;				// screen width
	int screen_height;			// screen height
	int level_width;				// level width
	int level_height;				// level height
	int fullscreen;					// fullscreen mode
	int menu;								// show this menu (game is also a "menu")
	int next_menu;
	int finished;						// map finished?
	int reset;							// reset map
	int keydown;						// key pressed?
	int toolbar_size;				// height of editor toolbar
	char map[512];					// name of the map
	char map_prev[512];					// name of the map
	unsigned current_key;		//
	DIR *map_dir;						// map directory
	SDL_Rect camera;				// area which should be rendered
	SDL_Surface *background;// background image
	SDL_Surface *button_l;	// left button image
	SDL_Surface *button_c;	// center button image
	SDL_Surface *button_r;	// right button image
	SDL_Surface *button_lh;	// left hover button image
	SDL_Surface *button_ch;	// center hover button image
	SDL_Surface *button_rh; // right hover button image
	SDL_Surface *paused_surface; // paused image
	SDL_Surface *texture[16];// object textures
	SDL_Surface *save_as;
	FILE *settings;					// settings file
	TTF_Font *font_yo_frankie;	// font
};

enum menus
{
	INGAME = 0,
	MAINMENU,
	MAPBROWSER,
	EDITOR
};

enum textures
{
	PLAYER = 0,
	END,
	BOX,
	DEATH,
	BREAK_BOX,
	BREAK_DEATH,
	BOX_DEATH,
	DEATH_BOX,
	MOVE_BOX,
	MOVE_DEATH,
	KEY,
	LOCK
};
