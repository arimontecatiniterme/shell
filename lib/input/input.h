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
#include <string>
#include <regex>

#ifndef __INPUT__
#define __INPUT__

using namespace std;

class input
{

private:
  boolean __mode__;                        // indica se il sistema si trova in modalita' input mode
  boolean __HideEcho__;                    // se true maschera i caratteri digitati

  string __row__;                          // riga digitata - oggetto di tipo string
  String __ROW__;                          // riga digitata - oggetto di tipo String

  int __SesTimeout__;                      // tempo di time out
  int MaxSerRx;                            // massimo numero di caratteri per riga

  const string WHITESPACE = " \n\r\t\f\v"; // stringa di escape

public:
  input(); // costruttore di default

  void setMode(boolean __set__) { __mode__ = __set__; }     // imposta lo stato della classe
  void setEcho(boolean __set__) { __HideEcho__ = __set__; } // imposta il modo di echo dei caratteri
  void read_Rx();                                           // legge i caratteri da tastiera
  void rtrim();                                             // cacella i caratteri a dx della stringa con il match della stringa WHITESPACE
  void ltrim();                                             // cancella i caratteri a sx della stringa con il match della stringa WHITESPACE
  void trim();                                              // cancella i caratteri a sx e a dx della stringa con il match della stringa WHITESPACE
  void clear()                                              // svuota il contenuto utile dell'oggetto
  {
    __row__ = "";
    __ROW__ = "";
  }

  void setR(string __value__)
  {
    __row__ = __value__;
    input::str2STR(__value__);
  }


  string STR2str(String); // conversione da String a std::string
  string opt();           // cerca se vi sono opzioni nella stringa di input
  string regS(string);    // cerca la struttura di una path nella stringa di input

  String str2STR(string); // conversione da std::string a String

  boolean isMode() { return __mode__; } // ritorna lo stato della classe

  char c2s(string); //

  // -----------------------------------------------------------------------------------------------------------------------

  const String &getSTR() const { return __ROW__; }; // conversione da std::string a String
  const string &getstr() const { return __row__; }  // ritorna il contenuto utile della classe

  // ------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // friend   istream& operator>>(istream& is, input& __obj__){ __obj__.__row__=__obj__.STR2str();return is;} // sovracaricamento dell'operatore  >>
  friend istream &operator>>(istream &is, input &__obj__)
  {
    __obj__.read_Rx();
    return is;
  }                                                                                                     // sovracaricamento dell'operatore  >>
  friend ostream &operator<<(ostream &os, input &__obj__) { return os << __obj__.__row__; }             // sovracaricamento dell'operatore <<

  friend void operator>>(const string &lhs, input &__obj__) { __obj__.__row__ = lhs; }                  // sovracaricamento operator >> string -> input

  friend string &operator+=(string &lhs, const input &__obj__) { return lhs += __obj__.__row__; }       // sovracaricamento operator string += input
  friend String &operator+=(String &LHS, const input &__obj__) { return LHS += __obj__.__ROW__; }       // sovracaricamento operator String += input

  friend boolean operator==(const string &lhs, const input &__obj__) { return lhs == __obj__.__row__; } // sovracaricamento operator string==input
  friend boolean operator==(const input &__obj__, const string &rhs) { return rhs == __obj__.__row__; } // sovracaricamento operator input==string

  friend boolean operator==(const String &LHS, const input &__obj__) { return LHS == __obj__.__ROW__; } // sovracaricamento operator String==input
  friend boolean operator==(const input &__obj__, const String &RHS) { return RHS == __obj__.__ROW__; } // sovracaricamento operator input==String

  operator String const &() const { return getSTR(); }                                                  // permette assegmamenti del tipo String=input
  operator string const &() const { return getstr(); }                                                  // permette assegmamenti del tipo string=input

  // ------------------------------------------------------------------------------------------------------------------------------------------------------------------
};

#endif
