

#include "SPI.h"
#include "bluefruit.h"
#include "Arduino.h"
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include "Adafruit_TCS34725.h"
#include "Adafruit_LEDBackpack.h"


int lives = 3;
  int shots = 10;
  int score = 0;
  int capTouch = 0;
  int shot_timer = 0;

BLEUart bleuart;
BLEDfu bledfu;
// Central uart client
BLEClientUart clientUart;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  //spectrum_sensor.begin(Wire, 0, 0);
  tcs.begin();
  pinMode(13, OUTPUT);
  pinMode(5, INPUT);
  digitalWrite(13, LOW);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  Bluefruit.begin(1,1);
  Bluefruit.setTxPower(4);
  Bluefruit.setName("LASERTAG_P1");
  Bluefruit.Periph.setConnectCallback(prph_connect_callback);
  Bluefruit.Periph.setDisconnectCallback(prph_disconnect_callback);

  // Callbacks for Central
  Bluefruit.Central.setConnectCallback(cent_connect_callback);
  Bluefruit.Central.setDisconnectCallback(cent_disconnect_callback);
  bledfu.begin();
  // Configure and Start BLE Uart Service
  bleuart.begin();
  bleuart.setRxCallback(prph_bleuart_rx_callback);

  // Init BLE Central Uart Serivce
  clientUart.begin();
  clientUart.setRxCallback(cent_bleuart_rx_callback);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);     
  startAdv();
  alpha4.begin(0x70);
  alpha4.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  //TODO: Figure out why I2C slows down execution so much

  shot_timer += 1;
  if ((shot_timer % 200) == 0) {
    shots += 1;
    
    if (shot_timer == 200) {
      shot_timer = 1;
    }
  }
  int shots_temp = shots;

  int j;
  j = shots_temp % 10;
  alpha4.writeDigitAscii(3,48+j);
  shots_temp = shots_temp / 10;
  j = shots_temp % 10;
  alpha4.writeDigitAscii(2,48+j);
  shots_temp = shots_temp / 10;
  j = shots_temp % 10;
  alpha4.writeDigitAscii(1,48+j);
  shots_temp = shots_temp / 10;
  j = shots_temp % 10;
  alpha4.writeDigitAscii(0,48+j);
  alpha4.writeDisplay();
  
  uint16_t r,g,b,c,colorTemp,lux;
  tcs.getRawData(&r, &g, &b, &c);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  //Serial.println(lux);
  char str[1] = { 0 };
  clientUart.read(str,1);
  //if capTouch == 1:
  capTouch = 0;
  //capTouch = mcp.readPort(MCP23017Port::B);
  //int phototransistors[] {analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3), analogRead(A4), analogRead(A5)};
  capTouch = digitalRead(5);
  
  if (capTouch == 1) {
    if (shots > 0) {
      shots -= 1;
    
      bleuart.print('s');
     
      digitalWrite(13,HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    
  }

  
  

  
  
    //Serial.println(phototransistors[idx]);
  if (*str == 's'){
    for (int idx = 0; idx < 6; idx++) {
      //if (Serial) Serial.println(phototransistors[idx]);
      if (lux <= 100) {
        
        lives -= 1;
        Serial.println(lives);
      }
      
      //digitalWrite(5, LOW);
      
      //display -> lives
      if (!lives) {
        //display -> game over
      }
    }
  }
  //if (Serial) Serial.println(lives);
  
}
void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void prph_connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Prph] Connected to ");
  Serial.println(peer_name);
}

void prph_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.println("[Prph] Disconnected");
}

void prph_bleuart_rx_callback(uint16_t conn_handle)
{
  (void) conn_handle;
  
  // Forward data from Mobile to our peripheral
  char str[20+1] = { 0 };
  bleuart.read(str, 20);

  //Serial.print("[Prph] RX: ");
  //Serial.println(str);  

  if ( clientUart.discovered() )
  {
    clientUart.print(str);
  }else
  {
    bleuart.println("[Prph] Central role not connected");
  }
}
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet  
  Bluefruit.Central.connect(report);
}

void cent_connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.print("[Cent] Connected to ");
  Serial.println(peer_name);;

  if ( clientUart.discover(conn_handle) )
  {
    // Enable TXD's notify
    clientUart.enableTXD();
  }else
  {
    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }  
}

void cent_disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  
  Serial.println("[Cent] Disconnected");
}

/**
 * Callback invoked when uart received data
 * @param cent_uart Reference object to the service where the data 
 * arrived. In this example it is clientUart
 */
void cent_bleuart_rx_callback(BLEClientUart& cent_uart)
{
  char str[20+1] = { 0 };
  cent_uart.read(str, 20);
      
  Serial.print("[Cent] RX: ");
  Serial.println(str);

  if ( bleuart.notifyEnabled() )
  {
    // Forward data from our peripheral to Mobile
    bleuart.print( str );
  }else
  {
    // response with no prph message
    clientUart.println("[Cent] Peripheral role not connected");
  }  
}
