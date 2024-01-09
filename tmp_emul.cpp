#include "emulatetag.h"
#include "NdefMessage.h"

#include <PN532_I2C.h>
#include <PN532.h>

#define I2C_SDA 27
#define I2C_SCL 26

TwoWire I2Ccustom = TwoWire(0);

PN532_I2C pn532_i2c(I2Ccustom);
EmulateTag nfc(pn532_i2c);

uint8_t ndefBuf[120];
NdefMessage message;
int messageSize;

uint8_t uid[3] = {0x12, 0x34, 0x56};

void setup()
{
    I2Ccustom.begin(I2C_SDA, I2C_SCL);
    Serial.begin(115200);
    Serial.println("------- Emulate Tag --------");

    message = NdefMessage();
    message.addTextRecord("Hello, world!");
    messageSize = message.getEncodedSize();
    if (messageSize > sizeof(ndefBuf))
    {
        Serial.println("ndefBuf is too small");
        while (1)
        {
        }
    }

    Serial.print("Ndef encoded message size: ");
    Serial.println(messageSize);

    message.encode(ndefBuf);

    // comment out this command for no ndef message
    nfc.setNdefFile(ndefBuf, messageSize);

    // uid must be 3 bytes!
    nfc.setUid(uid);

    nfc.init();
}

void loop()
{
    // uncomment for overriding ndef in case a write to this tag occured
    // nfc.setNdefFile(ndefBuf, messageSize);

    // start emulation (blocks)
    nfc.emulate();

    // or start emulation with timeout
    // if (!nfc.emulate(1000))
    //{ // timeout 1 second
    //  Serial.println("timed out");
    //}

    // deny writing to the tag
    // nfc.setTagWriteable(true);

    if (nfc.writeOccured())
    {
        Serial.println("\nWrite occured !");
        uint8_t *tag_buf;
        uint16_t length;

        nfc.getContent(&tag_buf, &length);
        NdefMessage msg = NdefMessage(tag_buf, length);
        msg.print();
    }

    delay(1000);
}