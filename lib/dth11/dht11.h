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
 *  BASETTA SENSORE
 *     +-----------+
 *     |           |  1) DATA
 *     |Vista Alto |  2) VCC
 *     |	       |  3) GND
 *     |    DHT11  |  
 *     |	       |
 *     |  1  2  3  |
 *     +--+--+--+--+
 *        |  |  |  
 *        |  |  |  
 *        D  V  G   
 *        A  C  N   
 *        T  C  D  
 *        A
 *
 *
 *
 *
 *
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

#ifndef __DHT11__
#define __DHT11__

class dht11
{

private:

DHT dht11sensor;

public:
    /* costruttori della classe */
    dht11();    /* costruttore di default - sensore collegato al pin di default 2 */
    dht11(int); /* costruttore personalizzato - permette di selezionare il PIN a cui e' collegato il sensore */

    /* metodi per la gestione del sensore */
    float readTEMP();                /* ritorna il valore la temperatura rilevata dal sensore */
    void readTEMP(String, String); /* stampa su file la temperatura rilevata dal sensore readTEMP( FILE, MODE) */
    float readUMD();                 /* ritorna il valore del tasso di umidità rilevata dal sensore */
    void readUMD(String, String);  /* stampa su file il valore del tasso di umidita' rilevato dal sensore readUMD(FILE, MODE) */
    

}; /* end define class DHT11 */

#endif