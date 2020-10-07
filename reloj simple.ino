#include <LiquidCrystal.h>

#define CLOCK_MODE 0
#define SET_ALARM_MODE 1
#define BUZZER_PIN 2
#define BUTTON_RIGHT 0
#define BUTTON_UP 1
#define BUTTON_DOWN 2
#define BUTTON_LEFT 3
#define BUTTON_SELECT 4
#define BUTTON_NONE 5

/*********** Declaración de variables ********/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
unsigned long hours, minutes, seconds;
int hoursAlarm, minutesAlarm, secondsAlarm;
bool alarmActivated = true;
int mode = CLOCK_MODE;

/********** Caracteres personalizados ********/
uint8_t alarmIcon[8] = {
    0b00000,
    0b00100,
    0b01110,
    0b11011,
    0b10001,
    0b10001,
    0b11111,
    0b00100};

uint8_t ringingAlarm[8] = {
    0b11011,
    0b10101,
    0b01110,
    0b11011,
    0b10001,
    0b10001,
    0b11111,
    0b00100};


uint8_t upArrow[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100};

uint8_t downArrow[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b11111,
    0b01110,
    0b00100};

uint8_t leftArrow[8] = {
    0b00000,
    0b00100,
    0b01100,
    0b11111,
    0b11111,
    0b01100,
    0b00100,
    0b00000};

uint8_t rightArrow[8] = {
    0b00000,
    0b00100,
    0b00110,
    0b11111,
    0b11111,
    0b00110,
    0b00100,
    0b00000};

/*********** Prototipo de funciones **********/
void clock();
void showClock();
void showAlarm();
void showSetAlarm();
int readButtons();
void checkAlarm();
void setAlarm();

/**************** VOID / LOOP ***************/
void setup()
{
    lcd.createChar(1, alarmIcon);
    lcd.createChar(2, upArrow);
    lcd.createChar(3, downArrow);
    lcd.createChar(4, leftArrow);
    lcd.createChar(5, rightArrow);
    lcd.createChar(6, ringingAlarm);
    lcd.begin(16, 2);
}

int previousButton = BUTTON_NONE;
void loop()
{
    clock();
    
    int currentButton = readButtons();
    if(currentButton==BUTTON_SELECT && previousButton==BUTTON_NONE){
        mode = !mode;
    }
    previousButton = currentButton;
    switch (mode)
    {
    case CLOCK_MODE:
        showClock();
        showAlarm();
        checkAlarm();
        break;
    case SET_ALARM_MODE:
        showSetAlarm();
        showAlarm();
        setAlarm();
    }
}
/************** END VOID / LOOP **************/

void clock()
{
    unsigned long currentMillis = millis();
    seconds = currentMillis / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
}

void showClock()
{
    unsigned long hoursDisplay = hours % 24;
    unsigned long minutesDisplay = minutes % 60;
    unsigned long secondsDisplay = seconds % 60;

    lcd.setCursor(0, 0);
    if (hoursDisplay < 10)
        lcd.print("0");
    lcd.print(hoursDisplay);
    lcd.print(":");
    if (minutesDisplay < 10)
        lcd.print("0");
    lcd.print(minutesDisplay);
    lcd.print(":");
    if (secondsDisplay < 10)
        lcd.print("0");
    lcd.print(secondsDisplay);
}

void showAlarm()
{
    lcd.setCursor(0, 1);
    if (hoursAlarm < 10)
        lcd.print("0");
    lcd.print(hoursAlarm);
    lcd.print(":");
    if (minutesAlarm < 10)
        lcd.print("0");
    lcd.print(minutesAlarm);
    lcd.print(":");
    if (secondsAlarm < 10)
        lcd.print("0");
    lcd.print(secondsAlarm);
}

void checkAlarm()
{
    if (alarmActivated)
    {
        // La alarma va a sonar un minuto completo
        int hoursCompare = hours % 24;
        int minutesCompare = minutes % 60;
        lcd.setCursor(9, 1);
        if (hoursCompare == hoursAlarm && minutesCompare == minutesAlarm)
        {
            tone(BUZZER_PIN, 2000);
            lcd.write(6);
        }
        else
        {
            tone(BUZZER_PIN, 0);
            lcd.write(1);
        }
    }
    else
    {
        tone(BUZZER_PIN, 0);
        lcd.write(1);
    }
}

void showSetAlarm()
{
    lcd.setCursor(0, 0);
    lcd.write(2);
    lcd.write(3);
    lcd.write(" ");
    lcd.write(4);
    lcd.write(5);
    lcd.print("   "); // Estos espacios están para borrar los posibles caracteres que hayan quedado en el display
}

int previousButtonAlarm = BUTTON_NONE;
void setAlarm()
{
    int currentButton = readButtons();
    if (previousButtonAlarm == BUTTON_NONE && currentButton != BUTTON_NONE)
    {
        switch (currentButton)
        {
        case BUTTON_UP:
            hoursAlarm++;
            if (hoursAlarm == 24)
                hoursAlarm = 0;
            break;
        case BUTTON_DOWN:
            hoursAlarm--;
            if (hoursAlarm == -1)
                hoursAlarm = 23;
            break;
        case BUTTON_LEFT:
            minutesAlarm--;
            if (minutesAlarm == -1)
                minutesAlarm = 59;
            break;
        case BUTTON_RIGHT:
            minutesAlarm++;
            if (minutesAlarm == 60)
                minutesAlarm = 0;
            break;
        }
    }
    previousButtonAlarm = currentButton;
}

int readButtons()
{
    int value = analogRead(0); // Leemos A0
    // Mis botones dan:  0, 145, 329,507,743
    // Y ahora los comparamos con un margen comodo
    if (value > 900)
        return BUTTON_NONE; // Ningun boton pulsado
    if (value < 50)
        return BUTTON_RIGHT;
    if (value < 250)
        return BUTTON_UP;
    if (value < 450)
        return BUTTON_DOWN;
    if (value < 650)
        return BUTTON_LEFT;
    if (value < 850)
        return BUTTON_SELECT;
    return BUTTON_NONE; // Por si todo falla
}

