/*

 _                      _       _   _
| |                    (_)     | | | |
| |      ___ __      __ _  ___ | |_| |  ___
| |     / _ \\ \ /\ / /| |/ __||  _  | / _ \
| |____|  __/ \ V  V / | |\__ \| | | ||  __/
\_____/ \___|  \_/\_/  |_||___/\_| |_/ \___|

Compatible with all TTGO camera products, written by LewisHe
03/28/2020
*/

#if defined(T_Camera_V16_VERSION)
  #define PWDN_GPIO_NUM       -1
  #define RESET_GPIO_NUM      -1
  #define XCLK_GPIO_NUM       4
  #define SIOD_GPIO_NUM       18
  #define SIOC_GPIO_NUM       23
  #define Y9_GPIO_NUM         36
  #define Y8_GPIO_NUM         15
  #define Y7_GPIO_NUM         12
  #define Y6_GPIO_NUM         39
  #define Y5_GPIO_NUM         35
  #define Y4_GPIO_NUM         14
  #define Y3_GPIO_NUM         13
  #define Y2_GPIO_NUM         34
  #define VSYNC_GPIO_NUM      5
  #define HREF_GPIO_NUM       27
  #define PCLK_GPIO_NUM       25
  #define AS312_PIN           19
  #define BUTTON_1            0
  #define I2C_SDA             21
  #define I2C_SCL             22
  #define SSD130_MODLE_TYPE   0       // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32
  #define IIS_SCK             26
  #define IIS_WS              32
  #define IIS_DOUT            33

#elif defined(T_Camera_V05_VERSION)
  #define PWDN_GPIO_NUM       26
  #define RESET_GPIO_NUM      -1
  #define XCLK_GPIO_NUM       32
  #define SIOD_GPIO_NUM       13
  #define SIOC_GPIO_NUM       12
  #define Y9_GPIO_NUM         39
  #define Y8_GPIO_NUM         36
  #define Y7_GPIO_NUM         23
  #define Y6_GPIO_NUM         18
  #define Y5_GPIO_NUM         15
  #define Y4_GPIO_NUM         4
  #define Y3_GPIO_NUM         14
  #define Y2_GPIO_NUM         5
  #define VSYNC_GPIO_NUM      27
  #define HREF_GPIO_NUM       25
  #define PCLK_GPIO_NUM       19
  #define AS312_PIN           33
  #define BUTTON_1            34
  #define I2C_SDA             21
  #define I2C_SCL             22
  #define SSD130_MODLE_TYPE   0       // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32

#elif defined(T_Camera_JORNAL_VERSION)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM       5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17  
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21  
  #define I2C_SDA           14
  #define I2C_SCL           13  
  #define BUTTON_1          32  
  #define SSD130_MODLE_TYPE   1   // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32

#elif defined(T_Camera_PLUS_VERSION)
  #define PWDN_GPIO_NUM       -1
  #define RESET_GPIO_NUM      -1
  #define XCLK_GPIO_NUM       4
  #define SIOD_GPIO_NUM       18
  #define SIOC_GPIO_NUM       23
  #define Y9_GPIO_NUM         36
  #define Y8_GPIO_NUM         37
  #define Y7_GPIO_NUM         38
  #define Y6_GPIO_NUM         39
  #define Y5_GPIO_NUM         35
  #define Y4_GPIO_NUM         26
  #define Y3_GPIO_NUM         13
  #define Y2_GPIO_NUM         34
  #define VSYNC_GPIO_NUM      5
  #define HREF_GPIO_NUM       27
  #define PCLK_GPIO_NUM       25
  #define I2C_SDA             18
  #define I2C_SCL             23
  #define IIS_SCK             14
  #define IIS_WS              32
  #define IIS_DOUT            33
  #define TFT_CS_PIN          12
  #define TFT_DC_PIN          15
  #define TFT_MOSI_PIN        19
  #define TFT_MISO_PIN        22
  #define TFT_SCLK_PIN        21
  #define TFT_BL_PIN          2
  #define SDCARD_CS_PIN       0
  #define ENABLE_IP5306
  #define ENABLE_TFT

#elif defined(T_Camera_V162_VERSION)
  #define PWDN_GPIO_NUM       -1
  #define RESET_GPIO_NUM      -1
  #define XCLK_GPIO_NUM       4
  #define SIOD_GPIO_NUM       18
  #define SIOC_GPIO_NUM       23
  #define Y9_GPIO_NUM         36
  #define Y8_GPIO_NUM         37
  #define Y7_GPIO_NUM         38
  #define Y6_GPIO_NUM         39
  #define Y5_GPIO_NUM         35
  #define Y4_GPIO_NUM         14
  #define Y3_GPIO_NUM         13
  #define Y2_GPIO_NUM         34
  #define VSYNC_GPIO_NUM      5
  #define HREF_GPIO_NUM       27
  #define PCLK_GPIO_NUM       25
  #define I2C_SDA             21
  #define I2C_SCL             22
  #define BUTTON_1            15
  #define SSD130_MODLE_TYPE   0   // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32
  #define AS312_PIN           19  //not rtc io ,can't form deepsleep wakeup
  #define IIS_SCK             26
  #define IIS_WS              32
  #define IIS_DOUT            33
  #define ENABLE_IP5306

#elif defined(T_Camera_MINI_VERSION)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    32
  #define SIOD_GPIO_NUM    13
  #define SIOC_GPIO_NUM    12
  #define Y9_GPIO_NUM      39
  #define Y8_GPIO_NUM      36
  #define Y7_GPIO_NUM      38
  #define Y6_GPIO_NUM      37
  #define Y5_GPIO_NUM      15
  #define Y4_GPIO_NUM      4
  #define Y3_GPIO_NUM      14
  #define Y2_GPIO_NUM      5
  #define VSYNC_GPIO_NUM   27
  #define HREF_GPIO_NUM    25
  #define PCLK_GPIO_NUM    19
  #define CAMERA_PWR_PIN   26
  #define I2C_SDA          21
  #define I2C_SCL          22
  #define BUTTON_1         33
  #define POWER_CONTROL_PIN     26
  #define ENABLE_AXP192

#elif defined(T_Camera_V17_VERSION)
  #define PWDN_GPIO_NUM       -1
  #define RESET_GPIO_NUM      -1
  #define XCLK_GPIO_NUM       32
  #define SIOD_GPIO_NUM       13
  #define SIOC_GPIO_NUM       12
  #define Y9_GPIO_NUM         39
  #define Y8_GPIO_NUM         36
  #define Y7_GPIO_NUM         23
  #define Y6_GPIO_NUM         18
  #define Y5_GPIO_NUM         15
  #define Y4_GPIO_NUM         4
  #define Y3_GPIO_NUM         14
  #define Y2_GPIO_NUM         5
  #define VSYNC_GPIO_NUM      27
  #define HREF_GPIO_NUM       25
  #define PCLK_GPIO_NUM       19
  #define I2C_SDA             21
  #define I2C_SCL             22
  #define BUTTON_1            34
  #define SSD130_MODLE_TYPE   0   // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32
  #define AS312_PIN           33
  #define ENABLE_IP5306

#elif defined(ESPRESSIF_ESP_EYE)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    4
  #define SIOD_GPIO_NUM    18
  #define SIOC_GPIO_NUM    23
  #define Y9_GPIO_NUM      36
  #define Y8_GPIO_NUM      37
  #define Y7_GPIO_NUM      38
  #define Y6_GPIO_NUM      39
  #define Y5_GPIO_NUM      35
  #define Y4_GPIO_NUM      14
  #define Y3_GPIO_NUM      13
  #define Y2_GPIO_NUM      34
  #define VSYNC_GPIO_NUM   5
  #define HREF_GPIO_NUM    27
  #define PCLK_GPIO_NUM    25
  #define BUTTON_1         0
  
#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM 15
  #define XCLK_GPIO_NUM 27
  #define SIOD_GPIO_NUM 25
  #define SIOC_GPIO_NUM 23
  #define Y9_GPIO_NUM 19
  #define Y8_GPIO_NUM 36
  #define Y7_GPIO_NUM 18
  #define Y6_GPIO_NUM 39
  #define Y5_GPIO_NUM 5
  #define Y4_GPIO_NUM 34
  #define Y3_GPIO_NUM 35
  #define Y2_GPIO_NUM 32
  #define VSYNC_GPIO_NUM 22
  #define HREF_GPIO_NUM 26
  #define PCLK_GPIO_NUM 21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM 15
  #define XCLK_GPIO_NUM 27
  #define SIOD_GPIO_NUM 25
  #define SIOC_GPIO_NUM 23
  #define Y9_GPIO_NUM 19
  #define Y8_GPIO_NUM 36
  #define Y7_GPIO_NUM 18
  #define Y6_GPIO_NUM 39
  #define Y5_GPIO_NUM 5
  #define Y4_GPIO_NUM 34
  #define Y3_GPIO_NUM 35
  #define Y2_GPIO_NUM 17
  #define VSYNC_GPIO_NUM 22
  #define HREF_GPIO_NUM 26
  #define PCLK_GPIO_NUM 21

#elif defined(CAMERA_MODEL_M5STACK_PSRAM_B)
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM 15
  #define XCLK_GPIO_NUM 27
  #define SIOD_GPIO_NUM 22
  #define SIOC_GPIO_NUM 23
  #define Y9_GPIO_NUM 19
  #define Y8_GPIO_NUM 36
  #define Y7_GPIO_NUM 18
  #define Y6_GPIO_NUM 39
  #define Y5_GPIO_NUM 5
  #define Y4_GPIO_NUM 34
  #define Y3_GPIO_NUM 35
  #define Y2_GPIO_NUM 32
  #define VSYNC_GPIO_NUM 25
  #define HREF_GPIO_NUM 26
  #define PCLK_GPIO_NUM 21
#else
  #error "Please select the model of the board you want to use in main.cpp"
#endif
