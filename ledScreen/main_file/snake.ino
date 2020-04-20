#include <FastLED.h>
#define WIDTH 21
#define HEIGHT 14

struct snake {
  int x;
  int y;
  int old_x;
  int old_y;
  int dir = 2;
  int length;
  int tail [NUM_LEDS][2];
} snake;

struct apple {
  int x;
  int y;
} apple;

int x_center = WIDTH / 2;
int y_center = HEIGHT / 2;
unsigned long loop_counter = 0;

// end game animation
// displays a small animation and points,
// resets the snake, as well as takes the user back to the main menu.
void endGame(int points)
{
    // fill the screen with red color, 10ms delay
    for (int i=0; i<NUM_LEDS; i++)
    {
        leds[i] = CRGB::Red; 
        FastLED.show(); 
        delay(10);
    } 
    delay(1000);
    FastLED.clear();


    displayWord("score", 1, 6, 0, 255, 51);
    delay(2000);
    FastLED.clear();
    
    displayNumbers(points,5,5, 6, 0, 255, 229);
    delay(3000);
    FastLED.clear();

    // reseting this make the main loop call  ResetSnake() function
    loop_counter = 0;
    snake.length = 0; 
    valinta = 0;
    
}

// setup the snake, everything to initial values.
void Snake_setup()
{
  loop_counter = 0;
  FastLED.clear();
  Spawn_apple();
  snake.length = 0;
  //center the snake
  snake.x = x_center; 
  snake.y = y_center;

  //spawn apple to random coordinates.
  apple.x = random(WIDTH-1);
  apple.y = random(HEIGHT-1);
}

void MoveSnake() {
  // 0.2 second delay, read user input for that time.
  for (int i = 0; i < 200; i++) {
    //function for reading bluetooth module.
    ReadBluetooth();
    delay(1);
  }
  //set the direction of the snake
  // ex. turn left if we are not going right.
  if (RightState == buttonPressed && snake.dir != 2) {
    snake.dir = 1;
  }
  else if (DownState == buttonPressed && snake.dir != 4) {
    snake.dir = 3;
  }
  else if (UpState == buttonPressed && snake.dir != 3) {
    snake.dir = 4;
  }
  else if (LeftState == buttonPressed && snake.dir != 1) {
    snake.dir = 2;
  }
  
//move snake
  if(snake.dir == 1)
    snake.x++;
  else if(snake.dir == 2)
    snake.x--;
  else if(snake.dir == 3)
    snake.y--;
  else if(snake.dir == 4)
    snake.y++;

  // let snake go around the edges of screen, "teleport" to opposite side.
  if (snake.x > WIDTH - 1) {
    snake.x = 0;
  }
  if (snake.y > HEIGHT - 1) {
    snake.y = 0;
  }
  if (snake.x < 0) {
    snake.x = WIDTH - 1;
  }
  if (snake.y < 0) {
    snake.y = HEIGHT - 1;
  }
}

int check_collision()
{
  //check the collison between snake head and snake tail.
  for (int i = 0; i < snake.length; i++) {

    //if snake head x and y are same that tail x and y, game is over.
    if (snake.x == snake.tail[i][0] && snake.y == snake.tail[i][1]) {
      endGame(snake.length*5);
      return false;
    }
  }

  return true;
}

void Spawn_apple()
{
  //function for spawning apple 
  int randX = random(WIDTH-1);
  int randY = random(HEIGHT-1);
  
  //we dont want the apple to spawn on the snake tail.
  for(int i = 0; i < snake.length; i++) {
    while(snake.tail[i][0] == randX && snake.tail[i][1] == randY) {
      randX = random(WIDTH-1);
      randY = random(HEIGHT-1);
      i=0;
    }
  }
  
  apple.x = randX;
  apple.y = randY;
}

void Snake()
{
    // check for crash before writing new frame
   if(check_collision() == false)
       return;
       
    // eating apple
     if (snake.x==apple.x&&snake.y==apple.y)
      {
      snake.length++;
      Spawn_apple();      
      }
      //moving the tail
     // put new snake tail values to be the same as last tail position values.
      snake.old_x = snake.tail[0][0];
      snake.old_y = snake.tail[0][1];
      
      snake.tail[0][0] = snake.x;
      snake.tail[0][1] = snake.y;
      
      int old_2x,old_2y;
      
      for(int i = 1; i<snake.length; i++)
      {
        old_2x=snake.tail[i][0];
        old_2y=snake.tail[i][1];
        snake.tail[i][0] = snake.old_x;
        snake.tail[i][1] = snake.old_y;
        snake.old_x=old_2x;
        snake.old_y=old_2y;
      }

    MoveSnake();
}

void Draw()
{
  //clear the screen first, we dont want to write over the last frame.
  FastLED.clear();
  
  //snake head will be blue.
  leds[ XY( snake.x, snake.y ) ] = CRGB::Blue;
  
  //apples will be red.
  leds[ XY( apple.x, apple.y ) ] = CRGB::Red;
  
  //tails will be green.
  for (int i = 0; i < snake.length; i++) {
    leds[ XY( snake.tail[i][0], snake.tail[i][1] ) ] = CRGB::Green;
  }
  
  //send the data to screen.
  FastLED.show();
}

void snake_loop() {
  // running the loop first time, setup snake position:
  if (loop_counter == 0) {
    Snake_setup();
  }

  // jos snake peli on valittu.
  if(valinta == 1) {
    Snake();
    Draw();
    loop_counter++;
 }
}
