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
 * G0     -->     M0
 * G2     -->     M1
 * V5     -->     VCC
 * GND    -->     GND
 * G21    -->     AUX
 * TDX    -->     TX
 * RDX    -->     RX
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
 * 3.3      -->  VCC
 *
 */

/*
 * Importazione delle librerie specifiche per ESP 32
 */
#include "driver/gpio.h" // Libreria per la gestione dei PIN della ESP32
#include <Arduino.h>     // Libreria standard di Arduino
#include <FS.h>          // Libreria per le funzionalita del file system
#include <SPIFFS.h>      /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */
#include <WebServer.h>   /* libreria per la gestione del webserver */
#include <WiFi.h>        /* libreria per la gestione del modulo wifi */

/*
* Importazione delle librerie di terze parti
*/
#include "LoRa_E32.h"    /* Libreia di utility per la gestione di una board LoRa scritta da Renzo Mischianti https://www.mischianti.org/ */


/* 
* librerie specifiche del progetto path ./lib/..... 
*/
#include "input.h"       /* simula l'input da riga di comando sulla porta seriale */
#include "shell.h"       /* mette a disposizione una shell utilizzabile con la seriale oppure in modo trasparente */



/* Associazione dei PIN della ESP32 */
#define BUTTON 16 // pin per l'accesso al menu
#define M0 GPIO_NUM_0
#define M1 GPIO_NUM_2


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
void makesAnything()
{
  std::cout << "...questa funzione stampa qualcosa...\n";
}

/* funzione di esempio di input */
void inputExample()
{

  /* oggetto di tipo input */
  input in;

  /* definizione delle variabili locali */
  String STR;
  string str;

  in.setEcho(false);

  std::cout << "...input mode..." << std::endl;

  std::cin >> in;
  in.trim();

  str = in;
  std::cout << "string=" << str << "\n";

  STR = in;
  std::cout << "String=" << STR << "\n";

  std::cout << std::endl
            << "in=" << in << std::endl;
  std::cout << "str==in -> " << (str == in) << "\n";
  std::cout << "STR==in -> " << (STR == in) << "\n";
  std::cout << "in==str -> " << (in == str) << "\n";
  std::cout << "in==STR -> " << (in == STR) << "\n";

  str += in;
  std::cout << "str+=input " << str << "\n";
}

void setup()
{

  /* Inizializza la seriale */
  Serial.flush();
  Serial.begin(9600);
  Serial.println("Serial is ready to accept input");

  /* imposta il modo dei PIN */
  pinMode(BUTTON, INPUT);
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  /* inizializza il filesystem */
  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin(true))
  {
    Serial.println(F("done."));
  }
  else
  {
    Serial.println(F("fail."));
  }

  /* impostazione della configurazione generale della scheda */
  myBoard.__timeout__ = 30000;        // 30 sec di timeout
  myBoard.__name__ = "WEMOS D1 R32";  // nome macchina
  myBoard.__cgffile__ = "config.ini"; // file di configurazione del sw
  myBoard.__sCmd__ = "";
  myBoard.bshMode = true;
}

void loop()
{
  // alla pressione del tasto BUTTON

  if (myBoard.bshMode)
  // esegue la modalita' shell
  {
    /* sh.start() ritorna true se il comando e' esterno alla shell, false
     * altrimenti */
    myBoard.bshMode = sh.start();
  }

  if (!myBoard.bshMode)
  /* se il sistema non e' in modalita' shell si eseguono le altre istruzioni
   */
  {
    makesAnything();
    delay(2000);
  }
}
