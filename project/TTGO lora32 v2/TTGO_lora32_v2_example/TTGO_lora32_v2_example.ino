

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <U8x8lib.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>




/**
 *                                       !!! IMPORTANT !!!
 * 
 * Make sure to configure ".../Arduino/libraries/MCCI_LoRaWAN_LMIC_library/project_config/lmic_project_config.h"
 * 
 */

#define BUILTIN_LED 2

#define OLED_SDA 21
#define OLED_SCL 22 
//#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


int count = 0;





// the OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}
//0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x01, 0xA6
// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8]={   0xA6, 0x01, 0x05, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {0xCB, 0x36, 0x59, 0x80, 0xE9, 0xFD, 0x0C, 0x41, 0xBA, 0xB6, 0xE4, 0xC5, 0xA0, 0xA5, 0x66, 0x58};
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

static uint8_t mydata[] = "test";
static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 30;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 23,
  .dio = {26, 33, 32},
};
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void printHex2(unsigned v) {
    v &= 0xff;
    if (v < 16)
        Serial.print('0');
    Serial.print(v, HEX);
}


void newOledPrintf(int col, int row, String fmt, ...) {
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(col,row);

  display.setTextColor(WHITE, BLACK);
  display.print(fmt);
  display.display();



  
}


void onEvent (ev_t ev) {
    long now = os_getTime();
    newOledPrintf(0,0,"LoRaWAN IOT devices",now);
    //oledPrintf(0, 5, "Time %lu", now);
    // clear the lines
    //3 = 10, 6 = 20, 7 = 30
    newOledPrintf(0, 10, "");
    newOledPrintf(0, 20, "");
    newOledPrintf(0, 30, "");

    switch(ev) {
        case EV_SCAN_TIMEOUT:
            newOledPrintf(0, 30, "EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
             newOledPrintf(0, 30, "EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
             newOledPrintf(0, 30, "EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
             newOledPrintf(0, 30, "EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
             newOledPrintf(0, 30, "EV_JOINING");
            break;
        case EV_JOINED:
             newOledPrintf(0, 30, "EV_JOINED");
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
             /**
              Serial.print("netid: ");
              Serial.println(netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<sizeof(artKey); ++i) {
                if (i != 0) Serial.print("-");
                printHex2(artKey[i]);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0) Serial.print("-");
                      printHex2(nwkKey[i]);
              }
              Serial.println();
            **/
            }
            
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
              // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            newOledPrintf(0, 30, "EV_RFU1");
            break;
        case EV_JOIN_FAILED:
             newOledPrintf(0, 30, "EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            newOledPrintf(0, 30, "EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
             newOledPrintf(0, 30, "EV_TXCOMPLETE");
            digitalWrite(BUILTIN_LED, LOW);
            if (LMIC.txrxFlags & TXRX_ACK) {
              String msg3 = "rssi:"+ String(LMIC.rssi)+", snr:"+String(LMIC.snr);
              newOledPrintf(0, 10, msg3);
               newOledPrintf(0, 20, "Received ack");
               String msg5 =  "packages send "+ String(count);
                  newOledPrintf(0, 40, msg5);
            }
            if (LMIC.dataLen) {
              newOledPrintf(0, 10, "rssi:%d, snr:%1d", LMIC.rssi, LMIC.snr);
               newOledPrintf(0, 20, "Received %d", LMIC.dataLen);
                  String msg5 =  "packages send "+ String(count);
                  newOledPrintf(0, 40, msg5);
              /**
              Serial.print("Data:");
              for(size_t i=0; i<LMIC.dataLen; i++) {
                Serial.print(" ");
                printHex2(LMIC.frame[i + LMIC.dataBeg]);
              }
              Serial.println();
            **/
            }
            
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            newOledPrintf(0, 30, "EV_LOST_TSYNC");
            break;
        case EV_RESET:
             newOledPrintf(0, 30, "EV_RESET");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
             newOledPrintf(0, 30, "EV_RXCOMPLETE");
            break;
        case EV_LINK_DEAD:
             newOledPrintf(0, 30, "EV_LINK_DEAD");
            break;
        case EV_LINK_ALIVE:
             newOledPrintf(0, 30, "EV_LINK_ALIVE");
            break;
        case EV_SCAN_FOUND:
             newOledPrintf(0, 30, "EV_SCAN_FOUND");
            break;
        case EV_TXSTART:
             newOledPrintf(0, 30, "EV_TXSTART");
            break;
        case EV_TXCANCELED:
            newOledPrintf(0, 30, "EV_TXCANCELED");
            break;
        case EV_RXSTART:
             newOledPrintf(0, 30, "EV_RXSTART");
            break;
        case EV_JOIN_TXCOMPLETE:
            newOledPrintf(0, 30, "EV_JOIN_TXCOMPLETE");
            break;
        default:
             newOledPrintf(0, 30, "Unknown event %ud", ev);
            break;
    }
}

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
     //   Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        // Prepare upstream data transmission at the next possible time.
        u1_t confirmed = 1;
        LMIC_setTxData2(1, mydata, sizeof(mydata)-1, confirmed);
       // Serial.println(F("Packet queued"));
        digitalWrite(BUILTIN_LED, HIGH);
      
        
          count++;
      
        
    }
   
    // Next TX is scheduled after TX_COMPLETE event.
}



void setup() {
    Serial.begin(115200);    
 
   digitalWrite(OLED_RST, LOW);
   delay(20);
   digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  } 
  display.clearDisplay();



    //pinMode(OLED_RST, OUTPUT);

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    LMIC_setAdrMode(false);

    // Start job (sending automatically starts OTAA too)
    
    
    do_send(&sendjob);

  //  pinMode(BUILTIN_LED, OUTPUT);
  //  digitalWrite(BUILTIN_LED, LOW);
}

void loop() {
    os_runloop_once();
}
