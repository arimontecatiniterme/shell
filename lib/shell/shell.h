/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Dichiara i metodi per la classe shell che modella l'omonimo ambiente da utilizzare su porta seriale
 *
 */

/*
 * include le librerie standard per ESP32
 */
#include <Arduino.h>         /* si utilizza per gli oggetti String */
#include <BluetoothSerial.h> /* libreria per la gestione BT */
#include <FS.h>              /* Libreria per le funzionalita del file system */
#include <SPIFFS.h>          /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */
#include <SoftwareSerial.h>  /* include la libreria per la personalizzazione della comunicazione seriale */

/*
 * Importazione delle librerie di terze parti
 * Libreria di utility per la gestione di una board LoRa scritta da Renzo Mischianti https://www.mischianti.org/
 */
#include "LoRa.h"

/* importazione delle librerie personali */
#include "bt.h"
#include "edlin.h"
#include "fbase.h"
#include "input.h"

/*
 * include le librerie standard per C++
 */
#include <LittleFS.h>
#include <SD.h>
#include <Update.h>
#include <algorithm> /* include gli algoritmi di uso comune */
#include <fstream>
#include <map>
#include <regex>
#include <vector>

#ifndef __SHELL__
#define __SHELL__

class shell
{
private:
  /*
   * una volta defniti i PIN si crea una struttura che modella la coppia
   * (nome PIN, numero PIN) di modo da poterla utilizzare in un vettore che poi potra' essere
   * passato a classi in cui sara' utilizzato
   *
   */
  std::map<std::string, int> myPIN;

  // definisce una variabile di tipo LoRa
  /*
   * Definisce una variabile che modella una board di tipo LoRa
   *
   * valori di default
   *
   * TX  ---> GPIO_NUM_16
   * RX  ---> GPIO_NUM_17
   *
   * AUX ---> GPIO_NUM_18
   *
   * MO  ---> GPIO_NUM_21
   * M1  ---> GPIO_NUM_19
   *
   * &Serial2  comunicazione da/per board ESP32
   *
   * HS  ---> NULL
   *
   */

  /* modella una struttura che rappresenta una borad esp32 */
  struct board
  {
    String sNAME;           // nome della board
    String sMODEL;          // modello della borad
    int iTimeout;           // indica il tempo massimo di attesa di un ciclo in ms
    input inpCommand;       // ultimo comando digitato
    boolean bShell = false; // se true siamo nel modo shell interattivo
  };

  /* modella la board corrente */
  struct board myBoard;

  /* mappa delle variabili di memoria */
  std::map<std::string, std::string> mVar;

  /* struttura e vettore dei flag */
  struct strPosix
  {
    std::string sPattern;
    std::string sValue;
    int iPos;
  };

  // FUNZIONI DI UTILITA' DELLA CLASSE SHELL
  void s2IP(std::string, int[]);                                                            // converte una stringa in un indirizzo IP
  std::string trim(const std::string &);                                                    // elimina gli spazi iniziali e finali
  std::string rsearch(std::string, std::string);                                            // esegue la ricerca con regexp
  String s2S(std::string);                                                                  // converte da string a String
  std::string S2s(String);                                                                  // converte da String a string
  int dbg();                                                                                // attiva disattiva il debug
  static boolean sortFlag(const strPosix &a, const strPosix &b) { return a.iPos < b.iPos; } // ordina la posizione dei flag

  // Comandi shell interni
  void lora(std::string); // gestisce una scheda lora collegata alla board ESP32
  void txlora(String);    // invia il messaggio mediante lora
  void bts(std::string);  // gestisce la comunicazione bluetoothSerial

  // FUNZIONI DI UTILITA' DELLA SHELL
  void exec(std::string, std::string); // esegue un comando di shell

public:
  shell();            // costruttore di default inizializza l'oggetto con i valori di default
  shell(std::string); // overload dei costruttore con parametri personalizzati

  /*
   * Avvio della sessione di shell.
   * Quando si passa la stringa del parametro formale la shell viene avviata in modo trasparente
   * mentre quando il metodo viene richiamato senza alcun parametro la shell viene avviata in modo interattivo
   */

  /* COMANDI DI SHELL PER LA GESTIONE DEI FILE */
  boolean rm(String);              // cancella un file ( ok )
  boolean cp(String, String);      // copia un file ( ok )
  boolean mv(String, String);      // sposta un file (ok )
  boolean format();                // formatta il file system ( ok )
  void ls(String);                 // elenca i file della directory passata come parametro e stampa a video (ok)
  void ls(String, String, String); // elenca i file della directory passata come parametro e stampa su file (ok)

  /* COMANDI PUBBLICI DI UTILITY SHELL */
  void set(std::string);                          // imposta il valore di una vairabile
  void echo(std::string);                         // visualizza a video o salva su il valore una variabile
  void grep(std::string, String);                 // applica una regexp al contenuto di un file con stampa a video
  void grep(std::string, String, String, String); // applica una regexp al contenuto di un file con stampa su file
                                                  /* void grep(regex, file da leggere, file da scrivere, modo scrittura)*/
  void start();                                   // avvio della shell in modo interattivo
  void start(std::string);                        // avvio della sessione di shell in modo trasparent
  void start(String);                             // avvio della sessione di shell in modo trasparente ma con comando di tipo String nativo
  void macro(String);                             // esecuzione dei comandi contenuti in un file di tipo mcr

}; // end class shell

#endif
