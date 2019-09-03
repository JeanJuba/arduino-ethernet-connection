#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_Sensor.h>
#include <DHT.h> //INCLUSÃO DE BIBLIOTECA
#include <DHT_U.h>
#include <string.h>

#define TARGET_IP "192.168.0.200"
#define TARGET_PORT 3000

#define DHTPIN 53 //PINO DIGITAL UTILIZADO PELO DHT22
#define DHTTYPE DHT22 //DEFINE O MODELO DO SENSOR (DHT22 / AM2302)

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x83, 0x9D };
byte ip[] = { 192, 168, 0, 010 }; // Deve ser único na rede local
byte gateway[] = { 192, 168, 0, 1 }; // Deixei isso para consultas no caso de você precisar deles
byte subnet[] = { 255, 255, 255, 0 }; // Deixei isso para consultas no caso de você precisar deles

DHT_Unified dht(DHTPIN, DHTTYPE); //PASSA OS PARÂMETROS PARA A FUNÇÃO
EthernetServer server(80);
EthernetClient client;
uint32_t delayMS;

void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(9600); //Apenas para a saída para que possamos ver se o seu trabalho
  dht.begin();
  delay(1000);

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);           // imprime os detalhes do Sensor de Temperatura
  Serial.println("------------------------------------");
  Serial.println("Temperatura");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Valor max:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Valor min:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolucao:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");

  dht.humidity().getSensor(&sensor);            // imprime os detalhes do Sensor de Umidade
  Serial.println("------------------------------------");
  Serial.println("Umidade");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Valor max:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Valor min:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolucao:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
  Serial.println();
  Serial.println();
  delayMS = sensor.min_delay / 1000;            // define o atraso entre as leituras
  delay(1000);
}

void loop() {

  sensors_event_t event;                        // inicializa o evento da Temperatura
  dht.temperature().getEvent(&event);           // faz a leitura da Temperatura

  if (isnan(event.temperature)) {
    Serial.println("Erro na leitura da Temperatura!");
  } else {
    Serial.print("Temperatura: ");              // imprime a Temperatura
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  dht.humidity().getEvent(&event);              // faz a leitura de umidade
  if (isnan(event.relative_humidity)) {
    Serial.println("Erro na leitura da Umidade!");
  } else  {
    Serial.print("Umidade: ");                  // imprime a Umidade
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }

  if (client.connect(TARGET_IP, TARGET_PORT))  {

    Serial.println("Conectado"); //Isso não é necessário, apenas para o gabarito
    Serial.println();

    String values = "{\"latitude\":\"4\",\"longitude\":\"452\",\"temperature\":\"" + String(event.temperature) + "\"}";
    //String values = "{\"latitude\":\"4\",\"longitude\":\"452\",\"temperature\":\"" + String(event.temperature) + "\",\"humidity\":\"" + String(event.relative_humidity) + "\"}";

    int str_len = values.length() + 1;

    // Prepare the character array (the buffer)
    char char_array[str_len];

    // Copy it over
    values.toCharArray(char_array, str_len);

    client.println("POST /coordinates/ HTTP/1.1");
    client.println("Host: " + String(TARGET_IP) + ":" + String(TARGET_PORT));
    client.println("User-Agent: arduino/1.8.9");

    client.println("Content-Type:  application/json");
    client.print("Content-Length: ");
    client.println(strlen(char_array));
    client.println("Connection: close");
    client.println();
    client.println(char_array);
    client.stop();    // Closing connection to server
  } else {
    Serial.println("Connection Failed.");
    Serial.println();
  }
  delay(5000);
}
