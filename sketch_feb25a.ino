
/*
  File: Trådlöss Kaffemaskin
  Author: Philip Röden
  Date: 2023-02-25
  Description: Ett program som använder sig av WiFi modulen på en Arduino WiFi för att kunna trådlöss kunna starta och stänga av en relay som är kopplade till en kaffe bryggare
 */

char ssid[] = "iPhone";             // Your network SSID (name)
char pass[] = "52820009";           // Your network password
int keyIndex = 0;                   // Your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;        // Connection status
WiFiServer server(80);              // Server socket
WiFiClient client = server.available(); // Create a client object for incoming connections
int ledPin = 2;

/*
Function that checks WiFi module for connections and then connects to the WiFi 
*/
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  while (!Serial);
  enable_WiFi();    // Function call to check WiFi module
  connect_WiFi();   // Function call to connect to WiFi network
  server.begin();   // Start the server
  printWifiStatus(); // Function call to print WiFi status
}

/* 

*/
void loop() {
  client = server.available(); // Check for incoming client connections
  if (client) {
    printWEB(); // Function call to handle client requests
  }
}
/*
Function that prints out the ssid of the network the Arduino is connected to, it also prints the ip address of the website used to control the relay.
*/
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("The page: http://");
  Serial.println(ip);
}

/* function that checks the status of the WiFi module and return error messages to the terminal if it fails to connect to the WiFi module, then checks the version of the WiFi modules firmware and return error message.
*/
void enable_WiFi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  String fv = WiFi.firmwareVersion();  // Check firmware version 
    if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

/* 
function that try to connect to WiFi
*/
void connect_WiFi() {
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);    // Connect to WPA/WPA2 network
    delay(10000);

  }
}
/*
Handles incoming client requests. If a client connects and sends data, it reads the data line by line. If the request is for turning an LED on or off it sends that signal to the LED. The code also constructs and sends an HTML page with buttons to control the LED.
*/
void printWEB() {
  if (client) {
    Serial.println("New client");
    String currentLine = ""; // String to hold incoming data from the client
    while (client.connected()) { // Loop while the client is connected
      if (client.available()) {   // If there's data available from the client
        char c = client.read();  // Read a byte
        // Print the byte to the serial monitor
        Serial.write(c);
        if (c == '\n') { // If the byte is a newline character
          if (currentLine.length() == 0) {  // If the current line is
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<a href=\"/H\">Starta</a> kaffe maskin<br>");
            client.print("<a href=\"/L\">Stoppa</a> kaffe maskin<br><br>");
            client.println();
            break;
          }
          else {
            currentLine = "";
          }
        }
        else if (c != '\r') {    // If it's not a carriage return character
          currentLine += c;      // Add it to the current line
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(ledPin, HIGH);        
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(ledPin, LOW);       
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}
