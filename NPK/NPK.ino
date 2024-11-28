#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

#include <ModbusMaster.h>
BluetoothSerial SerialBT;
ModbusMaster mode;

int nitrogen = 0, phosphorus = 0, potassium = 0, err = 0;

void setup() {
  SerialBT.begin("ESP32test");          // Bluetooth device name
  Serial2.begin(9600, SERIAL_8N1, 17, 16);  // Serial1 on RX=17, TX=16 for Modbus
  //Serial.begin(115200);
  mode.begin(1, Serial2);               // Initialize Modbus on Serial1
}

int npk() {
  uint8_t result = mode.readHoldingRegisters(0x001E, 3);
  if (result == mode.ku8MBSuccess) {
    nitrogen = mode.getResponseBuffer(0);
    phosphorus = mode.getResponseBuffer(1);
    potassium = mode.getResponseBuffer(2);
    err = 0;
  } else {
    err = 1;
  }
  return nitrogen;
}

void loop() {
  npk();  // Call npk() to update nitrogen, phosphorus, and potassium values
  
  // Check if there was an error in communication
  if (err == 0) {
    // Send NPK values over Bluetooth
    SerialBT.print("Nitrogen: ");
    SerialBT.println(nitrogen);
    SerialBT.print("Phosphorus: ");
    SerialBT.println(phosphorus);
    SerialBT.print("Potassium: ");
    SerialBT.println(potassium);
  } else {
    // If there was an error, send an error message
    SerialBT.println("Error reading NPK values");
  }

  delay(2000);  // Wait for 2 seconds before the next read
}
