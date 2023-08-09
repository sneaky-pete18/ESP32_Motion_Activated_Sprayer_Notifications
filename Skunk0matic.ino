
#include <ESP32Servo.h>
//________________SEND EMAIL CODE Defs____________________________

//To use send Email for Gmail to port 465 (SSL), less secure app option should be enabled. https://myaccount.google.com/lesssecureapps?pli=1

#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

/** The smtp host name e.g. smtp.gmail.com for GMail or smtp.office365.com for Outlook or smtp.mail.yahoo.com
 * For yahoo mail, log in to your yahoo mail in web browser and generate app password by go to
 * https://login.yahoo.com/account/security/app-passwords/add/confirm?src=noSrc
 * and use the app password as password with your yahoo mail account to login.
 * The google app password signin is also available https://support.google.com/mail/answer/185833?hl=en
*/
#define SMTP_HOST "smtp.gmail.com"

/** The smtp port e.g. 
 * 25  or esp_mail_smtp_port_25
 * 465 or esp_mail_smtp_port_465
 * 587 or esp_mail_smtp_port_587
*/
#define SMTP_PORT 465

/* The sign in credentials */
#define AUTHOR_EMAIL ""
#define AUTHOR_PASSWORD ""

/* The SMTP Session object used for Email sending */
SMTPSession smtp;

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status);





//______ANDREWS CODE DEFs_____________________
Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 18;
int pin = 22;
int relayPin = 23;
int DIR = 16;
int PWM = 17;
int pwmon = 255; 
int pwmoff = 0;
unsigned long lastMillis = 5*60*1000;
int EmailInterval = 5*60*1000;

void setup()
{  
  Serial.begin(115200);
  //======== WIFI ========
  Serial.print("Connecting to AP");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();



  
  //=========== ANDREW's CODE SETUP =========
  myservo.write(75);
  pinMode(DIR, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(pin,INPUT);
  pinMode(relayPin, OUTPUT);
  //Serial.begin(115200);
  

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  //myservo.write(75);
  //myservo.attach(servoPin, 500, 2500); // attaches the servo on pin 18 to the servo object
  myservo.attach(servoPin); //try without setting min/max
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep
  delay(60*1000);


}
 
void loop()
{

  //myservo.write(75);
  bool isDetected = LOW;
  isDetected = digitalRead(pin);
  
 
  if((isDetected) && (millis() - lastMillis >= EmailInterval)){
    Serial.println("Presence detected");
    digitalWrite(PWM, HIGH);
    analogWrite(DIR, pwmoff);
    digitalWrite(relayPin, HIGH);
    myservo.write(120);
    
//
   delay(4000);
//    

    myservo.write(75);
    digitalWrite(relayPin, LOW);
    digitalWrite(PWM, LOW);
    analogWrite(DIR, 0);
    lastMillis = millis();

        //========EMAIL SEND TEST =========

     /** Enable the debug via Serial port
   * none debug or 0
   * basic debug or 1
  */
  smtp.debug(1);

  /* Set the callback function to get the sending results */
  smtp.callback(smtpCallback);

  /* Declare the session config data */
  ESP_Mail_Session session;

  /** ########################################################
   * Some properties of SMTPSession data and parameters pass to 
   * SMTP_Message class accept the pointer to constant char
   * i.e. const char*. 
   * 
   * You may assign a string literal to that properties or function 
   * like below example.
   *   
   * session.login.user_domain = "mydomain.net";
   * session.login.user_domain = String("mydomain.net").c_str();
   * 
   * or
   * 
   * String doman = "mydomain.net";
   * session.login.user_domain = domain.c_str();
   * 
   * And
   * 
   * String name = "Jack " + String("dawson");
   * String email = "jack_dawson" + String(123) + "@mail.com";
   * 
   * message.addRecipient(name.c_str(), email.c_str());
   * 
   * message.addHeader(String("Message-ID: <abcde.fghij@gmail.com>").c_str());
   * 
   * or
   * 
   * String header = "Message-ID: <abcde.fghij@gmail.com>";
   * message.addHeader(header.c_str());
   * 
   * ###########################################################
  */

  /* Set the session config */
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "mydomain.net";

  /* Declare the message class */
  SMTP_Message message;

  /* Set the message headers */
  message.sender.name = "Skunk-0-matic";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "[ALERT] Sprayer FIREDDDD";
  message.addRecipient("Someone");
  String textMsg = "Skunk0matic fired!";
  message.text.content = textMsg.c_str();

  /** The Plain text message character set e.g.
   * us-ascii
   * utf-8
   * utf-7
   * The default value is utf-8
  */
  message.text.charSet = "us-ascii";

  /** The content transfer encoding e.g.
   * enc_7bit or "7bit" (not encoded)
   * enc_qp or "quoted-printable" (encoded)
   * enc_base64 or "base64" (encoded)
   * enc_binary or "binary" (not encoded)
   * enc_8bit or "8bit" (not encoded)
   * The default value is "7bit"
  */
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  /** The message priority
   * esp_mail_smtp_priority_high or 1
   * esp_mail_smtp_priority_normal or 3
   * esp_mail_smtp_priority_low or 5
   * The default value is esp_mail_smtp_priority_low
  */
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;

  /** The Delivery Status Notifications e.g.
   * esp_mail_smtp_notify_never
   * esp_mail_smtp_notify_success
   * esp_mail_smtp_notify_failure
   * esp_mail_smtp_notify_delay
   * The default value is esp_mail_smtp_notify_never
  */
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  /* Set the custom message header */
  message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());


  }
  else if ((isDetected) && (millis() - lastMillis <= EmailInterval)){

    Serial.println("Presence detected");
    digitalWrite(PWM, HIGH);
    analogWrite(DIR, pwmoff);
    digitalWrite(relayPin, HIGH);
    myservo.write(120);
    

//
   delay(4000);
//    

    myservo.write(75);
    digitalWrite(relayPin, LOW);
    digitalWrite(PWM, LOW);
    analogWrite(DIR, 0);




    
  }
  else{
     Serial.println("Nothing");

  }
  delay(500);
}

/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status)
{
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success())
  {
    Serial.println("----------------");
    Serial.printf("Message sent success: %d\n", status.completedCount());
    Serial.printf("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      localtime_r(&result.timesstamp, &dt);

      Serial.printf("Message No: %d\n", i + 1);
      Serial.printf("Status: %s\n", result.completed ? "success" : "failed");
      Serial.printf("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      Serial.printf("Recipient: %s\n", result.recipients);
      Serial.printf("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}
