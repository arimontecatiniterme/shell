#include <Arduino.h>

#include <iostream>
#include <string>
#include <vector>

#include "input.h"

#ifndef __SHELL__

using namespace std;

class shell
{
private:
 

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

  string trim(const std::string &); // elimina gli spazi iniziali e finali
  string rsearch(string, string);   // esegue la ricerca con regexp

  void s2IP(string, int[]); // converte una stringa in un indirizzo IP

  struct
  {
    string sFlag;  // contiene il flag della regexp
    int iPos;      // contiene l'eventuale risultato della regexp
    string sValue; // valore del flag
  } typedef cell;

  cell aReg[6];


struct {
  string name;
  string value;
} typedef memvar;

  std::vector<memvar> aVar;

public:
  shell(); // costruttore di default

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
  void echo(string);       // visualizza il valore una variabile
  int ifup(string);        // attiva la scheda wifi in una delle seguenti modalita'
                           // (c - client / a - acces point )
  void grep();             // ritorna la sottostringa estratta dal comando grep se
                           // impostato il paramatero --var

  /* UTILITY DI SHELL */
  void setMode(boolean __set__) { cfgshell.__mode__ = __set__; } // imposta la modalita' di shell
  String s2S(string);                                            // converte da String a string
  string S2s(String);                                            // converte da string a String
  boolean start();                                               // avvio della sessione di shell
  string readLastCmd() { return cfgshell.__cmd__; }              // legge l'utimo comando digitato
  string getPath() { return cfgshell.__cur_path__; }             // legge la path corrente
  string row();                                                  // attiva l'input del comando
  string extract(string, string);                                // ritorna il valore di una regexp
  int dbg();                                                     // attiva disattiva il debug



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
