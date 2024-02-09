// библиотека для работы с GPRS устройством
#include <AmperkaGPRS.h>

// длина сообщения
#define MESSAGE_LENGTH 160
// текст сообщения о включении розетки
#define MESSAGE_ON  "On"
// текст сообщения о выключении розетки
#define MESSAGE_OFF  "Off"
// текст сообщения о состоянии розетки
#define MESSAGE_STATE  "State"
// пин, к которому подключено реле
#define RELAY 5
// пин, к которому подключен диод включения
#define LED_ON 10
// пин, к которому подключен диод работы розетки
#define LED_POWER 9
 
// текст сообщения
char message[MESSAGE_LENGTH];
// номер, с которого пришло сообщение
char phone[16];
// дата отправки сообщения
char datetime[24];
 
bool stateRelay = false;
 
// создаём объект класса GPRS и передаём в него объект Serial1 
GPRS gprs(Serial1);
 
void setup()
{
  // настраиваем пины диодов в режим выхода
  pinMode(LED_ON, OUTPUT);
  pinMode(LED_POWER, OUTPUT);
  // настраиваем пин реле в режим выхода
  pinMode(RELAY, OUTPUT);
  // подаём на пин реле «низкий уровень» (размыкаем реле)
  digitalWrite(RELAY, LOW); 
  // включаем GPRS шилд
  gprs.powerOn();
  
 // открываем Serial-соединение с GPRS Shield
  Serial1.begin(9600);
    while (!gprs.begin()) {
    // если связи нет, ждём 1 секунду
    // процесс повторяется в цикле
    // пока не появится ответ от GPRS устройства
    gprs.powerOn();
    delay(5000);
  }
  //включаем индикатор включения
  digitalWrite(LED_ON, HIGH);
}
 
void loop()
{
  // если пришло новое сообщение
  if (gprs.incomingSMS()) {
    // читаем его
    // если есть хотя бы одно непрочитанное сообщение,
    // читаем его
    gprs.readSMS(message, phone, datetime);
    // вызываем функцию изменения состояния реле
    // в зависимости от текста сообщения
    setRelay(phone, message);
  }
}
 
void setRelay(char f_phone[], char f_message[])
{
  if (strcmp(f_message, MESSAGE_ON) == 0) {
    // если сообщение — с текстом «On»,
    // подаём на замыкаем реле
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED_POWER, HIGH);
    stateRelay = true;
    // на номер, с которого пришёл запрос,
    // отправляем смс с текстом о включении питания
    gprs.sendSMS(f_phone, "Power is On");
  } else if (strcmp(f_message, MESSAGE_OFF) == 0) {
    // если пришло сообщение с текстом «Off»,
    // размыкаем реле
    digitalWrite(RELAY, LOW);
    digitalWrite(LED_POWER, LOW);
    stateRelay = false;
    // на номер, с которого пришёл запрос
    // отправляем смс с текстом о выключении питания
    gprs.sendSMS(f_phone, "Power is Off");
  } else if (strcmp(f_message, MESSAGE_STATE) == 0) {
    // если пришло сообщение с текстом «State»,
    // отправляем сообщение с состоянием реле
    if (stateRelay) {
      gprs.sendSMS(f_phone, "Power is On");
    } else {
      gprs.sendSMS(f_phone, "Power is Off");
    }
  } else {
    // если сообщение содержит неизвестный текст,
    // отправляем сообщение с текстом об ошибке
    gprs.sendSMS(f_phone, "Error...unknown command!");
  }
}
