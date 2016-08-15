#include "main.h"
#include <math.h>

double sqrt(double num){
  double a, b, e=0.001;
  a=num;
  b=num/a;
  for(int i=0; fabs(a*a-num)>e && i<MAX_SQRT; i++){
    a=(a+b)/2;
    b=num/a;
  }
  return a;
}
int sign(double num){
  if(num>0)
    return 1;
  else if(num<0)
    return -1;
  return 0;
}

bool rectCollision(GRect rec1, GRect rec2){
  rec1.origin.x+=rec1.size.w/2;
  rec1.origin.y+=rec1.size.h/2;
  rec2.origin.x+=rec2.size.w/2;
  rec2.origin.y+=rec2.size.h/2;
  if(abs(rec1.origin.x-rec2.origin.x)>(rec1.size.w+rec2.size.w)/2)
    return false;
  else if(abs(rec1.origin.y-rec2.origin.y)>(rec1.size.h+rec2.size.h)/2)
    return false;
  return true;
}

void scaleVec(Vec2d *vec, Vec2d *out, double multiplier){
  out->x=(vec->x)*multiplier;
  out->y=(vec->y)*multiplier;
}

void addVecs(Vec2d *vec1, Vec2d *vec2, Vec2d *out){
  out->x=vec1->x+vec2->x;
  out->y=vec1->y+vec2->y;
}

void subVecs(Vec2d *vec1, Vec2d *vec2, Vec2d *out){
  out->x=vec1->x-vec2->x;
  out->y=vec1->y-vec2->y;
}

double normVec(Vec2d *vec){
  double tmp=sqrt((vec->x)*(vec->x)+(vec->y)*(vec->y));
  return tmp;
}

void Move(Vec2d *crds, int obj){
  Vec2d *tmp=&coords;
  if(obj==1)
    tmp=&aiCoords;
  else if(obj==2)
    tmp=&ball;
  tmp->x=crds->x;
  tmp->y=crds->y;
  bool bounce=false;
  double num=paddleSize.y;
  if(obj==2)
    num=(double)ballRect.size.w/2;
  if(tmp->y-num < bounds.x){
    tmp->y=bounds.x+paddleSize.y;
    if(obj==2){
      tmp->y=bounds.x+(double)ballRect.size.w/2;
      bounce=true;
    }
  }
  else if(tmp->y+num > bounds.y){
    tmp->y=bounds.y-paddleSize.y;
    if(obj==2){
      tmp->y=bounds.y-(double)ballRect.size.w/2;
      bounce=true;
    }
  }
  
  if(bounce){
    ballVel.y*=-1.0;
  }
}

void updateAi(){
  Vec2d tmp;
  scaleVec(&aiSpeed, &tmp, multiplier);
  if(abs(aiCoords.y-ball.y)<tmp.y){
    tmp.x=aiCoords.x;
    tmp.y=ball.y;
  }
  else if(ball.y>aiCoords.y)
    addVecs(&aiCoords, &tmp, &tmp);
  else
    subVecs(&aiCoords, &tmp, &tmp);
  Move(&tmp, 1);
}

void updatePlayer(){
  Vec2d tmp;
  
  scaleVec(&speed, &tmp, multiplier);
  
  if(buttonsPressed.up){
    subVecs(&coords, &tmp, &coords);
    Move(&coords, 0);
  }
  
  if(buttonsPressed.down){
    addVecs(&coords, &tmp, &coords);
    Move(&coords, 0);
  }
}

void updateBall(){
  Vec2d tmp;
  scaleVec(&ballVel, &tmp, multiplier);
  addVecs(&ball, &tmp, &tmp);
  Move(&tmp, 2);
}

void bounceBall(int ai){
  if(sign(ballVel.x)==sign(ball.x-WIDTH/2)){
    ballVel.x*=-1.0;
    double num=coords.y;
    if(ai==2)
      num=aiCoords.y;
    ballVel.y=(maxBallSpd.y-minBallSpd.y)*(ball.y-num)/paddleSize.y+minBallSpd.y*sign(ball.y-num);
  }
}

void updatePaddleCoords(bool ai){
  if(!ai){
    paddle.origin.x=coords.x;
    paddle.origin.y=coords.y-paddleSize.y;
  }
  else{
    paddle.origin.x=aiCoords.x-paddleSize.x*2;
    paddle.origin.y=aiCoords.y-paddleSize.y;
  }
}

void updateBallRect(){
  ballRect.origin.x=(int)ball.x-ballRect.size.w/2;
  ballRect.origin.y=(int)ball.y-ballRect.size.h/2;
}

void reset(){
  aiCoords.y=HEIGHT/2;
  coords.y=HEIGHT/2;
  
  ball.x=WIDTH/2;
  ball.y=HEIGHT/2;
  
  ballVel.x=(double)((rand()%((int)maxBallSpd.x-(int)minBallSpd.x)+(int)minBallSpd.x)*((rand()%2)*2-1));
  ballVel.y=(double)((rand()%((int)maxBallSpd.y-(int)minBallSpd.y)+(int)minBallSpd.y)*((rand()%2)*2-1));
}

void checkCollisions(){
  updatePaddleCoords(false);
  updateBallRect();
  if(rectCollision(paddle, ballRect))
    bounceBall(1);
  updatePaddleCoords(true);
  if(rectCollision(paddle, ballRect))
    bounceBall(2);
  if(ball.x<=0.0){
    scoreInt[1]++;
    reset();
  }
  else if(ball.x>WIDTH){
    scoreInt[0]++;
    reset();
  }
}

void drawObjects(Layer *layer, GContext *gcx){
  graphics_context_set_fill_color(gcx, GColorWhite);
  //drawing player paddle
  updatePaddleCoords(false);
  graphics_fill_rect(gcx, paddle, 0, GCornerNone);
  //drawing ai paddle
  updatePaddleCoords(true);
  graphics_fill_rect(gcx, paddle, 0, GCornerNone);
  //drawing ball
  updateBallRect();
  graphics_fill_rect(gcx, ballRect, 0, GCornerNone);
}

void updateGame(Layer *layer, GContext *gcx){
  snprintf(score_buffer, 9, "%d %d", scoreInt[0], scoreInt[1]);
  text_layer_set_text(score, score_buffer);
  updateBall();
  updateAi();
  updatePlayer();
  checkCollisions();
  drawObjects(layer, gcx);
}

void timerHandler(void *context){
  layer_mark_dirty(gc);
  app_timer_register(frameTime, timerHandler, NULL);
}

void upTrue(){
  buttonsPressed.up=true;
}

void upFalse(){
  buttonsPressed.up=false;
}

void downTrue(){
  buttonsPressed.down=true;
}

void downFalse(){
  buttonsPressed.down=false;
}

void test(){
  window_raw_click_subscribe(BUTTON_ID_UP, upTrue, upFalse, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, downTrue, downFalse, NULL);
}

void config_provider(Window *window){
  test();
}

void init_window(Window *window){
  Layer *window_layer=window_get_root_layer(window);
  GRect windowBounds=layer_get_bounds(window_layer);
  
  gc=layer_create(windowBounds);
  score=text_layer_create((GRect){{0,0}, {WIDTH, 30}});
  
  backgroundBitmap=gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  background=bitmap_layer_create(windowBounds);
  
  bitmap_layer_set_bitmap(background, backgroundBitmap);
  
  text_layer_set_background_color(score, GColorClear);
  text_layer_set_text_color(score, GColorWhite);
  text_layer_set_text_alignment(score, GTextAlignmentCenter);
  text_layer_set_font(score, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  
  layer_add_child(window_layer, bitmap_layer_get_layer(background));
  layer_add_child(window_layer, gc);
  layer_add_child(window_layer, text_layer_get_layer(score));
  
  layer_set_update_proc(gc, updateGame);
}

void close_window(Window *window){
  layer_destroy(gc);
  gbitmap_destroy(backgroundBitmap);
  bitmap_layer_destroy(background);
  text_layer_destroy(score);
}

void init(){
  main_window=window_create();
  window_set_background_color(main_window, GColorBlack);
  
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load=init_window,
    .unload=close_window
  });
  
  window_set_click_config_provider(main_window, (ClickConfigProvider) config_provider);
  
  window_stack_push(main_window, true);
  
  reset();
  app_timer_register(1000, timerHandler, NULL);
}

void deinit(){
  window_destroy(main_window);
}

int main(){
  init();
  
  app_event_loop();
  
  deinit();

  return 0;
}