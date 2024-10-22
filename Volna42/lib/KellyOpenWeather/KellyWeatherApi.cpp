#include <KellyWeatherApi.h>

KellyWeatherApi::KellyWeatherApi(int timeout) {

    connectionTimeout = timeout;
}

KellyOWIconType KellyWeatherApi::getMeteoIconState(int weatherCode) {
    return kowUnknown;
}

void KellyWeatherApi::end() {

}
