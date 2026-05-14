#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial fromESP(10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte degreeChar[8] = {
  0b00110, 0b01001, 0b01001, 0b00110,
  0b00000, 0b00000, 0b00000, 0b00000
};

int i=0;
int j=0;
int randomnum;
int k=0;
byte customChar1[] = {
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};

byte customChar2[] = {
  0x00,
  0x00,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};
byte customChar3[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x1F,
  0x1F,
  0x1F
};
byte customChar4[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x1F,
  0x1F
};
byte customChar5[] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

void setup() {
  Serial.begin(9600);
  fromESP.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.createChar(4, customChar1);
  lcd.createChar(3, customChar2);
  lcd.createChar(2, customChar3);
  lcd.createChar(1, customChar4);
  lcd.createChar(0, customChar5);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aromin-Haro :]");
  lcd.setCursor(0, 1);
  lcd.print("Mata-Gallentes");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Loading...    ");

  for (int col = 0; col < 16; col++) {
    lcd.setCursor(col, 1);
    lcd.write(4); // full block (customChar1)
    delay(120);
  }
  delay(500);
  lcd.clear();

  lcd.createChar(0, degreeChar); // re register
  lcd.setCursor(0, 0);
  lcd.print("Room Temperature ");
  lcd.setCursor(0, 1);
  lcd.print(" Waiting data...");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (fromESP.available()) {
    String data = fromESP.readStringUntil('\n');
    data.trim();

    Serial.println("Received: " + data);

    if (data == "ERR") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor Error!   ");
      lcd.setCursor(0, 1);
      lcd.print("Check DHT11     ");
      return;
    }

    float temp = -1, hum = -1, hindex = -1;
    int tIdx  = data.indexOf("T:");
    int hIdx  = data.indexOf(",H:");
    int hiIdx = data.indexOf(",HI:");

    if (tIdx != -1 && hIdx != -1 && hiIdx != -1) {
      temp   = data.substring(tIdx + 2, hIdx).toFloat();
      hum    = data.substring(hIdx + 3, hiIdx).toFloat();
      hindex = data.substring(hiIdx + 4).toFloat();
    }

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.write(byte(0));
    lcd.print("C ");
    lcd.print("H:");
    lcd.print((int)hum);
    lcd.print("%  ");

    lcd.setCursor(0, 1);
    lcd.print("HI:");
    lcd.print(hindex, 1);
    lcd.write(byte(0));
    lcd.print("C ");
    if (hindex < 27)       lcd.print("Comfy  ");
    else if (hindex < 32)  lcd.print("Warm   ");
    else if (hindex < 41)  lcd.print("Caution");
    else                   lcd.print("DANGER!");
  }
}
