
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
#include "gameworld.h"
#include "input.h"
#include "graphic.h"
#include "editor.h"
#include "mapbrowser.h"

int main(int argc, char *argv[])
{
	int	i;
	char buf[512];
	SDL_Surface	*screen = NULL,
							*message = NULL;
	SDL_PixelFormat fmt;
	TTF_Font *tmp_font;
	SDL_Color text_color_black = {0,0,0};
	SDL_Color text_color_white = {255, 255, 255};
	//Mix_Music *music = NULL;
	//Mix_Chunk *scratch = NULL;
	Uint32 timer_start = 0, timer_last_frame = 0;
	struct game game;
	
	// initialisation
	if(init_game(&game))
		return 1;
	screen = SDL_GetVideoSurface();
	fmt = *(screen->format);
	///////////////
	// main loop //
	///////////////
	timer_start = SDL_GetTicks();
	timer_last_frame = SDL_GetTicks();
	
	while(!game.quit)
	{
		// update time
		game.timer_delta = ((double)(SDL_GetTicks() - (double)timer_last_frame) / 1000);
		timer_last_frame = SDL_GetTicks();
		
		///////////
		// Input //
		///////////
		read_input(&game);
		
		///////////
		// Logic //
		///////////
		if(game.finished)
		{
			//game.menu = MAPBROWSER;
			sscanf(game.map, "%d", &i);
			i++;
			sprintf(game.map, "%d", i);
			
			game.reset = 1;
			game.update = 1;
			game.finished = 0;
		}
		
		// Ingame
		if(game.menu == INGAME)
		{
			if(!game.paused || game.reset)
			{
				if(game.pause_screen)
					game.pause_screen = 0;
			
				// reset game
				if(game.reset)
				{
					if(!load_map(&game))
					{
						printf("Invalid map!\n");
						game.menu = MAINMENU;
					}
					game.reset = 0;
					game.paused = 0;
					game.update = 1;
					game.moves = 0;
					game.keys = 0;
				}
				
				// loop through all objects
				for(i=0; i<game.num_objects; i++)
				{
					// save old position
					game.object[i].x_Prev = game.object[i].x;
					game.object[i].y_Prev = game.object[i].y;
				
					// move object
					move(i, &game);
					
					// configure cursor
					if(!strcmp(game.object[i].type, "player"))
					{
						set_cursor (&game, i);
					}
					
					// check if screen refresh is needed
					if( (int)game.object[i].x != (int)game.object[i].x_Prev ||
							(int)game.object[i].y != (int)game.object[i].y_Prev)
					{
						game.update = 1;
					}
				}
				
				// set camera to player
				for(i=0; i<game.num_objects; i++)
				{
					if(!strcmp(game.object[i].type, "player"))
					{
						game.camera.x = (game.object[i].x + game.object[i].w / 2) - game.screen_width / 2;
						game.camera.y = (game.object[i].y + game.object[i].h / 2) - game.screen_height / 2;
					}
				}
				if(game.camera.x < 0)
					game.camera.x = 0;
				if(game.camera.x + game.camera.w > game.level_width)
					game.camera.x = game.level_width - game.camera.w;
				if(game.camera.y < 0)
					game.camera.y = 0;
				if(game.camera.y + game.camera.h > game.level_height)
					game.camera.y = game.level_height - game.camera.h;
				
			}
		}
		
		// Editor
		else if(game.menu == EDITOR)
		{
			game.num_buttonlist = 0;
			
			if(game.mouse.x_input != game.mouse.x_prev || game.mouse.y_input != game.mouse.y_prev)
				process_buttons(&game);
			
			game.paused = 0;
			if(game.reset)
			{
				if(!game.map[0])// && !game.enter_mapname)
					new_map(&game);
				else
					load_map(&game);

				// remove "player" from toolbox
				// if the map contains a player tile
				game.tool_object[PLAYER].visible = 1;
				for(i=0; i<game.num_objects; i++)
					if(!strcmp(game.object[i].type, "player"))
						game.tool_object[PLAYER].visible = 0;
											
				game.reset = 0;
			}
			set_map_properties(&game);
			scroll_map(&game);
			drag_objects(&game);
			handle_tools(&game);
			if(game.enter_mapname)
				enter_mapname(&game);
			
			if(strcmp(game.map_prev, game.map))
			{
				if(game.save_as)
					SDL_FreeSurface(game.save_as);
				game.save_as = TTF_RenderUTF8_Blended(game.font_yo_frankie, game.map, text_color_white);
				strcpy(game.map_prev, game.map);
				game.update = 1;
			}
		}
		
		// Mainmenu
		else if(game.menu == MAINMENU)
		{
			game.num_buttonlist = 0;
			
			if(game.mouse.x_input != game.mouse.x_prev || game.mouse.y_input != game.mouse.y_prev)
				process_buttons(&game);
			
			// reset cursor
			for(i=1; i<5; i++)
			{
				game.mouse.clip[i].x = 0; game.mouse.clip[1].y = 0;
				game.mouse.clip[i].x = 0;  game.mouse.clip[2].y = 0;
				game.mouse.clip[i].x = 0; game.mouse.clip[3].y = 0;
				game.mouse.clip[i].x = 0; game.mouse.clip[4].y = 0;
				game.mouse.point_dir = 0;
			}
		}
		
		// Mapbrowser
		else if(game.menu == MAPBROWSER)
		{
			process_list(&game);
			// reset cursor
			for(i=1; i<5; i++)
			{
				game.mouse.clip[i].x = 0; game.mouse.clip[1].y = 0;
				game.mouse.clip[i].x = 0;  game.mouse.clip[2].y = 0;
				game.mouse.clip[i].x = 0; game.mouse.clip[3].y = 0;
				game.mouse.clip[i].x = 0; game.mouse.clip[4].y = 0;
				game.mouse.point_dir = 0;
			}
		}
		
		if(game.mouse.x_input != game.mouse.x_prev || game.mouse.y_input != game.mouse.y_prev)
		{
			game.mouse.x_prev = game.mouse.x_input;
			game.mouse.y_prev = game.mouse.y_input;
			//if(SDL_GetTicks() % 2 == 0)
				game.update = 1;
		}

		////////////
		// Render //
		////////////
		
		// don't update screen if it is not required
		if(game.update)
		{
			//printf("updating screen\n");
			//SDL_SetAlpha(wall_surface, SDL_SRCALPHA, 0); 
			//printf("%d | %d » %d | %d || %d\n", game.mouse.x_prev, game.mouse.y_prev, game.mouse.x, game.mouse.y, game.mouse.click);
			if(game.menu == INGAME)
			{
				render_game(&game);
				
				message = SDL_CreateRGBSurface(0, 300, 50, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
				SDL_SetAlpha(message, SDL_SRCALPHA, 150);
				SDL_FillRect(message, &message->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
				apply_surface(0, game.screen_height - message->h, message, screen, NULL);
				SDL_FreeSurface(message);
				
				sprintf(buf, "Moves: %d  Keys: %d", game.moves, game.keys);
				message = TTF_RenderUTF8_Blended(game.font_yo_frankie, buf, text_color_black);
				apply_surface(5, game.screen_height - message->h - 5, message, screen, NULL);
				SDL_FreeSurface(message);
			}
			else if(game.menu == EDITOR)
			{
				render_game(&game);
				/*tmp_font = TTF_OpenFont("data/fonts/yo_frankie.ttf", 18);
				message = TTF_RenderUTF8_Blended(tmp_font, "By Sören Wegener", text_color_black);
				apply_surface(game.screen_width / 2 - message->w / 2, 150, message, screen, NULL);
				TTF_CloseFont(tmp_font);
				SDL_FreeSurface(message);*/
				render_toolbox(&game);
				render_buttons(&game);
				if(game.enter_mapname)
				{
					i = 450;
					
					if(game.save_as && game.save_as->w > 430)
							i = game.save_as->w + 20;
					
					message = SDL_CreateRGBSurface(0, i, 70, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
					SDL_SetAlpha(message, SDL_SRCALPHA, 180);
					SDL_FillRect(message, &message->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
					apply_surface(game.screen_width / 2 - message->w / 2, game.screen_height / 2 - message->h / 2, message, screen, NULL);
					SDL_FreeSurface(message);
					
					message = TTF_RenderUTF8_Blended(game.font_yo_frankie, "Enter Mapname and press Enter", text_color_white);
					apply_surface(game.screen_width / 2 - message->w / 2, game.screen_height / 2 - message->h / 2 - 15, message, screen, NULL);
					if(game.save_as)
						apply_surface(game.screen_width / 2 - game.save_as->w / 2, game.screen_height / 2 - game.save_as->h / 2 + 15, game.save_as, screen, NULL);
				}
			}
			else if(game.menu == MAINMENU)
			{
				SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
				render_buttons(&game);
				
				tmp_font = TTF_OpenFont("data/fonts/yo_frankie.ttf", 56);
				message = TTF_RenderUTF8_Blended(tmp_font, "<Lustiger Spielname hier>", text_color_black);
				apply_surface(game.screen_width / 2 - message->w / 2, 50, message, screen, NULL);
				TTF_CloseFont(tmp_font);
				SDL_FreeSurface(message);
				
				tmp_font = TTF_OpenFont("data/fonts/yo_frankie.ttf", 18);
				message = TTF_RenderUTF8_Blended(tmp_font, "By Sören Wegener", text_color_black);
				apply_surface(game.screen_width / 2 - message->w / 2, 150, message, screen, NULL);
				TTF_CloseFont(tmp_font);
				SDL_FreeSurface(message);
				
				tmp_font = TTF_OpenFont("data/fonts/yo_frankie.ttf", 14);
				message = TTF_RenderUTF8_Blended(tmp_font, "Yo Frankie font created by Pablo Vazquez and converted by Mango Jambo using FontForge 2.0", text_color_black);
				apply_surface(0, game.screen_height - message->h, message, screen, NULL);
				TTF_CloseFont(tmp_font);
				SDL_FreeSurface(message);
			}
			else if(game.menu == MAPBROWSER)
			{
				SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
				render_list(&game);
			}
			
			render_cursor(&game);
						
			// refresh screen
			SDL_Flip(screen);
			game.update = 0;
		}
		
		// needed to prevent 100% CPU usage
		SDL_Delay(0);
	}
	
	cleanup(&game);
	return 0;
}
