#include <LiquidCrystal_I2C.h>
  #define TOFF 1 //state tracker "turned off"
  #define TON 0 // state tracker "turned on"
  LiquidCrystal_I2C lcd(0x27,16,2);//library function adress column and row data is here
  int button1Pin = 2;//button connected to pin 2
  int button2Pin = 3;//button connected to pin 3
  int led1Pin = 4;//led connected to pin 4
  int led2Pin = 5;//led connected to pin 5
  int buttonState1;//state for button connected to pin 2 
  int buttonState2;//state for button connected to pin 3 
  int userInput;//variable that holds data which user inputs
  void menuReturn();//function that prints default menu interface
  void menuCall();//function that starts sequence for user io
  void mod1();//first mode of the game, single player 
  void mod2();//second mode of the game, multiplayer
  void setup() {
    pinMode(button1Pin, INPUT);
    pinMode(button2Pin, INPUT);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    digitalWrite(button1Pin, HIGH);//for checking button voltage is given and circuit is complete, when button is pressed circuit is cut
    digitalWrite(button2Pin, HIGH);
    randomSeed(analogRead(1));// this is due to reason that analog when is empty always returns a random value since it tries to read a voltage that doesnt exist and we use it to change random sequence
    Wire.begin();//i2c function for lcd communication
    lcd.init();//i2c function
    lcd.backlight();//turns on lcd backlight
  }

  void loop() {
    lcd.clear();
    userInput = callMenu();//variable userInput is assigned to what is returned by menu
    if(userInput==1){
      mod1();
    }
    else if(userInput==2){
      mod2();
    }
    delay(2000);//giving some time for it to reboot
  }

  //Menu will be called, user will press buttons to select between modes, and it will be double checked

  int callMenu(){
    int flag1=0;//mod1 counter,we use flags to double check
    int flag2=0;//mod2 counter
    //a stream to inform player
    lcd.setCursor(0,0);
    lcd.print("Please select");
    delay(1000);
    lcd.clear();
    lcd.print("Mode1 Button 1");
    lcd.setCursor(0,1);
    lcd.print("Mode2 Button 2");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode 1 -- Mode 2");
    while(1){
      buttonState1 = digitalRead(button1Pin);
      buttonState2 = digitalRead(button2Pin);
      delay(200);//a delay to prevent double check to be useless since device will read a lot of states in a mere second
      if (buttonState1 == TON && buttonState2 == TOFF){
        menuReturn(); 
        lcd.setCursor(0,1);
        lcd.print("^");
        flag2=0;
        flag1++;
        if(flag1==1){
          lcd.setCursor(5,1);
          lcd.print("Press again");
        }
      }
      else if (buttonState1 == TOFF && buttonState2 == TON){
        menuReturn();
        lcd.setCursor(15,1);
        lcd.print("^");
        flag1=0;
        flag2++;
        if(flag2==1){
          lcd.setCursor(0,1);
          lcd.print("Press again");
        }
      }
      else if (buttonState1 == TON && buttonState2 == TON){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("You cant do that");//Shouldnt be possible,just in case
        delay(2000);
        menuReturn();
        flag2=0;
        flag1=0;
      }
      if(flag1==2){
        lcd.clear();
        return 1;//returns mode that will be put into action
      }
      else if(flag2==2){
        lcd.clear();
        return 2;//same but for mod2 
      }

    }
  }

  void menuReturn(){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode 1 -- Mode 2");//we print this line several times while double-check process so writing this as a function makes sense
  }

  // mod1 is a competition by yourself, led is turned on for a brief time and you need to press correct button that is aligned with led
  void mod1(){
    delay(2000);
    int array[10]={700,600,400,350,325,300,250,200,150,100}//array that holds values for time in ms
    int i=0;
    int intendedTime;//time variable for a level will be changed according to "array"
    int life=3;//life variable; when it hits zero, game is over
    int gameLed,ngameLed,gameBut,ngameBut;// gameLed will turn on ,player needs to press gameBut, life is decremented when ngameBut is pressed
    int gamePoints=0;//points
    lcd.setCursor(0,0);//sets cursor
    lcd.print("P: ");
    lcd.setCursor(0,1);
    lcd.print("Life: +++");//8,7,6 will be modified into " "
    while(i<10 &&life>0){
      unsigned long startTime=millis();//millis counts time passed since boot,startTime is set here to count time differences at each cycle of loop
      intendedTime = array[i];//time variable changed
      gameLed= random(4,6);//game selects whichever led-button combination randomly
      gameBut=gameLed-2;//circuitry is set up so that they have standard pin layout so thats the reason behind "2"
      if(gameBut==button1Pin){
        ngameLed=gameLed+1;
        ngameBut=gameBut+1;
      }
      //this whole section determines the way game will go with pins and stuff
      else{
        ngameLed=gameLed-1;
        ngameBut=gameBut-1;
      }
      digitalWrite(gameLed,HIGH);//gameLed for this cycle is turned on
      while(1){ 
        if((millis()-startTime)<intendedTime){
          if(digitalRead(gameBut)==TON && digitalRead(ngameBut)==TOFF){
            lcd.setCursor(gamePoints+3,0);
            lcd.print("+");
            gamePoints++;
            digitalWrite(gameLed,LOW);
            break;
          }//uses millis() to determine time passed and prints info on lcd to inform
          else if(digitalRead(gameBut)==TOFF && digitalRead(ngameBut)==TON){
            lcd.setCursor(life+5,1);
            lcd.print(" ");//here life is given as 3 by default but in order to delete "+++" we simply change one of the "+" to " " the life+5 on the upper line is done for this reason
            life--;
            digitalWrite(gameLed,LOW);
            break;
          }
        }
        else{
          digitalWrite(gameLed,LOW);
          //this round failed since player hadnt pressed any of the buttons at all
          break;
        }
      }
      digitalWrite(gameLed,LOW);
      delay(1500);
      i++;
    } 
    //user information
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("You got:");
    lcd.print(gamePoints);
    lcd.print(" Points");
  }
  //mod2 is multiplayer, both leds are turned on and whichever player first presses button gets a point
  void mod2(){
    int i=0;
    int buttonCheat=0;
    int score1=0;
    int score2=0;
    while(i<11){
      digitalWrite(led1Pin,LOW);
      digitalWrite(led2Pin,LOW); 
      buttonState1=digitalRead(button1Pin);
      buttonState2=digitalRead(button2Pin);
      if(buttonState1==TON || buttonState2==TON){
        //this prevents players from cheating
        //if a player presses button before leds turn on, game stops until they let go
        i--;
        delay(800);
      }
      else{
        delay(random(300,700));//random intervals so player shouldnt understand patterns between times
        digitalWrite(led1Pin,HIGH);//both leds are turned on
        digitalWrite(led2Pin,HIGH);
        while(1){
          buttonState1=digitalRead(button1Pin);
          buttonState2=digitalRead(button2Pin);
          //fundamentals,checking buttonstates and incrementing points
          if(buttonState1==TOFF && buttonState2==TON){
            score1++;
            for(int k=0;k<score1;k++){
              lcd.setCursor(k,0);
              lcd.print("+");
            }
            break;
          }
          else if(buttonState1==TON && buttonState2==TOFF){
            score2++;
            for(int k=15;15-k<score2;k--){
              lcd.setCursor(k,1);
              lcd.print("+");
            }
            break;
          }
          else if(buttonState1==TON && buttonState2==TON){
            i--;
            //shouldnt really be possible but just in case
            break;
          }
        } 
          delay(100);
      }
      i++;
      if(i==11){
          digitalWrite(led1Pin,LOW);
          digitalWrite(led2Pin,LOW);
          delay(500);
          lcd.clear();
        }
    }
    if(score1>score2){
      lcd.setCursor(0,0);
      lcd.print("Player 1 Won");
    }
    else{
      lcd.setCursor(0,0);
      lcd.print("Player 2 Won");
    }
  }