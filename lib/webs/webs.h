/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Dichiara i metodi per la classe del webserver
 *
 */

/*
 * include le librerie standard per ESP32
 */
#include <Arduino.h>           /* si utilizza per gli oggetti String */
#include <ESPAsyncWebServer.h> /* include la libreria del web server asincrono */
#include <SPIFFS.h>            /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */
#include <Ticker.h>

/*
 * Include le librerie personaizzate
 */

#include "shell.h"

#ifndef __WEBS__
#define __WEBS__

class webs
{

private:
    AsyncWebServer websrv;
    Ticker timeOutTimer;

public:
    webs();    /* costruttore di default */
    webs(int); /* costruttore personalizzato - ws( port )*/

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione di un file ad
     * un URL.
     *
     * signature : void onFile(String, string, int, AsyncWebServer *)
     *             void onFile( url della risorsa,
     *                          path completo del file associato,
     *                          modo HTTP, [HTTP_GET,HTTP_POST]
     *                          )
     *
     */
    void onFile(String, String, int);

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione di un file ad
     * un URL con esecuzione di un comando shell .
     *
     * signature : void onFile(String, string, int, AsyncWebServer *)
     *             void onFile( url della risorsa,
     *                          path completo del file associato,
     *                          modo HTTP, [HTTP_GET,HTTP_POST],
     *                          comando shell
     *                          )
     *
     */
    void onFile(String, String, int, shell *, String);

    /*
     * Metodo per la gestione dei form con restituzione di testo
     *
     * associa le variabile del form a delle variabili in memoria
     * utilizzando la modalita' trasparente della shell
     *
     * signature : void onFile(String, string, int, AsyncWebServer *)
     *             void onFile( url della risorsa,
     *                          path completo del file associato,
     *                          modo HTTP, [HTTP_GET,HTTP_POST],
     *                          comando shell
     *                          )
     *
     */
    void onForm_FORM_Text(String, String, int, std::string);

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione di una stringa ad un
     * un URL.
     *
     * signature : void onText(String, String, int, AsyncWebServer *)
     *             void onText( url della risorsa,
     *                          Stringa da associare,
     *                          modo HTTP, [HTTP_GET,HTTP_POST]
     *                          )
     *
     */
    void onText(String, String, int, AsyncWebServer *);

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione di una stringa ad un
     * un URL con esecuzione di un comando shell.
     *
     * signature : void onText(String, String, int, AsyncWebServer *)
     *             void onText( url della risorsa,
     *                          Stringa da associare,
     *                          modo HTTP, [HTTP_GET,HTTP_POST]
     *                          puntatore a oggetto shell,
     *                          comando shell
     *                          )
     *
     */
    void onText(String, String, int, shell*,  String);

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione di una file media ad un
     * un URL.
     *
     * signature : void onText( String, String, int, AsyncWebServer *)
     *             void onText( url della risorsa,
     *                          nome del file da associare,
     *                          mime type,
     *                          modo HTTP, [HTTP_GET,HTTP_POST]
     *                          )
     *
     */
    void onMedia(String, String, String, int);

    /*
     * Metodo per la gestione delle risorse di web server mediante l'associazione del comando ls ad un
     * un URL.
     *
     * signature : void onText( String, String, int)
     *             void onText( url della risorsa,
     *                          comando da eseguire,
     *                          modo HTTP, [HTTP_GET,HTTP_POST]
     *                          oggetto di tipo shell
     *                          )
     *
     */
    void onDownload(String);

    /* avvia il web server */
    void begin();

    /* pulisce la coda delle richieste */
    void clear();
};

#endif
