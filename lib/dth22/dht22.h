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
 *     |        	  |  1) VCC 3.3 - 5 v
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
 *    BASETTA SENSORE
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
 */

/* include le librerie standard di ESP32 */
#include <Arduino.h>
#include <SPIFFS.h>

/* include le librerie del sensore */
#include <DHT.h>

/* include le librei standard C++ */
#include <iostream>
#include <string>


#ifndef __DHT22__
#define __DHT22__

class dht22
{

private:
    DHT dht22sensor;

public:
    /* costruttori della classe */
    dht22();    /* costruttore di default - sensore collegato al pin di default 14 */
    dht22(int); /* costruttore personalizzato - permette di selezionare il PIN a cui e' collegato il sensore */

    /* metodi per la gestione del sensore */
    float readTEMP();                 /* ritorna il valore la temperatura rilevata dal sensore */
    void writeOnFile(String, String); /* stampa su file temperatura e umidita rilevate dal sensore readTEMP( FILE, MODE) */
    float readUMD();                  /* ritorna il valore del tasso di umidità rilevata dal sensore */

}; /* end define class DHT22 */

#endif