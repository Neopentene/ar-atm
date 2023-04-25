#undef HIGH
#undef LOW

#include <Keypad.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

const char* ssid = "Password is 1234";//put your wifi network name here
const char* password = "";//put your wifi password here
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

int amount = 10000;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {D0, D1, D2, D3};
byte colPins[COLS] = {D4, D5, D6, D7};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo myservo;

String passcode="";
String myPasscode="5432"; //predefined passcode, change as per your desire

void setup(){
    Serial.begin(115200);
    myservo.attach(15); //attach the servo on pin D8)
    myservo.write(150);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}
void setup_wifi() {
    delay(100);
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    String message;
    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
        message+=(char)payload[i];
    }
    if(message==myPasscode) {
        publish("Approved");
        dispatchMoney();
    } else {
        publish("Denied");
        incorrectPass();
    }
    Serial.println();
    Serial.println("-----------------------");
}

void publish(String msg){
    char message[100];
    msg.toCharArray(message,100);
    client.publish("/ABC/ATM/ACK", message);
}

void reconnect() {
// Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        //if you MQTT broker has clientID,username and password
        //please change following line to if (client.connect(clientId,userName,passWord))
        if (client.connect(clientId.c_str())) {
            Serial.println("connected");
            //once connected to MQTT broker, subscribe command if any
            client.subscribe("/ABC/ATM/RCV");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 6 seconds before retrying
            delay(6000);
        }
    }
} //end reconnect()

void loop(){
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    char myKey = keypad.getKey();
    if (myKey != NULL){
        if(myKey =='#') {
            Serial.print("You entered: ");
            Serial.println(passcode);
            if(passcode == myPasscode) {
                Serial.println("Your are welcome");
                dispatchMoney();
            } else {
                //Serial.println("Password did not match");
                incorrectPass();
                passcode="";
            }
        } else {
            passcode+=myKey;
        }
        delay(200);
    }
}

void dispatchMoney() {
    Serial.println("Dispatching money...");
    amount = amount - 10;
    Serial.print("Balance: ");
    Serial.println(amount);
    myservo.write(90); //rotates the motor counterclockwise at slow speed
    delay(5000);
    myservo.write(150); //rotates the motor counterclockwise at slow speed
}

void incorrectPass() {
  Serial.println("Incorrect PIN Code...");
  Serial.print("Balance: ");
  Serial.println(amount);
}
