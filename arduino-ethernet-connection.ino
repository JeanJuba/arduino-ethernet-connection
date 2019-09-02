#include <SPI.h>
#include <Ethernet.h>


#define TARGET_IP "192.168.0.200"
#define TARGET_PORT 3000

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x83, 0x9D };
byte ip[] = { 192, 168, 0, 010 }; // Deve ser único na rede local
byte gateway[] = { 192, 168, 0, 1 }; // Deixei isso para consultas no caso de você precisar deles
byte subnet[] = { 255, 255, 255, 0 }; // Deixei isso para consultas no caso de você precisar deles

EthernetServer server(80);
EthernetClient client;
void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600); //Apenas para a saída para que possamos ver se o seu trabalho
  delay(1000);
}

void loop() {
  if (client.connect(TARGET_IP, TARGET_PORT))  {

    Serial.println("Conectado"); //Isso não é necessário, apenas para o gabarito
    Serial.println();

    char PostData[] = "{\"latitude\":\"4\",\"longitude\":\"452\",\"temperature\":\"345\"}";
   
    client.println("POST /coordinates/ HTTP/1.1");
    client.println("Host: " + String(TARGET_IP) + ":" + String(TARGET_PORT));
    client.println("User-Agent: arduino/1.8.9");

    client.println("Content-Type:  application/json");
    client.print("Content-Length: ");
    client.println(strlen(PostData));
    client.println("Connection: close");
    client.println();
    client.println(PostData);
    client.stop();    // Closing connection to server
  } else {
    Serial.println("Connection Failed.");
    Serial.println();
  }
  delay(5000);
}
