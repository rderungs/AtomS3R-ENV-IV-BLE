#include <Arduino.h>
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedENV.h>
#include <NimBLEDevice.h>
#include <math.h>

m5::unit::UnitUnified Units;
m5::unit::UnitENV4 unitENV4;
auto &sht40 = unitENV4.sht40;
auto &bmp280 = unitENV4.bmp280;

static const uint16_t SVC_ENV       = 0x181A;
static const uint16_t CHR_TEMP      = 0x2A6E;
static const uint16_t CHR_HUM       = 0x2A6F;
static const uint16_t CHR_PRESSURE  = 0x2A6D;

NimBLEServer *server;
NimBLECharacteristic *cTemp, *cHum, *cPres;

uint32_t nextEnvMs = 0;

class ServerCBs : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& info) override {
    (void)s; (void)info;
  }
  void onDisconnect(NimBLEServer* s, NimBLEConnInfo& info, int reason) override {
    (void)s; (void)info; (void)reason;
    NimBLEDevice::startAdvertising();
  }
};

void setup()
{
  auto cfg = M5.config();
  cfg.internal_imu = false;
  cfg.internal_mic = false;
  cfg.internal_spk = false;
  cfg.led_brightness = 0;

  M5.begin(cfg);
  M5.Display.setBrightness(0);
  M5.Display.sleep();

  // External I2C for ENV4 on Port.A
  auto pin_sda = M5.getPin(m5::pin_name_t::port_a_sda);
  auto pin_scl = M5.getPin(m5::pin_name_t::port_a_scl);
  Wire.begin(pin_sda, pin_scl, 400000U);
  if (!Units.add(unitENV4, Wire) || !Units.begin()) {
    while (true) { m5::utility::delay(10000); }
  }

  NimBLEDevice::init("AtomS3R-ENV");
  NimBLEDevice::setMTU(247);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);

  server = NimBLEDevice::createServer();
  server->setCallbacks(new ServerCBs());

  NimBLEService *svcEnv = server->createService(NimBLEUUID((uint16_t)SVC_ENV));
  cTemp = svcEnv->createCharacteristic(NimBLEUUID((uint16_t)CHR_TEMP), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  cHum  = svcEnv->createCharacteristic(NimBLEUUID((uint16_t)CHR_HUM), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  cPres = svcEnv->createCharacteristic(NimBLEUUID((uint16_t)CHR_PRESSURE), NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  svcEnv->start();

  NimBLEAdvertising *adv = NimBLEDevice::getAdvertising();
  NimBLEAdvertisementData advData;
  advData.setFlags(0x06);
  adv->setAdvertisementData(advData);
  adv->addServiceUUID(NimBLEUUID((uint16_t)SVC_ENV));
  adv->enableScanResponse(true);
  NimBLEAdvertisementData sr;
  sr.setName("AtomS3R-ENV");
  adv->setScanResponseData(sr);

  NimBLEDevice::startAdvertising();

  nextEnvMs = millis();
}

void loop()
{
  M5.update();
  Units.update();

    if (sht40.updated()) {
      float T = sht40.temperature();
      int16_t t100 = (int16_t)lroundf(T * 100.0f);
      cTemp->setValue((uint8_t *)&t100, sizeof(t100));
      cTemp->notify();

      float H = sht40.humidity();
      uint16_t h100 = (uint16_t)lroundf(H * 100.0f);
      cHum->setValue((uint8_t *)&h100, sizeof(h100));
      cHum->notify();
    }

    if(bmp280.updated()) {
      float P = bmp280.pressure();
      uint32_t pPa = (uint32_t)lroundf(P);
      cPres->setValue((uint8_t *)&pPa, sizeof(pPa));
      cPres->notify();
    }

  delay(1000);
}