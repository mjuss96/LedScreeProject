
int BarrierNumber;
boolean BarrierSpawned = false;
boolean sf = false;

int stone1 [2];
boolean stone1On=false;
int stone2 [2][2];
boolean stone2On=false;
int stone3 [4][2];
boolean stone3On=false;
int bird1 [2];
boolean bird1On=false;
int bird2 [2];
boolean bird2On=false;

// Format the characters
int figure [3][2]={{2,3},{2,4},{2,5}};
int smallfigure [2][2];


unsigned long distanceCounter;
int barrierCounter;
int moveCounter;

// Game setup. Set all barriers to false and counters to zero
void TRexSetup()
{
  stone1On = false;
  stone2On = false;
  stone3On = false;
  bird1On = false;
  bird2On = false;
  distanceCounter = 0;
  barrierCounter = 0;
  moveCounter = 0;
}

// Speed of the game. When the distanceCounter rises, the speed will grow.
void Speed()
{
  int nopeus = 30;
  if (distanceCounter > 100) {nopeus=25;}
  if (distanceCounter > 300) {nopeus=20;}
  if (distanceCounter > 500) {nopeus=15;}
  if (distanceCounter > 700) {nopeus=10;}
  for (int i = 0; i < nopeus; i++)
  {
    ReadBluetooth();
    delay(10);
  }
}

void Smallfigure()
{
  // When the "jump" button is pressed, will give to the figure the following coordinates.
  if (UpState == buttonPressed)
  {
    smallfigure[0][0] = 2;
    smallfigure[0][1] = 5;
    smallfigure[1][0] = 2;
    smallfigure[1][1] = 6;
    sf = true;
    // And the following coordinates, when the "down" button is pressed.
  } else if (DownState == buttonPressed)
  {
    smallfigure[0][0] = 2;
    smallfigure[0][1] = 3;
    smallfigure[1][0] = 2;
    smallfigure[1][1] = 4;   
    sf = true;
  }
  buttonPressed = 0;
}

// Barrier spawning. First we draw the random barrier number (stone or bird). 
// The functions below will draw the type of the stone / bird.
// The birds won't spawn, when the distanceCounter is below 100.
void SpawnBarrier()
{
  if (barrierCounter > 9)
  {
    BarrierNumber = random(1,3);
    do
    {
      if (BarrierNumber == 1 && distanceCounter > 100)
      {
        RandomBird();
      }
      else
      {
        RandomStone();
      }
    } while(BarrierSpawned == false);
  }
}

// This function draws the type of stone.
// First it draws the stone, and then gives coordinates to it.
// It sets barrier to true, and also the barrier in question to true.
int RandomStone()
{ 
int StoneNumber;
  {
    StoneNumber = random(1,4);
    
    if (StoneNumber == 1 && stone1On == false)
    {
        stone1[0] = 20;
        stone1[1] = 3;
        BarrierSpawned = true;
        stone1On = true;
        barrierCounter = 0;
    }
    
    if (StoneNumber == 2 && stone2On == false)
    {
        stone2[0][0] = 20;
        stone2[0][1] = 3;
        stone2[1][0] = 20;
        stone2[1][1] = 4;
        BarrierSpawned = true;
        stone2On = true;
        barrierCounter = 0;
    }
    
   if (StoneNumber == 3 && stone3On == false)
    {
        stone3[0][0] = 20;
        stone3[0][1] = 3;
        stone3[1][0] = 20;
        stone3[1][1] = 4;
        stone3[2][0] = 19;
        stone3[2][1] = 3;
        stone3[3][0] = 19;
        stone3[3][1] = 4;
        BarrierSpawned = true;
        stone3On = true;
        barrierCounter = 0;
    }
  }
}

// This function draws the type of bird. It works the same way as the RandomStone function.
int RandomBird()
{
int BirdNumber;
    BirdNumber = random(1,3);
    if (BirdNumber == 1 && bird1On == false)   
    {
      bird1[0] = 20;
      bird1[1] = 4;
      BarrierSpawned = true;
      bird1On = true;
      barrierCounter = 0;
    }
    
    if (BirdNumber == 2 && bird2On == false)
    {
      bird2[0] = 20;
      bird2[1] = 5;
      BarrierSpawned = true;
      bird2On = true;
      barrierCounter = 0;
    }
}

// This function moves barriers. First it checks if the barrier in question is true.
// Then begins to reduce the value of the x coordinate of the selected barrier.
void moveBarrier()
{
  if (stone1On==true)
    {
      stone1[0]--;
    }
  if (stone2On==true)
    {
      stone2[0][0]--; stone2[1][0]--;
    }
  if (stone3On==true)
    {
      stone3[0][0]--; stone3[1][0]--; stone3[2][0]--; stone3[3][0]--;
    } 
  if (bird1On==true)
    {
      bird1[0]--;
    }
  if (bird2On==true)
    {
      bird2[0]--;
    }   
}

// This function "stops" the barriers when they come to the edge of the screen. This also checks, if the barrier in question is true.
// When the X-coordinate is below one, put that barrier into false.
void StopBarrier()
{
  if (stone1On==true)
    {
      if (stone1[0] < 1)
      {
        stone1On = false;
      } 
    }
  if (stone2On==true)
    {
      if (stone2[0][0] < 1)
      {
        stone2On = false;
      }
    }
  if (stone3On==true)
    {
      if (stone3[0][0] < 1)
      {
        stone3On = false;
      }
    } 
  if (bird1On==true)
    {
      if (bird1[0] < 1)
      {
        bird1On = false;
      }
    }
  if (bird2On==true)
    {
      if (bird2[0] < 1)
      {
        bird2On = false;
      }
    }   
}

// In this function we check the collision.
// First we set the collision into false.
// If the player is jumping or at ground, we'll go into the first if-sentence. If the figure "stands", we go into the else-sentence.
// There we check is the barrier true.
// If it is, we check the coordinates of the figure and the barrier. If they are same, the collision is true.
// If the collision is true the game is over, and we're going into GameOver-function.
void CheckCollision()
{
  boolean collision = false;
  if (sf == true)
  {
    if (stone1On == true && smallfigure[0][0] == stone1[0] && smallfigure[0][1] == stone1[1]) {collision = true;}
    if (stone2On == true && smallfigure[1][0] == stone2[1][0] && smallfigure[1][1] == stone2[1][1]) {collision = true;}
    if (stone3On == true && smallfigure[1][0] == stone3[3][0] && smallfigure[1][1] == stone3[3][1]) {collision = true;}
    if (bird1On == true && smallfigure[1][0] == bird1[0] && smallfigure[1][1] == bird1[1]) {collision = true;}
    if (bird2On == true && smallfigure[0][0] == bird2[0] && smallfigure[0][1] == bird2[1]) {collision = true;}
  }
  else
  {
    if (stone1On == true && figure[0][0] == stone1[0] && figure[0][1] == stone1[1]) {collision = true;}
    if (stone2On == true && figure[1][0] == stone2[1][0] && figure[1][1] == stone2[1][1]) {collision = true;}
    if (stone3On == true && figure[1][0] == stone3[3][0] && figure[1][1] == stone3[3][1]) {collision = true;}
    if (stone3On == true && figure[1][0] == stone3[1][0] && figure[1][1] == stone3[1][1]) {collision = true;}
    if (bird1On == true && figure[1][0] == bird1[0] && figure[1][1] == bird1[1]) {collision = true;}
    if (bird2On == true && figure[2][0] == bird2[0] && figure[2][1] == bird2[1]) {collision = true;}
  }
  if (collision == true) 
  {
    GameOver();   
  }
}

// The game has ended. This function will display the points, then it goes back to setup-function, and the game starts again.
void GameOver()
{
  endGame(distanceCounter);
  distanceCounter = 0;
}


// This function is for drawing.
// First it draws the ground, and gives it a color.
// Then it checks is the figure jumping or at ground. If it is, it draws the small figure into the given coordinates.
// Else it draws the normal figure.
// It checks is the barrier in question true, if it is it sets the previously entered coordinates to red.
// The last command "FastLED.show" will light up the given LEDs.
void Draw_trex()
{ 
  for (int i = 0; i<63; i++)
  {
    leds[i].setRGB(130, 90, 12);
  }
  
  if (sf == true && moveCounter < 6) 
  {
    for(int i=0;i<2;i++)
    {
      leds[ XY( smallfigure[i][0],smallfigure[i][1] ) ] = CRGB::Green;
    }
  }
  else
  {
    sf = false;
    moveCounter = 0;
    for(int i=0;i<3;i++)
    {
      leds[ XY( figure[i][0],figure[i][1] ) ] = CRGB::Green;
    }  
  }
  
    if (stone1On == true)
    {
      leds[ XY( stone1[0],stone1[1] ) ] = CRGB::Red;
    }

    if (stone2On == true)
    {
      for(int i=0;i<2;i++)
      {
        leds[ XY( stone2[i][0],stone2[i][1] ) ] = CRGB::Red;
      }
    }

    if (stone3On == true)
    {
      for(int i=0;i<4;i++)
      {
        leds[ XY( stone3[i][0],stone3[i][1] ) ] = CRGB::Red;
      }
    }

    if (bird1On == true)
    {
      leds[ XY( bird1[0],bird1[1] ) ] = CRGB::Red;
    }

    if (bird2On == true)
    {
      leds[ XY( bird2[0],bird2[1] ) ] = CRGB::Red;
    } 
  FastLED.show();
}

// Here is the main loop with all the necessary functions.
// Let's play!
void tRex_loop() 
{
  // jos snake peli on valittu.
  if(valinta == 2) {
    Speed();
    SpawnBarrier();
    moveBarrier();
    Smallfigure();
    StopBarrier();
    Draw_trex(); 
    CheckCollision();
    FastLED.clear();
    barrierCounter++;
    distanceCounter++;
    moveCounter++;
 }
}
