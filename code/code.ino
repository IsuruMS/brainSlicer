#include <Stepper.h> // header file for stepper motors
#include <SPI.h>    // header for communication 
#include <Wire.h>     //hedder for communication
#include <LiquidCrystal_I2C.h>    //header for LCD display
#include <Keypad.h>     // header for 4 x 4 matrix keypad

LiquidCrystal_I2C lcd(0x27, 16, 2); //initialize the LCD display (I2C address, display size)

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//key map for ease of identifying the characters
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'.', '0', '#', 'D'}
};

byte colPins[numRows] = {9,8,7,6}; //Rows 0 to 3
byte rowPins[numCols]= {5,4,3,2}; //Columns 0 to 3

//initializes an object of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

#define STEPS 32    //number of steps per revolution in the stepper motor

// create an object of the stepper class using the steps and pins
Stepper stepper1(STEPS, 22, 24, 23, 25);
Stepper stepper2(STEPS, 40, 42,41, 43);

//global variables to drive the program
int val = 0;
String str = "";
bool stop = false;
int direction = -1;

void setup() 
{
  Serial.begin(9600);   // serial communication for debugging

  //set the speeds of stepper motors
  stepper1.setSpeed(500);
  stepper2.setSpeed(500);

  //initialize the lcd display
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(">");
  lcd.print("    WELCOME");
  delay(2000);
}

//loops the program again and again
void loop() 
{
  getGenericInput();
}

//function to move the base in the x direction(horizontal)
void XAxisMovement()
{
  int steps = direction * 22000;
  stepper2.step(steps);
  stepper2.step(steps);
  direction *= -1;
  delay(1000);
}

//get the thickness of the slice using the matrix keypad
int getSlice()
{
  str="";
  initializeDisplay();    //initialize the display
  char key1;    //pressed key
  do
  {
   key1 = myKeypad.getKey();
   if (key1 != NO_KEY)        //the program is working only if a key is pressed
    {
      if ( (key1 >= '0') && (key1<= '9'))      //program takes in only the keys 0 - 9
      {
         str = str+key1;
         lcd.setCursor(0, 1);
         lcd.print(str);
      }
    }
  }while(key1!='#');    //#key as the 'ok' button
  
  int a = str.toInt(); //convert the string to an integer to be used as the guide to drive the lead screws

  //validation of inputs
  // thickness of a slice should be in the range of 5mm <= a <= 50mm
  if((a<=5)&&(a!=0))
  {  
    a=5;
    str = "5 mm";
  }
  if(a>50)
  {  
    a=50;
    str = "50 mm";
  }
  
  initializeDisplay();
  lcd.setCursor(0, 1);
  lcd.print(str);      //prints the entered number
  return a;
}

//drive the display
void initializeDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Thickness (mm)");
  lcd.setCursor(0, 1);
}

//function to select the mode of the machine
//two modes Slice by slice cut and full brain cut

//slice by slice means the machine will get the 
//slice thickness cut it and stop to 
//ask the thickness of the next slice

//full brain cut means the input will be
//the thickness of all the cuts

void getGenericInput()
{
  char key;
  Display();
  while(1)
  {
    key = myKeypad.getKey();  //get the pressed key
    if(key !=NO_KEY)  //NO_KEY is a built in constant
    { 
      //key 'A' for the slice by slice mode
      if(key == 'A')
      {
        j:
        initializeDisplay();
        val = getSlice();
        val*=250;
        stepper1.step(val);
        XAxisMovement();
        val = 0;
        goto j;
      }
      //key 'B' for the whole brain mode
      if(key == 'B')
      {
        int direction = 1;
        initializeDisplay();
        val = getSlice();
        int turns = 50/val;
        val*=250;
        for(int i=0; i<turns; i++)
        {
          stepper1.step(val);
          XAxisMovement();
          delay(500);
        }
      }
      if(key=='D')
      {
        
      }
    }
  }
}

//menu screen
void Display()
{
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("A -Slice/Slice");
  lcd.setCursor(0,1);
  lcd.print("B - Whole");
}
