# AtomS3R-ENV BLE Sensor

This sketch turns an **M5Stack AtomS3R** with an **M5Unit ENV IV** into a small,
headless BLE environmental sensor.  
It exposes temperature, humidity and pressure via the standard BLE
**Environmental Sensing Service**.

---

## Features

- Uses **M5Unified** + **M5UnitUnified** + **M5UnitENV4**
- Reads from:
  - SHT40: temperature & humidity  
  - BMP280: pressure
- Publishes data over BLE:
  - Service: Environmental Sensing (`0x181A`)
  - Characteristics:
    - Temperature (`0x2A6E`)
    - Humidity (`0x2A6F`)
    - Pressure (`0x2A6D`)
- Notifies values once per second
- Powers down unused peripherals:
  - Display off (backlight + panel sleep)
  - Internal IMU, mic and speaker disabled
  - On-board mono LED off
- BLE name: **`AtomS3R-ENV`**

---

## Hardware

- **M5Stack AtomS3R**
- **M5Unit ENV IV** (SHT40 + BMP280)
- Connection:
  - Plug ENV IV into **Port.A** (Grove) of the AtomS3R base / hub

---

## Dependencies

Arduino libraries (via Library Manager or git):

- `M5Unified`
- `M5UnitUnified`
- `M5UnitENV`
- `NimBLE-Arduino`

ESP32 board support:

- ESP32 core by Espressif (for AtomS3R)

---

## BLE Details

- **Device name:** `AtomS3R-ENV`
- **Service UUID:** `0x181A` (Environmental Sensing)

Characteristics:

| Characteristic | UUID    | Type    | Description     |
|----------------|---------|---------|-----------------|
| Temperature    | `0x2A6E`| sint16  | °C × 100        |
| Humidity       | `0x2A6F`| uint16  | %RH × 100       |
| Pressure       | `0x2A6D`| uint32  | Pressure in Pa  |

All characteristics support **READ** and **NOTIFY**.

---

## Building & Flashing

1. Open the sketch in **Arduino IDE** or **PlatformIO**.
2. Select the correct **ESP32-S3** board and serial port for your AtomS3R.
3. Install the dependencies listed above.
4. Build and upload the sketch.
5. After reset, scan with a BLE app (e.g. nRF Connect) and look for
   **`AtomS3R-ENV`**.

You should see the Environmental Sensing Service and the three characteristics
updating about once per second.

---

## Notes

- The device is designed to run headless with the screen turned off.
- To debug, you can re-enable the display and peripherals in `setup()`
  by adjusting the `cfg` fields and removing the display sleep/brightness calls.
