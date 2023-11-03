/*
 * ARI Montecatini
 *
 * https://arimontecatini.it
 *
 *
 * IU5HLS - Andrea Venuti
 *
 * Amplia la gestione dell'input direttamente su porta seriale permettendo di scrivere stringhe in modo continuo
 *
 */

/*
 * Include la libreria delle definizioni dei metodi implementati in questo file
 */
#include "input.h"

// #include <Arduino.h>
// #include <string.h>

// #include <iostream>
// #include <regex>

input::input()
{
  __mode__ = true;
  __SesTimeout__ = 30000;
  __row__ = "...digita help...";
  __ROW__ = "...digita help...";
  __HideEcho__ = false;

  MaxSerRx = 128;

} // costruttore di default

/*

  SerSrvMenu -  Serial Communication, get characters from  Serial (USB) Port
  ------------------------------------------------------------------------


  La funzione permette permette di acquisire una stringa digitata da tastiera.
  I caratteri digitati vengono memorizzati nell'array di carartteri
  receivedchars[] terminato dal carattere '\0' quando si preme il gtasto return.
  Per evitare lo sconfinamento dei limiti dell'array ad ogni pressione del tasto
  si incremnta l'indice dell'array che quando raggiunge il limite massimo viene
  decrementato di uno di modo che nell'ultima posizione ci sia sempre l'ultimo
  carattere digitato che alla pressione del tasto return viene sostituito dal
  carattere di escape '\0'.

  Inoltre vi è un controlllo della durata del tempo di acquisiszione con il
  contatore che viene resettato ad ogni pressione di tasto.

  Infine l'impostazione a true della variabile HideEcho implica che il carattere
  digitato viene visualizzato con il carattere *.


  Ser_Rx()
  |
  +---[controllo del timeout]
  |
  +---[blocco di lettura]{
  |
  +----[blocco di acquisizione]

*/
void input::read_Rx()
{
  unsigned long SessStart = millis(); // durata del timeout
  char endMarker1 = '\r';             // Some terminal client apps send a CR
  char endMarker2 = '\n';             // Others just a LF character, therefore we need to check for both
  char rc;                            // carattere digitato
  const byte numChars = MaxSerRx;
  boolean newData = false;
  static byte ndx = 0;          // inizializza l'indice dell'array dei caratteri digitati utilizzato nella funzione read_RX
  char receivedChars[numChars]; // an array to store the received data

  // Serial.println(numChars);

  if (__row__.length() > 0)
  {
    ndx = __row__.length();
    for (int iAux = 0; iAux <= ndx; iAux++)
      receivedChars[iAux] = __row__[iAux];
  }

  while (newData == false)
  { // blocco di lettura

    // controllo della durata della sessione di lettura
    // if (millis() - SessStart > __SesTimeout__) { // blocco di controllo
    //   Serial.print("The session has timed out (");
    //   Serial.print(__SesTimeout__ / 1000);
    //   Serial.println(" seconds)...");
    //   break;
    // } // end of control block

    std::cout << std::flush;

    /* Ciclo di acquisizione della stringa. Si esegue fino a quando non si preme
       return. Ad ogni pressione del tasto si resetta il contatore del timeout.
       Alla pressione del tasto return */
    while (Serial.available())
    { // blocco di acquisizione dei caratteri
      SessStart = millis();
      rc = Serial.read();

      if (rc == endMarker1 || rc == endMarker2)
      {                            // if a CR or LF was received
        receivedChars[ndx] = '\0'; // terminate the character array (string)...
        ndx = 0;
        newData = true;
        // char temp =  Serial.read();
        goto ReturnReceivedString; // return everything
      }
      else if (rc == 127)
      { // A DEL character (decimal 127) was received
        ndx =
            ndx - 1; // Set the Array-Pointer back to delete the last characte
        Serial.print(
            rc); // Echo del DEL character back that the Terminal Client app
      }          // removes the last character from the screen
      else
      {
        receivedChars[ndx] = rc; // Receive normal characters..
        ndx++;
        // Serial.print(ndx);
        if (ndx >= numChars)
        {
          ndx = numChars - 1;
        }

        if (__HideEcho__ ==
            false)
        { // Hide echo if user types in a password...
          Serial.print(rc);
        }
        else
        { // Normal echo
          Serial.print("*");
        }
      }

      //

    } // fine del blocco di acquisizione dei caratteri
  }   // fine del blocco di lettura

ReturnReceivedString:

  // copia l'array di input nell'oggetto STRING
  __ROW__.clear();
  for (int iAux = 0; receivedChars[iAux] != '\0'; iAux++)
    __ROW__ += receivedChars[iAux];

  // coversione dell'oggetto String in string
  __row__.clear();
  __row__ = input::STR2str(__ROW__);

} // fine della funzione Ser_Rx()

/* converte la stringa di arduino in std::string */
std::string input::STR2str(String STR)
{
  int len = 0;
  int pos;
  char rc;
  std::string str;

  len = __ROW__.length();
  char buffer[len + 1];
  __ROW__.toCharArray(buffer, len + 1);

  pos = 0;

  while (buffer[pos] != '\0')
  {
    rc = buffer[pos];
    str = str + rc;
    pos++;
  }

  return str;
}

/* converte da std::string a String */
String input::str2STR(std::string str)
{
  int len = 0;
  int pos;
  char rc;
  String STR;

  len = str.length();
  char buffer[len + 1];

  strcpy(buffer, str.c_str());

  pos = 0;

  while (buffer[pos] != '\0')
  {
    rc = buffer[pos];
    STR = STR + rc;
    pos++;
  }

  return STR;

} // end str2STR()

/* cancella spazi e caratteri di controllo a sx della stringa */
void input::ltrim()
{
  int start = this->__row__.find_first_not_of(WHITESPACE);

  if (start == std::string::npos)
  {
    this->__row__ = this->__row__ = "";
  }
  else
  {
    this->__row__ = this->__row__.substr(start);
  }

  this->__ROW__ = input::str2STR(this->__row__);
}

/* cancella spazi e caratteri di controllo a dx della stringa */
void input::rtrim()
{
  int end = this->__row__.find_last_not_of(WHITESPACE);

  if (end == std::string::npos)
  {
    this->__row__ = this->__row__ = "";
  }
  else
  {
    this->__row__ = this->__row__.substr(0, end + 1);
  }

  this->__ROW__ = input::str2STR(this->__row__);
}

/* cancella spazi e caratteri di controllo a dx e a sx della stringa */
void input::trim()
{
  this->rtrim();
  this->ltrim();
}

/* converte la stringa in un intero */
int input::stoi() const {

  try
  {
    int convertedValue = std::stoi(__row__);
    return convertedValue;
  }
  catch (const std::exception &e)
  {
    return 0;
  }
}