#include <LiquidCrystal_I2C.h>

int totalColumns = 16;
int totalRows = 4;

LiquidCrystal_I2C lcd(0x27, 16,4);  

String staticMessage = "Welcome to ARATM";
String scrollingMessage = "Please scan the Target-Image to proceed";

void scrollMessage(int row, String message, int delayTime, int totalColumns) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    delay(delayTime);
  }
}

void setup(){
  lcd.init();                    
  lcd.backlight();
}

void loop(){
  lcd.setCursor(0, 0);
  lcd.print(staticMessage);
  scrollMessage(1, scrollingMessage, 250, totalColumns);
}
