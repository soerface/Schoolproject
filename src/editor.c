#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "gamestruct.h"
#include "editor.h"

void enter_mapname(struct game *game)
{
	int len;
	
	if(game->keydown)
	{
		if(	(strlen(game->map) - 1 < sizeof(game->map) || !game->map[0]) &&
				((game->current_key == (Uint16)' ') ||
				(game->current_key == (Uint16)'.') ||
				(game->current_key == (Uint16)'-') ||
				(game->current_key == (Uint16)'_') ||
				(game->current_key == (Uint16)'?') ||
				(game->current_key == (Uint16)'!') ||
				(game->current_key == (Uint16)'#') ||
				(game->current_key == (Uint16)'(') ||
				(game->current_key == (Uint16)')') ||
				((game->current_key >= (Uint16)'0') && (game->current_key <= (Uint16)'9')) ||
				((game->current_key >= (Uint16)'A') && (game->current_key <= (Uint16)'Z')) ||
				((game->current_key >= (Uint16)'a') && (game->current_key <= (Uint16)'z'))))
		{
			len = strlen(game->map);
			game->map[len] = game->current_key;
			game->map[len+1] = '\0';
		}
		if(strlen(game->map) > 0 && game->current_key == SDLK_BACKSPACE)
			game->map[strlen(game->map)-1] = '\0';
	}
}

void drag_objects(struct game *game)
{
	int i, j;
	double x_prev, y_prev;
	
	if(!game->enter_mapname) // refuse dragging objects when player should enter mapname
	{
		for(i=0; i<game->num_objects; i++)
		{
			if(	game->mouse.click && game->mouse.button == 1 &&
					game->mouse.x >= game->object[i].x && game->mouse.x <= game->object[i].x + game->object[i].w &&
					game->mouse.y >= game->object[i].y && game->mouse.y <= game->object[i].y + game->object[i].h)
			{
				// copy object (but not player object)
				if(game->editor_copy && strcmp(game->object[i].type, "player"))
				{
					// glue new object to mouse
					game->object[game->num_objects].dragged = 1;
					game->mouse.x_offset = game->mouse.x - game->object[i].x;
					game->mouse.y_offset = game->mouse.y - game->object[i].y;
				
					// copy object type and texture (pointer) to new object
					game->object[game->num_objects].texture = game->object[i].texture;
					strcpy(game->object[game->num_objects].type, game->object[i].type);
				
					game->object[game->num_objects].x_Prev = -1;
					game->object[game->num_objects].y_Prev = -1;
					game->object[game->num_objects].w = 32;
					game->object[game->num_objects].h = 32;
					game->object[game->num_objects].visible = 1;
					game->num_objects++;
					if(game->num_objects >= sizeof(game->object))
						game->num_objects = sizeof(game->object) - 1;
				}
				// move object
				else
				{
					game->object[i].dragged = 1;
					game->mouse.x_offset = game->mouse.x - game->object[i].x;
					game->mouse.y_offset = game->mouse.y - game->object[i].y;
				}
			}
			else if(game->mouse.button != 1)
			{
				x_prev = game->object[i].x;
				y_prev = game->object[i].y;
				
				// hide objects which are out of map
				if( game->object[i].visible &&
						(game->object[i].x + game->object[i].w / 2 > game->level_width ||
						game->object[i].y + game->object[i].h / 2 > game->level_height ||
						game->object[i].x + game->object[i].w / 2 < 0 ||
						game->object[i].y + game->object[i].h / 2 < 0))
				{
					game->object[i].visible = 0;
					
					// show toolbar object "player" if all player object were removed
					game->tool_object[PLAYER].visible = 1;
					for(j=0; j<game->num_objects; j++)
						if(!strcmp(game->object[j].type, "player") && game->object[j].visible)
							game->tool_object[PLAYER].visible = 0;
								
				}
			

				game->object[i].x_Unit = ((game->object[i].x + game->object[i].w / 2) / 32);
				game->object[i].y_Unit = ((game->object[i].y + game->object[i].h / 2) / 32);
				for(j=0; j<game->num_objects; j++)
				{
					if(	i != j && game->object[j].visible &&
							game->object[i].x_Unit == game->object[j].x_Unit &&
							game->object[i].y_Unit == game->object[j].y_Unit)
					{
						if(	i == game->num_objects-1 && (game->object[i].x_Prev < 0 && game->object[i].y_Prev < 0))
							game->num_objects--;
						else
						{
							game->object[i].x_Unit = game->object[i].x_Prev;
							game->object[i].y_Unit = game->object[i].y_Prev;
						}
					}
				}
			
				game->object[i].x = game->object[i].x_Unit * 32;
				game->object[i].y = game->object[i].y_Unit * 32;
				game->object[i].x_Prev = game->object[i].x_Unit;
				game->object[i].y_Prev = game->object[i].y_Unit;
			
				game->object[i].dragged = 0;
				if(	x_prev != game->object[i].x ||
						y_prev != game->object[i].y)
				{
					game->update = 1;
				}
			}
			if(game->object[i].dragged )
			{
				game->object[i].x = game->mouse.x - game->mouse.x_offset;
				game->object[i].y = game->mouse.y - game->mouse.y_offset;
			}
		}
	}
}

void handle_tools(struct game *game)
{
	int i;

	if(!game->enter_mapname) // refuse adding objects when player should enter mapname
	{
		for(i=0; i<game->num_tool_objects; i++)
		{	
			// pickup an object from the toolbar
			if(	game->mouse.click && game->mouse.button == 1 &&
					game->tool_object[i].visible &&
					game->mouse.x_input >= game->tool_object[i].x && game->mouse.x_input <= game->tool_object[i].x + game->tool_object[i].w &&
					game->mouse.y_input >= game->tool_object[i].y && game->mouse.y_input <= game->tool_object[i].y + game->tool_object[i].h)
			{
				// glue new object to mouse
				game->object[game->num_objects].dragged = 1;
				game->mouse.x_offset = game->mouse.x_input - game->tool_object[i].x;
				game->mouse.y_offset = game->mouse.y_input - game->tool_object[i].y;
				
				// copy tool_object type and texture (pointer) to new object
				game->object[game->num_objects].texture = game->tool_object[i].texture;
				strcpy(game->object[game->num_objects].type, game->tool_object[i].type);
				
				// hide tool_object if it has type "player"
				if(!strcmp(game->tool_object[i].type, "player"))
					game->tool_object[i].visible = 0;
				game->object[game->num_objects].x_Prev = -1;
				game->object[game->num_objects].y_Prev = -1;
				game->object[game->num_objects].w = 32;
				game->object[game->num_objects].h = 32;
				game->object[game->num_objects].visible = 1;
				game->num_objects++;
				if(game->num_objects >= sizeof(game->object))
					game->num_objects = sizeof(game->object) - 1;
			}
		}
	}
}

void new_map(struct game *game)
{
	game->level_width = 10 * 32;
	game->level_height = 10 * 32;
	game->num_objects = 0;
	strcpy(game->map, "unnamed");
}

void set_map_properties(struct game *game)
{
	int i, inv = 1;
	Uint8 *keystates = SDL_GetKeyState(NULL);
	if(game->keydown)
	{
		if(keystates[SDLK_LSHIFT])
			inv = -1;
		if(keystates[SDLK_RIGHT])
			game->level_width += 32 * inv;;
		if(keystates[SDLK_DOWN])
			game->level_height += 32 * inv;
		if(keystates[SDLK_LEFT])
			game->level_width += 32 * inv;
		if(keystates[SDLK_UP])
			game->level_height += 32 * inv;
			
		for(i=0; i<game->num_objects; i++)
		{
			if(keystates[SDLK_LEFT])
				game->object[i].x_Unit += inv;
			if(keystates[SDLK_UP])
				game->object[i].y_Unit += inv;
			game->object[i].x = game->object[i].x_Unit * 32;
			game->object[i].y = game->object[i].y_Unit * 32;
			game->object[i].x_Prev = game->object[i].x_Unit;
			game->object[i].y_Prev = game->object[i].y_Unit;
		}
				
				
		if(keystates[SDLK_RETURN] && game->enter_mapname)
			save_map(game);
		
		
		game->update = 1;
	}
}

void save_map(struct game *game)
{
	int i;
	FILE *map;
	char path[512];
	
	if(game->map[0] != '\0')
	{
		game->enter_mapname = 0;
		sprintf(path, "data/maps/%s", game->map);
	
		map = fopen(path, "w+");
		fseek(map, 0, SEEK_END);
		fprintf(map, "level_size %d %d\n", game->level_width / 32, game->level_height / 32);
		for(i=0; i<game->num_objects; i++)
			if(game->object[i].visible)
				fprintf(map, "%s %d %d\n", game->object[i].type, game->object[i].x_Unit, game->object[i].y_Unit);
		fclose(map);
		printf("Map saved as \"%s\"\n", game->map);
	}
}

void scroll_map(struct game *game)
{
	int x_prev = game->camera.x, y_prev = game->camera.y;
	
	if(game->level_width >= game->screen_width)
	{
		// x scroll on middle mouseclick
		if(game->mouse.button == 2)
		{
			if(game->mouse.click)
			{
				game->mouse.x_offset = game->camera.x + game->mouse.x_input;
			}
			game->camera.x = game->mouse.x_offset - game->mouse.x_input;
		}
		// x scroll on border
		else
		{
			if(game->mouse.x_input < 16)
				game->camera.x--;
			else if(game->mouse.x_input > game->screen_width - 16)
				game->camera.x++;
		}
		if(game->camera.x < 0)
			game->camera.x = 0;
		if(game->camera.x + game->camera.w > game->level_width)
			game->camera.x = game->level_width - game->camera.w;
	}	
	
	if(game->level_height >= game->screen_height)
	{
		// y scroll on middle mouseclick
		if(game->mouse.button == 2)
		{
			if(game->mouse.click)
			{
				game->mouse.y_offset = game->camera.y + game->mouse.y_input;
			}
			game->camera.y = game->mouse.y_offset - game->mouse.y_input;
		}
		// y scroll on border
		else
		{
			if(game->mouse.y_input < 16)
				game->camera.y--;
			else if(game->mouse.y_input > game->screen_height - 16)
				game->camera.y++;
		}
		
		if(game->camera.y < game->toolbar_size * -1)
			game->camera.y = game->toolbar_size * -1;
		if(game->camera.y + game->camera.h > game->level_height)
			game->camera.y = game->level_height - game->camera.h;
	}
	
	if(x_prev != game->camera.x || y_prev != game->camera.y)
		game->update = 1;
}
