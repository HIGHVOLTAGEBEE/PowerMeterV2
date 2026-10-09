#ifndef WEBPAGES_H
#define WEBPAGES_H

#include <Arduino.h>

// Jeder Tab ist eine eigenstaendige Seite (Navigation via navHtml auf den Seiten)
String webPageLive();     // "/"
String webPageLogger();   // "/logger"
String webPageEfuse();    // "/efuse"
String webPageDimmer();   // "/dimmer"
String webPageSettings(); // "/config"

// Kalibrierungs-Wizard
String webPageCalibrate();

#endif
