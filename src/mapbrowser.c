
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "gamestruct.h"

void process_list(struct game *game)
{
	int i = 0, row = 0, tmp = 0;
	struct dirent *dirent;
	
	dirent = readdir(game->map_dir);
	while(dirent)
	{
		if(strcmp(game->buttonlist[i].label, dirent->d_name) || game->buttonlist[i].menu == game->menu)
			game->update = 1;
	
		strcpy(game->buttonlist[i].label, dirent->d_name);
		game->buttonlist[i].menu = MAPBROWSER;
	
		if(game->buttonlist[i].label[0] == '.')
			game->buttonlist[i].menu = 0;
		if(i-tmp > 0)
		{
			game->buttonlist[i].x = game->buttonlist[i-1-tmp].w + game->buttonlist[i-1-tmp].x + 5;
			if(game->buttonlist[i].x + game->buttonlist[i].w > game->screen_width - 20)
			{
				row++;
				game->buttonlist[i].x = 20;
			}
			game->buttonlist[i].y = (game->button_c->h + 10) * row + 20;
		}
		else
		{
			game->buttonlist[i].x = 20;
			game->buttonlist[i].y = 20;
		}
		i++;
		dirent = readdir(game->map_dir);
	}
	if(game->num_buttonlist != i)
	{
		game->num_buttonlist = i;
		game->update = 1;
	}
	rewinddir(game->map_dir);
}
