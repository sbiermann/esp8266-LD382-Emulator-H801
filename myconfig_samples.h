/*************************************************/
/* Debugging                                     */
/*************************************************/
const bool debugOutput = true;  // set to true for serial OUTPUT

/*************************************************/
/* Settings for WLAN                             */
/*************************************************/
const char* ssid = "SSID";
const char* password = "ergerg123erg";
IPAddress ip(0,0,0,0);
IPAddress gateway(0,0,0,0);
IPAddress subnet(255,255,255,0);

/*************************************************/
/* Update settings                               */
/*************************************************/ 
const char* firmware_version = "myh801.0.1";
const char* update_server = "myhost";
const char* update_uri = "/path/update.php";
