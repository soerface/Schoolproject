
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include "gamestruct.h"
#include "core.h"
#include "graphic.h"
#include "editor.h"

void cleanup(struct game *game)
{
	printf("clean up\n");
	SDL_FreeSurface(game->background);
	SDL_FreeSurface(game->button_l);
	SDL_FreeSurface(game->button_c);
	SDL_FreeSurface(game->button_r);
	SDL_FreeSurface(game->button_lh);
	SDL_FreeSurface(game->button_ch);
	SDL_FreeSurface(game->button_rh);
	TTF_CloseFont(game->font_yo_frankie);
	//Mix_FreeMusic(music);
	TTF_Quit();
	Mix_CloseAudio();
	SDL_Quit();
}

int init_game(struct game *game)
{
	SDL_Surface *app_icon = NULL,
							*screen = 0;
	//SDL_Color text_color = {255,255,255};
	char map_path[512];
	int i;
	
	load_settings(game);
	
	SDL_Init(SDL_INIT_EVERYTHING);
	if(game->fullscreen)
		screen = SDL_SetVideoMode(game->screen_width, game->screen_height, 32, SDL_FULLSCREEN | SDL_SRCALPHA);	
	else
		screen = SDL_SetVideoMode(game->screen_width, game->screen_height, 32, SDL_SWSURFACE /*| SDL_RESIZABLE*/ | SDL_SRCALPHA);
	if(screen == NULL)
	{
		printf("Can't initialize screen\n");
		return 1;
	}
	if (TTF_Init() == -1)
	{
		printf("Can't initialize font\n");
		return 1;
	}
	if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		printf("Can't initialize mixer\n");
		return 1;
	} 
	
	// load images and fonts
	game->font_yo_frankie = TTF_OpenFont("data/fonts/yo_frankie.ttf", 26);
	game->background = load_image("bg.png");
	game->paused_surface = load_image("paused.png");
	game->texture[PLAYER] = load_image("player.png");
	game->mouse.sprite = load_image("cursor.png");
	game->texture[END] = load_image("end.png");
	game->texture[BOX] = load_image("box.png");
	game->texture[DEATH] = load_image("death.png");
	game->texture[BREAK_BOX] = load_image("break_box.png");
	game->texture[BREAK_DEATH] = load_image("break_death.png");
	game->texture[BOX_DEATH] = load_image("box_death.png");
	game->texture[DEATH_BOX] = load_image("death_box.png");
	game->texture[MOVE_BOX] = load_image("move_box.png");
	game->texture[MOVE_DEATH] = load_image("move_death.png");
	game->texture[KEY] = load_image("key.png");
	game->texture[LOCK] = load_image("lock.png");
	game->button_l = load_image("button_l.png");
	game->button_c = load_image("button_c.png");
	game->button_r = load_image("button_r.png");
	game->button_lh = load_image("button_lh.png");
	game->button_ch = load_image("button_ch.png");
	game->button_rh = load_image("button_rh.png");
	app_icon = load_image("app_icon.png");
	// music = Mix_LoadMUS("music.wav");
	
	
	// set button labels
	strcpy(game->button[0].label, "Start Game");
	strcpy(game->button[1].label, "Editor");
	strcpy(game->button[2].label, "Quit");
	strcpy(game->button[3].label, "Load");
	strcpy(game->button[4].label, "New");
	strcpy(game->button[5].label, "Save");
	
	game->num_buttons = 6;
	
	process_buttons(game);

	// set button positions
	game->button[0].x = game->screen_width / 2 - game->button[0].w / 2;
	game->button[0].y = game->screen_height / 2 - (game->button[0].h + game->button[1].h + game->button[2].h + 10) / 2 + 50;
	game->button[1].x = game->screen_width / 2 - game->button[1].w / 2;
	game->button[1].y = game->button[0].h + game->button[0].y + 10;
	game->button[2].x = game->screen_width / 2 - game->button[2].w / 2;
	game->button[2].y = game->button[1].h + game->button[1].y + 10;
	game->button[3].x = game->screen_width - game->button[3].w;
	game->button[3].y = 10;
	game->button[4].x = game->screen_width - game->button[4].w - game->button[3].w - 10;
	game->button[4].y = 10;
	game->button[5].x = game->screen_width - game->button[5].w - game->button[3].w - game->button[4].w - 20;
	game->button[5].y = 10;
	
	// which button goes with which menu?
	game->button[0].menu = MAINMENU;
	game->button[1].menu = MAINMENU;
	game->button[2].menu = MAINMENU;
	game->button[3].menu = EDITOR;
	game->button[4].menu = EDITOR;
	game->button[5].menu = EDITOR;
	
	strcpy(map_path, "./data/maps");
	SDL_WM_SetCaption("Spiel", NULL);
	SDL_WM_SetIcon(app_icon, NULL);
	SDL_WarpMouse(game->button[0].x + game->button[0].w / 2, game->button[0].y + game->button[0].h / 2) ;
	SDL_ShowCursor(SDL_DISABLE);
	game->camera.x = 0; game->camera.y = 0; game->camera.w = game->screen_width; game->camera.h = game->screen_height;
	game->quit = 0; game->paused = 0; game->pause_screen = 0; game->update = 1;
	game->mouse.button = 0; game->mouse.click = 0; /*game->mouse.drag = -1;*/ game->num_objects = 0;
	game->menu = MAINMENU; game->finished = 0; game->reset = 1; game->num_buttonlist = 0; game->enter_mapname = 0;
	game->save_as = NULL;
	
	strcpy(map_path, "./data/maps");
	game->map_dir = opendir(map_path);
	game->map[0] = '\0';
	
	game->mouse.clip[0].x = 32; game->mouse.clip[0].y = 0; game->mouse.clip[0].w = 32; game->mouse.clip[0].h = 32;
	game->mouse.clip[1].x = 64; game->mouse.clip[1].y = 0; game->mouse.clip[1].w = 32; game->mouse.clip[1].h = 32;
	game->mouse.clip[2].x = 0;  game->mouse.clip[2].y = 32; game->mouse.clip[2].w = 32; game->mouse.clip[2].h = 32;
	game->mouse.clip[3].x = 32; game->mouse.clip[3].y = 32; game->mouse.clip[3].w = 32; game->mouse.clip[3].h = 32;
	game->mouse.clip[4].x = 64; game->mouse.clip[4].y = 32; game->mouse.clip[4].w = 32; game->mouse.clip[4].h = 32;
	
	
	// Editor toolbox
	game->num_tool_objects = 12;
	game->toolbar_size = game->button_c->h + 20;
	for(i=0; i<game->num_tool_objects; i++)
	{
		game->tool_object[i].x = 10 + 40 * i;
		game->tool_object[i].y = 10;
		game->tool_object[i].w = 32;
		game->tool_object[i].h = 32;
		game->tool_object[i].texture = game->texture[i];
		game->tool_object[i].visible = 1;
		game->tool_object[i].dragged = 0;
	}
	strcpy(game->tool_object[PLAYER].type, "player");
	strcpy(game->tool_object[END].type, "end");
	strcpy(game->tool_object[BOX].type, "box");
	strcpy(game->tool_object[DEATH].type, "death");
	strcpy(game->tool_object[BREAK_BOX].type, "break_box");
	strcpy(game->tool_object[BREAK_DEATH].type, "break_death");
	strcpy(game->tool_object[BOX_DEATH].type, "box_death");
	strcpy(game->tool_object[DEATH_BOX].type, "death_box");
	strcpy(game->tool_object[MOVE_BOX].type, "move_box");
	strcpy(game->tool_object[MOVE_DEATH].type, "move_death");
	strcpy(game->tool_object[KEY].type, "key");
	strcpy(game->tool_object[LOCK].type, "lock");
	
	return 0;
}

void load_settings(struct game *game)
{
	char option[512], line[512];
	int value, i = 0;
	game->settings = fopen("settings.cfg", "r");
	printf("Loading \"settings.cfg\"\n");
	if(game->settings)
	{
		while(fgets(line, 64, game->settings) != NULL)
		{
			sscanf(line, "%s %d", option, &value);
			printf("Setting \"%s\" to \"%d\"\n", option, value);
			if(!strcmp(option, "screen_width"))
				game->screen_width = value;
			else if(!strcmp(option, "screen_height"))
				game->screen_height = value;
			else if(!strcmp(option, "fullscreen"))
				game->fullscreen = value;
			i++;
		}
	}
	else
	{
		printf("Failed to open \"settings.cfg\", using default settings...\n");
		game->screen_width = 800;
		game->screen_height = 600;
		game->fullscreen = 0;
	}
}

int load_map(struct game *game)
{
	char setting[512], buf[512], line[512];
	int value1, value2, i = 0, state = 0, j;
	FILE *level_file = NULL;
	
	
	sprintf(buf, "data/maps/%s", game->map);
	//printf("Loading level \"%s\"\n", buf);
	level_file = fopen(buf, "r");
	state = 1;
	if(level_file == NULL)
	{
		printf("Error opening map\n");
		state = 0;
	}
	else
	{
		while(fgets(line, 64, level_file) != NULL)
		{
			sscanf(line, "%s %d %d", setting, &value1, &value2);
			//printf("%s » %d %d\n", setting, value1, value2);
			if(value1 < 0 || value2 < 0)
				state = 0;
			if(!strcmp(setting, "level_size"))
			{
				game->level_width = value1 * 32;
				game->level_height = value2 * 32;
			}
			else
			{
				game->object[i].w = 32;
				game->object[i].h = 32;
				game->object[i].x = value1 * 32;
				game->object[i].y = value2 * 32;
				game->object[i].x_Orig = game->object[i].x;
				game->object[i].y_Orig = game->object[i].y;
				game->object[i].x_Unit = ((int)game->object[i].x + game->object[i].w / 2) / 32;
				game->object[i].x_Next_Unit = game->object[i].x_Unit;
				game->object[i].y_Unit = ((int)game->object[i].y + game->object[i].h / 2) / 32;
				game->object[i].y_Next_Unit = game->object[i].y_Unit;
				game->object[i].direction = 0;
				game->object[i].direction_Prev = 0;
				game->object[i].visible = 1;
				game->object[i].passable = 0;
				if(!strcmp(setting, "player"))
					game->object[i].texture = game->texture[PLAYER];
				else if(!strcmp(setting, "end"))
					game->object[i].texture = game->texture[END];
				else if(!strcmp(setting, "box"))
					game->object[i].texture = game->texture[BOX];
				else if(!strcmp(setting, "death"))
					game->object[i].texture = game->texture[DEATH];
				else if(!strcmp(setting, "break_box"))
					game->object[i].texture = game->texture[BREAK_BOX];
				else if(!strcmp(setting, "break_death"))
					game->object[i].texture = game->texture[BREAK_DEATH];
				else if(!strcmp(setting, "box_death"))
					game->object[i].texture = game->texture[BOX_DEATH];
				else if(!strcmp(setting, "death_box"))
					game->object[i].texture = game->texture[DEATH_BOX];
				else if(!strcmp(setting, "move_box"))
					game->object[i].texture = game->texture[MOVE_BOX];
				else if(!strcmp(setting, "move_death"))
					game->object[i].texture = game->texture[MOVE_DEATH];
				else if(!strcmp(setting, "key"))
				{
					game->object[i].passable = 1;
					game->object[i].texture = game->texture[KEY];
				}
				else if(!strcmp(setting, "lock"))
					game->object[i].texture = game->texture[LOCK];
				else
					state = 0;
				strcpy(game->object[i].type, setting);
				i++;
			}
		}
		fclose(level_file);
	}
	game->num_objects = i;
	
	// validate map
	j = 0;
	for(i=0; i<game->num_objects; i++)
	{
		// object placed out of map?
		if(	game->object[i].x_Unit >= game->level_width / 32 ||
				game->object[i].x_Unit < 0 ||
				game->object[i].y_Unit >= game->level_height / 32 ||
				game->object[i].y_Unit < 0)
		{
			state = 0;
		}
		
		// count number of "player" tiles
		if(!strcmp(game->object[i].type, "player"))
			j++;
	}
	
	// player tile used exactly one time?
	if(j != 1)
		state = 0;
	return state;
}

void process_buttons(struct game *game)
{
	SDL_Surface *message = NULL;
	SDL_Color color = {0,0,0};
	int i, tmp;
		
	for(i=0; i<game->num_buttons; i++)
	{
		message = TTF_RenderText_Solid(game->font_yo_frankie, game->button[i].label, color);
		tmp = game->button[i].selected;
		if(	game->mouse.x_input >= game->button[i].x - game->button_l->w && game->mouse.x_input <= game->button[i].x + message->w + game->button_r->w &&
				game->mouse.y_input >= game->button[i].y && game->mouse.y_input <= game->button[i].y + game->button_c->h && game->button[i].menu == game->menu)
		{
			game->button[i].selected = 1;
		}
		else
			game->button[i].selected = 0;
			
		if(tmp != game->button[i].selected && game->button[i].menu == game->menu)
			game->update = 1;
			
		game->button[i].w = message->w + game->button_l->w + game->button_r->w;
		if(message->h > game->button_c->h)
			game->button[i].h = message->h;
		else
			game->button[i].h = game->button_c->h;
			
		SDL_FreeSurface(message);
	}
}

