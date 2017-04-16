#ifndef UTILITY_H
#define UTILITY_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <TimeLib.h>            // Time library
/*******************************************************************************
** MISC CONSTANTS **************************************************************
*******************************************************************************/
#define SDA 20                  // sda 2 - uno, 20 - mega
#define SDL 21                  // sdl 3 - uno, 21 - mega
#define CS 53                   // chipselect 10 - uno, 53 - mega
#define LED 13                  // led pin
#define SI 4                    // selectinput
/*******************************************************************************
** VARIABLES *******************************************************************
*******************************************************************************/
tmElements_t rtc;
char* msg;
typedef struct {
  long duration;
  int frequency;
} Song;
bool timerStatus = false;   
uint8_t startTime, endTime;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
typedef struct {
    uint8_t month, week, day, hour, minute, second;
} timerResults;
/*******************************************************************************
** UTILITY METHODS *************************************************************
*******************************************************************************/
class Utility {
    public:
        /**
         * Meters to feet conversiuon
         * @param int meters
         * @return int
         */
        int _m2ft(int meters)
        {
            return (meters * 3.28084);
        }

        /**
         * Kilometers to feet conversion
         * @param  int km
         * @return int
         */
        int _km2ft(int km) {
            return (_m2ft(km/1000));
        }

        void startTimer() {
            timerStatus = true;
            startTime = millis();
        }

        void stopTimer() {
            if (timeStatus) {
                timerStatus = false;
                endTime = millis();
            }
        }

        char* calculateTimerResults(uint8_t timespan, bool verbose = false) {
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

        uint8_t getTimerResults() {
            stopTimer();
            return (endTime - startTime);
        }

        /**
         * Set time from a string
         * @param  const char *str
         * @return bool
         */
        bool setTimeFromString(const char *str) {
          int Hour, Min, Sec;

          if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
          rtc.Hour = Hour;
          rtc.Minute = Min;
          rtc.Second = Sec;
          return true;
        }

        /**
         * Set date from a string
         * @Param const char *str
         * @return bool
         */
        bool setDateFromString(const char *str)
        {
          char Month[12];
          int Day, Year;
          uint8_t monthIndex;

          if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
          for (monthIndex = 0; monthIndex < 12; monthIndex++) {
            if (strcmp(Month, monthName[monthIndex]) == 0) break;
          }
          if (monthIndex >= 12) return false;
          rtc.Day = Day;
          rtc.Month = monthIndex + 1;
          rtc.Year = CalendarYrToTm(Year);
          return true;
        }

        /**
         * Get datetime string
         * @param  null|tmElements_t tm
         * @param  char* fmt
         * @return char*
         */
        char* getDateTimeString(tmElements_t tm = rtc, char* fmt = NULL) {   
            sprintf(msg, "%d/%d/%d %d:%d:%d", tm.Month, tm.Day, tmYearToCalendar(tm.Year), tm.Hour, tm.Minute, tm.Second);
            return msg;
        }

        /**
         * Print
         * @param  char *str
         * @param  bool xbee
         * @param  bool serial
         */
        void print(char *str, bool log=true, bool xbee=true, bool serial=true) {
            if (serial) {
                #ifdef SERIAL
                Serial.print(str);
                #endif
            }
            if (xbee) {
                #ifdef XBEE
                XBee.print(str);
                #endif
            }
            if (log) {
                #ifdef LOG
                logfile.print(str);
                #endif
            }
        }

        /**
         * Print line
         * @param  char *str
         * @param  bool xbee
         * @param  bool serial
         */
        void println(char *str=NULL, bool log=true, bool xbee=true, bool serial=true) {
            print(str, log, xbee, serial);
            print("\n", log, xbee, serial);
        }

        /**
         * Blink LED
         * @param  uint8_t pin
         * @param  int     times
         * @param  int     delay
         */
        void blinkLED(uint8_t pin, int times=10, int wait=500) {
            for (int i=0; i < times; i++) {
                digitalWrite(pin, HIGH);
                delay(wait);
                digitalWrite(pin, LOW);
                if (times > (i + 1))    delay(wait);
            }
        }

        /**
         * Error
         * @param  char *str
         * @param  bool halt
         */
        void error(char* str, bool halt=true) {
            blinkLED(LED);
            sprintf(str, "[ERROR] %s", str);
            println(str);
            if (halt)   while(1);
        }

        /**
         * Warning
         * @param  char *str
         */
        void warning(char *str) {
            blinkLED(LED, 5);
            sprintf(str, "[WARNING] %s", str);
            println(str);
        }

        /**
         * Dot dot dot
         * @param  uint8_t dot_total
         * @param  char dot
         * @param  uint8_t ms_per_dot
         */
        void dotdotdot(uint8_t dot_total=10, char* dot=".", uint8_t ms_per_dot=100, bool newline=false) {
            for (int x = 0; x < dot_total; x++) {
                print(dot, false);
                delay(ms_per_dot);
            }
        }
};
#endif