#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define TARGET_SOIL 65          //  65   
#define PUMP_SEC 5              //  5
#define WAIT_SEC 120            //  120
#define PUMPS_TO_ERROR 5        //  5

#define PUMP_PIN 7

LiquidCrystal_I2C lcd(63,16,2);  

bool fault = false; 

int pumpCount = 0;


void setup() {
  
  Serial.begin(9600);                 // initialize serial communication at 9600 bits per second:
  
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, HIGH);

  
  
  lcd.init();                     
  lcd.backlight();
  lcd.print("Garduino");
  lcd.setCursor(2, 1);
  lcd.print("%");
  setStatus("");
  
}


void loop() 
{
  int soil = readSoil();
  
  if (!fault) 
  {     
    int i = 0;
    
    while (soil < TARGET_SOIL)          //do maximum "PUMPS_TO_ERROR" number of attempts to run the pump
    {
      i++;
      
      if (i > PUMPS_TO_ERROR)        // check if there were too many attempts already
      {
        fault = true;
        setStatus("Fill Tank");        
        break;
      }
      else 
      {
        pumpFor(PUMP_SEC);          //switch on the pump
        waitFor(WAIT_SEC);           //wait      
      }
      
      soil = readSoil();                //Read soil moisture again
    }
  }


  delay(1000);                      // delay in between reads for stability
}


int readSoil(){                     //Returns soil moisture percentage
  //read sensor voltage
  int sensorValue = analogRead(A0);
  sensorValue = constrain(sensorValue, 0, 1023);

  //map the value to a percentage
  int soilPercentage = map(sensorValue, 0, 1023, 100, 0);

  //show on the screen
  lcd.setCursor(0, 1);
  if (soilPercentage < 10){
    lcd.print(" ");
  }
  lcd.print(soilPercentage);

  Serial.println(soilPercentage);
  Serial.println(pumpCount);
  
  return soilPercentage;
}

void pumpFor(int sec) {             //Switch on the pump for XX seconds

  pumpCount++;
  lcd.setCursor(10, 0);
  lcd.print(String(pumpCount));
  
  digitalWrite(PUMP_PIN, LOW);


  
  for (int i = sec; i > 0; i--){
    setStatus("Pump " + String(i));
    delay(1000);
    readSoil();
  }

  digitalWrite(PUMP_PIN, HIGH);

  setStatus("");
  
}


void waitFor(int Sec) {               //waiting water to get into the soil fo XX seconds
  
  
  
  for (int i = Sec; i > 0; i--){
    setStatus("Wait " + String(i));
    delay(1000);
    readSoil();
  }

  setStatus("");
  
}


void setStatus(String text) {         //write status to the display
  lcd.setCursor(5, 1);
  lcd.print("            ");
  lcd.setCursor(5, 1);
  lcd.print(text);
}

