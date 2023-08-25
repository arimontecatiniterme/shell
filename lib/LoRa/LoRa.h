/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 */

/*
 * Associazione dei PIN con la board LoRa
 * --------------------------------------
 *
 *     GND   AUX   RXD    M0
 *      |  |  |  |  |  |  |
 *   +--|--|--|--|--|--|--|--+
 *   |    VCC   TXD    M1    |
 *   |                       |
 *   |                       |
 *   |  Board LoRa vista     |
 *   |  lato circuito        |
 *   |                       |
 *   |                       |
 *   |                       |
 *   |   \|/                 |
 *   |    |                  |
 *   +--|   |----------------+
 *      |   |
 *      \   /
 *       +-+
 *
 * ESP32 / LoRa
 *
 * G19    -->     M0
 * G18    -->     M1
 * V5     -->     VCC
 * GND    -->     GND
 * G16    -->     AUX
 * G17    -->     TX
 * G05    -->     RX
 */

/*
 * Utilizzi Vari dei PIN
 * ----------------------
 *
 * GPIO16     -->     pulsante esterno per l'attivazione dell'ambiente di shell su seriale
 *
 */

/*
 * Utilizzo dei PIN per il debug con la board ESP-PROG
 * ---------------------------------------------------
 *
 * https://arimontecatini.it/tutorials/esp8266/debug.html
 *
 *
 * Connettore JTAG di ESP-PROG
 *      +----+
 * VCC  |*  *| TMS
 * GND  |*  *| TCK
 * GND  |*  *| TDO
 * GND  |*  *| TDI
 * GND  |*  *|
 * GND  |*  *|
 *      +----+
 *
 * ESP32 / JTAG
 *
 * GPIO14   -->  TMS
 * GPIO12   -->  TCK
 * GPIO13   -->  TDI
 * GPIO15   -->  TD0
 * GND      -->  GND
 * 5        -->  VCC
 *
 */

/*
 * Importazione delle librerie specifiche per ESP 32
 */
#include <Arduino.h> // Libreria standard di Arduino
#include <SPIFFS.h>  /* gestione del file system tipo SPIFFS */
#include <iostream>  /* gestione dell'input */
#include <map>       /* libreria per la gestione delle mappe */
#include <regex>     /* libreia per la gestione delle espressioni regolari */

#ifndef __LORA__
#define __LORA__

#include "LoRa_E220.h" /* libreria di Renzo Mischianti per la gestione della boar LoRa ebyte E220 */

/* Associazione dei PIN della ESP32 alla board LoRa
 * G21    -->     M0
 * G19    -->     M1
 * V5     -->     VCC
 * GND    -->     GND
 * G18    -->     AUX
 * G16    -->     TX
 * G17    -->     RX
 */
#define BUTTON GPIO_NUM_12 // pin per il button che permette l'accesso alla shell
#define M0 GPIO_NUM_21
#define M1 GPIO_NUM_19
#define AUX GPIO_NUM_18
#define TX GPIO_NUM_16
#define RX GPIO_NUM_17

class LoRa
{
private:
    // LoRa_E220 e220ttl(&Serial2, AUX, M0, M1);
    LoRa_E220 e220ttl;

    // Sending message as struct
    struct ChatMessage
    {
        char user[20];
        char message[250];
    };

    String s2S(std::string);     /* converte da std::string a String */
    std::string S2s(String STR); /* converte da String a std::string */

public:
    LoRa();                                /* costruttore di default */
    LoRa(HardwareSerial *, int, int, int); /* costruttore personalizzato */
    void rlConf(String, String);           /* Stampa su file la Configurazione di una board */
    void rlConf();                         /* Stampa a video la Configurazione di una board */
    void slConf(std::string, std::string); /* Imposta la Configurazione di una board */
    void slConf(String);                   /* Imposta la confgigurazione di una scheda lora da file */
    void rlMsg();                          /* legge un messaggio lora */
    void slMsg(String, String, int);       /* invia un messaggio lora di tipo struct */
    void slMsg(String, int);               /* invia un messaggio LoRa non strutturato */
    void slMsg(int, int, int, String);     /* invia un messaggio in modo FIXED */
    void init();                           /* inizializza una scheda Lora */

}; // end class lora

#endif // endif di #ifndef __LORA__