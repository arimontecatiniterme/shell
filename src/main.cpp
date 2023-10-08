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
 * 3.3      -->  VCC
 *
 */

/*
 * Importazione delle librerie specifiche per ESP 32
 */
#include <Arduino.h>           /* Libreria standard di Arduino */
#include <ESPAsyncWebServer.h> /* webserve asincrono */
#include <FS.h>                /* Libreria per le funzionalita del file system */
#include <SPIFFS.h>            /* Libreria specifica per la gestione del filesystem di tipo SPIFFS installato della memoria flash della ESP32 */

/*
 * librerie specifiche del progetto path ./lib/.....
 */
#include "input.h" /* simula l'input da riga di comando sulla porta seriale */
#include "shell.h" /* mette a disposizione una shell utilizzabile con la seriale oppure in modo trasparente */
                   /* mette a disposizione oggetti di tipo LoRa e BT */
#include "iw.h"    /* include le funzioni di utility del wifi */
#include "webs.h"  /* include le funzionalita' per la gestione del web-server */

#define BUTTON GPIO_NUM_12 // pin per il button che permette l'accesso alla shell

using namespace std;

/* imposta e variabili locali */
const int iPort = 80; /* porta di ascolto del web server */

/* dichiara gli oggetti */
shell sh;        /* oggetti di tipo shell */
webs wbs(iPort); /* imposta il web server */

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

  /* inizializza il filesystem */
  Serial.print(F("Inizializing FS..."));
  if (SPIFFS.begin(true))
  {
    Serial.println(F("done."));
    // sh.start("iw --connect passwd=saksdfiower45grtwr8@[]] ssid=anonimasarda");
    delay(3000);
    sh.start("iw --connect passwd=settembre63 ssid=bakunin");
    sh.start("iw --status");
    sh.start("ls");
    wbs.onFile("/", "/index.html", HTTP_GET);
    wbs.onFile("/shell.html", "/shell.html", HTTP_GET);
    wbs.onLS("/ls.html", HTTP_GET);
    wbs.onFile("/shell.txt", "/shell.txt", HTTP_GET);
    
    wbs.onMedia("/shell.txt", "/shell.txt", "text/plain", HTTP_GET);
    wbs.onMedia("/worg.css", "/worg.css", "text/plain", HTTP_GET);

    wbs.begin();
  }
  else
  {
    Serial.println(F("fail."));
  }
}

void loop()
{

  sh.start();
}
