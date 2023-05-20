//********************************************************************
//*
//*   Tablo na WS2812b + mqtt + bme280 (2)
//*   2021.02.01 - 2023.04.23
//*   v.512
//*
//*   A - градус
//*   В - минус
//*   С - двоеточие
//*   D - пусто
//*   E - точка
//*   F -
//********************************************************************
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include "tablo_ws2812.h"

#define TIME_DAY 8
#define TIME_NIGHT 19

// const char ssid[] = "MikroTik-2-ext";
char ssid[] = "link";
char pass[] = "dkfgf#*12091997";
// char ssid[] = "Huawei-M2";
// char pass[] = "998877**";

const uint32_t utcOffsetInSeconds = 10800;
const uint32_t utcPeriodMseconds = 86400000; // 3сутки-259200000 // 1сутки-86400000;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp3.vniiftri.ru", utcOffsetInSeconds, utcPeriodMseconds);
// NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, utcPeriodMseconds);

const char *mqtt_server = "178.20.46.157";
const uint16_t mqtt_port = 1883;
const char *mqtt_client = "Tablo_Villa_002";
const char *mqtt_client2 = "Villa_bme280_base";
// const char *mqtt_client2 = "Home_bme280";
const char *mqtt_client3 = "Villa_bme280_yama";
const char *mqtt_user = "mqtt";
const char *mqtt_pass = "qwe#*1243";

const char *inTopicTemp = "/Temp";
const char *inTopicPres = "/Pres";
const char *inTopicHum = "/Hum";
// const char *inTopicTemp2 = "/TempOut";
const char *inTopicTempOut = "/Temp";
const char *inTopicHumOut = "/Hum";

String outTempData = "00.00A";
String outTempData2 = "00.00A";
String outTempDataOut = "00.00A";
String outPresData = "000";
String outHumData = "00";
String outHumDataOut = "00";

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t timeUpdateH = 14; //  время обновления NTP
uint32_t timeUpdateM = 05; //  время обновления NTP
String z = "";             //  строка данных для отображения

uint32_t colorTime = 16766720;   //  цвет часы (желтый)
uint32_t colorTemp = 16711935;   //  цвет температура (красный)
uint32_t colorTemp2 = 65280;     //  цвет внешняя температура (зеленый)
uint32_t colorTempOut = 9127187; //  цвет температура2 (коричневый)
uint32_t colorPress = 9830400;   //  цвет давление (розовый)
uint32_t colorWiFi = 16728935;   //  цвет WiFi (томато)
uint32_t colorHum = 52945;       //  цвет влажность (бирюзовый)
uint32_t colorHumOut = 255;      //  цвет влажность2 (синий)
uint16_t strOld = 0;
uint8_t count = 0;

uint32_t timeReadOld = 0;
uint32_t color = 0;

bool flagSec = true;
bool flagTemp = false;
bool flagTemp2 = false;
bool flagTempOut = false;
bool flagPress = false;
bool flagHum = false;
bool flagHumOut = false;
bool flagVis = true;

// Время отображения каждого данного:
// 0-время, 1-темп, 2-давл, 3-влажн, 4-темп2, 5-влажн2
uint16_t visData[] = {5, 2, 2, 2, 2, 2, 2};
uint32_t timeOld = 0;
uint8_t brightVis = BRIGHT_DAY;

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

//-----------------------------------
inline bool mqtt_subscribe(PubSubClient &client, const String &topic)
{
    Serial.print("Subscribing to: ");
    Serial.println(topic);
    return client.subscribe(topic.c_str());
}

//-----------------------------------
inline bool mqtt_publish(PubSubClient &client, const String &topic, const String &value)
{
    Serial.print("Publishing topic ");
    Serial.print(topic);
    Serial.print(" = ");
    Serial.println(value);
    return client.publish(topic.c_str(), value.c_str());
}

//-----------------------------------
void reconnect()
{
    // visibleWork("DDDDD", colorWiFi, BRIGHT_DAY);
    // strip.show();
    uint8_t countMqtt = 100;
    while (!client.connect(mqtt_client, mqtt_user, mqtt_pass))
    {
        visibleWork("0B0B0", colorWiFi, BRIGHT_DAY);
        strip.show();
        if (countMqtt)
        {
            countMqtt--;
            delay(500);
        }
        else
        {
            ESP.restart();
        }
    }

    String topic('/');
    topic += mqtt_client2;
    topic += inTopicTemp;
    mqtt_subscribe(client, topic);

    // topic = '/';
    // topic += mqtt_client2;
    // topic += inTopicTemp2;
    // mqtt_subscribe(client, topic);

    topic = '/';
    topic += mqtt_client2;
    topic += inTopicPres;
    mqtt_subscribe(client, topic);

    topic = '/';
    topic += mqtt_client2;
    topic += inTopicHum;
    mqtt_subscribe(client, topic);

    topic = '/';
    topic += mqtt_client3;
    topic += inTopicTempOut;
    mqtt_subscribe(client, topic);

    topic = '/';
    topic += mqtt_client3;
    topic += inTopicHumOut;
    mqtt_subscribe(client, topic);

    static uint32_t cownt = 0;
    Serial.print(cownt++);
    Serial.println("\tMQTT - Ok!\n");
}

//********************************************************************
//  подключение к WiFi
void setupWiFi()
{
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("\n\nSetup WiFi: ");
    visibleWork("ABABA", colorWiFi, BRIGHT_DAY);
    strip.show();

    WiFi.begin(ssid, pass);
    uint8_t countWiFi = 10;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
        if (countWiFi)
        {
            countWiFi--;
        }
        else
        {
            ESP.restart();
        }
    }
    digitalWrite(LED_BUILTIN, HIGH);

    //-----------------------------------
    // индикация IP
    String l = WiFi.localIP().toString();
    l = l.substring(l.lastIndexOf('.') + 1, l.length());

    if (l.toInt() < 100)
    {
        l = "DDE" + l;
        visibleWork(l, colorWiFi, BRIGHT_DAY);
    }
    else
    {
        l = "DE" + l;
        visibleWork(l, colorWiFi, BRIGHT_DAY);
    }

    strip.show();
    Serial.print("\nWiFi connected !\n");
    Serial.println(WiFi.localIP());
    delay(2000);

    //-----------------------------------
    // индикация силы сигнала
    int16_t RSSI_MAX = -50;
    int16_t RSSI_MIN = -100;
    int16_t dBm = WiFi.RSSI();
    Serial.print("RSSI dBm = ");
    Serial.println(dBm);
    int16_t quality;
    if (dBm <= RSSI_MIN)
    {
        quality = 0;
    }
    else if (dBm >= RSSI_MAX)
    {
        quality = 99;
    }
    else
    {
        quality = 2 * (dBm + 100);
    }
    Serial.print("RSSI % = ");
    Serial.println(quality);
    Serial.println("\n");
    l = "DDE";
    l += quality;
    visibleWork(l, colorWiFi, BRIGHT_DAY);
    strip.show();
    delay(2000);

    Serial.println();

    while (!timeClient.update())
    {
        z = "BBBBB";
        visibleWork(z, colorTemp, BRIGHT_DAY);
        strip.show();
        delay(500);
        Serial.print('.');
    }
    flagVis = true;
    count = 0;
}

//********************************************************************
//  MQTT
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print(topic);
    Serial.print("\t");
    for (uint8_t i = 0; i < length; ++i)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    //------------------------

    char *topicBody = topic + strlen(mqtt_client3) + 1;
    // Serial.print("*topicBody  ");
    // Serial.println(topicBody);

    if (!strncmp(topic + 1, mqtt_client3, strlen(mqtt_client3)))
    {
        if (!strncmp(topicBody, inTopicTempOut, strlen(inTopicTempOut)))
        {
            outTempDataOut = "";
            for (uint8_t i = 0; i < length; i++)
                outTempDataOut += (char)payload[i];
            Serial.print("TempOut: ");
            Serial.println(outTempDataOut);
            Serial.println();
        }
        else if (!strncmp(topicBody, inTopicHumOut, strlen(inTopicHumOut)))
        {
            outHumDataOut = "";
            for (uint8_t i = 0; i < length; i++)
                outHumDataOut += (char)payload[i];
            Serial.print("HumOut: ");
            Serial.println(outHumDataOut);
            Serial.println();
        }
    }

    //------------------------

    // char *topicBody = topic + strlen(mqtt_client2) + 1;
    // *topicBody += *mqtt_client2;
    // if (!strncmp(topicBody, inTopicTemp2, strlen(inTopicTemp2)))
    // {
    //     outTempData2 = "";
    //     for (uint8_t i = 0; i < length; i++)
    //         outTempData2 += (char)payload[i];
    //     Serial.print("Temp2: ");
    //     Serial.println(outTempData2);
    // }
    // else
    if (!strncmp(topic + 1, mqtt_client2, strlen(mqtt_client2)))
    {

        if (!strncmp(topicBody, inTopicTemp, strlen(inTopicTemp)))
        {
            outTempData = "";
            for (uint8_t i = 0; i < length; i++)
                outTempData += (char)payload[i];
            Serial.print("Temp: ");
            Serial.println(outTempData);
        }
        else if (!strncmp(topicBody, inTopicPres, strlen(inTopicPres)))
        {
            outPresData = "";
            for (uint8_t i = 0; i < length; i++)
                outPresData += (char)payload[i];
            Serial.print("Pres: ");
            Serial.println(outPresData);
        }
        else if (!strncmp(topicBody, inTopicHum, strlen(inTopicHum)))
        {
            outHumData = "";
            for (uint8_t i = 0; i < length; i++)
                outHumData += (char)payload[i];
            Serial.print("Hum : ");
            Serial.println(outHumData);
            Serial.println();
        }
    }
}

//********************************************************************
//  пересчет millis()
void TimeMillis()
{
    color = colorTime;

    z = "";
    if (timeClient.getHours() < 10)
    {
        z += "D";
    }
    z += timeClient.getHours(); //  часы
    if (flagSec)
    {
        z += "C";
    }
    else
    {
        z += "D";
    }
    if (timeClient.getMinutes() < 10)
    {
        z += "0";
    }
    z += timeClient.getMinutes(); //  минуты

    if ((timeClient.getHours() >= TIME_DAY) && (timeClient.getHours() < TIME_NIGHT))
    {
        brightVis = BRIGHT_DAY;
    }
    else
    {
        brightVis = BRIGHT_NIGHT;
    }
}

//********************************************************************
//  BME280
void bmeTemp()
{
    z = "";
    String t = "";
    t += outTempData;

    if (t.charAt(0) == '-')
    {
        if (t.indexOf('.') == 2)
        {
            z = "B" + t.substring(1, t.indexOf('.'));
            z += 'E' + t.substring(3, 4);
        }
        else
        {
            z = "DB" + t.substring(1, t.indexOf('.'));
        }
    }
    else
    {
        if (t.indexOf('.') == 2)
        {
            z = "DD" + t.substring(0, t.indexOf('.'));
        }
        else
        {
            z = 'D' + t.substring(0, t.indexOf('.'));
            z += 'E' + t.substring(2, 3);
        }
    }
    t.substring(0, 1);

    z += 'A';
    color = colorTemp;
    flagTemp = true;
}

void bmeTemp2()
{
    z = "";
    String t = "";
    t += outTempData2;

    if (t.charAt(0) == '-')
    {
        if (t.indexOf('.') == 2)
        {
            z = "B" + t.substring(1, t.indexOf('.'));
            z += 'E' + t.substring(3, 4);
        }
        else
        {
            z = "DB" + t.substring(1, t.indexOf('.'));
        }
    }
    else
    {
        if (t.indexOf('.') == 2)
        {
            z = "DD" + t.substring(0, t.indexOf('.'));
        }
        else
        {
            z = 'D' + t.substring(0, t.indexOf('.'));
            z += 'E' + t.substring(2, 3);
        }
    }
    t.substring(0, 1);

    z += 'A';
    color = colorTemp2;
    flagTemp2 = true;
}

void bmeTempOut()
{
    z = "";
    String t = "";
    t += outTempDataOut;

    if (t.charAt(0) == '-')
    {
        if (t.indexOf('.') == 2)
        {
            z = "B" + t.substring(1, t.indexOf('.'));
            z += 'E' + t.substring(3, 4);
        }
        else
        {
            z = "DB" + t.substring(1, t.indexOf('.'));
        }
    }
    else
    {
        if (t.indexOf('.') == 2)
        {
            z = "DD" + t.substring(0, t.indexOf('.'));
        }
        else
        {
            z = 'D' + t.substring(0, t.indexOf('.'));
            z += 'E' + t.substring(2, 3);
        }
    }
    t.substring(0, 1);

    z += 'A';
    color = colorTempOut;
    flagTempOut = true;
}

void bmePress()
{
    z = "";
    String t = "";
    t += outPresData;
    z += "DD" + t.substring(0, 3);
    color = colorPress;
    flagPress = true;
}

void bmeHum()
{
    z = "";
    String t = "";
    t += outHumData;
    z += "DDD" + t.substring(0, 2);
    color = colorHum;
    flagHum = true;
}

void bmeHumOut()
{
    z = "";
    String t = "";
    t += outHumDataOut;
    z += "DDD" + t.substring(0, 2);
    color = colorHumOut;
    flagHumOut = true;
}

//********************************************************************
//  запись значений датчиков в память
// void dataWrite(uint8_t n, String str)
// {
//     data[n].num = str.substring(0, 1).toInt();
//     if (str.indexOf('%') > 0)
//         data[n].hum = str.substring(str.indexOf('%') + 1, str.indexOf('%') + 3).toInt();
//     if (str.indexOf('*') > 0)
//         data[n].temp = str.substring(str.indexOf('*') + 1, str.indexOf('*' + 5)).toFloat();
//     if (str.indexOf('$') > 0)
//         data[n].press = str.substring(str.indexOf('$') + 1, str.indexOf('$') + 4).toInt();
//     if (str.indexOf('^') > 0)
//         data[n].voltage = str.substring(str.indexOf('^') + 1, str.indexOf('^') + 4).toInt();
// }
//********************************************************************
// void parserBuf(String tmp)
// {
//     if (tmp.substring(0, 2).toInt() == sensorNum_1)
//     {
//         dataWrite(0, tmp);
//     }
//     // else if (tmp.substring(0, 2).toInt() == sensorNum_2)
//     // {
//     //     dataWrite(1, tmp);
//     // }
// }
//********************************************************************
//  визуальные эффекты
// void show_1()
// {
//     strip.clear();
//     for (uint8_t i = 0; i < PIXEL_COUNT / 2; ++i)
//     {
//         strip.setPixelColor(i, strip.Color(random(1, 255), random(1, 255), random(1, 255)));
//         strip.setPixelColor(PIXEL_COUNT - i - 1, strip.Color(random(1, 255), random(1, 255), random(1, 255)));
//         strip.show();
//         delay(5);
//     }
//     delay(500);
//     for (uint8_t i = 0; i < PIXEL_COUNT / 2; ++i)
//     {
//         strip.setPixelColor(i, 0, 0, 0);
//         strip.setPixelColor(PIXEL_COUNT - i - 1, 0, 0, 0);
//         strip.show();
//         delay(5);
//     }
//     delay(500);
//     z = "DDDDD";
// }

//********************************************************************
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);

    strip.begin();
    z = "DDDDD";
    strip.show();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(mqtt_callback);

    // show_1();
    setupWiFi();

    timeClient.begin();
    server.begin();
    httpUpdater.setup(&server);

    timeOld = millis();
}

//********************************************************************
void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        setupWiFi();
    }

    if (!client.connected())
    {
        reconnect();
    }

    if (flagVis)
    {
        switch (count)
        {
        case 0:
        {
            TimeMillis();
            flagTemp = false;
            flagPress = false;
            flagHum = false;
            flagTempOut = false;
            flagHumOut = false;
            break;
        }
        case 1:
        {
            if (flagTemp == false)
            {
                // strip.clear();
                bmeTemp();
            }
            break;
        }
        case 2:
        {
            if (flagPress == false)
            {
                // strip.clear();
                bmePress();
            }
            break;
        }
        case 3:
        {
            if (flagHum == false)
            {
                // strip.clear();
                bmeHum();
            }
            break;
        }
        case 4:
        {
            if (flagTempOut == false)
            {
                // strip.clear();
                bmeTempOut();
            }
            timeReadOld = millis();
            break;
        }
        case 5:
        {
            if (flagHumOut == false)
            {
                // strip.clear();
                bmeHumOut();
            }
            break;
        }
        }
        flagVis = false;

        visibleWork(z, color, brightVis);
    }

    //  Отображение секундного тире
    if (millis() - timeReadOld >= 500 && (!count)) // || count == 1))
    {
        flagSec = !flagSec;
        timeReadOld = millis();

        flagVis = true;
    }

    // Счетчик отображаемых данных
    if (millis() - timeOld >= (visData[count] * 1000))
    {
        (count >= 5) ? count = 0 : count++;
        flagVis = true;
        timeOld = millis();
    }

    server.handleClient();
    client.loop();
    delay(1);
}

//********************************************************************