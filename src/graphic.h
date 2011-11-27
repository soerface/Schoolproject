
void apply_surface(int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip);
SDL_Surface *load_image(char filename[]);
void render_cursor(struct game *game);
void render_game(struct game *game);
void render_toolbox(struct game *game);
void render_buttons(struct game *game);
void render_list(struct game *game);
