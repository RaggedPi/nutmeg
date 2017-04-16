#include <Utiltiy.h>
#include <TimeLib.h>            // Time library

void Utility::startTimer(void) {
    timerStatus = true;
    startTime = millis();
}

void Utility::stopTimer(void) {
    if (timeStatus) {
        timerStatus = false;
        endTime = millis();
    }
}

char* Utility::calculateTimerResults(uint8_t timespan, bool verbose) {
    timerResults timerResults;
    int x = 0;
    while (timespan % 2628002880) {
        x++;
        timespan -= 2628002880;
    } timerResults.month = x;
    x = 0;
    while (timespan % 604800000) {
        x++;
        timespan -= 604800000;
    } timerResults.week = x;
    x = 0;
    while (timespan % 86400000) {
        x++;
        timespan -= 86400000;
    } timerResults.day = x;
    x = 0;
    while (timespan % 3600000) {
        x++;
        timespan -= 3600000;
    } timerResults.hour = x;
    x = 0;
    while (timespan % 60000) {
        x++;
        timespan -= 60000;
    } timerResults.minute = x;
    x = 0;
    while (timespan % 1000) {
        x++;
        timespan -= 1000;
    } timerResults.second = x;
    x = 0;
    if (verbose || (0 < timerResults.month)) {
        sprintf(msg, "%d months", timerResults.month);
    }
    if (verbose || (0 < timerResults.week)) {
        sprintf(msg, "%d weeks", timerResults.week);
    }
    if (verbose || (0 < timerResults.day)) {
        sprintf(msg, "%d days", timerResults.day);
    }
    if (verbose || (0 < timerResults.week)) {
        sprintf(msg, "%d hours", timerResults.hour);
    }
    if (verbose || (0 < timerResults.minute)) {
        sprintf(msg, "%d minutes", timerResults.minute);
    }
    if (verbose || (0 < timerResults.second)) {
        sprintf(msg, "%d seconds", timerResults.second);
    }
    return msg;
}
uint8_t Utility::getTimerResults(void) {
    stopTimer();
    return (endTime - startTime);
}