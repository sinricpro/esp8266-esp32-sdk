#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Update.h>

// Task handle for the update task
TaskHandle_t updateTaskHandle = NULL;

// Cloudflare Root CA Certificate (Baltimore CyberTrust Root)
const char rootCACertificate[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ
RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD
VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX
DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y
ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy
VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr
mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr
IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK
mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu
XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy
dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye
jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1
BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3
DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92
9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx
jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0
Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz
ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS
R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp
-----END CERTIFICATE-----
)EOF";

struct Version {
    int major;
    int minor;
    int patch;

    String toString() const {
        return String(major) + "." + String(minor) + "." + String(patch);
    }
};

Version parseVersion(const String& versionStr) {
    Version v;
    int firstDot = versionStr.indexOf('.');
    int secondDot = versionStr.lastIndexOf('.');    
    v.major = versionStr.substring(0, firstDot).toInt();
    v.minor = versionStr.substring(firstDot + 1, secondDot).toInt();
    v.patch = versionStr.substring(secondDot + 1).toInt();    
    return v;
}

bool isNewerVersion(const Version& currentVersion, const Version& newVersion) {
    if (newVersion.major > currentVersion.major) return true;
    if (newVersion.major < currentVersion.major) return false;
    
    if (newVersion.minor > currentVersion.minor) return true;
    if (newVersion.minor < currentVersion.minor) return false;
    
    return newVersion.patch > currentVersion.patch;
}

// Function to perform the OTA update
bool performUpdate(const String& url) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client->setCACert(rootCACertificate);
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
void updateTask(void * parameter) {
  String url = *((String*)parameter);
  delete (String*)parameter;

  Serial.println("Starting OTA update");
  bool updateSuccess = performUpdate(url);

  if (updateSuccess) {
    Serial.println("Update successful. Rebooting...");
    delay(1000);  // Give some time for the message to be sent
    ESP.restart();
  } else {
    Serial.println("Update failed.");
  }

  // Task is done, delete itself
  vTaskDelete(NULL);
}

void startOTAUpdate(const String& url) {
  String* urlCopy = new String(url);

  // Create the task, passing the URL as a parameter
  xTaskCreate(
    updateTask,          // Function to implement the task
    "otaUpdateTask",        // Name of the task
    10000,               // Stack size in words
    (void*)urlCopy,      // Task input parameter
    1,                   // Priority of the task
    &updateTaskHandle    // Task handle
  );
}

