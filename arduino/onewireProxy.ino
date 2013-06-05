#include <OneWire.h>

OneWire ds(12);

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

void romRead(void) {
      uint8_t someBytes[9];
    memset(someBytes, 0, sizeof(someBytes));
    if (ds.reset())
    {
      ds.write(0x33);
      ds.read_bytes(someBytes, sizeof(someBytes));
    }
    Serial.write(someBytes, sizeof(someBytes));
}

void flashRead(void) {
    uint8_t moreBytes[16];
    memset(moreBytes, 0, sizeof(moreBytes));  
    if (ds.reset())
    {
      ds.write(0xCC);
      ds.write(0xF0);
      ds.write(0x00);
      ds.write(0x00);
      // Read each byte of 512 and mirror onto prox
      for( int i = 0; i < 512; i++) 
      {
        Serial.write(ds.read());
      }
    }
}

void flashWrite(void) {
      // Write to the Flash
    // Allocate and clear some ram to buffer into
    uint8_t newFlash[512];
    memset(newFlash, 0, sizeof(newFlash));

    //Fill it with data
    Serial.readBytes((char*) newFlash, sizeof(newFlash));

    // Each loop is a 4 byte chunk
    for( uint16_t offset = 0; offset < (512/4); offset++)
    {
      uint16_t memAddress = offset * 4;
      // Fail if presense fails
      if (!ds.reset()) { 
        Serial.write("pf1"); 
        return; 
      }
      ds.write(0xCC); // Skip ROM
      ds.write(0x0F); // Write Scratch Pad

      // Compute the two byte address
      uint8_t a1 = (uint8_t) memAddress;
      uint8_t a2 = (uint8_t) (memAddress >> 8);

      // Write the offset bytes . . . why in this order?!?!?
      ds.write(a1);
      ds.write(a2);

      // Fill the 4 byte scratch pad
      for (uint8_t i = 0; i < 4; i++)
        ds.write(newFlash[memAddress + i]);

      // Fail if presense fails
      if (!ds.reset()) { 
        Serial.write("pf2"); 
        return; 
      }
      ds.write(0xCC); // Skip ROM
      ds.write(0xAA); // Read scratch pad

      // Confirm offset gets mirrored back, otherwise fail
      if(ds.read() != a1) {
        Serial.write("a1f"); 
        return;
      }
      if(ds.read() != a2) {
        Serial.write("a2f"); 
        return;
      }

      // Read the confirmation byte, store it
      uint8_t eaDS = ds.read();
      Serial.println(eaDS, HEX);
      // Check the scratchpad contents
      for (int i = 0; i < 4; i++)
      {  
        uint8_t verifyByte = ds.read();
        if ( verifyByte != newFlash[(offset*4 + i)]) 
        { 
          Serial.println("f Scratch Verify"); 
          Serial.println(offset); 
          Serial.println(i);
          Serial.println(a1, HEX);
          Serial.println(a2, HEX);
          Serial.println(verifyByte, HEX);
          Serial.println(newFlash[(offset*4 + i)], HEX);
          return; 
        }
        else
        {
           Serial.println("Verified 4 Bytes");
        }
      }
      // Fail if presense fails
      if (!ds.reset()) { 
        Serial.write("pf3"); 
        return; 
      }
      ds.write(0xCC); // Skip ROM
      ds.write(0x55); // Copy scratchpad

      // Confirmation bytes
      ds.write(a1);
      ds.write(a2);
      ds.write(eaDS, 1); // Pullup!
      delay(5); // Wait 5 ms per spec.
      
    }
    Serial.println("ttt");
}



void loop(void) {
  if (Serial.available() == 0)
    return;

  int bRead = Serial.read();
  if (bRead == -1)
    return;
  char cmdBuffer = bRead;

  switch (cmdBuffer)
  {
  case 'r':       // Read the ROM 
    romRead();
    break;
  case 'f':      //  Read the Flash
    flashRead();
    break;
  case 'w':  
    flashWrite();
    break;
  case 'x':  // Check   from chip present/absent
    if (ds.reset())
      Serial.write("p"); 
    else 
      Serial.write("a");
    break; 
  case 'e':   // Echo
    Serial.write('e');
    break;
  }
}






