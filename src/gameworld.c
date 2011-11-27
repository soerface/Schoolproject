
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "gamestruct.h"

int collision(struct game *game, int A, int B)
{
	int collide = 0;
	if(game->object[A].x_Unit == game->object[B].x_Unit && game->object[A].y_Unit == game->object[B].y_Unit)
		collide = 1;
	return collide;
}

int hit(struct game *game, int x, int y, int w, int h)
{
	int hitted = 0;
	
	if(	game->mouse.x_input >= x &&
			game->mouse.x_input <= x + w &&
			game->mouse.y_input <= y + h)
		hitted = 1;
	return hitted;
}

void move(int obj_num, struct game *game)
{
	int i, orig_x_Unit, orig_y_Unit, tmp1, tmp2;
	
	if(	game->object[obj_num].x_Unit == game->object[obj_num].x_Next_Unit &&
			game->object[obj_num].y_Unit == game->object[obj_num].y_Next_Unit)
	{
		// save current position
		orig_x_Unit = game->object[obj_num].x_Unit;
		orig_y_Unit = game->object[obj_num].y_Unit;

		// find the next available position
		while(game->object[obj_num].direction != 0)
		{
			game->object[obj_num].x_Next_Unit = game->object[obj_num].x_Unit;
			game->object[obj_num].y_Next_Unit = game->object[obj_num].y_Unit;
			if(game->object[obj_num].direction == 1)
				game->object[obj_num].y_Unit--;
			else if(game->object[obj_num].direction == 2)
				game->object[obj_num].x_Unit++;
			else if(game->object[obj_num].direction == 3)
				game->object[obj_num].y_Unit++;
			else if(game->object[obj_num].direction == 4)
				game->object[obj_num].x_Unit--;

			// handle collision with other objects
			for(i=0; i<game->num_objects; i++)
			{
				if(game->object[i].visible)
				{
					tmp1 = game->object[i].x_Unit;
					tmp2 = game->object[i].y_Unit;
					game->object[i].x_Unit = game->object[i].x_Next_Unit;
					game->object[i].y_Unit = game->object[i].y_Next_Unit;
					if(	obj_num != i &&
							game->object[i].visible &&
							game->object[i].x_Unit * 32 >= 0 &&
							game->object[i].x_Unit * 32 < game->level_width &&
							game->object[i].y_Unit * 32 >= 0 &&
							game->object[i].y_Unit * 32 < game->level_height &&
							collision(game, obj_num, i))
					{
						game->object[obj_num].direction_Prev = game->object[obj_num].direction;
						game->object[obj_num].direction = 0;
					}
					game->object[i].x_Unit = tmp1;
					game->object[i].y_Unit = tmp2;
				}
			}
			// handle collision with level border
			if(	game->object[obj_num].x_Unit * 32 < -32 ||
					game->object[obj_num].x_Unit * 32 > game->level_width ||
					game->object[obj_num].y_Unit * 32 < -32 ||
					game->object[obj_num].y_Unit * 32 > game->level_height)
			{
				game->object[obj_num].direction_Prev = game->object[obj_num].direction;
				game->object[obj_num].direction = 0;
			}
		}
		
		// reset position after search for next position
		game->object[obj_num].x_Unit = orig_x_Unit;
		game->object[obj_num].y_Unit = orig_y_Unit;
		game->object[obj_num].x = game->object[obj_num].x_Unit * 32;
		game->object[obj_num].y = game->object[obj_num].y_Unit * 32;
		
	}
	
	
	// do animation
	
	if(game->object[obj_num].x_Unit > game->object[obj_num].x_Next_Unit)
	{
		if(game->object[obj_num].x_Unit * 32 - 32 >= game->object[obj_num].x)
			game->object[obj_num].x_Unit--;
		game->object[obj_num].x -= 32 * 20 * game->timer_delta;
		if(game->object[obj_num].x_Unit * 32 - 32 > game->object[obj_num].x)
			game->object[obj_num].x = game->object[obj_num].x_Unit * 32 - 32;
	}
	
	else if(game->object[obj_num].x_Unit < game->object[obj_num].x_Next_Unit)
	{
		if(game->object[obj_num].x_Unit * 32 + 32 <= game->object[obj_num].x)
			game->object[obj_num].x_Unit++;
		game->object[obj_num].x += 32 * 20 * game->timer_delta;
		if(game->object[obj_num].x_Unit * 32 + 32 < game->object[obj_num].x)
			game->object[obj_num].x = game->object[obj_num].x_Unit * 32 + 32;
	}
	
	if(game->object[obj_num].y_Unit > game->object[obj_num].y_Next_Unit)
	{
		if(game->object[obj_num].y_Unit * 32 - 32 >= game->object[obj_num].y)
			game->object[obj_num].y_Unit--;
		game->object[obj_num].y -= 32 * 20 * game->timer_delta;
		if(game->object[obj_num].y_Unit * 32 - 32 > game->object[obj_num].y)
			game->object[obj_num].y = game->object[obj_num].y_Unit * 32 - 32;
	}
	
	else if(game->object[obj_num].y_Unit < game->object[obj_num].y_Next_Unit)
	{
		if(game->object[obj_num].y_Unit * 32 + 32 <= game->object[obj_num].y)
			game->object[obj_num].y_Unit++;
		game->object[obj_num].y += 32 * 20 * game->timer_delta;
		if(game->object[obj_num].y_Unit * 32 + 32 < game->object[obj_num].y)
			game->object[obj_num].y = game->object[obj_num].y_Unit * 32 + 32;
	}
	
	// trigger actions
	if(game->object[obj_num].direction_Prev != 0 &&
					game->object[obj_num].x_Unit == game->object[obj_num].x_Next_Unit &&
					game->object[obj_num].y_Unit == game->object[obj_num].y_Next_Unit)
	{
		// save current position
		orig_x_Unit = game->object[obj_num].x_Unit;
		orig_y_Unit = game->object[obj_num].y_Unit;
		
		// go one step forward to get the next tile
		if(game->object[obj_num].direction_Prev == 1)
			game->object[obj_num].y_Unit--;
		else if(game->object[obj_num].direction_Prev == 2)
			game->object[obj_num].x_Unit++;
		else if(game->object[obj_num].direction_Prev == 3)
			game->object[obj_num].y_Unit++;
		else if(game->object[obj_num].direction_Prev == 4)
			game->object[obj_num].x_Unit--;
	
		// handle collision with other objects
		for(i=0; i<game->num_objects; i++)
		{
			if(obj_num != i && game->object[i].visible && collision(game, obj_num, i))
			{
				// special actions if player collides
				if(!strcmp(game->object[obj_num].type, "player"))
				{
					if(!strcmp(game->object[i].type, "end"))
						game->finished = 1;
					else if(!strcmp(game->object[i].type, "death") ||
									!strcmp(game->object[i].type, "move_death") ||
									!strcmp(game->object[i].type, "break_death") ||
									!strcmp(game->object[i].type, "death_box"))
						game->reset = 1;
					else if(!strcmp(game->object[i].type, "key"))
					{
						game->keys++;
						/*game->moves--;
						game->object[i].visible = 0;
						game->object[obj_num].direction = game->object[obj_num].direction_Prev;*/
					}
					else if(!strcmp(game->object[i].type, "lock") && game->keys > 0)
					{
						game->keys--;
						game->object[i].visible = 0;
					}
					
					
					if(game->object[i].passable)
						game->moves--;
				}

				if(	!strcmp(game->object[i].type, "break_box") ||
						!strcmp(game->object[i].type, "break_death"))
					game->object[i].visible = 0;
				else if(!strcmp(game->object[i].type, "box_death"))
				{
					strcpy(game->object[i].type, "death");
					game->object[i].texture = game->texture[DEATH];
				}
				else if(!strcmp(game->object[i].type, "death_box"))
				{
					strcpy(game->object[i].type, "box");
					game->object[i].texture = game->texture[BOX];
				}
				else if(!strcmp(game->object[i].type, "move_box") ||
								!strcmp(game->object[i].type, "move_death"))
					game->object[i].direction = game->object[obj_num].direction_Prev;
					
				
				if(game->object[i].passable)
				{
					game->object[i].visible = 0;
					game->object[obj_num].direction = game->object[obj_num].direction_Prev;
				}
			}
		}
		// border collision
		if(game->object[obj_num].x_Unit < 0 || game->object[obj_num].x_Unit * 32 >= game->level_width || game->object[obj_num].y_Unit < 0 || game->object[obj_num].y_Unit * 32 >= game->level_height)
		{
			if(!strcmp(game->object[obj_num].type, "player"))
				game->reset = 1;
			else
				game->object[obj_num].visible = 0;
		}
		
		// reset position
		game->object[obj_num].x_Unit = orig_x_Unit;
		game->object[obj_num].y_Unit = orig_y_Unit;
		game->object[obj_num].x = game->object[obj_num].x_Unit * 32;
		game->object[obj_num].y = game->object[obj_num].y_Unit * 32;
		game->object[obj_num].direction_Prev = 0;
		
		// count moves
		if(!strcmp(game->object[obj_num].type, "player"))
			game->moves++;
			
		game->update = 1;
	}
}

void set_cursor(struct game *game, int i)
{
	int j, x1, y1, x2, y2;

	x1 = game->mouse.x;
	y1 = game->mouse.y;
	x2 = game->object[i].x + game->object[i].w / 2;
	y2 = game->object[i].y + game->object[i].h / 2;
	if(abs(abs(x1)-abs(x2)) > abs(abs(y1)-abs(y2)) + 32 && abs(abs(x1)-abs(x2)) < 128)
	{
		game->mouse.clip[1].x = 0;  game->mouse.clip[1].y = 0; // up
		game->mouse.clip[3].x = 0;  game->mouse.clip[3].y = 0; // down
		if(x1 > x2)
		{
			game->mouse.clip[2].x = 0;  game->mouse.clip[2].y = 32; // right
			game->mouse.clip[4].x = 0; game->mouse.clip[4].y = 0; // left
			game->mouse.point_dir = 2;
		}
		else
		{
			game->mouse.clip[2].x = 0;  game->mouse.clip[2].y = 0; // right
			game->mouse.clip[4].x = 64; game->mouse.clip[4].y = 32; // left
			game->mouse.point_dir = 4;
		}
	}
	else if(abs(abs(x1)-abs(x2)) < abs(abs(y1)-abs(y2)) - 32 && abs(abs(y1)-abs(y2)) < 128)
	{
		game->mouse.clip[2].x = 0;  game->mouse.clip[2].y = 0; // right
		game->mouse.clip[4].x = 0; game->mouse.clip[4].y = 0; // left
		if(y1 > y2)
		{
			game->mouse.clip[3].x = 32; game->mouse.clip[3].y = 32; // down
			game->mouse.clip[1].x = 0; game->mouse.clip[1].y = 0; // up
			game->mouse.point_dir = 3;
		}
		else
		{
			game->mouse.clip[3].x = 0; game->mouse.clip[3].y = 0; // down
			game->mouse.clip[1].x = 64; game->mouse.clip[1].y = 0; // up
			game->mouse.point_dir = 1;
		}
	}
	else
	{
		for(j=1; j<5; j++)
		{
			game->mouse.clip[j].x = 0; game->mouse.clip[j].y = 0;
		}
		game->mouse.point_dir = 0;
	}
}
