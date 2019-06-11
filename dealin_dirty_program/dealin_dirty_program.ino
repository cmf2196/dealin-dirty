/*
 * Author: Dealin' Dirty: Columbia University Senior Design Group 13, Class of 2019
 * This program runs the Card Dealer 300, automatic card dealer and interactive gaming systemn
 * Five games are encorperated into the device
 */





// _____________________________________________Library Packages_____________________________________________________________
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include "Keypad.h"
#include <Stepper.h>


//_______________________________________________Global Variables______________________________________________________________
int numStepsPush = 200;   // The number of steps is specific to your motor
int numStepsSlide = 200;   // The number of steps is specific to your motor
int numStepsPick = 200;    // The number of steps is specific to your motor
float rollerRadius = 0.32;  // what is the radius in inches of our rollers
int pulsesPerRev =16 * 131;    // the gear ration is 131   // the gear ratio is 131 . This is adjusted for error
volatile long counter = 0;   // this will change in the background




int topSpeed = 255;  // 255 is the maximum speed
int lowSpeed = 130; // play around with this number
float distance = 3.2;   // How far in inches do you want to travel
int baseSpeed = 160;         // base speed
int homeSpeed = 140;         // homing speed for the base




char dir;                     // base motor initialize
char dirBase;                 // base motor initialize
char reset = 'P';   
int screenNum = 0;   // initialize ithere

//// list of games 
char gameOptions[] = {'1' , '2' , '3', '4', '5'};        // this is how we make sure we selected a game which is availible
int numberOfGames = 5;
//_______________________________________________Define Pins___________________________________________________________________

// encoder pins
 const int outputA = 2;      
 const int outputB = 3;   
   
// endstop pins
 int endStopTop = 13;
 int endStopBot = 4;
 
//_______________________________________________LCD Set UP____________________________________________________________________
 
// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

//_______________________________________________Keypad Set UP_________________________________________________________________

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
//byte rowPins[ROWS] = {37 , 43 , 45 , 47}; //connect to the row pinouts of the keypad . (mega)
//byte colPins[COLS] = {35 , 39  ,43}; //connect to the column pinouts of the keypad .   (mega)
//Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

byte rowPins[ROWS] = {39 , 45 , 47 , 49}; //connect to the row pinouts of the keypad . (mega)
byte colPins[COLS] = {37 , 41  ,43}; //connect to the column pinouts of the keypad .   (mega)
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//_______________________________________________Stepper Motor Set UP______________________________________________________________


// initialize pins for card pusher stepper motor 
const int stepPinPush = 28; 
const int dirPinPush = 29; 
const int enPinPush = 30;

// initialize pins for card picker stepper motor 
const int stepPinPick = 22; 
const int dirPinPick = 23; 
const int enPinPick = 24;

// initialize pins for Slide stepper motor 
const int stepPinSlide = 25; 
const int dirPinSlide = 26; 
const int enPinSlide = 27;

//_______________________________________________DC Motor Set UP______________________________________________________________

// shooter motor
int enB = 5;
int in3 = 7;
int in4 = 6;

// Base motor
int enA = 10;
int in1 = 11;
int in2 = 12;

//_______________________________________________RFID Set Up______________________________________________________________

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         8           // Configurable, see typical pin layout above
#define SS_PIN          9          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

//_______________________________________________Void Set Up___________________________________________________________________


void setup() {

  Serial.begin(115200); 
  
// Set up for RFID
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                              // Init MFRC522 card  
// initialize endstop pins
  pinMode(endStopTop , INPUT);

// initialize both encoder input pins
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);

// initialize endstop pins
  pinMode(endStopTop , INPUT);
  pinMode(endStopBot , INPUT);
// initialize the DC motor pins
  pinMode(enA , OUTPUT);
  pinMode(in1 , OUTPUT);
  pinMode(in2 , OUTPUT);
  pinMode(enB , OUTPUT);
  pinMode(in3 , OUTPUT);
  pinMode(in4 , OUTPUT);

// initialize the Slide Stepper pins
  pinMode(stepPinSlide,OUTPUT); 
  pinMode(dirPinSlide,OUTPUT);

  pinMode(enPinSlide,OUTPUT);
  digitalWrite(enPinSlide,LOW);     // this powers on the motor . (could use it to turn off during program)

// initialize the pick Stepper pins
  pinMode(stepPinPick,OUTPUT); 
  pinMode(dirPinPick,OUTPUT);

  pinMode(enPinPick,OUTPUT);
  digitalWrite(enPinPick,LOW);


// initialize the push Stepper pins
  pinMode(stepPinPush,OUTPUT); 
  pinMode(dirPinPush,OUTPUT);

  pinMode(enPinPush,OUTPUT);
  digitalWrite(enPinPush,LOW);  



// Attatch interrupts to pin 2
  attachInterrupt(0, pin_A, RISING);

// set up the LCD's number of rows and columns: 
  lcd.begin(4, 20);

// start with key set to No_KEY
  char key = NO_KEY;  




}
//_______________________________________________Void Loop ___________________________________________________________________

void loop() {
// initialize variables 
  bool gameFinished = false;
  int numPlayers;
  char gameChoice = '0';

  
// home screen
  while (screenNum == 0) {
     gameChoice = '0'; 
     screenNum = startScreen();   // always returns 1
  }
// get number of players  
  while (screenNum == 1) {
  
    gameChoice = '0';
    numPlayers = getPlayers();
    if (numPlayers == 0) {
      screenNum = 0;
      break;
    }
    else {
    screenNum = 2;
    }
  }
// select your game 
  while (screenNum ==2) {
    if (gameChoice == '0') {
      gameChoice = selectGame();
    }
    // This should return to the previous screen
    else if (gameChoice == '*') {

      screenNum = 1;

      }
    
// if they choose Texas holdem
    else if ((gameChoice =='1') && (gameFinished == false)) {
      gameChoice = checkGame(gameChoice);
      if ((gameChoice =='1') && (gameFinished == false)) {
        homing();
        gameFinished = texasHoldem(numPlayers);
      }
    }
// if they choose blackjack
    else if ((gameChoice =='2') && (gameFinished == false)) {
      gameChoice = checkGame(gameChoice);
      if ((gameChoice =='2') && (gameFinished == false)) {
        homing();
        gameFinished = blackJack(numPlayers);
      }
    }
// if they choose Cacino War    
    else if ((gameChoice =='3') && (gameFinished == false)) {
      gameChoice = checkGame(gameChoice);
      if ((gameChoice =='3') && (gameFinished == false)) {
       homing();
       gameFinished = casinoWar(numPlayers);
      }
    }
// if they Choose Five Card Draw
    else if ((gameChoice =='4') && (gameFinished == false)) {
      gameChoice = checkGame(gameChoice);
      if ((gameChoice =='4') && (gameFinished == false)) {
        homing();
        gameFinished = five_card_draw(numPlayers);
      }
    }

// if they Choose Go Fish
    else if ((gameChoice =='5') && (gameFinished == false)) {
      gameChoice = checkGame(gameChoice);
      if ((gameChoice =='5') && (gameFinished == false)) {
        homing();
        gameFinished = go_fish(numPlayers);
      }
    }


   else if (gameFinished == true) {

      screenNum = closingScreen(gameChoice);     
      gameFinished = false;
   }
     
  }

}


//_______________________________________________LCD Screen Functions___________________________________________________________________

// starting screen.  This takes in nothing, but will return the number of players 
int startScreen() {
  char key = NO_KEY;      // this indicates that the key will be selected from the keypad


 
  
  
while ( key == NO_KEY) {            // while no key has been selected, present the opening screen
  lcd.setCursor(0,0);
  lcd.print("   DEALIN' DIRTY"); 
  lcd.setCursor(0,1);
  lcd.print("  CARD DEALER 3000");
  lcd.setCursor(0,2);
  lcd.print("   PRESS ANY KEY!");
  key = keypad.getKey();  
       
}
  lcd.clear();                    // . clear the screen and set the key variable to something not on the keypad

                   // . clear the screen and set the key variable to something not on the keypad
    return 1;
}


int getPlayers() {
  
  int numPlayers = 0;              //  initialize the number of players to be zero  
  int shift2 = 0;            // this will be used to toggle
  char key = keypad.getKey();                      // we will be getting the value from the keypad
  while (numPlayers == 0) {          //   This loop will get the number of players    

    if (shift2 == 0 ) {                        // prompt the user 
    lcd.setCursor(0,0);
    lcd.print("  SELECT THE NUMBER "); 
    lcd.setCursor(0,1);
    lcd.print("     OF PLAYERS     ");
    lcd.setCursor(0,3);
    lcd.print("<- *     ");
    key = keypad.getKey();                      // we will be getting the value from the keypad
      if (key == '#'   || key == '0' || key == '9'  || key == '8'  || key == '7') {              // these are all the situations where we return an error message
        shift2 = 1;
    }
    
    
      if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6') {
        shift2 = 2;
      }
      if ( key == '*') {
        lcd.clear();
        return 0;
      }
    }
    

    
    
    else if (shift2 == 1) {              // these are all the situations where we return an error message
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   SELECT A NUMBER  "); 
      lcd.setCursor(0,1);
      lcd.print("  BETWEEN 1 AND 6!  ");
      delay(1000);
      lcd.clear();
      key = reset;      
      shift2 =0;
    }
    else {                   //  return the number of players as an integer
      String number = String(key);
      lcd.clear();
      numPlayers = number.toInt();
      
    }
}

return numPlayers;
}


// Game Select. This will initiate after the number of players has been indicated

char selectGame() {   
      
int shift = 0;            // this will be used to toggle
bool gameSel = false;      // This will be used to terminate this function
char key = keypad.getKey();  

while ((gameSel== false)  ) {               // when a game has not been selected
  key = keypad.getKey();        

    if (shift ==0) {                        // the first screemn

      lcd.setCursor(0,0);
      lcd.print("   SELECT A GAME");
      lcd.setCursor(0,1);
      lcd.print("1 TEXAS HOLD'EM");
      lcd.setCursor(0,2);
      lcd.print("2 BLACKJACK");
      lcd.setCursor(0,3);
      lcd.print(" <- *          # ->"); 
    
       if (key == '#') {                   // use the number sign to move forward
        shift = shift + 1;
        lcd.clear();
        key = reset;
  
      }
      else if (key == '*') {
        lcd.clear();
        return key;
        
      }
      else {
        for( int i=0; i< numberOfGames; i++)        // at any time, a player can select their game (even if it isn't showing on their screen)
          if ( gameOptions[i] == key){
            lcd.clear();
            gameSel = true;                         // terminates the while loop
          
    }
      }
      
    }
    
    else if (shift == 1) {                        // this is the second screen 
    lcd.setCursor(0,0);
    lcd.print("3 CASINO WAR");
    lcd.setCursor(0,1);
    lcd.print("4 FIVE CARD DRAW");
    lcd.setCursor(0,2);
    lcd.print("5 Go Fish");
    lcd.setCursor(0,3);
    lcd.print(" <- *               ");

      if (key == '*') {                         // use the star symbol to move to the previous screen
        shift = shift - 1;
        key = reset;
        lcd.clear();
      }
      else {
        for( int i=0; i< numberOfGames; i++)      // at any time, a player can select their game (even if it isn't showing on their screen)
          if ( gameOptions[i] == key){
            lcd.clear();
            gameSel = true;                       // terminates the while loop
            
          
    }
      }
      }

    }

    return key;             // returns the selected game key 
}


/////// Code for the closing screen interface /////////

char closingScreen(char number) {
  char key = keypad.getKey();
  
  while ((key != '*') && (key != '#')) {
 
    lcd.setCursor(0,0);
    lcd.print("THANKS FOR PLAYING!");
    lcd.setCursor(0,2);
    lcd.print("PRESS # TO RESTART");
    lcd.setCursor(0,3);
    lcd.print("PRESS * FOR HOME");  
    key = keypad.getKey();
  }

  if (key == '*') {
    lcd.clear();
    return 0;
      }
  else if (key == '#') {
    lcd.clear();
    return 2;
  }

}

//_______________________________________________Confirm Screen Functions___________________________________________________________________
////  Programs Used to doubkle check User's Game Choice

char checkGame(char number) {
/*   
 *    this function confirms the game selection
 *    it takes in the char which was selected for the game during the game select screen
 *    it either returns '0' to return to the previous games or the char that was brought in
 */
  String gameTitle;
  char key;
 
  if (number == '1') {
    gameTitle = "   TEXAS HOLD'EM";
  }
  else if (number == '2') {
    gameTitle = "     BLACKJACK";
  }
  else if (number =='3') {
    gameTitle = "     CASINO WAR";
  }
  else if (number == '4') {
    gameTitle = "  FIVE CARD POKER";
  }
  else if ( number == '5') {
    gameTitle = "      GO FISH";
  }
  while ((key != '*') && (key != '#')) {
 
    lcd.setCursor(0,0);
    lcd.print(gameTitle);
    lcd.setCursor(0,1);
    lcd.print("#) CONFIRM");  
    lcd.setCursor(0,2);
    lcd.print("*) RETURN");  
    key = keypad.getKey();
  }

  if (key == '*') {
    lcd.clear();
    return '0';
      }
  else if (key == '#') {
    lcd.clear();
    return number;
  }
}




//_______________________________________________GamePlay Functions___________________________________________________________________



//________________________________Texas Holdem___________________________________

bool texasHoldem(int numPlayers) {    
  
  bool gameFinished = false;      // this will terminate the game
  char key = NO_KEY;     // we will be getting keys from the keypad
  int betNumber = 0;         // this will be used for the different betting periods

//// These are the variables which indicate base motor movement for the deal ////////

  int num = numPlayers  - 1;  
  int turn = (180 / numPlayers) ;
  float adjustFloat = turn / 2.0;
  int adjust =  floor(adjustFloat); 
  if (numPlayers ==0) {
    turn = 0;
  }
  int init = 90 - adjust;   // this is useful for a dealer starting directly center
  int backToStart =  turn * (num) ;  // first iteration of the deal
  int backToStart2 = turn * (num + 1);   // second iteration of the deal
  int burn = 58 ;   // this is the distance from horizontal
  int flopStart = 70 ;   // initial location of flop
  int cardSpace = 12 ;   // spacing between cards on flop, turn, river
  int tempBurn1 = backToStart + adjust - burn;  // adjustment variable for first burn
  int moveToFlop = flopStart - burn;  // adjustment variable for the flop
  int tempBurn2 = 2* cardSpace + flopStart - burn;  // adjustment variable for the second burn
  int turnLoc = flopStart + cardSpace*3 - burn;     // adjustment variable for the turn
  int tempBurn3 = 3* cardSpace + flopStart - burn;  // adjustment variable for the third burn
  int riverLoc = flopStart + cardSpace*4 - burn;    // adjustment variable for the river 
/////// This is the Deal //////////
  moveBase(false , adjust, baseSpeed);             // move to first player
  dealCard(topSpeed);                 // Deal a Card close to the player

  for (int j = 0; j<=1 ; j++){
    if (j==0) {                        // this is the first round 
      for (int i=0; i<=num-1; i++){
        moveBase(false , turn , baseSpeed );             // move to the next player
        dealCard(topSpeed);                 // Deal a Card close to the player
      }
    moveBase(true, backToStart  , baseSpeed );         // move to first player
    }
    
  else{                                 // this is the second round
    for (int p=0; p<=num-1; p++){
      dealCard(topSpeed);
      moveBase(false , turn, baseSpeed );
      }
  dealCard(topSpeed);                 // Deal a Card close to the final player

  }
}

   moveBase(true , tempBurn1 , baseSpeed );        // move to burn pile

betNumber = 1;                        // start the first betting period

while ( betNumber ==1) {            // wait for the players to finishing betting
   key = keypad.getKey(); 
 
  if ((key != '#') && (key != '*')) {
  lcd.setCursor(0,0);
  lcd.print(" RAISE! FOLD! CALL! "); 
  lcd.setCursor(0,2);
  lcd.print(" *) End Round ");   
  lcd.setCursor(0,3);
  lcd.print("               # ->");  

  }
  else if (key == '#') {                              // the players are ready, reset the variables and clear the screen
    lcd.clear();
    key = NO_KEY;
    betNumber = 0;
  }
  else {
    gameFinished = true;
    lcd.clear();
    return gameFinished;
  }
}
// Burn

  dealCard(lowSpeed);                 // Burn a Card close to the dealer

// Flop

   moveBase(false , moveToFlop  , baseSpeed );
   slideTop(false);                   // Needs to be face up
   dealCard(lowSpeed);                // deal first card
   for ( int k = 0 ; k <=1 ; k++){        // deal 2 more cards 
    moveBase(false , cardSpace , baseSpeed );
    dealCard(lowSpeed);                 // Deal a Card close to the dealer
   }

//   burn another facedown Card
  moveBase(true , tempBurn2  , baseSpeed );        // move to burn pile
  slideTop(true);                    // needs to be facedown

betNumber = 2;                        // start the second betting period

while ( betNumber ==2) {            // wait for the players to finishing betting
  key = keypad.getKey(); 
 
  if ((key != '#') && (key != '*')) {
  lcd.setCursor(0,0);
  lcd.print(" RAISE! FOLD! CALL! "); 
  lcd.setCursor(0,2);
  lcd.print(" *) End Round ");   
  lcd.setCursor(0,3);
  lcd.print("               # ->");  

  }
  else if (key == '#') {                              // the players are ready, reset the variables and clear the screen
    lcd.clear();
    key = NO_KEY;
    betNumber = 0;
  }
  else {
    gameFinished = true;
    lcd.clear();
    return gameFinished;
  }
}

  dealCard(lowSpeed);                 // Burn a Card close to the dealer

//  NOW DO THE TURN
  moveBase(false , turnLoc , baseSpeed );           // go to turn position
  slideTop(false);                     // card needs to be face up
  dealCard(lowSpeed);                 // deal card close to the dealer

// burn one face down card 
  moveBase(true , tempBurn3 , baseSpeed );        // move to burn pile
  slideTop(true);                    // card needs to be face down


betNumber = 3;
while ( betNumber ==3) {            // wait for the players to finishing betting
   key = keypad.getKey(); 
 
  if ((key != '#') && (key != '*')) {
  lcd.setCursor(0,0);
  lcd.print(" RAISE! FOLD! CALL! "); 
  lcd.setCursor(0,2);
  lcd.print(" *) End Round ");   
  lcd.setCursor(0,3);
  lcd.print("               # ->");  

  }
  else if (key == '#') {                              // the players are ready, reset the variables and clear the screen
    lcd.clear();
    key = NO_KEY;
    betNumber = 0;
  }
  else {
    gameFinished = true;
    lcd.clear();
    return gameFinished;
  }
}


  dealCard(lowSpeed);                 // Burn a card close to the dealer

// NOW DO THE RIVER
 moveBase(false , riverLoc , baseSpeed );           // move to the river location
 slideTop(false);                      // the card needs to be face up
 dealCard(lowSpeed);                  // deal a card close to the dealer

 betNumber = 4;
while ( betNumber ==4) {            // wait for the players to finishing betting
   key = keypad.getKey(); 
 
  if ((key != '#') && (key != '*')) {
  lcd.setCursor(0,0);
  lcd.print(" RAISE! FOLD! CALL! ");   
  lcd.setCursor(0,3);
  lcd.print("               # ->");  

  }
  else if (key == '#') {                              // the players are ready, reset the variables and clear the screen
    lcd.clear();
    key = NO_KEY;
    betNumber = 0;
  }
}
 
  gameFinished = true;              // tell the system the game is concluded
  lcd.clear();                      // clear the screen

  return gameFinished;        
}

// _______________________________________________________________________________________Black Jack___________________________________________________________________________________________
//________________________________Game______________________________________________________________

bool blackJack(int numPlayers) {              // Thjs requires RFID
/*
 * This function will play a game of Black Jack
 * The dealer will hold on 17 or higher. Ace, 6 is a hold for the dealer 
 * The dealer's First card is face up. second is face down
 * 
 */
 
  bool gameFinished = false;
// Variables needed for base motion
  int num = numPlayers  - 1;  
  int turn = (180 / numPlayers) ;
  float adjustFloat = turn / 2.0;
  int adjust =  floor(adjustFloat); 
  if (numPlayers ==0) {
    turn = 0;
  }
  int init = 90 - adjust;                          // this is useful for a dealer starting directly center
  int backToStart =  turn * (num) ;                // first iteration of the deal
  int backToStart2 = turn * (num + 1);             // second iteration of the deal
  int lastPlayerToDealer = turn * (num) - init ; 
  int dealerToFirst = init;

// variables needed for game play  
  int playerCard1[numPlayers] = {0};               // initialize array of player first card
  int playerCard2[numPlayers] = {0};               // initialize array of player second card   
  int playerScores[numPlayers] = {0};              // initialize array of player scores 
  String playerCard1Str[numPlayers] = {"0"};
  String playerCard2Str[numPlayers] = {"0"};
  int playerSplitScores[numPlayers] = {0};          // scores for second hand if the player splits
  int dealerScore = 0;                             // initialize score of dealer
  int dealerCard1 = 0;                             // initialize dealer's first card
  int dealerCard2 = 0;

  String card;                                     // card place holder (will change) 
  int cardValue;                                   // card value place holder (will change)
  int currScore;                                   // another useful place holder (will change)
  bool moveOn;                                     // useful when prompting players
  int hitStay;                                     // used to give players answer for hit_stay_double_split 
  bool split;                                      // used if the player has opportunity to split    
  bool playerDouble;                               // used to see if player wants to double
   
/////// This is the Deal //////////
  slideTop(false);                                  // card needs to be face up
  moveBase(false , adjust  , baseSpeed);             // move to first player
  card = dealCard(topSpeed);                        // Deal a Card close to the player
  cardValue = blackJackGetCard(card);               // get card Value
  currScore = playerScores[0] + cardValue;          // update the player's score
  playerScores[0] = currScore;
  playerCard1[0] = cardValue;                       // update player's first card
  playerCard1Str[0] = card;

  

  for (int j = 0; j<=1 ; j++){
    if (j==0) {                                    // this is the first round 
      for (int i=1; i<=num; i++){
        int currPlayerNumber = i +1;
        moveBase(false , turn  , baseSpeed );        // move to the next player
        card = dealCard(topSpeed);                        // Deal a Card close to the player
        cardValue = blackJackGetCard(card);               // get card Value
        currScore = playerScores[i] + cardValue;          // update the player's score
        playerScores[i] = currScore;
        playerCard1[i] = cardValue;                       // update player's first card
        playerCard1Str[i] = card;

        
       }
      moveBase(true , lastPlayerToDealer , baseSpeed );    // move to dealer
      card = dealCard(lowSpeed);                            // deal a card close to dealer
      cardValue = blackJackGetCard(card);                   // get card Value
      dealerScore = dealerScore + cardValue;                // update dealer's score
      dealerCard1 = dealerCard1 + cardValue;                // update the dealer's first card
      

      moveBase(true, dealerToFirst  , baseSpeed );         // move to first player
    }
    
    else{                                                 // this is the second round
      for (int p=0; p<num; p++){
        int currPlayerNumber = p +1;
        card = dealCard(topSpeed);                        // Deal a Card close to the player
        cardValue = blackJackGetCard(card);               // get card Value
        currScore = playerScores[p] + cardValue;          // update the player's score
        playerScores[p] = currScore;
        playerCard2[p] = cardValue;                       // update player's second card
        playerCard2Str[p] = card;
        moveBase(false , turn , baseSpeed ); 

        }
    card = dealCard(topSpeed);                            // Deal a Card close to the last player
    cardValue = blackJackGetCard(card);                   // get card Value
    currScore = playerScores[num] + cardValue;            // update the player's score

    playerScores[num] = currScore;
    playerCard2[num] = cardValue;                         // update player's second card
    playerCard2Str[num] = card;
    
    moveBase(true , lastPlayerToDealer , baseSpeed );         // move to Dealer
    slideTop(true);
    card = dealCard(lowSpeed);                                 // deal a card close to dealer
    cardValue = blackJackGetCard(card);                        // get card Value
    dealerScore = dealerScore + cardValue;                     // update dealer's score
    dealerCard2 = dealerCard2 + cardValue;                // update the dealer's first card    
    moveBase(true , dealerToFirst , baseSpeed );              // move to first player
    slideTop(false);                                          // face up the rest of the game
    }
  }























  

////// GAME PLAY Set up ////////

// check to see if the dealer should offer an insurance
  if (dealerCard1 == 11) {
    insurance();                                               // wait for players to make insurance bets

  }
    if (dealerScore == 21) {                                   // the dealer has blackJack
       dealerBlackJack();  
       gameFinished = true;                                   
       return gameFinished;
    }  

  int currPosition = 1;                                   // need some positional awareness
  
// need a loop that will check ahead of time whether or not the player will split
// it also needs to get the number of acs
int splits[numPlayers] = {0}; 
int numAces[numPlayers] = {0};
  for (int l = 0; l < numPlayers; l ++ ) {
    int playerNum = l + 1;                                // need the acutal player number
    int card1 = playerCard1[l];                         // get player information
    int card2 = playerCard2[l];
    int score = playerScores[l];
    String card1Str = playerCard1Str[l];
    String card2Str = playerCard2Str[l];
    int playerAce = 0;
    if (card1 == 11) {
      playerAce = playerAce +1 ;
    }
    if (card2 == 11) {
      playerAce = playerAce +1 ;
    }
    numAces[l] = playerAce;
        
        split = false;   // only will change if player decides to split
        if (card1 == card2) {                                     // if the player can split 
          if (card1 != 10) {
      
            splits[l] = 1;
            
          }        
          else {
            if (((card1Str.indexOf("1") > 0) && (card2Str.indexOf("1") > 0)) || ((card1Str.indexOf("K") > 0) && (card2Str.indexOf("K") > 0)) || ((card1Str.indexOf("Q") > 0) && (card2Str.indexOf("Q") > 0)) || ((card1Str.indexOf("J") > 0) && (card2Str.indexOf("J") > 0)) ){          
            
            splits[l] = 1;
            
            }
            else {
 
              continue;
            }
          }
        }
  }



// Game Play ////
    
// see which player wants to hit another card
  for (int k = 0; k < numPlayers; k ++ ) {
    int playerNum = k + 1;                                // need the acutal player number
    int card1 = playerCard1[k];                         // get player information
    int card2 = playerCard2[k];
    int score = playerScores[k];
    String card1Str = playerCard1Str[k];
    String card2Str = playerCard2Str[k];
  
    
    
    // is there a blackJack  
       if (score == 21) {                             // black Jack!
          black_jack_display(playerNum);                // display screen . 
          continue;
        }
        int currPositionAngle = currPosition * turn + adjust;
        int playerAngle = playerNum * turn + adjust;
        int moveDiff = playerAngle - currPositionAngle ;

         moveBase(false, moveDiff , baseSpeed);
         currPosition = playerNum;       

        
        // see if player wants to split
        split = false;   // only will change if player decides to split
        if (splits[k] ==1) {
           split = playerSplit(playerNum); 
          }        
         else {
            
              split = false;
            }
          
        
  
        if (split == true) {
          card = dealCard(topSpeed);
          cardValue = blackJackGetCard(card);
          int score1 = playerCard1[k] + cardValue;
          int score2 = playerCard2[k];
          if (playerCard1[k] != 11) {
          score1 = hit_stay_interaction(playerNum, score1 , numAces[k]);
          }
          playerScores[k] = score1;
          // second split
          moveBase(false , 8 , baseSpeed);
          String card2 = dealCard(topSpeed);
          int cardValue2 = blackJackGetCard(card2);
          score2 = score2 + cardValue2;
          if (playerCard1[k] != 11) {
          score2 = hit_stay_interaction(playerNum, score2 , numAces[k]);
          }
          playerSplitScores[k] = score2;
          moveBase(true , 8 , baseSpeed);
        }
        else if (split == false) {

   
            score = hit_stay_interaction(playerNum, score , numAces[k]);
            playerScores[k] = score;
        }
//        if (k < numPlayers - 1) {
//        moveBase(false, turn , baseSpeed);
//        }
  

  }
// Logic to move back to the dealer
   int currPositionAngle = (currPosition -1) * turn + adjust;
   if (currPositionAngle <90) {
    int diff = 90 - currPositionAngle;
    moveBase( false , diff , baseSpeed);
   }
   else {
    int diff = currPositionAngle - 90 ;
    moveBase( true , diff , baseSpeed);
   }
  flip_dealer_card();
// if dealer's score is less than 17 
  int dealerAces= 0;
  if (dealerCard1 ==11 ) {
    dealerAces = dealerAces + 1;    
  }
  if (dealerCard2 == 11) {
    dealerAces = dealerAces + 1;
  }

   int  numInGame = 0;
   for (int n = 0; n < numPlayers; n++) { 
      if ((playerScores[n] < 22) && (playerScores[n] != 0)) {
        numInGame = numInGame + 1;
      }
      if ((playerSplitScores[n] < 22) && (playerSplitScores[n] !=0)) {
        numInGame = numInGame + 1;
      }
   }

  
  while ((dealerScore < 17) &&  (numInGame != 0)) {
    card = dealCard(lowSpeed);                                 // deal card close to dealer
    cardValue = blackJackGetCard(card);                        // get card Value
    dealerScore = dealerScore + cardValue;                     // update dealer's score
    if (cardValue == 11) {
      dealerAces = dealerAces + 1;
    }





    
    if (dealerScore >21)  {
      if (dealerAces != 0) {
        dealerScore = dealerScore - 10;
        dealerAces = dealerAces - 1;
      }
    }
  }

  displayDealerScore(dealerScore);



  int listOfWinners[numPlayers] = {0};

  

  for (int x = 0; x < numPlayers; x ++ ) {
    int currPlayerNum = x + 1;
      if ((playerScores[x] < 22) && (playerScores[x] > dealerScore) || (playerScores[x] < 22) && (dealerScore >= 22) ) {
        listOfWinners[x] = currPlayerNum;
      }
      if ((playerSplitScores[x] < 22) && (playerSplitScores[x] > dealerScore)) {
        listOfWinners[x] = currPlayerNum;
      }    
  }

  int dealerWin = 0;
  for (int z = 0; z < numPlayers; z ++ ) {
    dealerWin = dealerWin + listOfWinners[z];
  }

  if ((dealerWin == 0 )&& (dealerScore <= 21)) {
    casinoWarDealerWins();
  }
  else {
    casinoWarWinners(listOfWinners , numPlayers * 2);
  }

  
  gameFinished = true;
  lcd.clear();

  return gameFinished;
}

// ______________________________________________________Helper functions______________________________________________________

void flip_dealer_card() {
  /*   
 *    this function prompts the user to flip the dealer's card
 *    it does not need to return anything
 */
   char key = NO_KEY;             
  
  while (key != '#') {
 
    lcd.setCursor(0,0);
    lcd.print("  FLIP DEALER'S  ");
    lcd.setCursor(0,1);
    lcd.print("       CARD       ");    
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
    key = keypad.getKey();             // we will be getting values from the keyPad
  }
  if (key == '#') {
    lcd.clear();
    return;
  }

}







int hit_stay_double(int player , int score) {
  /*
 * this function will ask the player if he/she would like to hit or stay or doub;e
 * it will return 0 1 or 2
 */
 bool answer = false;
 int numCards;
 String playerLine = get_line(player);
 
 while (answer == false) {
    lcd.setCursor(0,0);
    lcd.print(playerLine); 
    lcd.setCursor(18 , 0);
    lcd.print(score);
    lcd.setCursor(0,1);
    lcd.print("1) Hit     ");
    lcd.setCursor(0,2);
    lcd.print("2) Stay    ");
    lcd.setCursor(0,3);
    lcd.print("3) Double    ");    
    char key = keypad.getKey();                      // we will be getting the value from the keypad

    if (key == '1') {
      numCards = 1;
      answer = true;
    }
    else if (key == '2') {
      numCards = 0;
      answer = true;
    }

    else if (key == '3') {
      numCards = 3;
      answer = true;
    }

 
 }
 lcd.clear();
 return numCards;

}








int hit_stay_interaction(int player, int score , int numAces ) {
/*       
 * this function is a loop which continuously promots the player if they would like to hit or stay. 
 * it will break if the score exceeds 21
 */
   bool moveOn = false;                // prep for interacting with the player
   bool hitStay;
   String card;
   int cardValue;
  
   if (score == 22) {
    score = 12;
    numAces = numAces - 1;
   }
   
   int choice = hit_stay_double(player, score);
   if (choice ==3) {
    card = dealCard(topSpeed);                               // player gets exactly one card
    cardValue = blackJackGetCard(card);                      // get card Value
    score = score + cardValue;                               // update player's score
    int playerAcesDouble = numAces;
    if (cardValue == 11) {
    playerAcesDouble = playerAcesDouble + 1;
    
    }
    if ((score > 21) && (playerAcesDouble != 0)) {
    score = score - 10;
    }

    return score;                             
   }


  else if  (choice == 0) {
       moveOn = true;
       return score;
   }

   else{
       card = dealCard(topSpeed);                               //non double deal the firs card
       cardValue = blackJackGetCard(card);                      // get card Value
       score = score + cardValue;                               // update player's score
   
          if (cardValue == 11) {
        numAces = numAces + 1; 
       }
       if (score >= 22) {

        if (numAces != 0)  {
   
          score = score -10;
          numAces = numAces - 1;
        }
        else {
          bust_display(player);
          return score;                                            // the player has busted
       }
     
   }

   }

  // this is non double
  
   while (moveOn == false) {
     hitStay = hit_or_stay(player , score);
     if (hitStay == 0) {
       moveOn = true;
       return score;
     }
     else {
       card = dealCard(topSpeed);                               // player gets exactly one card
       cardValue = blackJackGetCard(card);                      // get card Value
       score = score + cardValue;                               // update player's score

       if (cardValue == 11) {
        numAces = numAces + 1; 
       }
       if (score >= 22) {

        if (numAces != 0)  {
   
          score = score -10;
          numAces = numAces - 1;
          continue;
        }
        bust_display(player);
        return score;                                            // the player has busted
       }
     }  
   }




return score;

}



int hit_or_stay(int player , int score) {
/*
 * this function will ask the player if he/she would like to hit or stay
 * it will return 0 or 1
 */
 bool answer = false;
 int numCards;
 String playerLine = get_line(player);
 
 while (answer == false) {
    lcd.setCursor(0,0);
    lcd.print(playerLine); 
    lcd.setCursor(18 , 0);
    lcd.print(score);
    lcd.setCursor(0,1);
    lcd.print("1) Hit     ");
    lcd.setCursor(0,2);
    lcd.print("2) Stay    ");
    char key = keypad.getKey();                      // we will be getting the value from the keypad

    if (key == '1') {
      numCards = 1;
      answer = true;
    }
    else if (key == '2') {
      numCards = 0;
      answer = true;
    }
 
 }
 lcd.clear();
 return numCards;

}

void insurance() {
/*   
 *    this function prompts the user's for an insurance
 *    it does not need to return anything
 */
   char key = NO_KEY;             
  
  while (key != '#') {
 
    lcd.setCursor(0,0);
    lcd.print("  PLACE INSURANCE  ");
    lcd.setCursor(0,1);
    lcd.print("       CHIPS       ");    
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
    key = keypad.getKey();             // we will be getting values from the keyPad
  }
  if (key == '#') {
    lcd.clear();
    return;
  }

}


void dealerBlackJack() {
  /*
   * This is the Dealer's black Jack screen.
   */

     char key = NO_KEY;             
  
  while (key != '#') {                    // while loop waits for players to move forward    
    lcd.setCursor(0,0);
    lcd.print("  CARD DEALER 3000  ");
    lcd.setCursor(0,1);
    lcd.print("    HAS BLACKJACK!");
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
    key = keypad.getKey();                // we will be getting values from the keyPad  
  } 

  if (key == '#') {
    lcd.clear();
    return;
  }

}



void displayDealerScore(int dealerScore) {
  /*
   * This is the Dealer's black Jack score.
   */

  char key = NO_KEY;             
  
  while (key != '#') {                    // while loop waits for players to move forward    
    lcd.setCursor(0,0);
    lcd.print("  CARD DEALER 3000  ");
    lcd.setCursor(9,1);
    lcd.print(dealerScore);
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
    key = keypad.getKey();                // we will be getting values from the keyPad  
  } 

  if (key == '#') {
    lcd.clear();
    return;
  }

}


bool playerDoublePrompt(int player) {
/*
 * this function will ask the player if he/she would like to double 
 * it will return true or false
 */
 bool answer = false;
 bool playerDouble;
 String playerLine = get_line(player);
 
 while (answer == false) {
    lcd.setCursor(0,0);
    lcd.print(playerLine); 
    lcd.setCursor(0,1);
    lcd.print("1) Double     ");
    lcd.setCursor(0,2);
    lcd.print("2) Do Not Double    ");
    char key = keypad.getKey();                      // we will be getting the value from the keypad

    if (key == '1') {
      playerDouble = true;
      answer = true;
    }
    else if (key == '2') {
      playerDouble = false;
      answer = true;
    }
 
 }
 lcd.clear();
 return playerDouble;
}

bool playerSplit(int player) {
  /*
 * this function will ask the player if he/she would like to split  
 * it will return true or false
 */
 
 bool answer = false;
 bool split;
 String playerLine = get_line(player);
 
 while (answer == false) {
    lcd.setCursor(0,0);
    lcd.print(playerLine); 
    lcd.setCursor(0,1);
    lcd.print("1) Split     ");
    lcd.setCursor(0,2);
    lcd.print("2) Do Not Split    ");
    char key = keypad.getKey();                      // we will be getting the value from the keypad

    if (key == '1') {
      split = true;
      answer = true;
    }
    else if (key == '2') {
      split = false;
      answer = true;
    }
 
 }
 lcd.clear();
 return split;

}



// ______________________________________________________________________________________Casino War ___________________________________________________________________________________________
//________________________________Game______________________________________________________________


bool casinoWar(int numPlayers) {                // this requires RFID

  bool gameFinished = false;      // this will terminate the game
  char key = NO_KEY;     // we will be getting keys from the keypad
//// These are the variables which indicate base motor movement for the deal ////////

  int num = numPlayers  - 1;  
  int turn = (180 / numPlayers) ;
  float adjustFloat = turn / 2.0;
  int adjust =  floor(adjustFloat); 
  if (numPlayers ==0) {
    turn = 0;
  }
  int init = 90 - adjust;   // this is useful for a dealer starting directly center
  int backToStart =  turn * (num) ;  // first iteration of the deal

  int lastPlayerToDealer = turn * (num) - init ; 
  int dealerToFirst = init;
  int toPlayer;
  int burnToDealer = 90;
  String card;
  int cardVal;
  String dealerCard;
  int dealerCardVal;
  int playerCards[numPlayers];
  int playerNumber;
  bool match;


/////// This is the Deal //////////
  moveBase(false , adjust , baseSpeed );             // move to first player
  slideTop(false);                      // cards are face up
  card = dealCard(topSpeed);                 // Deal a Card close to the player
  cardVal = casinoWarGetCard(card);             // this needs to be integrated into dealCard .   _____________________ CHANGE HERE ____________________
  playerCards[0] = cardVal;
  for (int i=1; i<=num; i++){
    moveBase(false , turn  , baseSpeed );                            // move to the next player
    card = dealCard(topSpeed);                 // Deal a Card close to the player
    cardVal = casinoWarGetCard(card);             // this needs to be integrated into dealCard .   _____________________ CHANGE HERE ____________________
    playerCards[i] = cardVal;
  }
  moveBase(true , lastPlayerToDealer , baseSpeed );    // move to dealer
  
     dealerCard = dealCard(lowSpeed);             
     dealerCardVal = casinoWarGetCard(dealerCard);             // this needs to be integrated into dealCard .   _____________________ CHANGE HERE ____________________  
     moveBase(true, burnToDealer  , baseSpeed );         // move to burn pile



/// START GAMEPLAY AT Burn
  int listOfWinners[numPlayers] = {0};
  int listOfTies[numPlayers] = {0};

// STEP 1: GO THROUGH TABLE, SEE WHO WON, AND WHO TIED  
  for(int i = 0; i < numPlayers; i++){
     int playerNumber = i + 1;
     bool match;
     if (dealerCardVal == playerCards[i]){
       listOfTies[i] = playerNumber;
     }
       
     else if (playerCards[i] > dealerCardVal) {                  // update winner list
      listOfWinners[i] = playerNumber;
     }
  }
// STEP 2: ASK EACH TIED PLAYER IF THEY WANT A MATH OR SURRENDER, THEN DEAL

// need a while loop for sequential ties. 

//This loop checks to see if there are ties
int playersInWar = 0;

 for(int j = 0; j < numPlayers; j++){
    if (listOfTies[j] != 0) {
      playersInWar = playersInWar+1;
    }
 }
while (playersInWar != 0) {
 int listOfTiesScores[numPlayers] = {0};
 for(int j = 0; j < numPlayers; j++){
    if (listOfTies[j] == 0) {
      continue;
    }
    int playerNumber = j + 1;
    String newPlayerCard;              // read by rfid
    int newPlayerCardVal = 0;              // converted to int
    int toPlayer = 0;
    match = casinoWarTie(playerNumber);                         // see if player wants to match or surrender
    Serial.println(match);
    if (match == false) {                                            // if surrender move to next player
       listOfTies[j] = 0;
       listOfTiesScores[j] = 0;   
       playersInWar = playersInWar - 1;           // update while loop vairable.  
    }
    else {

       slideTop(true);                     // burn face down cards 
       for (int k = 0; k < 3; k ++) {
         dealCard(lowSpeed);
       }
       toPlayer = adjust + (turn * j);     // move back to player 
       moveBase(false , toPlayer , baseSpeed );
       slideTop(false);                      // face up card 
       newPlayerCard = dealCard(topSpeed);             
       newPlayerCardVal = casinoWarGetCard(newPlayerCard); 
       Serial.println("newPlayerCard");  
       Serial.println(newPlayerCardVal);  
       listOfTiesScores[j] = newPlayerCardVal;
     
       moveBase(true , toPlayer  , baseSpeed );           // back to burn
       }
         
    }

// STEP 3: COMPARE NEW SCORES WITH DEALERS 
  int doWeDeal = 0;   
  for(int m = 0; m < numPlayers; m++){
    int playerNumber = m + 1;
    Serial.println("comparing with dealer");
    if (listOfTies[m] == 0) {
      continue;
    }
    else {
      doWeDeal = 1;
    }
  }

  if (doWeDeal ==1) {
       String newDealerCard;              // read by rfid
       int newDealerCardVal = 0;              // converted to int
       slideTop(true);                     // burn face down cards 
       for (int k = 0; k < 3; k ++) {
          dealCard(lowSpeed);
       }  
       moveBase(false , burnToDealer, baseSpeed );    // move to dealer
       slideTop(false);
       newDealerCard = dealCard(lowSpeed);
       newDealerCardVal = casinoWarGetCard(newDealerCard); 
       Serial.println("newDealerCard");
       Serial.println(newDealerCardVal);

       for (int z = 0; z < numPlayers ; z++) {
       int playerNumber = z + 1;
       if ( listOfTiesScores[z] > newDealerCardVal) {        // update winner list
            Serial.println("updateWinnerList and drop players in war");
            listOfWinners[z] = playerNumber;
            playersInWar = playersInWar -1;
            Serial.println(playersInWar);
            listOfTies[z] = 0;
       }
      else if ((listOfTiesScores[z] < newDealerCardVal) && (listOfTiesScores[z] != 0)) {       // remove losers from the list of ties
         Serial.println("drop players in war");
         playersInWar = playersInWar -1;
         Serial.println(playersInWar);
         listOfTies[z] = 0;
         
      }
       }
    }   
moveBase(true , burnToDealer, baseSpeed );    // move to burn
Serial.println("number in war is");
Serial.println(playersInWar);
}
 
 
// STEP FOUR: PROVIDE WINNERS
  int dealerWin = 0;
  for ( int p = 0; p < numPlayers; p ++) {
    int playerValue = listOfWinners[p];
    dealerWin = dealerWin + playerValue;
  }
  
  if (dealerWin == 0) {
    casinoWarDealerWins();
  }
  else {
    casinoWarWinners(listOfWinners , numPlayers);
  }
  
  gameFinished = true;
  lcd.clear();
  return gameFinished;
}







bool casinoWarTie(int playerNumber) {
/* 
 *  THIS FUNCTION prompts the player on whether or not they would like to continue
 *  returns true to continue
 *  returns false to surrender 
 */
  char key = NO_KEY;      
  Serial.println("casinoWarTie");       
  String playerLine = get_line(playerNumber);

  while ((key != '#') && (key != '*')) {
 
    lcd.setCursor(0,0);
    lcd.print(playerLine);
    lcd.setCursor(0,1);
    lcd.print("#) MATCH BET");    
    lcd.setCursor(0,2);
    lcd.print("*) SURRENDER");  
    key = keypad.getKey();             // we will be getting values from the keyPad
  }
  if (key == '#') {
    lcd.clear();
    return true;
  }
  else if (key == '*') {
    lcd.clear();
    return false;
  }
}

void casinoWarWinners(int listOfPlayers[], int numPlayers ) {
  /* 
   *  THIS FUNCTION DISPLAYS A LIST OF WINNING PLAYERS
   */
  char key = NO_KEY;              
  Serial.println("casinoWarWinners");  
  while (key != '#') {
 
    lcd.setCursor(0,0);
    lcd.print("WINNING PLAYERS:");
    for ( int i = 0; i < numPlayers; i ++) {
      int playerNum = listOfPlayers[i];
      if (playerNum != 0) {
       lcd.setCursor(2 * i, 1);
        lcd.print(playerNum);   
      }
    }
    lcd.setCursor(0,3);
    lcd.print("      PRESS #");  
    key = keypad.getKey();             // we will be getting values from the keyPad
  }
  if (key == '#') {
    lcd.clear();
    return;
  }

}


void casinoWarDealerWins() {
  char key = NO_KEY;                
  Serial.println("casinoWarDealerWins");  
  while (key != '#') {
 
    lcd.setCursor(0,0);
    lcd.print("   DEALIN' DIRTY");
    lcd.setCursor(0,1);
    lcd.print("   DID YOU DIRTY!");    
    lcd.setCursor(0,3);
    lcd.print("      PRESS #");  
    key = keypad.getKey();             // we will be getting values from the keyPad
  }
  if (key == '#') {
    lcd.clear();
    return;
  }

}










//________________________________Five Card Draw___________________________________

bool five_card_draw(int numPlayers) {    
  bool gameFinished = false;      // this will terminate the game
  char key = NO_KEY;     // we will be getting keys from the keypad
  int betNumber = 0;         // this will be used for the different betting periods
  int exchangeNumber = 0;   // this will be used for the different exchange periods
  int numCards = 0;        // this is used to see how many cards to exchange for each player
//// These are the variables which indicate base motor movement for the deal ////////

  int num = numPlayers  - 1;  
  int turn = (180 / numPlayers) ;
  float adjustFloat = turn / 2.0;
  int adjust =  floor(adjustFloat);
  if (numPlayers ==0) {
    turn = 0;
  } 
  int init = 90 - adjust;   // this is useful for a dealer starting directly center
  int backToStart =  turn * (num) ;  // first iteration of the deal
  int backToStart2 = turn * (num + 1);   // second iteration of the deal



///// first need the ante /////
 
betNumber = 1;                        // start the first betting period


while ( betNumber ==1) {            // wait for the players to finishing betting
  key = keypad.getKey(); 
 
  if (key != '#') {
    lcd.setCursor(0,0);
    lcd.print("      ANTE UP!      "); 
    lcd.setCursor(0,3);
    lcd.print("               # ->");  

  }
// escape the while loop
  else {
  betNumber = 0;
  lcd.clear();
  }

}
/////// This is the Deal //////////
  moveBase(false, adjust , baseSpeed );             // move to first player
  dealCard(topSpeed);                 // Deal a Card close to the player

  for (int j = 0; j<=4 ; j++){
    if (j==0) {                        // this is the first round 
      for (int i=0; i<=num-1; i++){
        moveBase(false, turn , baseSpeed );             // move to the next player
        dealCard(topSpeed);                 // Deal a Card close to the player
      }
    moveBase( true, backToStart  , baseSpeed );         // move to first player
    }
    
  else{                                 // this is the second round
    for (int p=0; p<=num-1; p++){
      dealCard(topSpeed);
      moveBase(false, turn, baseSpeed );
      }
    dealCard(topSpeed);                 // Deal a Card close to the final player
    
    moveBase(true, backToStart  , baseSpeed );         // move to first player
  }
}


  betNumber = 2;                        // start the second betting period
  
  while ( betNumber ==2) {            // wait for the players to finishing betting
    key = keypad.getKey(); 
   
    if (key != '#') {
    lcd.setCursor(0,0);
    lcd.print(" RAISE! FOLD! CALL! "); 
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
  
    }
    else {                           // the players are ready, reset the variables and clear the screen
      lcd.clear();       
      key = NO_KEY;
      betNumber = 0;
    }
  }


  ////////// Exchange cards //////////
   
  int numDeals = how_many_to_exchange(1);  // asking player 1  
  for (int i = 0; i<numDeals ; i++){
     dealCard(topSpeed);
  }
     
  // player one has his cards, escape the while loop
  exchangeNumber = 0; 
  numDeals = 0;  // reset variable
 
  // now loop to get the rest of the players
  for (int k = 2; k<=num+1; k++){
      moveBase(false, turn  , baseSpeed );
      exchangeNumber = k;
       int numDeals = how_many_to_exchange(k);  // asking the other players  
       for (int i = 0; i<numDeals ; i++){
          dealCard(topSpeed);
       }
       
        
   }
  
  
  betNumber = 3;                        // start the second betting period
  
  while ( betNumber ==3) {            // wait for the players to finishing betting
    key = keypad.getKey(); 
   
    if (key != '#') {
    lcd.setCursor(0,0);
    lcd.print(" RAISE! FOLD! CALL! "); 
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
  
    }
    else {                           // the players are ready, reset the variables and clear the screen
      lcd.clear();       
      key = NO_KEY;
      betNumber = 0;
    }
  }

// this is the end of the game!
  gameFinished = true;

  return gameFinished;

  }
 
//________________________________Go Fish___________________________________


bool go_fish(int numPlayers) { 
  
  bool gameFinished = false;      // this will terminate the game
  char key = NO_KEY;     // we will be getting keys from the keypad
  int numCardsLeft = 52;        // this is used to count the number of cards left in the deck
//// These are the variables which indicate base motor movement for the deal ////////

  int num = numPlayers  - 1;  
  int turn = (180 / numPlayers) ;
  float adjustFloat = turn / 2.0;
  int adjust =  floor(adjustFloat); 
  if (numPlayers ==0) {
    turn = 0;
  }
  int init = 90 - adjust;   // this is useful for a dealer starting directly center
  int backToStart =  turn * (num) ;  // first iteration of the deal
  int backToStart2 = turn * (num + 1);   // second iteration of the deal
  int numDealtCards;
  int numFish;
// get the number of card to deal each player
  if (numPlayers ==2) {
    numDealtCards = 7;
  }
  else {
    numDealtCards = 5;
  }
  

/////// This is the Deal //////////
  moveBase(false , adjust , baseSpeed );             // move to first player
  dealCard(topSpeed);                 // Deal a Card close to the player

  for (int j = 0; j<= numDealtCards -1 ; j++){
    if (j==0) {                        // this is the first round 
      for (int i=0; i<=num-1; i++){
        moveBase(false , turn , baseSpeed );             // move to the next player
        dealCard(topSpeed);                 // Deal a Card close to the player
      }
    moveBase(true, backToStart , baseSpeed );         // move to first player
    }
    
  else{                                 // this is the second round
    for (int p=0; p<=num-1; p++){
      dealCard(topSpeed);
      moveBase(false , turn , baseSpeed );
      }
    dealCard(topSpeed);                 // Deal a Card close to the final player
    
    moveBase(true , backToStart , baseSpeed );         // move to first player
  }
}

// subtract cards from the deal
  numCardsLeft = numCardsLeft - (numDealtCards * numPlayers);

// start the gameplay

  while (numCardsLeft >= 1) {                                // while there are still cards, continuously prompt each playyer
    
    for (int p = 0 ; p <= num ; p ++) {                       // need to cycle through all players
        numFish = fishCards(numCardsLeft);                    // get number of cards 
        for ( int k = 0; k <= numFish - 1; k ++ ) {           // deal number of cards
          dealCard(topSpeed);               
          numCardsLeft = numCardsLeft - 1;                    // subtract from the card count
          if (numCardsLeft == 0) {                            // If you run out of cards
            completeGoFish();
            return gameFinished;
            
          }
        }
        if ( p < num) {
          moveBase(false, turn , baseSpeed );
          
        }
        else {             
          moveBase(true , backToStart , baseSpeed );                           // move back to first player  
        }                             

         
      }

      
  
    }
  

  gameFinished = true;
  return gameFinished;
}

void completeGoFish() {
  
  char key;             
  
  while (key != '#') {                    // while loop waits for players to move forward    
    lcd.setCursor(0,0);
    lcd.print("  ENJOY GO FISH  ");
    lcd.setCursor(0,1);
    lcd.print(" SELECT # WHEN DONE ");
    lcd.setCursor(0,3);
    lcd.print("               # ->");  
    key = keypad.getKey();                // we will be getting values from the keyPad  
  } 

  if (key == '#') {
    lcd.clear();
    return;
  }

}
  




//________________________________Game Play Help Functions___________________________________  

/*
 * this is a function specific to go_fish
 * it is designed to retrieve the number of cards a player would like to get
 */

int fishCards(int numCardsLeft){

  int numCards = 0;  // set to zero for now
  int shift4 = 0;            // this will be used to toggle
  char key = keypad.getKey();                      // we will be getting the value from the keypad


while (numCards == 0) {          //   This loop will get the number of players    

    if (shift4 == 0 ) {                        // prompt the user 
    lcd.setCursor(0,0);
    lcd.print("      Go Fish!      "); 
    lcd.setCursor(0,1);
    lcd.print("     1) Go Fish     ");
    lcd.setCursor(0,2);
    lcd.print("     2) New Hand    ");
    key = keypad.getKey();                      // we will be getting the value from the keypad
      if (key == '#' || key == '*'  || key == '0' || key == '9'  || key == '8'  || key == '7'|| key == '3' || key == '4' || key == '5' || key == '6') {              // these are all the situations where we return an error message
        shift4 = 1;
    }
    
    
      if (key == '1') {
        shift4 = 2;
      }
      if (key == '2') {
        shift4 = 3;
      }
    }

    
    
    else if (shift4 == 1) {              // these are all the situations where we return an error message
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("    Please Select   "); 
      lcd.setCursor(0,1);
      lcd.print("       1 or 2!      ");
      delay(1000);
      lcd.clear();
      key = reset;      
      shift4 =0;
    }
    else if (shift4 == 2) {                   //  return the number of players as an integer
      lcd.clear();
      numCards = 1;
      
    }
    else if (shift4 == 3) {                   //  return the number of players as an integer
      if (numCardsLeft <= 5) {
        numCards = numCardsLeft;
      }
      else{
        numCards =5;
      }

      
    }
}
lcd.clear();
return numCards;
  
}




String get_line(int playerNum){
/*   
 *    this function is used because concatenation of strings in C++ is not ideal
 */
  String playerLine;
  if (playerNum == 1){
    playerLine = "PLAYER 1:           ";
  }
  else if (playerNum ==2) {
    playerLine = "PLAYER 2:           ";
  }
  else if (playerNum == 3){
    playerLine = "PLAYER 3:           ";
  }
  else if (playerNum ==4) {
    playerLine = "PLAYER 4:           ";
  }
  else if (playerNum == 5){
    playerLine = "PLAYER 5:           ";
  }
  else if (playerNum ==6) {
    playerLine = "PLAYER 6:           ";
  }
  return playerLine;
}
  

void bust_display(int player) {
/*
 */
  
     String playerLine = get_line(player);

      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("        BUST!     ");

      delay(500);
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("        BUST!     ");

      delay(500);
      lcd.clear();

      
      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("        BUST!     ");

      delay(1000);
      lcd.clear();
}


void black_jack_display(int player) {
/*
 */
  
     String playerLine = get_line(player);

      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("    BLACK JACK!     ");

      delay(500);
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("    BLACK JACK!     ");

      delay(500);
      lcd.clear();

      
      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,2);
      lcd.print("    BLACK JACK!     ");

      delay(1000);
      lcd.clear();
}


  
int how_many_to_exchange(int playerNum) {
/*
 * this is a function specific to five_card_draw
 * it is designed to retrieve the number of cards a player would like to get
 */
  int numCards = 0;  // set to zero for now
  int shift3 = 0;    // flip screens
  String playerLine = get_line(playerNum);
  char key = keypad.getKey();                      // we will be getting the value from the keypad
  while (numCards == 0) {          //   This loop will get the number of players    
       
      if (shift3 == 0 ) {                        // prompt the user 
      lcd.setCursor(0,0);
      lcd.print(playerLine); 
      lcd.setCursor(0,1);
      lcd.print("  SELECT NUMBER OF  ");
      lcd.setCursor(0,2);
      lcd.print("     NEW CARDS     ");
      key = keypad.getKey();
      
        if (key == '#' || key == '*'  || key == '6' || key == '9'  || key == '8'  || key == '7' || key == '4' || key == '5') {              // these are all the situations where we return an error message
          shift3 = 1;
      }
      
      
        else if (key == '1' || key == '2' || key == '3' || key == '0') {
          shift3 = 2;
        }
      }
  
      
      
      else if (shift3 == 1) {              // these are all the situations where we return an error message
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   SELECT A NUMBER  "); 
        lcd.setCursor(0,1);
        lcd.print("  BETWEEN 0 AND 3!  ");
        delay(1000);
        lcd.clear();
        key = reset;      
        shift3 = 0;
      }
      else if (shift3 == 2){                   //  return the number of players as an integer
        String number = String(key);
        lcd.clear();
        numCards = number.toInt();
        
      }
  }
  Serial.println(numCards);
  return numCards; 
}



//_______________________________________________Motor Control Functions___________________________________________________________________
  

bool testStart() {
  char key = keypad.getKey();
  
  while ((key != '*') && (key != '#')) {
 
    lcd.setCursor(0,0);
    lcd.print("   Start Test ");
    lcd.setCursor(0,3);
    lcd.print("<- *           # ->");  
    key = keypad.getKey();


  }

  if (key == '#') {
    lcd.clear();
    return true;
    
    
}
  else if (key == '*') {
    lcd.clear();
    return false;
    
    
}

}

bool testStart2() {
  char key = keypad.getKey();
  
  while ((key != '*') && (key != '#')) {
 
    lcd.setCursor(0,0);
    lcd.print("   Move Base ");
    lcd.setCursor(0,3);
    lcd.print("<- *           # ->");  
    key = keypad.getKey();

  }

  if (key == '#') {
    lcd.clear();
    return true;
    
    
}
  else if (key == '*') {
    lcd.clear();
    return false;
    
    
}

}




void failed_card() {
    char key = NO_KEY;
  
  while (key != '#') {
 
    lcd.setCursor(0,0);
    lcd.print(" PLEASE RESET CARD ");
    lcd.setCursor(0,1);
    lcd.print(" ON TOP OF THE SLIDE ");
    lcd.setCursor(0,3);
    lcd.print("<- *           # ->");  
    key = keypad.getKey();

  }

  if (key == '#') {
    lcd.clear();
    return true;
    
    
}

}


String dealCard(int pace) {
/* 
 *  This function deals a card by combining some of the below motor control functions.
 *  It will return the value of the Card dealt as a String
 */ 
 pickCard(false);

 String val = getCardVal();
 Serial.println(val);
 while (val == "false") {
  failed_card();
  val = getCardVal();
  Serial.println(val);
 }
 
 shoot_card(pace);  

 return val;
//
//pickCard(false);
//delay(1000);
//shoot_card(pace);

}

void shoot_card(int pace) {
/*
 * This functions operates the dealing functionality
 * The DC motor is turned on, then the push stepper feeds the card into the DC shooter motor
 * The motor is then released
 */

// turn on the Dc motor
   digitalWrite(in3 , LOW);
   digitalWrite(in4 , HIGH);
// set the speed of the DC motor
  analogWrite(enB , pace);
// push the card into the DC motor
  pushCard(false);  
// delay for one half Second
  delay(500);
// turn off the motor
  analogWrite(enB , 0);      // this should brake the motor
  digitalWrite(in3 , LOW);    // this will kill the power
  digitalWrite(in4 , LOW);    // kill the power
}

void pushCard( bool dir) {
/*
 * This function runs the pusher motor in a 360 degree rotation. 
 * The selected motor has 200 Steps per revolution
 */

// Case 1, turn the motor one direction
if (dir ==true) {
  digitalWrite(dirPinPush,HIGH);
}
// case 2, turn the motor in the other direction
else {
  digitalWrite(dirPinPush,LOW);
}

// Run: move the motor in a full rotation
  for(int x = 0; x < numStepsPick; x++) {
    digitalWrite(stepPinPush,HIGH); 
    delayMicroseconds(1250); 
    digitalWrite(stepPinPush,LOW); 
    delayMicroseconds(1250); 
  }
    
}


void moveBase(bool dir, int angle , int pace) {
/*
 * This function moves the base an indicated direction, speed, and number of degrees 
 * Each movement is incrimental. i.e. the base hase no positional awareness. 
 * dir (boolean): true indicates a counter clockwise movement. false indicates clockwise
 * angle (integer): angular adjustment, indicated in degrees
 * pace (int): speed of the motor, range: 0 - 255
 */

// Calculation: determines the number of encoder pulses, provided the given angular adjustment
 float numPulses = (angle / 360.0) * pulsesPerRev;
 int numPulsesInt = floor(numPulses);
 
// Case 1: Counter Clockwise
if (dir == true) {
  digitalWrite(in1 , HIGH);
  digitalWrite(in2 , LOW); 
}
// case 2: Clockwise
else {
  digitalWrite(in1 , LOW);
  digitalWrite(in2 , HIGH);
}

 

// move: Power the motor while the counter reads less than the number of encoder pulses
while (counter < numPulsesInt) {
 analogWrite(enA , pace);
 if (digitalRead(endStopBot) == 0) {
  counter = numPulsesInt;
  // MAYBE A BASE HOME CODE IMPLIMENTED HERE
  break;
 }
}

// Brake:
analogWrite(enA , 0);      // this should brake the motor
digitalWrite(in1 , LOW);    // this will kill the power
digitalWrite(in2 , LOW);    // kill the power

 
delay(500);                 // short delay to allow motor to stop before resetting the counter
counter = 0;

}



void pickCard( bool dir) {
/*
 * This function runs the picker motor in a 360 degree rotation. 
 * The selected motor has 200 Steps per revolution
 */

// Case 1, turn the motor one direction
if (dir ==true) {
  digitalWrite(dirPinPick,HIGH);
}
// case 2, turn the motor in the other direction
else {
  digitalWrite(dirPinPick,LOW);
}

// Run: move the motor in a full rotation
  for(int x = 0; x < numStepsPick; x++) {
    digitalWrite(stepPinPick,HIGH); 
    delayMicroseconds(900); 
    digitalWrite(stepPinPick,LOW); 
    delayMicroseconds(900); 
  }
    
}


void slideTop(bool dir) {
  
  if (dir == true) {
    digitalWrite(dirPinSlide,HIGH);
  }
  else {
    digitalWrite(dirPinSlide,LOW);
  }

  
  float circ = 2 * M_PI * rollerRadius;
  float howFar = (distance / circ) * numStepsSlide;
  int rotateStepper= floor(howFar);                   // it needs to be an integer, use the floor function

  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < rotateStepper; x++) {
    digitalWrite(stepPinSlide,HIGH); 
    delayMicroseconds(1000); 
    digitalWrite(stepPinSlide,LOW); 
    delayMicroseconds(1000); 
  }
}

void homing(){
/* 
 *  A function used to home the Dealer
 *  It makes use of two endStops: It first homes the top of the dealer, then homes the base
 *  The LCD Board prompts the User to wait as it homes the device
 *  The EndStops function as Normally Closed (read High while not engaged)
 */

// Interface: Display Message
  digitalWrite(dirPinSlide,HIGH);
    lcd.setCursor(0,0);
    lcd.print("    Homing Your   ");
    lcd.setCursor(0,1);
    lcd.print("       Device ");  
    char key = keypad.getKey();

// home the top of the device    
  while (digitalRead(endStopTop) == 1) {
    
    digitalWrite(stepPinSlide,HIGH); 
    delayMicroseconds(4000); 
    digitalWrite(stepPinSlide,LOW); 
    delayMicroseconds(4000); 
  }

  digitalWrite(dirPinSlide,LOW);
  for(int x = 0; x < 15; x++) {
    digitalWrite(stepPinSlide,HIGH); 
    delayMicroseconds(8000); 
    digitalWrite(stepPinSlide,LOW); 
    delayMicroseconds(8000); 

}



// home the bottom

  digitalWrite(in1 , HIGH);
  digitalWrite(in2 , LOW); 

  while (digitalRead(endStopBot) == 1) {
    analogWrite(enA , homeSpeed);   

  }
  
  analogWrite(enA , 0);      // this should brake the motor
  digitalWrite(in1 , LOW);    // this will kill the power
  digitalWrite(in2 , LOW);    // kill the power

  digitalWrite(in1 , LOW);
  digitalWrite(in2 , HIGH); 
  while (digitalRead(endStopBot) == 0) {
  analogWrite(enA , homeSpeed);  
}
  analogWrite(enA , 0);      // this should brake the motor
  digitalWrite(in1 , LOW);    // this will kill the power
  digitalWrite(in2 , LOW);    // kill the power
  delay(500);
  counter = 0;
  moveBase(false, 7 , homeSpeed);
  lcd.clear();

}



//_______________________________________________RFID function___________________________________________________________________   
// encorporate this into the dealCard Function. should return an int.


String getCardVal() {
/*
 * This function reads an RFID Tag Which is placed on a Card
 * The cards are preWritten using MFRC552 Library Example Code "rfid_write_personal_data".  
 * This is modified from the MFRC552 Library Example Code "rfid_read_personal_data"
 * Each has the Card Value as "first name" and Suit as "last Name"
 * The Card and Suit Values are represented as Bytes. This code converts them to a string and returns it
 * using while loops, the program waits until a valid card is read, then returns the card value as a String.
 */

// Indication: Variables used in the program
  bool foundCard = false;
  bool test = false;
  String cardVal;
  String newCard;
  String lastName = "";


  while (foundCard ==false) {
    while (test == false) {
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
continue;
    }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    continue;
  }

// Useful Variables
  byte buffer1[18];
  block = 4;
  len = 18;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
   return "false";
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    return "false";
  }

// Retreive: This Converts the Bytes which represent Card Value. Converts to a string  
  String cardVal = "";
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      char card = (char) buffer1[i];
      if (card !=""); {
        cardVal += card;
      }
    }
  }
  newCard = cardVal;
 
  byte buffer2[18];
  block = 1;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &key, &(mfrc522.uid)); //line 834
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return "false";
    
  }

  status = mfrc522.MIFARE_Read(block, buffer2, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return "false";
   
  }

// This Gets the suit of the Card
  String cardSuit = "";
  for (uint8_t i = 0; i < 16; i++) {

    char val = (char) buffer2[i];
    cardSuit += val;
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
// Confirm: if a card has been found, exit the while loops      

    if (cardSuit.substring(1, 6) == "SPADE" || cardSuit.substring(1, 6) == "HEART" || cardSuit.substring(1, 5) == "CLUB" || cardSuit.substring(1,8) == "DIAMOND" ) {
      foundCard = true;
     
      test = true;

    }
 
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  }
// Return: the card value as a string
   return newCard;
 }



int casinoWarGetCard(String card){
/*  
 *  Convert the cardValue recorded by the RFID module into integer card values for Casino War Gameplay 
 *  integers will allow for < , > , = , comparison which is useful during gameplay
 *  Ace is high 
 */
 
// initialize: CardValue as an integer   
  int cardValue;
  
// indicate:  Logic which sets integer values to the card characters  
  if (card.charAt(1) ==  'J') {
    cardValue = 11;
  }
  else if (card.charAt(1) == 'Q') {
    cardValue = 12;
  }
  else if (card.charAt(1) == 'K') {
    cardValue = 13;
  }
  else if (card.charAt(1) == 'A') {
    cardValue = 14;
  }
  else if (card.substring(1, 2) == "10") {
    cardValue = 10;

  }
  else {
    cardValue = card.toInt();
  }
// Return: 
  return cardValue;
}




int blackJackGetCard(String card ){
/*  
 *  Convert the cardValue recorded by the RFID module into integer card values for Casino War Gameplay 
 *  integers will allow for < , > , = , comparison which is useful during gameplay
 *  Ace is considered high until the player reaches a value above 21
 */  

 // initialize: cardValue as an integer
  int cardValue;

// logic: set each card Value
  if (card.charAt(1) ==  'J' || card.charAt(1) == 'Q' || card.charAt(1) == 'K' || card.substring(1, 2) == "10") {
    cardValue = 10;
  }
  else if (card.charAt(1) == 'A') {
    cardValue = 11;
  }
  
  else {
    cardValue = card.toInt();
  }

// Return:
  return cardValue;
}





//_______________________________________________interupt function___________________________________________________________________

void pin_A() {
  counter ++;
}
