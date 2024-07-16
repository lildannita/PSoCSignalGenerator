#include <SoftwareSerial.h>
//                    имя                пароль             скорость           CMODE          получить адрес        записать адрес            роль
//запрос            AT+NAME?            AT+PSWD?            AT+UART?          AT+CMODE?          AT+ADDR?               AT+BIND?            AT+ROLE?
//вариант ответа  +NAME:SLAVE         +PIN:"1234"        +UART:9600,0,0        +CMODE:1                          AT+BIND=18,E4,400006       +ROLE:0
//--------------------------------------------------------------------------------------------------------------------------------------------------
//для слейва    | AT+NAME=SLAVE     AT+PSWD="1234"       AT+UART=9600,0,0     AT+CMODE=1    +ADDR:98da:50:02b0d7                             AT+ROLE=0
//для мастера   | AT+NAME=MASTER    AT+PSWD="1234"       AT+UART=9600,0,0     AT+CMODE=0                          AT+BIND=18,E4,400006     AT+ROLE=1

//сброс настроек  AT+ORGL это для крайнего случая

const int arduino_rx = 10;
const int arduino_tx = 11;
SoftwareSerial bleSerial(arduino_rx, arduino_tx);
char text[500];
int i = 0;

void setup() {
    pinMode(arduino_rx, INPUT);
    pinMode(arduino_tx, OUTPUT);
    Serial.begin(9600);
    bleSerial.begin(38400);
    Serial.println("<<< Start! >>>");
    bleSerial.println("AT");
}

void loop() {
    if (bleSerial.available()) {
        char c = bleSerial.read();
        Serial.print(c);
    }
    if (Serial.available()) {
        char c = Serial.read();
        text[i++] = c;
        if (c == '\n') {
            text[i] = '\0';
            Serial.print("-> ");
            Serial.print(text);
            bleSerial.print(text);
            i = 0;
        }
    }
}