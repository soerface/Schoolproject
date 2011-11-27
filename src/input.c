#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include "gamestruct.h"
#include "core.h"

void handle_mouseclick(struct game *game)
{
	int i;
	if(!game->enter_mapname && game->mouse.click == 1)
	{
		for(i=0; i<game->num_buttons; i++)
		{
			if(game->button[i].selected)
			{
				switch(i)
				{
					//	Select Map
					case 0:
						game->menu = MAPBROWSER;
						game->next_menu = INGAME;
						break;
					
					// Editor
					case 1:
						game->menu = EDITOR;
						game->next_menu = EDITOR;
						game->reset = 1;
						break;
		
					// Quit
					case 2:
						game->quit = 1;
						break;
						
					//	Load Map
					case 3:
						game->menu = MAPBROWSER;
						game->next_menu = EDITOR;
						break;
						
					//	New Map
					case 4:
						game->menu = EDITOR;
						game->next_menu = EDITOR;
						game->map[0] = '\0';
						game->reset = 1;
						break;
						
					//	Save as...
					case 5:
						game->menu = EDITOR;
						game->next_menu = EDITOR;
						game->enter_mapname = 1;
						break;
		
					default: ;
				}
				game->button[i].selected = 0;
				game->update = 1;
			}
		}
		for(i=0; i<game->num_buttonlist; i++)
		{
			if(game->buttonlist[i].selected)
			{
				game->reset = 1;
				strcpy(game->map, game->buttonlist[i].label);
				/*if(!load_map(game))
					printf("Invalid map!\n");
				else*/
					game->menu = game->next_menu;
				game->update = 1;
				game->buttonlist[i].selected = 0;
			}
		}
		switch(game->mouse.point_dir)
		{
			case 1:
				for(i=0; i<game->num_objects; i++)
					if(!strcmp(game->object[i].type, "player") && !game->paused)
						game->object[i].direction = 1;
				break;
		
			case 2:
				for(i=0; i<game->num_objects; i++)
					if(!strcmp(game->object[i].type, "player") && !game->paused)
						game->object[i].direction = 2;
				break;
		
			case 3:
				for(i=0; i<game->num_objects; i++)
					if(!strcmp(game->object[i].type, "player") && !game->paused)
						game->object[i].direction = 3;
				break;
		
			case 4:
				for(i=0; i<game->num_objects; i++)
					if(!strcmp(game->object[i].type, "player") && !game->paused)
						game->object[i].direction = 4;
		}
	}
}

void read_input(struct game *game)
{
	int i;
	SDL_Event event;
	//SDL_Surface *screen = SDL_GetVideoSurface();
	Uint8 *keystates = SDL_GetKeyState(NULL);
	
	game->mouse.click = 0;
	game->keydown = 0;
	game->current_key = 0;
	SDL_EnableUNICODE(SDL_ENABLE);
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				game->quit = 1;
				break;
			
			case SDL_KEYDOWN:
		
				if(keystates[SDLK_LCTRL])// && keystates[SDLK_LSHIFT])
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_s:
							if(game->menu == EDITOR)
								game->enter_mapname = 1;
							break;
						
						default: ;
					}
				}
				else
				{
					switch(event.key.keysym.sym)
					{
						case SDLK_p:
							if(game->paused == 0)
								game->paused = 1;
							else
								game->paused = 0;

							game->update = 1;
							break;
						
						case SDLK_r:
							if(game->menu == INGAME)
								game->reset = 1;
							break;
						
						case SDLK_ESCAPE:
							if(game->enter_mapname)
								game->enter_mapname = 0;
							else if(game->menu == INGAME || game->menu == MAPBROWSER || game->menu == EDITOR)
								game->menu = MAINMENU;
							else if(game->menu == MAINMENU)
								game->quit = 1;
							game->update = 1;
							break;
						
						case SDLK_UP:
							if(game->menu == INGAME)
								for(i=0; i<game->num_objects; i++)
									if(!strcmp(game->object[i].type, "player") && !game->paused && !game->object[i].direction)
										game->object[i].direction = 1;
							break;
						
						case SDLK_RIGHT:
							if(game->menu == INGAME)
								for(i=0; i<game->num_objects; i++)
									if(!strcmp(game->object[i].type, "player") && !game->paused && !game->object[i].direction)
										game->object[i].direction = 2;
							break;
						
						case SDLK_DOWN:
							if(game->menu == INGAME)
								for(i=0; i<game->num_objects; i++)
									if(!strcmp(game->object[i].type, "player") && !game->paused && !game->object[i].direction)
										game->object[i].direction = 3;
							break;
						
						case SDLK_LEFT:
							if(game->menu == INGAME)
								for(i=0; i<game->num_objects; i++)
									if(!strcmp(game->object[i].type, "player") && !game->paused && !game->object[i].direction)
										game->object[i].direction = 4;
							break;
						
						default: ;
					}
				}
				game->keydown = 1;
				game->current_key = event.key.keysym.unicode;
				break;
			
			/*case SDL_VIDEORESIZE:
				game->screen_width = event.resize.w;
				game->screen_height = event.resize.h;
				screen = SDL_SetVideoMode(game->screen_width, game->screen_height, 32, SDL_SWSURFACE | SDL_RESIZABLE);
				game->update = 1;
				break;*/
		
			case SDL_MOUSEMOTION:
				game->mouse.x_input = event.motion.x;
				game->mouse.y_input = event.motion.y;
				break;
			
			case SDL_MOUSEBUTTONDOWN:
				game->mouse.x_input = event.button.x;
				game->mouse.y_input = event.button.y;
				game->mouse.button = event.button.button;
				game->mouse.click = 1;
				break;
			
			case SDL_MOUSEBUTTONUP:
				game->mouse.x_input = event.button.x;
				game->mouse.y_input = event.button.y;
				game->mouse.button = 0;
				game->mouse.click = -1;
				break;
			
			/*case SDL_ACTIVEEVENT:
				if(event.active.state == SDL_APPINPUTFOCUS) //  SDL_APPACTIVE
				{
					if(!event.active.gain)
						game->paused = 1;
					game->update = 1;
				}
				break;*/
			default: ;
		}
	}
	SDL_EnableUNICODE(SDL_DISABLE);
	if(keystates[SDLK_LCTRL] || keystates[SDLK_RCTRL])
		game->editor_copy = 1;
	else
		game->editor_copy = 0;
	
	if(game->screen_width > game->level_width)
		game->mouse.x = game->mouse.x_input + (game->level_width - game->screen_width) / 2;
	else
		game->mouse.x = game->mouse.x_input + game->camera.x;
	if(game->screen_height > game->level_height)
		game->mouse.y = game->mouse.y_input + (game->level_height - game->screen_height) / 2;
	else
		game->mouse.y = game->mouse.y_input + game->camera.y;
	handle_mouseclick(game);
}
