#if defined(ESP32)

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

#define FOR_i(from, to) for(int i = (from); i < (to); i++)

// Task handle for the ota update task
TaskHandle_t otaUpdateTaskHandle = NULL;

// Ref: https://projects.petrucci.ch/esp32/?page=ssl.php&url=otaupdates.sinric.pro

const char rootCACertificate[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEVzCCAj+gAwIBAgIRAIOPbGPOsTmMYgZigxXJ/d4wDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw
WhcNMjcwMzEyMjM1OTU5WjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg
RW5jcnlwdDELMAkGA1UEAxMCRTUwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQNCzqK
a2GOtu/cX1jnxkJFVKtj9mZhSAouWXW0gQI3ULc/FnncmOyhKJdyIBwsz9V8UiBO
VHhbhBRrwJCuhezAUUE8Wod/Bk3U/mDR+mwt4X2VEIiiCFQPmRpM5uoKrNijgfgw
gfUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcD
ATASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1UdDgQWBBSfK1/PPCFPnQS37SssxMZw
i9LXDTAfBgNVHSMEGDAWgBR5tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcB
AQQmMCQwIgYIKwYBBQUHMAKGFmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0g
BAwwCjAIBgZngQwBAgEwJwYDVR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVu
Y3Iub3JnLzANBgkqhkiG9w0BAQsFAAOCAgEAH3KdNEVCQdqk0LKyuNImTKdRJY1C
2uw2SJajuhqkyGPY8C+zzsufZ+mgnhnq1A2KVQOSykOEnUbx1cy637rBAihx97r+
bcwbZM6sTDIaEriR/PLk6LKs9Be0uoVxgOKDcpG9svD33J+G9Lcfv1K9luDmSTgG
6XNFIN5vfI5gs/lMPyojEMdIzK9blcl2/1vKxO8WGCcjvsQ1nJ/Pwt8LQZBfOFyV
XP8ubAp/au3dc4EKWG9MO5zcx1qT9+NXRGdVWxGvmBFRAajciMfXME1ZuGmk3/GO
koAM7ZkjZmleyokP1LGzmfJcUd9s7eeu1/9/eg5XlXd/55GtYjAM+C4DG5i7eaNq
cm2F+yxYIPt6cbbtYVNJCGfHWqHEQ4FYStUyFnv8sjyqU8ypgZaNJ9aVcWSICLOI
E1/Qv/7oKsnZCWJ926wU6RqG1OYPGOi1zuABhLw61cuPVDT28nQS/e6z95cJXq0e
K1BcaJ6fJZsmbjRgD5p3mvEf5vdQM7MCEvU0tHbsx2I5mHHJoABHb8KVBgWp/lcX
GWiWaeOyB7RP+OfDtvi2OsapxXiV7vNVs7fMlrRjY1joKaqmmycnBvAq14AEbtyL
sVfOS66B8apkeFX2NY4XPEYV4ZSCe8VHPrdrERk2wILG3T/EGmSIkCYVUMSnjmJd
VQD9F6Na/+zmXCc=
-----END CERTIFICATE-----
)EOF";


// Function to perform the OTA update
bool performUpdate(const String& url) {
  WiFiClientSecure *client = new WiFiClientSecure;
  
  if(client) {
    client->setCACert(rootCACertificate);
    //client->setInsecure();
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, url)) {
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // file found at server
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            // get length of document (is -1 when Server sends no Content-Length header)
            int contentLength = https.getSize();
            Serial.printf("Content-Length: %d\n", contentLength);
  
            if (contentLength > 0) {
              bool canBegin = Update.begin(contentLength);
              if (canBegin) {
                WiFiClient * stream = https.getStreamPtr();
                size_t written = Update.writeStream(*stream);
  
                if (written == contentLength) {
                  Serial.println("Written : " + String(written) + " successfully");
                } else {
                  Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
                }
  
                if (Update.end()) {
                  Serial.println("OTA done!");
                  if (Update.isFinished()) {
                    Serial.println("Update successfully completed. Rebooting.");
                    return true;
                  } else {
                    Serial.println("Update not finished? Something went wrong!");
                  }
                } else {
                  Serial.println("Error Occurred. Error #: " + String(Update.getError()));
                }
              } else {
                Serial.println("Not enough space to begin OTA");
              }
            } else {
              Serial.println("There was no content length in the response");
            }
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  
  return false;
}

// Task function for OTA update
void otaUpdateTask(void * parameter) {
  String url = *((String*)parameter);
  delete (String*)parameter;

  Serial.println("Starting OTA update");
 
  FOR_i(1, 5) { 
     bool updateSuccess = performUpdate(url);

    if (updateSuccess) {
      Serial.println("Update successful. Rebooting...");
      ESP.restart();
    } else {
      Serial.println("Update failed. Retrying.."); // To fix random DNS lookup errors
      delay(1000);
    }
  } 

  // Task is done, delete itself
  vTaskDelete(NULL);
}

bool startOTAUpdate(const String& url) {
  String* urlCopy = new String(url);

  // Create the task, passing the URL as a parameter
  xTaskCreate(
    otaUpdateTask,          // Function to implement the task
    "otaUpdateTask",     // Name of the task
    10000,               // Stack size in words
    (void*)urlCopy,      // Task input parameter
    1,                   // Priority of the task
    &otaUpdateTaskHandle    // Task handle
  );

  return true;
}

#endif