#include <KellyWeatherApi.h>

KellyWeatherApi::KellyWeatherApi(int timeout) {

    connectionTimeout = timeout;
}

void KellyWeatherApi::clientEnd() {

    if (clientSecure) {
    
      #if defined(ESP32)
      clientSecure->stop();
      #else
      // Abort method is required
      // other "gentle" methods give memory leaks on ESP8266 if httpClient wifiClient stuck after connect (connected but no any available until timeout)

      clientSecure->stop(0);
      #endif

      // clientSecure->abort();
      // while (clientSecure->available()) clientSecure->read();
      // clientSecure->flush();
      // clientSecure->stop();

      delete clientSecure;
      Serial.println(F("[clientEnd] WiFiClientSecure")); 

    } else if (client) {

        #if defined(ESP32)
        client->stop();
        #else
        
        // Abort method is required
        // other "gentle" methods give memory leaks on ESP8266 if httpClient wifiClient stuck after connect (connected but no any available until timeout)

        client->abort();
        #endif
        // while (client->available()) client->read();
        // client->flush();
        // client->stop();

        delete client;        
        Serial.println(F("[clientEnd] WiFiClient")); 
    }

    client = NULL;
    clientSecure = NULL;
}

void KellyWeatherApi::clientStart(bool https) {
  
  if (https) {
    
    clientSecure = new WiFiClientSecure;
    clientSecure->setInsecure(); // we cant work with certs, since they always changes and cant be ez stored & updated on ESP

    #if defined(ESP32)
      clientSecure->setTimeout(connectionTimeout / 1000);
      Serial.println(F("[OpenWeather] HTTPS mode (+20kb RAM temp USED)"));
    #else 
      clientSecure->setBufferSizes(512, 512); // wiil get OOM without this reduce - Default require ~ +16kb + 6kb, This only ~7kb https://github.com/esp8266/Arduino/issues/7326
      clientSecure->setTimeout(connectionTimeout);
      Serial.println(F("[OpenWeather] HTTPS mode (+7kb RAM temp USED)"));
    #endif

    client = clientSecure;

  } else {

    client = new WiFiClient;
    #if defined(ESP32)
      client->setTimeout(connectionTimeout / 1000);
    #else 
      client->setTimeout(connectionTimeout);
    #endif
  }
}

KellyOWIconType KellyWeatherApi::getMeteoIconState(int weatherCode) {
    return kowUnknown;
}

int KellyWeatherApi::loadCurrent(String & nurl, String & login, String & pass) {
    return loadCurrent(nurl);
}

int KellyWeatherApi::loadCurrent(String & nurl, String & token) {
    return loadCurrent(nurl);
}

void KellyWeatherApi::end() {
    clientEnd();
}
