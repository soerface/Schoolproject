#include <stdio.h>
#include <string.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "gamestruct.h"

void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip)
{
	SDL_Rect offset;
	
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface(source, clip, destination, &offset);
}

SDL_Surface *load_image(char filename[])
{
	char path[512];
	SDL_Surface *loadedImage = NULL, *optimizedImage = NULL;
	sprintf(path, "data/images/%s", filename);
	printf("Loading %s\n", path);
	loadedImage = IMG_Load(path);
	if(loadedImage != NULL)
	{
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);
		SDL_FreeSurface(loadedImage);
	}
	/*if(optimizedImage != NULL)
	{
		Uint32 colorkey = SDL_MapRGB(optimizedImage->format, 0x00, 0xFF, 0xFF);
		SDL_SetColorKey(optimizedImage, SDL_SRCCOLORKEY, colorkey);
	}*/
	return optimizedImage;
}

void render_cursor(struct game *game)
{
	SDL_Surface *screen = SDL_GetVideoSurface();
	apply_surface(game->mouse.x_input - game->mouse.clip[0].w / 2, game->mouse.y_input - game->mouse.clip[0].h / 2, game->mouse.sprite, screen, &game->mouse.clip[0]);
	apply_surface(game->mouse.x_input - game->mouse.clip[1].w / 2, game->mouse.y_input - game->mouse.clip[1].h - 10, game->mouse.sprite, screen, &game->mouse.clip[1]);
	apply_surface(game->mouse.x_input + 10, game->mouse.y_input - game->mouse.clip[2].h / 2, game->mouse.sprite, screen, &game->mouse.clip[2]);
	apply_surface(game->mouse.x_input - game->mouse.clip[3].w / 2, game->mouse.y_input + 10, game->mouse.sprite, screen, &game->mouse.clip[3]);
	apply_surface(game->mouse.x_input - game->mouse.clip[4].w - 10, game->mouse.y_input - game->mouse.clip[4].h / 2, game->mouse.sprite, screen, &game->mouse.clip[4]);
}

void render_game(struct game *game)
{
	int i, j;
	SDL_Surface *screen = SDL_GetVideoSurface();
	
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
	if(game->screen_width > game->level_width)
		game->camera.x = (game->level_width - game->screen_width) / 2;
	if(game->screen_height > game->level_height)
		game->camera.y = (game->level_height - game->screen_height) / 2;

	// show background
	for(i=0; i<(game->screen_height + game->camera.y) && i<(game->level_height); i+=game->background->h)
		for(j=0; j<(game->screen_width + game->camera.x) && j<(game->level_width); j+=game->background->w)
			apply_surface(j - game->camera.x, i - game->camera.y, game->background, screen, NULL);	
	
	if(game->paused)
	{
		apply_surface((game->screen_width - game->paused_surface->w) / 2, (game->screen_height - game->paused_surface->h) / 2, game->paused_surface, screen, NULL);
		game->pause_screen = 1;
	}
	else
	{
		// show objects
		for(i=0; i<game->num_objects; i++)
			if(game->object[i].visible)
				apply_surface(game->object[i].x - game->camera.x, game->object[i].y - game->camera.y, game->object[i].texture, screen, NULL);
		game->pause_screen = 0;
	}
}

void render_toolbox(struct game *game)
{
	int i;
	SDL_Surface *toolbar = NULL, *screen = SDL_GetVideoSurface();
	SDL_PixelFormat fmt = *(screen->format);
	
	toolbar = SDL_CreateRGBSurface(0, screen->w, game->toolbar_size, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
	SDL_SetAlpha(toolbar, SDL_SRCALPHA, 180);
	SDL_FillRect(toolbar, &toolbar->clip_rect, SDL_MapRGB(screen->format, 0xDD, 0xDD, 0xDD));
	apply_surface(0, 0, toolbar, screen, NULL);
	SDL_FreeSurface(toolbar);
	
	/*clip.y = 0; clip.h = 1;
	for(i=0; i<=100; i++)
	{
		toolbar = SDL_CreateRGBSurface(0, screen->w, 1, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
		SDL_SetAlpha(toolbar, SDL_SRCALPHA, i*2);
		SDL_FillRect(toolbar, &clip, SDL_MapRGB(screen->format, 0xEE, 0xEE, 0xEE));
		apply_surface(0, 100 - i, toolbar, screen, NULL);
		SDL_FreeSurface(toolbar);
	}*/
	
	for(i=0; i<game->num_tool_objects; i++)
		if(game->tool_object[i].visible)
			apply_surface(game->tool_object[i].x, game->tool_object[i].y, game->tool_object[i].texture, screen, NULL);
}
void render_buttons(struct game *game)
{
	SDL_Surface *message = NULL, *screen = NULL;
	SDL_Color color_norm = {0,0,0};
	SDL_Color color_high = {255,255,255};
	int i, j;
	
	screen = SDL_GetVideoSurface();
	for(i=0; i<game->num_buttons; i++)
	{
		message = NULL;
		if(game->button[i].menu == game->menu)
		{
			if(game->button[i].selected)
			{
				message = TTF_RenderText_Blended(game->font_yo_frankie, game->button[i].label, color_high);
				for(j=0; j<game->button[i].w - (game->button_l->w + game->button_r->w); j+=game->button_ch->w)
					apply_surface(game->button[i].x+j, game->button[i].y, game->button_ch, screen, NULL);
				apply_surface(game->button[i].x - game->button_lh->w, game->button[i].y, game->button_lh, screen, NULL);
				apply_surface(game->button[i].x + j, game->button[i].y, game->button_rh, screen, NULL);
			}
			else
			{
				message = TTF_RenderText_Blended(game->font_yo_frankie, game->button[i].label, color_norm);
				for(j=0; j<game->button[i].w - (game->button_lh->w + game->button_rh->w); j+=game->button_c->w)
					apply_surface(game->button[i].x+j, game->button[i].y, game->button_c, screen, NULL);
				apply_surface(game->button[i].x - game->button_l->w, game->button[i].y, game->button_l, screen, NULL);
				apply_surface(game->button[i].x + j, game->button[i].y, game->button_r, screen, NULL);
			}
	
			apply_surface(game->button[i].x, game->button[i].y + (game->button_c->h - message->h) / 2, message, screen, NULL);
		}
		if(message)
			SDL_FreeSurface(message);
	}
}

void render_list(struct game *game)
{
	SDL_Surface *message = NULL, *screen = NULL;
	SDL_Color color_norm = {0,0,0};
	SDL_Color color_high = {255,255,255};
	int i, j, tmp;
	
	for(i=0; i<game->num_buttonlist; i++)
	{
		message = TTF_RenderText_Blended(game->font_yo_frankie, game->buttonlist[i].label, color_norm);
		screen = SDL_GetVideoSurface();
		tmp = game->buttonlist[i].selected;
		if(	game->mouse.x_input >= game->buttonlist[i].x - game->button_l->w && game->mouse.x_input <= game->buttonlist[i].x + message->w + game->button_r->w &&
				game->mouse.y_input >= game->buttonlist[i].y && game->mouse.y_input <= game->buttonlist[i].y + game->button_c->h)
		{
			SDL_FreeSurface(message);
			message = TTF_RenderText_Blended(game->font_yo_frankie, game->buttonlist[i].label, color_high);
			game->buttonlist[i].selected = 1;
		}
		else
			game->buttonlist[i].selected = 0;
			
		if(tmp != game->buttonlist[i].selected && game->buttonlist[i].menu == game->menu)
			game->update = 1;
			
		game->buttonlist[i].w = message->w + game->button_l->w + game->button_r->w;
		if(message->h > game->button_c->h)
			game->buttonlist[i].h = message->h;
		else
			game->buttonlist[i].h = game->button_c->h;

		if(game->buttonlist[i].menu == game->menu)
		{
			if(game->buttonlist[i].selected)
			{
				for(j=0; j<message->w; j+=game->button_ch->w)
					apply_surface(game->buttonlist[i].x+j, game->buttonlist[i].y, game->button_ch, screen, NULL);
				apply_surface(game->buttonlist[i].x - game->button_lh->w, game->buttonlist[i].y, game->button_lh, screen, NULL);
				apply_surface(game->buttonlist[i].x + j, game->buttonlist[i].y, game->button_rh, screen, NULL);
			}
			else
			{
				for(j=0; j<message->w; j+=game->button_c->w)
					apply_surface(game->buttonlist[i].x+j, game->buttonlist[i].y, game->button_c, screen, NULL);
				apply_surface(game->buttonlist[i].x - game->button_l->w, game->buttonlist[i].y, game->button_l, screen, NULL);
				apply_surface(game->buttonlist[i].x + j, game->buttonlist[i].y, game->button_r, screen, NULL);
			}
	
			apply_surface(game->buttonlist[i].x, game->buttonlist[i].y + (game->button_c->h - message->h) / 2, message, screen, NULL);
		}
		SDL_FreeSurface(message);
	}
}







/*SDL_Surface *CreateSurface(Uint32 flags, int width, int height, SDL_Surface *display)
{
	SDL_Surface *surface;
	SDL_PixelFormat fmt = *(display->format);
	
	return SDL_CreateRGBSurface(flags, width, height, fmt.BitsPerPixel, fmt.Rmask, fmt.Gmask, fmt.Bmask, fmt.Amask);
//	surface = SDL_DisplayFormatAlpha(surface);
//	return surface;
}*/
