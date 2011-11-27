
//int collision(struct object A, struct object B);
int collision(struct game *game, int A, int B);
int hit(struct game *game, int x, int y, int w, int h);
void move(int obj_num, struct game *game);
void set_cursor(struct game *game, int i);
