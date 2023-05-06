/*=========================================================
    Tablo ws2812
    = vlapa = v.509
    2021.02.01 - 2023.03.07
=========================================================*/
#pragma once

#define PIXEL_PIN D2
#define PIXEL_COUNT 210
#define RAZR_PIXEL 42
#define BRIGHT_DAY 5
#define BRIGHT_NIGHT 1

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const uint8_t digit = 5; //  кол-во разрядов табло

//********************************************************************
//  плата
void visibleWork(String visData, uint32_t color, uint8_t bright)
{ //  данные, цвет
    strip.setBrightness(bright);

    for (uint8_t razr = 0; razr < digit; ++razr)
    {
        uint8_t x;
        if (visData.charAt(razr) == 'A')
        {
            x = 10;
        }
        else if (visData.charAt(razr) == 'B')
        {
            x = 11;
        }
        else if (visData.charAt(razr) == 'C')
        {
            x = 12;
        }
        else if (visData.charAt(razr) == 'D')
        {
            x = 13;
        }
        else if (visData.charAt(razr) == 'E')
        {
            x = 14;
        }
        else
        {
            x = visData.substring(razr, razr + 1).toInt();
        }

        switch (x)
        {
        case 0:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if (i >= 0 && i < 36)
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 1:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if (i > 11 && i < 24)
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 2:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i > 5 && i < 18) || (i > 23 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 3:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i > 5 && i < 30) || (i > 36 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 4:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i >= 0 && i < 6) || (i > 11 && i < 24) || (i > 35 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 5:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i >= 0 && i < 12) || (i > 17 && i < 30) || (i > 35 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 6:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i >= 0 && i < 12) || (i > 17 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 7:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if (i > 5 && i < 24)
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 8:
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                strip.setPixelColor(i + razr * RAZR_PIXEL, color);
            }
            break;
        case 9:
            for (int i = 0; i < RAZR_PIXEL; i++)
            {
                if ((i >= 0 && i < 30) || (i > 35 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 10: //  градус ( A )
            for (int i = 0; i < RAZR_PIXEL; i++)
            {
                if ((i >= 0 && i < 18) || (i > 35 && i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 11: //  минус ( B )
            for (int i = 0; i < RAZR_PIXEL; i++)
            {
                if ((i > 35) && (i < 42))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 12: //  двоеточие ( C )
            for (int i = 0; i < RAZR_PIXEL; i++)
            {
                if ((i == 36) || (i == 37) || (i == 40) || (i == 41))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        case 13: //  null (пусто) ( D )
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
            }
            break;
        case 14: //  точка ( E )
            for (int i = 0; i < RAZR_PIXEL; ++i)
            {
                if ((i == 26) || (i == 27))
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, color);
                }
                else
                {
                    strip.setPixelColor(i + razr * RAZR_PIXEL, strip.Color(0, 0, 0));
                }
            }
            break;
        }
    }
    strip.show();
}