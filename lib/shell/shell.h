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
#include <Arduino.h>        /* si utilizza per gli oggetti String */
#include <FS.h>             // Libreria per le funzionalita del file system
#include <SPIFFS.h>         /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */
#include <SoftwareSerial.h> /* include la libreria per la personalizzazione della comunicazione seriale */
#include <WebServer.h>      /* libreria per la gestione del webserver */
#include <WiFi.h>           /* libreria per la gestione del modulo wifi */

/*
 * Importazione delle librerie di terze parti
 * Libreria di utility per la gestione di una board LoRa scritta da Renzo Mischianti https://www.mischianti.org/
 */
#include "LoRa_E220.h"

/*
 * include le librerie standard per C++
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

/*
 * include la libreria personalizzata per la gestione dell'input su seriale
 */
#include "input.h"

#ifndef __SHELL__

using namespace std;

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
  //LoRa_E220* e220ttl;



/*
* ---------------------------------------------------------------------------------------------
* Crea le mappe dei valori per la configurazione della scheda LoRa
*/

 
  /*
   * crea una struttura che modella lo stato della shell
   */
  struct
  {
    string __cmd__;      // ultimo comando digitato
    string __row__;      // riga digitata nell'ultimo input in formato string
    input __inpRow__;    // riga digitata
    boolean __mode__;    // indica se la modalita' shell e' attiva
    string __cur_path__; // path corrente dove verra' applicato il command
    int __SesTimeout__;  // imposta il tempo di timeout
    string __prmt__;     // stringa di prompt ( NOME MACHHINA + PATH)
    string __pipe__;     // nome del file di pipe
    string __mach__;     //
    string __temp__;     // variabile temporanea
  } cfgshell;            // configurazione dell'ambiente di shell

  /* definisce la struttura della matrice dei flag per i comandi che lo prevedono */
  struct
  {
    string sFlag;  // contiene il flag della regexp
    int iPos;      // contiene l'eventuale risultato della regexp
    string sValue; // valore del flag
  } typedef cell;

  /* crea l'array dei valori dei flag passati ad un comando */
  cell aReg[6];

  /* modella una variabile di memoria */
  struct
  {
    string name;
    string value;
  } typedef memvar;

  /* vettore delle variabili di memoria */
  std::vector<memvar> aVar;

  /*
   * Funzioni private della classe shell
   */

  /*
   * effettua il parsing delle opzioni passate da linea di comando oppure lette da file
   * per ogni opzione valida richiama il metodo LoRaSetConf per l'impostazione
   */
  void LoRaParseConf(string);                       // effetua la scansione delle opzioni
  void LoRaReadConf();                              // Legge la Configurazione di una board
  void LoRaSetConf(string, string);

  // FUNZIONI DI UTILITA' DELLA CLASSE SHELL
  void s2IP(string, int[]);         // converte una stringa in un indirizzo IP
  string trim(const std::string &); // elimina gli spazi iniziali e finali
  string rsearch(string, string);   // esegue la ricerca con regexp



public:
  shell(); // costruttore di default

  /*
   * Avvio della sessione di shell.
   * Quando si passa la stringa del parametro formale la shell viene avviata in modo trasparente
   * mentre quando il metodo viene richiamato senza alcun parametro la shell viene avviata in modo interattivo
   */
  boolean start(string); // avvio della sessione di shell

  /* COMANDI DI SHELL */
  void ifconfig();         // stampa lo stato della configurazione di rete
  void ls(string);         // elenca i file della directory passata come parametro
  void init(int);          // avvia/ferma un servizio
  void edlin(string);      // editor di linea
  void rm(string);         // cancella un file
  void cp(string, string); // copia un file
  boolean mkdir();         // crea una directory
  boolean format();        // formatta il file system
  boolean mv(string);      // sposta un file
  void set(string);        // imposta il valore di una vairabile
  void echo(string);       // visualizza a video o salva su il valore una variabile
  int ifup(string);        // attiva la scheda wifi in una delle seguenti modalita'
                           // (c - client / a - acces point )
  void grep();             // ritorna la sottostringa estratta dal comando grep se
                           // impostato il paramatero --var
  void lora();             // gestisce una scheda lora collegata alla board ESP32

  /* UTILITY DI SHELL */
  void setMode(boolean __set__) { cfgshell.__mode__ = __set__; } // imposta la modalita' di shell
  String s2S(string);                                            // converte da String a string
  string S2s(String);                                            // converte da string a String
  string readLastCmd() { return cfgshell.__cmd__; }              // legge l'utimo comando digitato
  string getPath() { return cfgshell.__cur_path__; }             // legge la path corrente
  string row();                                                  // attiva l'input del comando
  string extract(string, string);                                // ritorna il valore di una regexp
  int dbg();                                                     // attiva disattiva il debug
  
  /*
  * Imposta i PIN della board
  */
  void pinSet(string sName, int iValue, int iMode) {   
    pinMode(iValue, iMode);
    myPIN[sName] = iValue; }

  /* Utility della matrice dei Flag */
  void flag(string, string);                                                             // costruisce la matrice dei flag
  void setFlag(string __sVal__, int __iIndex__) { aReg[__iIndex__].sFlag = __sVal__; }   // imposta il valore del flag dell'omonima matrice
  void setPos(int __iVal__, int __iIndex__) { aReg[__iIndex__].iPos = __iVal__; }        // imposta il valore della posizione del flag
  void setValue(string __sVal__, int __iIndex__) { aReg[__iIndex__].sValue = __sVal__; } // imposta il valore del flag
  string readFlag(int __iIndex__) { return aReg[__iIndex__].sFlag; }                     // legge il valore della cella flag della matrice dei flag
  string readFlag(string);                                                               // legge il valore della cella flag della matrice dei flag
  int readPos(int __iIndex__) { return aReg[__iIndex__].iPos; }                          // legge la posizione del flag
  int readPos(string);                                                                   // legge la posizione del flag
  string readValue(int __iIndex__) { return aReg[__iIndex__].sValue; }                   // legge il valore del flag
  void printFlag();                                                                      // stampa la matrice dei flag
  void cleanFlag();                                                                      // pulisce la matrice dei flag

}; // end class shell

#endif // __SHELL__
