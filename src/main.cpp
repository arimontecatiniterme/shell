/* librerie native ESP 8266*/
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h> // libreria per la gestione del webserver
#include <WiFi.h>      //libreria per la gestione del wifi
#include "driver/gpio.h"

/* librerie C++ */
#include <iostream>
#include <regex>

/* librerie personali ./lib/..... */
#include "Regexp.h"
#include "input.h"
#include "shell.h"

/* definisce i PIN */
#define BUTTON 16 // pin per l'accesso al menu
#define M0 GPIO_NUM_0
#define M1 GPIO_NUM_2


/* Definizione delle variabili globali */



/* Definisce la struttura di una nuova varibile di tipo board */
struct
{
  string __name__;    // nome dispositivo
  string __cgffile__; // file di configurazione
  string __sCmd__;    // ultimo comando eseguito di tipo string
  string __row__;     // ultima riga inserita in formato string
  input __inpCmd__;   // ultimo riga inserita in formato input
  int __timeout__;    // valore di timeout
  boolean bshMode;    // indica che la borda e' in modalita' shell
} typedef machine;

/* Nuova variabile di tipo board */
machine myBoard;

/* variabile che descrive lo stato di una generica board LoRa */
struct {
  int iM0;               // livello di segnale del pin M0
  int iM1;               // livello di segnale del pin M1
  int iAux;              // livello di segnale del pin AUX
  int iCH;               // canale di trasmisisone
  int iLowByteAdr;       // valore della parte bassa dell'indirizzo [0 - 255]
  int iHighByteAdr;      // valore della parte alta dell'indirizzo [0 - 255]
} typedef LoRa;


/* variabile che modella lo stato di una scheda specifica scheda LoRa*/
LoRa myLoRa;


/* oggetto di tipo shell */
shell sh;

/* funzione senza scopo da utilizzare per impegnare il tempo */
void
makesAnything ()
{
  std::cout << "...questa funzione stampa qualcosa...\n";
}

/* funzione di esempio di input */
void
inputExample ()
{

  /* oggetto di tipo input */
  input in;

  /* definizione delle variabili locali */
  String STR;
  string str;

  in.setEcho (false);

  std::cout << "...input mode..." << std::endl;

  std::cin >> in;
  in.trim ();

  str = in;
  std::cout << "string=" << str << "\n";

  STR = in;
  std::cout << "String=" << STR << "\n";

  std::cout << std::endl << "in=" << in << std::endl;
  std::cout << "str==in -> " << (str == in) << "\n";
  std::cout << "STR==in -> " << (STR == in) << "\n";
  std::cout << "in==str -> " << (in == str) << "\n";
  std::cout << "in==STR -> " << (in == STR) << "\n";

  str += in;
  std::cout << "str+=input " << str << "\n";
}

void
setup ()
{

  Serial.flush ();
  Serial.begin (9600);
  Serial.println ("Serial is ready to accept input");

  /* imposta il modo dei PIN */
  pinMode (BUTTON, INPUT);
  pinMode (M0, OUTPUT);
  pinMode (M1, OUTPUT);
  

  Serial.print (F ("Inizializing FS..."));
  if (SPIFFS.begin (true))
    {
      Serial.println (F ("done."));
    }
  else
    {
      Serial.println (F ("fail."));
    }



  /* impostazione della configurazione generale della scheda */
  myBoard.__timeout__ = 30000;        // 30 sec di timeout
  myBoard.__name__ = "WEMOS D1 R32";  // nome macchina
  myBoard.__cgffile__ = "config.ini"; // file di configurazione del sw
  myBoard.__sCmd__ = "";
  myBoard.bshMode = true;

  /* isruzione di debug */
}

void
loop ()
{
  // alla pressione del tasto BUTTON

  if (myBoard.bshMode)
    // esegue la modalita' shell
    {
      /* sh.start() ritorna true se il comando e' esterno alla shell, false
       * altrimenti */
      myBoard.bshMode = sh.start ();
    }

  if (!myBoard.bshMode)
    /* se il sistema non e' in modalita' shell si eseguono le altre istruzioni
     */
    {
      makesAnything ();
      delay (2000);
    }
}
