#include <pebble.h>

#define MAX_SQRT 10
#define DIFFICULTY 2.0
#define HEIGHT 168.0
#define WIDTH 144.0

double sqrt(double num);
int sign(double num);

typedef struct {
  double x;
  double y;
} Vec2d;

typedef struct {
  bool up;
  bool down;
  bool sel;
  bool back;
} Buttons;

bool rectCollision(GRect rec1, GRect rec2);

void scaleVec(Vec2d *vec, Vec2d *out, double scalar);
void addVecs(Vec2d *vec1, Vec2d *vec2, Vec2d *out);
void subVecs(Vec2d *vec1, Vec2d *vec2, Vec2d *out);
double normVec(Vec2d *vec);

void timerHandler(void *context);

Window *main_window;
TextLayer *score;
BitmapLayer *background;
GBitmap *backgroundBitmap;
char score_buffer[9];
Layer *gc;//graphics context

const Vec2d screenSize={WIDTH, HEIGHT};
const Vec2d paddleSize={5.0, 15.0};
const Vec2d bounds={30.0, 138.0};//x refers to top bound and y refers to bottom. Both are y coordinate values
const Vec2d minBallSpd={100, 50.0};
const Vec2d maxBallSpd={250.0, 250.0};

const int frameTime=1000/30;
const double multiplier=30.0/1000.0;

GRect paddle={{0,0}, {10,30}};
GRect ballRect={{0,0}, {8,8}};

int scoreInt[2]={0,0};

Vec2d ball, ballVel, aiCoords={WIDTH, HEIGHT/2}, coords={0, HEIGHT/2};
Vec2d aiSpeed={0.0, DIFFICULTY*50.0};
Vec2d speed={0.0, 200.0};

Buttons buttonsPressed={false, false, false, false};