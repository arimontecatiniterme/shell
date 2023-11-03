
/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Dichiara i metodi per l'uso del sensore DHT11
 *
 *
 *     +-----------+
 *     |           |  1) VCC 3.3 - 5v
 *     |Vista Alto |  2) OUT ( segnale digitale )
 *     |	       |  3) GND
 *     |   DHT22   |
 *     |	       |
 *     |  1  2  3  |
 *     +--+--+--+--+
 *        |  |  |
 *        |  |  |
 *        V  O  G
 *        C  U  N
 *        C  T  D
 *
 *
 *
 */

/* include la libreria della funzioni di utility del sensore */
#include "dht22.h"

/* costrutture di default */
dht22::dht22() : dht22sensor(26, DHT22)
{
    Serial.println("init sensor to pin 26 ");
    dht22sensor.begin(); /* inizializza il sensore */
} /* end costruttore di default */

/* costruttore personalizzato */
dht22::dht22(int iGPIO) : dht22sensor(iGPIO, DHT22)
{
    Serial.print("init sensor to pin ");
    Serial.println(iGPIO);
    dht22sensor.begin(); /* inizializza il sensore */
} /* end costruttore personalizzato */

/* legge la temperatura misurata dal sensore */
float dht22::readTEMP()
{
    return dht22sensor.readTemperature();
} /* end readTEMP() */

/* legge il grado di umidita' misurata dal sensore */
float dht22::readUMD()
{
    return dht22sensor.readHumidity();
} /* end readTEMP() */

/* scrive su file la temperatura misurata dal sensore */
void dht22::writeOnFile(String sFILE, String sMODE)
{

    float fTemp = dht22sensor.readTemperature();
    float fUmid = dht22sensor.readHumidity();

    File pFILE = SPIFFS.open(sFILE, sMODE.c_str());
    pFILE.print("temp=");
    pFILE.println(fTemp);
    pFILE.print("umid=");
    pFILE.println(fUmid);
    pFILE.close();

} /* end writeOnFile(String sFILE, String sMODE) */
