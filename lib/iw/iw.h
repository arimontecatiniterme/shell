/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Definisce i metodi per la classe iw per la gestione di una rete wifi
 *
 */

/*
 * include le librerie standard per ESP32
 */
#include <Arduino.h> /* si utilizza per gli oggetti String */
#include <FS.h>      // Libreria per le funzionalita del file system
#include <SPIFFS.h>  /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */
#include <WiFi.h>    /* libreria delle funzioni wifi */
#include <iostream>  /* libreria standard per la gestione dell'input/output*/

#ifndef __WIFI__
#define __WIFI__

using namespace std;

class iw
{
private:
public:
    iw();                             /* costruttore di default */
    void mode(String);                /* imposta il modo di funzionamento del modulo wifi */
    void ipg(String, String, String); /* imposta IP - SUBNET - GW */
    void scan();                      /* esegue la scansione con stampa a video */
    void status();                    /* stampa a video lo stato della scheda */
    int connect(String, String);  /* attiva una connessione wifi iw::connect(sSID, sPASSWD) */

}; /* end class iw */

#endif