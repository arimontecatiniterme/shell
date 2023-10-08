#include <Arduino.h>

#ifndef __BT__
#define __BT__

#include <BluetoothSerial.h>
#include <SPIFFS.h>
#include <iostream>

using namespace std;

class bt
{

private:
    BluetoothSerial BTSerial; /* oggetto BT */
    String s2S(string);       /* converte da std::string a String */
    string S2s(String STR);   /* converte da String a std::string */

public:
    bt();                        /* costruttore di default */
    bt(String);                  /* overload del costruttore di default */
    void setNameBT(String);          /* imposta il nome della scheda BT */
    void readBT();               /* stampa a video quanto letto dalla scheda BT */
    void readBT(String, String); /* stampa su file quanto letto dalla scheda BT */
    void sendBT(String);         /* invia un messaggio sulla scheda BT */
    void iBT();                  /* imposta la scheda BT */
    void iBT(String);            /* imposta la sheda BT da file */
};

#endif // end __BT__