/*
 * ARI Montecatini
 *
 * https://arimontecatini.it
 *
 * IU5HLS - Andrea Venuti
 *
 * Amplia la gestione dell'input direttamente su porta seriale permettendo di scrivere stringhe in modo continuo
 *
 */

/*
 * inclusione delle librerie standard di ESP32
 */
#include <Arduino.h>

/*
 * inclusione delle librerie standard di C++
 */
#include <iostream>
#include <regex>
#include <string>

#ifndef __INPUT__
#define __INPUT__

using namespace std;

class input
{

private:
  boolean __mode__;                             // indica se il sistema si trova in modalita' input mode
  boolean __HideEcho__;                         // se true maschera i caratteri digitati
  std::string __row__;                          // riga digitata - oggetto di tipo string
  const std::string WHITESPACE = " \n\r\t\f\v"; // stringa di escape
  String __ROW__;                               // riga digitata - oggetto di tipo String
  int __SesTimeout__;                           // tempo di time out
  int MaxSerRx;                                 // massimo numero di caratteri per riga

  /* utility per la gestione della classe */
  std::string STR2str(String);                          // conversione da String a std::string
  String str2STR(std::string);                          // conversione da std::string a String
  boolean isMode() { return __mode__; }                 // ritorna lo stato della classe
  const String &getSTR() const { return __ROW__; }      // conversione da std::string a String
  const std::string &getstr() const { return __row__; } // ritorna il contenuto utile della classe

public:

  input();                                                  // costruttore di default
  void setMode(boolean __set__) { __mode__ = __set__; }     // imposta lo stato della classe
  void setEcho(boolean __set__) { __HideEcho__ = __set__; } // imposta il modo di echo dei caratteri
  void read_Rx();                                           // legge i caratteri da tastiera
  void rtrim();                                             // cacella i caratteri a dx della stringa con il match della stringa WHITESPACE
  void ltrim();                                             // cancella i caratteri a sx della stringa con il match della stringa WHITESPACE
  void trim();                                              // cancella i caratteri a sx e a dx della stringa con il match della stringa WHITESPACE
  int stoi() const;                                         // converte la stringa in un intero
  void clear()                                              // svuota il contenuto utile dell'oggetto
  {
    __row__ = "";
    __ROW__ = "";
  }

  // friend   istream& operator>>(istream& is, input& __obj__){ __obj__.__row__=__obj__.STR2str();return is;}
  // sovracaricamento dell'operatore  >>
  friend std::istream &operator>>(std::istream &is, input &__obj__)
  {
    __obj__.read_Rx();
    return is;
  }                                                                                                   // sovracaricamento dell'operatore  >>
  friend std::ostream &operator<<(std::ostream &os, input &__obj__) { return os << __obj__.__row__; } // sovracaricamento dell'operatore <<

  friend void operator>>(const std::string &lhs, input &__obj__) { __obj__.__row__ = lhs; } // sovracaricamento operator >> string -> input

  friend std::string &operator+=(std::string &lhs, const input &__obj__) { return lhs += __obj__.__row__; } // sovracaricamento operator string += input
  friend String &operator+=(String &LHS, const input &__obj__) { return LHS += __obj__.__ROW__; }           // sovracaricamento operator String += input

  friend boolean operator==(const std::string &lhs, const input &__obj__) { return lhs == __obj__.__row__; } // sovracaricamento operator string==input
  friend boolean operator==(const input &__obj__, const std::string &rhs) { return rhs == __obj__.__row__; } // sovracaricamento operator input==string

  friend boolean operator==(const String &LHS, const input &__obj__) { return LHS == __obj__.__ROW__; } // sovracaricamento operator String==input
  friend boolean operator==(const input &__obj__, const String &RHS) { return RHS == __obj__.__ROW__; } // sovracaricamento operator input==String

  /*
  La dichiarazione operator string const &() const { return getstr(); } all'interno di una classe è un esempio di operatore di
   conversione personalizzato. Questo operatore consente di definire una conversione implicita da un oggetto di quella classe a
   un altro tipo specificato, in questo caso string (ovvero std::string).

  Vediamo in dettaglio cosa fa questa dichiarazione:

  operator string const &(): Questo dichiara un operatore di conversione. La keyword operator indica che stai definendo un
  operatore. string è il tipo di dati di destinazione al quale stai cercando di convertire l'oggetto. const & indica che
  il risultato della conversione è una costante riferita (riferimento costante) a un oggetto di tipo string.

  const: Questa è una parte della dichiarazione che indica che il riferimento restituito è costante, il che significa che
  non puoi modificare l'oggetto attraverso questo riferimento.

  &(): Questa parte della dichiarazione definisce l'operatore di conversione come una funzione senza argomenti, quindi la
  sintassi () indica che è una funzione. L'operatore restituirà un riferimento al tipo di dati specificato, in questo
  caso string.

  { return getstr(); }: Questo è il corpo dell'operatore di conversione. getstr() è un metodo della classe input che
  restituisce il contenuto della variabile privata __row__ come string. L'operatore di conversione utilizza questo
  metodo per ottenere il valore da convertire in string e quindi restituisce il riferimento a tale valore.

  In sintesi, l'operatore di conversione operator string const &() ti consente di convertire un oggetto della classe input
  in un oggetto di tipo string utilizzando una conversione implicita. Ad esempio, se hai un oggetto input chiamato myInput,
  puoi assegnarlo a una variabile string come se fosse già un oggetto di tipo string. Questo offre una maggiore comodità e
  flessibilità nell'uso degli oggetti della classe input.
  */
  operator String const &() const { return getSTR(); }      // permette assegmamenti del tipo String=input
  operator std::string const &() const { return getstr(); } // permette assegmamenti del tipo string=input

  // ------------------------------------------------------------------------------------------------------------------------------------------------------------------

  input &operator=(const std::string &str) // overload operatore per assegnazioni del tipo input=string
  {
    this->clear();
    __row__ = str;
    __ROW__ = str2STR(__row__);
    return *this; // Restituiamo l'oggetto corrente per supportare gli assegnamenti concatenati
  }
};

#endif
