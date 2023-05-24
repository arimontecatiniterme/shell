/* librerie native ESP 8266*/
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h> // libreria per la gestione del webserver
#include <WiFi.h>      //libreria per la gestione del wifi

/* librerie C++ */
#include <iostream>
#include <regex>

/* librerie personali ./lib/..... */
#include "Regexp.h"
#include "input.h"
#include "shell.h"

#define BUTTON 16 // pin per l'accesso al menu

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

  pinMode (BUTTON, INPUT);

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
