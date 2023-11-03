
/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Gestione oggetto firebase
 *
 *
 */

#include <Arduino.h>
#include <SPIFFS.h>

#ifndef __FBASE__
#define __FBASE__

class fbase
{
private:
    bool signupOK = false;
    int sendDataPrevMillis = 0;
    File pFILE;

public:
    fbase();               /* costruttore di default */
    fbase(String, String); /* costruttore personalizzato */

    /*
     * metodi per l'inserimento dei dati
     *
     * set<tipo>(sPATH, valore)
     *
     */
    void set(String, std::string, std::string, String); /* effettua la conversione di tipo e inserisce un valore nel database di fireBase */
    void setInt(String, int);                           /* imposta un intero */
    void setString(String, String);                     /* imposta una stringa */
    void setFloat(String, float);                       /* imposta un valore float */
    void readFB(String);                                /* stampa a video il valore letto */
    int countNode(String);                              /* conta gli elementi di un array */
    void readFBtoPrintFile(String, String, String);     /* stampa su file il valore letto */
    bool signUP() { return signupOK; };                 /* ritorna il valore della variabile signupOK che indica lo stato della connessione */

}; /* end class fbase */

#endif