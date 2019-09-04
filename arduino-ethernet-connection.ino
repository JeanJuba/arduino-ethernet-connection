#include <SPI.h>
#include <Ethernet.h>
#include <dht_nonblocking.h>
#include <TinyGPS.h>
#include <SoftwareSerial.h>

#define TARGET_IP "192.168.0.200"
#define TARGET_PORT 3000
#define DHT_SENSOR_TYPE DHT_TYPE_22

static const int DHT_SENSOR_PIN = 53;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x83, 0x9D };
byte ip[] = { 192, 168, 0, 010 }; // Deve ser único na rede local
byte gateway[] = { 192, 168, 0, 1 }; // Deixei isso para consultas no caso de você precisar deles
byte subnet[] = { 255, 255, 255, 0 }; // Deixei isso para consultas no caso de você precisar deles

EthernetServer server(80);
EthernetClient client;
TinyGPS gps; // create gps object


void setup()
{
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.begin(4800);
}



/*
   Poll for a measurement, keeping the state machine alive.  Returns
   true if a measurement is available.
*/
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if ( millis( ) - measurement_timestamp > 4000ul )
  {
    if ( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return ( true );
    }
  }

  return ( false );
}



void loop() {
  while (Serial.available()) {
    int c = Serial.read();
    gps.encode(c);
  }

  long latitude, longitude;
  gps.get_position(&latitude, &longitude, NULL);
  Serial.print("Latitude:  "); Serial.println(latitude);
  Serial.print("Longitude:  "); Serial.println(longitude);

  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);

  float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
  Serial.print("LAT=");
  Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  Serial.print(" LON=");
  Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  Serial.print(" SAT=");
  Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
  Serial.print(" PREC=");
  Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  Serial.println();
  Serial.println();


  if (client.connect(TARGET_IP, TARGET_PORT))  {

    Serial.println("Conectado"); //Isso não é necessário, apenas para o gabarito
    Serial.println();

    float temperature;
    float humidity;

    /* Measure temperature and humidity.  If the functions returns
       true, then a measurement is available. */

    while (measure_environment( &temperature, &humidity ) == false) {
      //Serial.println("Tentando...");
    }

    Serial.print( "T = " );
    Serial.print( temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( humidity, 1 );
    Serial.println( "%" );

    //String values = "{\"latitude\":\"4\",\"longitude\":\"452\",\"temperature\":\"" + String(humidity) + "\"}";
    String values = "{\"latitude\":\"4\",\"longitude\":\"452\",\"temperature\":\"" + String(temperature) + "\",\"humidity\":\"" + String(humidity) + "\"}";

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
  delay(10000);
}
