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
 *     +--------------+
 *     |        	  |  1) VCC 5v
 *     |  Vista Alto  |	 2) OUT ( segnale digitale )
 *     |	       	  |	 3) NON COLLEGATO
 *     |    DHT11	  |	 4) GND
 *     |	       	  |
 *     |  1  2  3  4  |
 *     +--+--+--+--+--+
 *        |  |  |  |
 *        |  |  |  |
 *        V  O     G
 *        C  U     N
 *        C  T     D
 *
 *
 *
 */

/* include la libreria della funzioni di utility del sensore */
#include "dht11.h"

/* costrutture di default */
dht11::dht11() : dht11sensor(2, DHT11)
{
Serial.println("default");
} /* end costruttore di default */

/* costruttore personalizzato */
dht11::dht11(int iGPIO) : dht11sensor(iGPIO, DHT11)
{
Serial.println("pers");

} /* end costruttore personalizzato */

/* ritorna la temperatura letta dal sensore */
float dht11::readTEMP()
{
    return dht11sensor.readTemperature();
}/* end readTEMP() */

/* ritorna la temperatura letta dal sensore */
float dht11::readUMD()
{
    return dht11sensor.readHumidity();
} /* end readTEMP() */
