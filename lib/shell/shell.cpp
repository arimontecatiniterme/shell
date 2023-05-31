/* lib di arduino */

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>

/* lib C++ */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

/* lib personali */
#include "input.h"
#include "shell.h"

/* lib di LoRa*/
#include "LoRa_E32.h"

#define __MACHINE__ ARDUINO
#define DBG 1
#define __PRTDBG__                                                                               \
  if (shell::dbg())                                                                              \
  {                                                                                              \
    std::cout << "\n..." << __FILE__ << " - " << __LINE__ << " - " << __FUNCTION__ << std::endl; \
  }

#define __PRTVAR__(x, y)                                                            \
  if (shell::dbg())                                                                 \
  {                                                                                 \
    std::cout << "\n..." << __FILE__ << " - " << __LINE__ << " - " << __FUNCTION__; \
    std::cout << "\t" << x << "=\t[" << y << "]" << std::endl;                      \
  }

using namespace std;

/* costruttore di default */
shell::shell()
{
  cfgshell.__mode__ = false;
  cfgshell.__cur_path__ = "/";
  cfgshell.__mach__ = "IQ5MT";
  cfgshell.__SesTimeout__ = 30000; // tempo di timeout a 30 secondi
  cfgshell.__prmt__ = cfgshell.__cur_path__;
  cfgshell.__row__ = "";
  cfgshell.__pipe__ = "";
  cfgshell.__temp__ = "";

  // pulisce il vettore delle variabili
  aVar.clear();

  // inizializza il vettore delle configurazioni di LoRa
  // LoRaMode.push_back({M0,M1,nome modo});
  LoRaMode.push_back({1, 1, "program"}); /* modo programmazione */
  LoRaMode.push_back({0, 0, "txrx"});    /* modo txrx */
  LoRaMode.push_back({0, 1, "psave"});   /* modo power saved */
  LoRaMode.push_back({1, 0, "wakeup"});  /* modo tx segnale di wakeup*/
}

/* attiva disattiva il debug  */
int shell::dbg()
{

  int debug = 0;

  for (auto &innerVector : aVar)
  {
    if (innerVector.name == "debug")
    {
      debug = std::stoi(innerVector.value);
    }
  }

  return debug;
}

/* converte da std::string a String */
String shell::s2S(string str)
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

  STR = STR + '\0';

  return STR;

} // end

/* converte da String a std::string */
string shell::S2s(String STR)
{
  int len = 0;
  int pos;
  char rc;
  string str;

  len = STR.length();
  char buffer[len + 2];

  strcpy(buffer, STR.c_str());

  pos = 0;

  while (buffer[pos] != '\0')
  {
    rc = buffer[pos];
    str = str + rc;
    pos++;
  }

  return str;

} // end

//************************************************************/
//
//       ID :
// Descrive : elimina gli spazi iniziali e finali
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
string shell::trim(const std::string &source)
{
  std::string s(source);
  s.erase(0, s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
  return s;
}

/************************************************************/
//
//       ID :
// Descrive : Copia un file
//     Date :
//   Author : Andrea Venuti
//
/************************************************************/
// START function(yoZUM )
void shell::cp(string __files__, string __filed__)
{
  String FILES; // nome del file da copiare
  String FILED; // nome del file di destinazione
  String ROW;   // riga letta del file
  string sLine;

  File targetFile;

  input row;

  boolean bTest;

  std::smatch result;

  // conversione dei parametri attuali
  FILES = s2S(cfgshell.__cur_path__ + __files__);
  FILED = s2S(cfgshell.__cur_path__ + __filed__);

  bTest = true;

  // Apro il file sorgente in modalità di lettura
  File sourceFile = SPIFFS.open(FILES, "r");
  if (!sourceFile)
  {
    Serial.println("Errore nella lettura del file sorgente");
    return;
  }

  // se tutto va bene apro il file destinazione in modalità di scrittura
  if (SPIFFS.exists(FILED))
  {
    regex r("(sRow;|S|n|N){1}");
    do
    {
      std::cout << "\n...il file di destinazione esiste...lo cancello ? "
                   "[snSN]....";
      std::cin >> row;
      sLine = row;
    } while (!regex_search(sLine, result, r));

    if (sLine == "sRow;" ||
        sLine == "S")
    { // si e' scelto di cancellare il file di destinazione
      shell::rm(__filed__);
      bTest = true;
    }
  }
  else
  {
    targetFile = SPIFFS.open(FILED, "w");
    if (!targetFile)
    {
      Serial.println("Errore nella scrittura del file destinazione");
      sourceFile.close();
      return;
    }
    else
    {
      // Leggo il contenuto del file sorgente e scrivo nel file destinazione
      while (sourceFile.available())
      {
        targetFile.write(sourceFile.read());
      }
    }
  }

  // Chiudo i file
  sourceFile.close();
  targetFile.close();

} // END function( yoZUM )

//************************************************************/
//
//       ID :
// Descrive : rimuove un file
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::rm(string __file__)
{
  String FILE;
  string sLine;
  input row;
  smatch result;

  FILE = s2S(__file__);

  if (!SPIFFS.begin(true))
  {
    std::cout << "\n\n\tAn Error has occurred while mounting SPIFFS\n\n";
  }
  else
  {
    if (!SPIFFS.exists(FILE))
    {
      std::cout << "\n\n\tAn File not exist : " << __file__ << "\n\n";
    }
    else
    {
      regex r("(s|S|n|N){1}");

      do
      {
        row.clear();
        std::cout << "\n\n\tCancello il file ? [sN]....";
        std::cin >> row;
        sLine = row;
      } while (!regex_search(sLine, result, r));

      if (sLine == "s" || sLine == "S")
      { // si e' scelto di cancellare il file di destinazione
        if (SPIFFS.remove(FILE))
        {
          std::cout << "\n\n\tFile cancellato....";
        }
        else
        {
          std::cout << "\n\n\tImpossibile rimuovere il file....";
        }
      }
    }
  }

} // END function( AN4KM )

//************************************************************/
//
//       ID :
// Descrive : formatta il FS
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
// START function(AN4KM )
boolean shell::format()
{
  boolean bResult;
  string sLine;
  input row;
  smatch result;

  bResult = false;

  regex r("(sRow;|S|n|N){1}");

  do
  {
    std::cout << "\n\n\tFormatto il FS ? [sN]....";
    std::cin >> row;
    sLine = row;
  } while (!regex_search(sLine, result, r));

  if (sLine == "sRow;" ||
      sLine == "S")
  { // si e' scelto di cancellare il file di destinazione
    if (SPIFFS.format())
    {
      std::cout << "Formattazione eseguita correttamente..."
                << "\n";
      bResult = true;
    }
    else
    {
      bResult = false;
    }

    return bResult;
  }

  return false;
} // end format()

//************************************************************/
//
//       ID :
// Descrive : rinomina un file
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
// START function(6IyDN )
boolean shell::mv(string __cmd__)
{
  //-------------------|--------------------------------
  String FILES, FILED;
  String ROW;
  //--------------------|--------------------------------
  string __rows__;
  string __temp__;
  string sRow, sS, sD;
  string sTemp, sPrec, sSuc, sReg;
  string asSWITCH[8][4];
  //--------------------|--------------------------------
  char *cSwitch;
  //--------------------|--------------------------------
  int iIndex, iMax, iPrec;
  // int iSize = 4;
  //--------------------|--------------------------------
  smatch result, result0, result1, result2;
  //--------------------|--------------------------------
  File FD, FS;
  //--------------------|--------------------------------
  vector<string> aOutput;

  regex r("(-(s|d))");
  sPrec = "";
  sSuc = "";
  sRow = "";
  iIndex = 0;
  iPrec = 0;

  try
  {
    for (sregex_iterator it =
             sregex_iterator(__cmd__.begin(), __cmd__.end(), r);
         it != sregex_iterator(); it++)
    {
      result = *it;

      if (sRow.find(result.str(0)) != std::string::npos)
      {
        std::cout << "\n\n\t"
                  << "ATTENZIONE switch ripetuti....";
        return "";
      }
      else
      {
        if (sPrec != "")
        {
          sReg = "(" + sPrec + ")(.+)(" + result.str(0) + ")";
          sReg = trim(sReg);

          asSWITCH[iIndex - 1][0] = trim(sPrec);
          asSWITCH[iIndex - 1][1] = trim(sReg);
          asSWITCH[iIndex - 1][2] =
              trim(__cmd__.substr(iPrec + 2, result.position(0) - iPrec - 3));
        }
      }

      sPrec = result.str(0);
      iPrec = result.position(0);
      sRow += result.str(0);
      iIndex++;
    }

    sReg = "(" + result.str(0) + ")(.+)";
    iIndex--;
    asSWITCH[iIndex][0] = trim(sPrec);
    asSWITCH[iIndex][1] = trim(sReg);
    asSWITCH[iIndex][2] =
        trim(__cmd__.substr(iPrec + 2, __cmd__.length() - iPrec));
  }

  catch (const std::regex_error &r)
  {
    std::cout << "\n\n\tregex_error caught: " << r.what() << '\n';
    if (r.code() == std::regex_constants::error_brack)
    {
      std::cout << "\tThe code was error_brack\n";
    }
  }

  iMax = iIndex;

  // separazione delle regole di estrazione
  sTemp = "";
  sS = "";
  sD = "";

  for (iIndex = 0; iIndex <= iMax; iIndex++)
  {
    sTemp = trim(asSWITCH[iIndex][0]);
    cSwitch = &sTemp[1];

    // std::cout << "\n\t\t" << asSWITCH[iIndex][0] << "(" << cSwitch <<
    // ")\t" << asSWITCH[iIndex][1] << "\t" << asSWITCH[iIndex][2];

    switch (*cSwitch)
    {
    case 's':
    { // nome del file
      sS = asSWITCH[iIndex][2];
      FILES = s2S(cfgshell.__cur_path__ + trim(sS));
      if (!SPIFFS.exists(FILES))
      {
        std::cout << "\n\n\t\tIl file di origine non esiste.....\n\n";
        return false;
      }

      break;
    }
    case 'd':
    { // nome della variabile
      sD = asSWITCH[iIndex][2];
      FILED = s2S(cfgshell.__cur_path__ + trim(sD));
      if (SPIFFS.exists(FILED))
      {
        shell::rm(cfgshell.__cur_path__ + trim(sD));
      }
      break;
    }
    }
  }

  if (!SPIFFS.begin(true))
  {
    std::cout << "\n\n\tAn Error has occurred while mounting SPIFFS\n\n";
    return false;
  }
  else
  {
    if (!SPIFFS.rename(FILES, FILED))
    {
      return false;
    }
    else
    {
      return true;
    }
  }

} // END function( 6IyDN )

//************************************************************/
//
//       ID :
// Descrive : elenca i file di una directory
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::ls(string __cmd__)
{
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  File pFileOut;
  File pFileIn;

  string FO;
  string sOutput;

  boolean bVideo = true;

  /* controlla se vi sono redirect */

  if ((shell::readFlag(">")).size() > 0)
  {
    /* apertura del file in modo sovrascrittura */
    String FO = shell::s2S(getPath() + shell::readFlag(">"));
    pFileOut = SPIFFS.open(FO, "w");
    bVideo = false;
  }
  else if (shell::readFlag(">>").size() > 0)
  {
    /* Apertura del file in modo append */
    String FO = shell::s2S(getPath() + shell::readFlag(">>"));
    pFileOut = SPIFFS.open(FO, "a");
    bVideo = false;
  }

  std::cout << "\n";

  while (file)
  {

    sOutput = S2s(file.name());

    if (shell::readPos("-d"))
      sOutput = sOutput + " " + to_string(file.size());

    if (shell::readPos("-t"))
      sOutput = sOutput + " " + to_string(file.getLastWrite());

    if (bVideo)
    {
      std::cout << sOutput << "\n";
    }
    else
    {
      pFileOut.println(s2S(sOutput));
    }

    file = root.openNextFile();
  }
} // END function ls()

//************************************************************/
//
// Descrive : costruisce la matrice aReg
//   Retrun : void
//   Author : Andrea Venuti
//
//   1 : string __sValue__  = riga di comando di grep
//************************************************************/
void shell::flag(string __sReg__, string __sValue__)
{
  smatch result;

  string sRow;

  boolean bTest;
  boolean bTestExt;
  boolean bTestInt;

  int iStart = 0;    // valore di partenza dell'indice della matrice quando si
                     // inseriscono i valori del flag
  int iIndexFlg = 0; // indice dell'array asFlg

  /* switch consentiti */
  regex r(__sReg__);

  /* ciclo di separazione degli switch */
  for (sregex_iterator it =
           sregex_iterator(__sValue__.begin(), __sValue__.end(), r);
       it != sregex_iterator(); it++)
  {
    result = *it;
    shell::setFlag(result.str(0), iIndexFlg);
    shell::setPos(__sValue__.find(result.str(0)) + result.str(0).size(),
                  iIndexFlg);
    shell::setPos(__sValue__.size(), iIndexFlg + 1);
    iIndexFlg++;
  }

  for (int iIndexVal = iStart; iIndexVal <= iIndexFlg; iIndexVal++)
  {

    __PRTVAR__("__sValue__", __sValue__)
    __PRTVAR__("shell::readPos(iIndexVal)", shell::readPos(iIndexVal))
    __PRTVAR__("shell::readPos(iIndexVal + 1)", shell::readPos(iIndexVal + 1))
    __PRTVAR__("shell::readFlag(iIndexVal + 1)).size()", (shell::readFlag(iIndexVal + 1)).size())

    shell::setValue(
        shell::trim(__sValue__.substr(
            shell::readPos(iIndexVal),
            shell::readPos(iIndexVal + 1) - shell::readPos(iIndexVal) -
                (shell::readFlag(iIndexVal + 1)).size())),
        iIndexVal);
  }
}

//************************************************************/
//
// Descrive : esegue il comado grep con output a video
//   Retrun : void
//   Author : Andrea Venuti
//************************************************************/
void shell::grep()
{
  String FILES;
  string sROW;
  string sFiles;
  string sResarch;
  string sString;
  string sVar;
  string sOutput;
  smatch result;
  smatch resVar;

  /* Imposta di default l'output a video */
  boolean bVideo = true;

  File pFILE;
  File pFileOut;

  /* controlla se vi e' un redirect e che di che tipo e' */
  if ((shell::readFlag(">")).size() > 0)
  {
    /* apertura del file in modo sovrascrittura */
    String FO = shell::s2S(shell::getPath() + shell::readFlag(">"));
    __PRTVAR__("shell::readFlag(\">\")", shell::getPath() + shell::readFlag(">"))

    if (SPIFFS.exists(FO))
    {
      SPIFFS.remove(FO);
    }

    pFileOut = SPIFFS.open(FO, "w");

    bVideo = false;
  }
  else if (shell::readFlag(">>").size() > 0)
  {
    /* Apertura del file in modo append */
    String FO = shell::s2S(getPath() + shell::readFlag(">>"));
    pFileOut = SPIFFS.open(FO, "a");
    bVideo = false;
  }

  /* imposta la regex */
  regex r(shell::readFlag("--reg"));

  sFiles = shell::readFlag("<<"); // estrae il nome del file di input per la funzioneda una stringa

  if (sFiles.length() > 0)
  { /* se l'input del comando e' un file allora inizia
       il test dell'esistenza e il ciclo di lettura */
    FILES = shell::s2S(getPath() + sFiles);
    __PRTVAR__("FILES", getPath() + sFiles)

    if (SPIFFS.exists(FILES))
    { /* se il file dove cercare esiste inizia il
         ciclo di lettura */
      pFILE = SPIFFS.open(FILES);

      while (pFILE.available())
      { /* il ciclo di lettura continua fino alla fine
           del file */
        sResarch = shell::S2s(pFILE.readStringUntil('\n'));

        if (regex_search(sResarch, result,
                         r))
        { /* se la regola puo' essere applicata alla riga
             la estrae */
          cfgshell.__temp__ = result.str(0);
          if (bVideo)
          { /* se l'output e' il video stampa la riga letta */
            std::cout << "\n"
                      << cfgshell.__temp__;
          }
          else
          { /* altrimenti lo reindirizza nel file */
            pFileOut.println(shell::s2S(cfgshell.__temp__));
            __PRTVAR__("cfgshell.__temp__", cfgshell.__temp__)
          }
        }
      }
    }
  }
}

//************************************************************/
//
//       ID :
// Describe : edita un file
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
// START function(6IyDN )
void shell::edlin(string __file__)
{
  String FILE;  // nome del file da editare/visualizzare
  String ROW;   // riga letta del file
  string sLine; // input string
  string sTemp;

  char *cTemp;

  vector<string> buffer; // buffer che rappresenta il contenuto del file
  vector<string> temp;   // copia del buffer

  boolean bTest = true;
  boolean bTest1 = true;

  int iIndex; // posizione corrente nel buffer
  int iAux;   // indice ausiliario
  int iStart; // riga iniziale del buffer visualizzato
  int iEnd;   // riga finale del buffer visualizzato

  input iLine; // oggetto di tipo input

  smatch result;

  // convert name
  FILE = s2S(cfgshell.__cur_path__ + __file__);

  if (__file__.length() > 0 && !SPIFFS.exists(FILE))
  {
    regex r("(n|N|y|Y)");

    do
    {
      std::cout << "\n\n\tDo you want create file [yN] :";
      iLine.clear();
      std::cin >> iLine;
      sLine = iLine;
    } while (!regex_search(sLine, result, r));

    if (sLine == "y" || sLine == "Y")
    {
      File file = SPIFFS.open(FILE, "w");
      file.print(" \n");
      file.close();
    }
  }

  if (SPIFFS.exists(FILE))
  {
    File file = SPIFFS.open(FILE);

    // costruisce il buffer
    while (file.available())
    {
      buffer.push_back(S2s(file.readStringUntil('\n')));
    }

    file.close();

    // editing del
    // buffer----------------------------------------------------------------------------------------------------------------------------

    // START while( lFzAh )
    iIndex = 0;
    regex r("(p|n|q|s|([0-9]{1,}[i|d|e]))");

    while (bTest)
    {
      do
      { // ciclo di visualizzazione del buffer
        iAux = 0;
        iStart = iIndex;

        std::cout << "\033[2J\033[1;1H"; // pulisce lo schermo
        std::cout << "\n\n=============================================="
                     "=================================================="
                     "=======================================";

        while (iIndex < buffer.size() && iAux < 20)
        {
          cout << "\n"
               << iIndex << "\t" << buffer[iIndex];
          iIndex++;
          iAux++;
        }

        std::cout << "\n================================================"
                     "=================================================="
                     "======================================";
        std::cout << "\n[p] Pg UP - [n] Pg Down - [num. Linea]( [e] Edit | "
                     "[i] Ins | [d] Del. ) - [s] Save - [q] Exit : ";

        iLine.clear();
        std::cin >> iLine;
        sLine = iLine;

        if (regex_search(sLine, result, r))
        {
          bTest1 = false;
        }
        else
        {
          iIndex = iStart;
        }
      } while (bTest1);

      iEnd = iIndex;

      iLine.clear();
      cTemp = &sLine[sLine.length() - 1];

      switch (*cTemp)
      {
      case 'p':
      { // torna indietro di una pagina
        iIndex = iStart - 20;
        if (iIndex < 0)
        {
          iIndex = 0;
        }

        break;
      }

      case 'n':
      { // avanza di una pagina
        iIndex = iEnd;
        if (iIndex > buffer.size())
        {
          iIndex = iStart;
        }

        break;
      }

      case 'e':
      { // modifica una linea del buffer
        sTemp = sLine.substr(0, sLine.length() - 1);
        iAux = stoi(sTemp);
        std::cout << "\n\n"
                  << buffer[iAux] << "\n";
        iLine.clear();
        std::cin >> iLine;
        sTemp = iLine;
        buffer[iAux] = sTemp;

        iIndex = iStart;
        break;
      }

      case 'd':
      { // cancella una linea del buffer
        iIndex = iStart;
        sTemp = sLine.substr(0, sLine.length() - 1);
        iAux = abs(stoi(sTemp));
        if (iAux <= buffer.size())
        {
          buffer.erase(buffer.begin() + iAux);
        }
        break;
      }

      case 'i':
      { // aggiunge una line a del buffer

        iIndex = iStart;
        sTemp = sLine.substr(0, sLine.length() - 1);
        iAux = abs(stoi(sTemp));
        if (iAux <= buffer.size() + 1)
        {
          std::cout << "\n...Inserisci una nuova linea \n";
          std::cin >> iLine;
          sTemp = iLine;
          buffer.insert(buffer.begin() + iAux, sTemp);
        }
        break;
      }

      case 's':
      { // salva le modifiche

        iIndex = iStart;
        iIndex = 0;

        File file = SPIFFS.open(FILE, "w");
        if (file)
        {
          std::cout << "\n\n\tSaving";

          while (iIndex < buffer.size())
          {
            if (file.print(s2S(buffer[iIndex]) + "\n"))
            {
              std::cout << ".";
            }
            else
            {
              std::cout << "!";
            }

            iIndex++;
          }
        }
        else
        {
          std::cout << "\n\n\tImpossibile salvare...";
        }

        std::cout << "\n\n\tPremi INVIO per continuare...";
        std::cin >> iLine;
        file.close();
        iIndex = iStart;

        break;
      }

      case 'q':
      { // esce dall'editor
        bTest = false;
        break;
      }

      } // end switch cTemp
    }
  }

} // END function( WyJfK )

//************************************************************/
//
//       ID :
// Descrive : stampa la matrice aReg
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::printFlag()
{
  for (int iAux = 0; iAux < 6; iAux++)
  {
    std::cout << "\n\naReg[" << iAux << "].sFlag=" << shell::readFlag(iAux);
    std::cout << "\naReg[" << iAux << "].iPos=" << shell::readPos(iAux);
    std::cout << "\naReg[" << iAux << "].sValue=" << shell::readValue(iAux);
  }
}

//************************************************************/
//
//       ID :
// Descrive : ritorna il valore del flag passato come parametro attuale
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
string shell::readFlag(string __sFlag__)
{
  for (int iAux = 0; iAux < 6; iAux++)
  {
    // shell::grepReadFlag(iAux)
    if (aReg[iAux].sFlag == __sFlag__)
    {
      return shell::readValue(iAux);
    }
  }
  return "";
}

//************************************************************/
//
//       ID :
// Descrive : ritorna il valore della posizione del flag passato come parametro attuale
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
int shell::readPos(string __sFlag__)
{
  for (int iAux = 0; iAux < 6; iAux++)
  {
    if (aReg[iAux].sFlag == __sFlag__)
    {
      return shell::readPos(iAux);
    }
  }
  return 0;
}

/************************************************************/
//
//       ID :
// Descrive : estrae una regexp da una stringa
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
string shell::extract(string __reg__, string __cmd__)
{

  smatch result;

  regex s(__reg__);
  if (regex_search(__cmd__, result, s))
  {
    return result.str(0);
  }
  else
  {
    return "";
  }
}

//************************************************************/
//
//       ID :
// Descrive : imposta una variabile di memoria
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::set(string __cmd__)
{
  string sName;
  string sValue;
  string sReg;
  string sTemp;

  string sFile;
  String sFILE;
  File pFILE;

  char cMode; // a old var + new value
              // b new value + old var
              // d delete var
              // m modify var

  boolean bFoundVar = true;
  boolean bFound = false;
  boolean bCont = true;

  smatch result;

  int iIndex = 0;
  int iPos = 0;
  int iStart = 0;
  int iEnd = 0;

  __PRTDBG__

  // espressione del redirect
  sReg = "(<<)( )*([\\w.-_]{0,}){1,}";
  sFile = shell::trim(shell::extract(sReg, __cmd__));
  if (sFile.size() > 0)
    sFile = shell::trim(sFile.substr(2, sFile.size()));

  __PRTVAR__("sFile", sFile)

  if (sFile.size() > 0)
  {
    sFILE = shell::s2S(shell::getPath() + shell::trim(sFile));
    if (SPIFFS.exists(sFILE))
    {
      pFILE = SPIFFS.open(sFILE);
      __cmd__ = shell::S2s(pFILE.readStringUntil('\n'));
    }
  }

  do
  {

    /* combinazione del tipo $var;valore */
    sReg = "[$][\\w.-_]+(;)[\\w.@\\[\\]-_\\+\?]*";
    sTemp = shell::trim(shell::extract(sReg, __cmd__));
    if (sTemp.size() > 0 && bFoundVar)
    {
      // estrazione del nome
      sReg = "[$][\\w.-_]+(;)";
      sName = shell::extract(sReg, sTemp);
      sName = shell::trim(sName.substr(1, sName.size() - 2));

      // estrazione valore variabile
      sReg = "(;)[\\w.@\\[\\]-_\\+\?]*";
      sValue = shell::extract(sReg, sTemp);
      sValue = shell::trim(sValue.substr(1, sValue.size()));

      __PRTVAR__("sTemp", sTemp)
      __PRTVAR__("sName", sName)
      __PRTVAR__("sValue", sValue)

      if (sValue.size() > 0)
      {
        cMode = 'a';
      }
      else
      {
        cMode = 'q';
      }

      bFoundVar = false;
    }

    /* combinazione del tipo valore;$var */
    sReg = "( )+[\\w.@\\[\\]-_\\+\?]+(;)[$][\\w.-_]+";
    sTemp = shell::trim(shell::extract(sReg, __cmd__));
    if (sTemp.size() > 0 && bFoundVar)
    {

      // estrazione del nome
      sReg = "[$][\\w.-_]+";
      sName = shell::extract(sReg, sTemp);
      sName = shell::trim(sName.substr(1, sName.size()));

      // estrazione valore variabile
      sReg = "[\\w.@\\[\\]-_\\+\?]+(;)";
      sValue = shell::extract(sReg, sTemp);
      sValue = shell::trim(sValue.substr(0, sValue.size() - 1));

      bFoundVar = false;

      __PRTVAR__("sTemp", sTemp)
      __PRTVAR__("sName", sName)
      __PRTVAR__("sValue", sValue)

      if (sValue.size() > 0)
      {
        cMode = 'b';
      }
      else
      {
        cMode = 'q';
      }
    }

    /* combinazione del tipo var=valore */
    sReg = "[\\w.]+( )*(=)( )*[\\w.@\\[\\]-_\\+\\?]*";
    sTemp = shell::trim(shell::extract(sReg, __cmd__));
    if (sTemp.size() > 0 && bFoundVar)
    {
      bFoundVar = false;

      // estrazione del nome
      sReg = "[\\w.]+( )*(=)";
      sName = shell::extract(sReg, sTemp);
      sName = shell::trim(sName.substr(0, sName.size() - 1));

      // estrazione valore variabile
      sReg = "(=)( )*[\\w.@\\[\\]-_\\+\\?]*";
      sValue = shell::extract(sReg, __cmd__);
      sValue = shell::trim(sValue.substr(1, sValue.size()));

      __PRTVAR__("sTemp", sTemp)
      __PRTVAR__("sName", sName)
      __PRTVAR__("sValue", sValue)

      if (sValue.size() == 0)
      {
        cMode = 'd';
      }
    }

    /* aggiornamento del vettore delle variabili */

    // ricerca della varibile
    bFound = false;
    auto it = aVar.begin();
    iIndex = 0;
    while (it != aVar.end() && !bFound)
    {
      if (it->name == sName)
      {
        bFound = true;
      }
      else
      {
        it++;
      }
    }

    __PRTVAR__("cMode", cMode)

    // aggiornamento del vettore
    switch (cMode)
    {
    case 'a': /* old value + new value */
      it->value = it->value + sValue;
      break;

    case 'b': /* new value + old value */
      it->value = sValue + it->value;
      break;

    case 'd':
      aVar.erase(aVar.begin() + std::distance(aVar.begin(), it));
      break;

    default:
      if (it != aVar.end())
      {
        it->value = sValue;
      }
      else
      {
        if (sValue.size() > 0)
        {
          aVar.push_back({sName, sValue});
        }
      }

      break;
    }

    /* Esplorazione del file */
    if (pFILE.available())
    {
      __cmd__ = shell::S2s(pFILE.readStringUntil('\n'));
      bCont = true;
      bFoundVar = true;
    }
    else
    {
      bCont = false;
    }
  } while (bCont);
}

//************************************************************/
//
//       ID :
// Descrive : visualizza il valore di una variabile
//     Date :
//   Author : Andrea Venuti
//   return :
//
//************************************************************/
void shell::echo(string __cmd__)
{

  String sFILE;
  String sMode = "w";
  File pFILE;

  string sOut;
  string sVar, sFile;

  smatch result;

  __PRTDBG__
  regex q("((\\$)[\\w.-_]+)");
  if (regex_search(__cmd__, result, q))
  {
    __PRTDBG__
    sVar = result.str(0);
    sVar = shell::trim(sVar.substr(1, sVar.size()));
  }
  else
  {
    sVar = "";
  }

  __PRTDBG__
  regex w("((>>)( )*[\\w.-_!~]+)");
  __PRTDBG__
  if (regex_search(__cmd__, result, w))
  {
    __PRTDBG__
    sFile = result.str(0);
    sFile = shell::trim(cfgshell.__cur_path__ + shell::trim(sFile.substr(2, sFile.size())));
    sFILE = shell::s2S(sFile);
    sMode = "a+";
    __PRTVAR__("sFile", sFile)
  }
  else
  {
    regex w("((>)( )*[\\w.-_!~]+)");
    if (regex_search(__cmd__, result, w))
    {
      sFile = result.str(0);
      sFile = shell::trim(cfgshell.__cur_path__ + shell::trim(sFile.substr(1, sFile.size())));
      sFILE = shell::s2S(sFile);
      sMode = "w";
      __PRTVAR__("sFile", sFile)
    }
    else
    {
      sFile = "";
    }
  }

  int iVar = sVar.size();
  int iFile = sFile.size();
  int iPos = 0;

  __PRTDBG__

  // apertura del file di output con la modalita' prevista dall'operatore di reidirizzamento.
  if (iFile > 0)
  {
    if (!(pFILE = SPIFFS.open(sFILE.c_str(), sMode.c_str())))
    {
      return;
    }
  }

  // esplorazione del vettore delle variabili
  for (auto &innerVector : aVar)
  {
    if (iVar > 0)
    {
      if (innerVector.name == sVar)
      {

        sOut = innerVector.name + "=" + innerVector.value;
        if (pFILE)
        {
          pFILE.println(s2S(sOut));
        }
        else
        {
          std::cout << "\n"
                    << sOut;
        }
      }
    }
    else
    {
      sOut = innerVector.name + "=" + innerVector.value;
      if (pFILE)
      {
        pFILE.println(s2S(sOut));
      }
      else
      {
        std::cout << "\n"
                  << sOut;
      }
    }
  }

  pFILE.close();
}

//************************************************************/
//
//       ID :
// Descrive : Avvia una sessione di shell
//     Date :
//   Author : Andrea Venuti
//   return : true se il comando e' esterno
//
//************************************************************/
boolean shell::start()
{
  smatch result;
  boolean bCMD = false;
  boolean bInput = true;
  String FILE;
  File pFile;
  string __files__, __filed__;
  string random_string;
  string in;
  string out;
  string sReg;

  std::vector<std::string> __command__; // crea il vettore comandi

  int iPosix = 0;
  int iPipe = 0;
  int iStart = 0;
  int iEnd = 0;
  int iAux = 0;

  srand(time(NULL));

  /* regole di estrazione dei comandi */
  regex r("^([ ]{0,}(ls|cp|grep|set|echo|format|edlin|rm|mv|exit|cls|ifup|help|cat|lora))");
  /* status :         |  |   |    |  |   |  |   |     |   |   |    |   |    | */
  /*                 OK  |   |    | OK   |  |   |     |   |   |    OK  |    */
  /*                    OK   |    |     OK  |   |     |   |   OK       | */
  /*                        OK    |        OK  OK     |   OK           | */
  /*                              OK                  OK               OK  */

  /* regole di estrazione del nome del file */
  regex f("([~\\w.]+)$");
  regex fo("(-fo )([(\\w)-._]{0,}){1,}");
  regex fd("(-fd )([(\\w)-._]{0,}){1,}");

  shell::setMode(true);  // imposta la modalita' di echo dei caratteri
  cfgshell.__cmd__ = ""; // pulisce la variabile che indica l'ultimo comando

  while (cfgshell.__mode__)
  {
    do
    {
      if (bInput)
      {
      ReturnToInput:
        __command__.clear();
        cfgshell.__inpRow__.clear();
        std::cout << "\n...digita un comando valido oppure help per aiuto...\n";
        std::cout << cfgshell.__cur_path__;
        std::cin >> cfgshell.__inpRow__;
        cfgshell.__inpRow__.trim();
        cfgshell.__row__ = cfgshell.__inpRow__;

        iPipe = 0;
        iStart = 0;
        iEnd = cfgshell.__row__.size();
        in = "";

        while ((iEnd = cfgshell.__row__.find("|", iStart + 1)) !=
               string::npos)
        {
          bInput = false;

          out = "out" + to_string(iPipe) + ".tmp";

          __command__.push_back(
              shell::trim(cfgshell.__row__.substr(iStart, iEnd - iStart) +
                          " > " + out + in));
          iStart = iEnd + 1;

          iPipe++;
          in = " << " + out;
        }

        /* Inserimento ultimo comando della catena */
        __command__.push_back(shell::trim(
            cfgshell.__row__.substr(iStart, iEnd - iStart) + in));

        /* estrazione del primo comando */
        cfgshell.__row__ = __command__[0];
        iAux = 1;
      }
      else
      {
        /* estrazione dei comandi successivi */
        if (iAux > iPipe)
        {
          bInput = true;
          goto ReturnToInput;
        }
        else
        {
          cfgshell.__row__ = __command__[iAux];
          iAux++;
        }
      }

    } while (!regex_search(cfgshell.__row__, result, r));

    __PRTVAR__("command", cfgshell.__row__)

    cfgshell.__cmd__ = result.str(0);

    // ciclo di selezione del comando
    if (result.str(0) == "ls")
    { // esecuzione del comando ls ( comando interno )
      shell::flag("((-(d|t))|>>|>)", cfgshell.__row__);
      shell::ls(cfgshell.__cmd__);
      cfgshell.__row__ = "";
      shell::cleanFlag();
    }

    else if (result.str(0) == "format")
    { // esecuzione del comando format ( comando interno )
      shell::format();
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "mv")
    { // muove un file
      shell::mv(cfgshell.__cmd__);
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "echo")
    { // imposta una variabile
      shell::echo(cfgshell.__row__);
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "set")
    { // imposta una variabile

      __PRTDBG__;
      shell::set(cfgshell.__row__);
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "rm")
    { // muove un file
      regex_search(cfgshell.__row__, result, f);
      shell::rm(shell::getPath() + result.str(0));
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "cls")
    { // cancella lo schermo
      std::cout << "\033[2J\033[1;1H";
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "edlin")
    { // esecuzione del comando edlin ( comando interno )
      regex_search(cfgshell.__row__, result, f);
      shell::edlin(result.str(0));
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "cp")
    { // esecuzione del comando ( comando interno )

      regex_search(cfgshell.__row__, result, fo);
      __files__ = shell::getPath() + result.str(0);
      FILE = shell::s2S(shell::getPath() + result.str(0));
      std::cout << "\n";
      pFile = SPIFFS.open(FILE);
      while (pFile.available())
      {
        std::cout << shell::S2s(pFile.readStringUntil('\n')) << "\n";
      }
      pFile.close();
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "cat")
    { // esecuzione del comando ( comando interno )

      regex_search(cfgshell.__row__, result, f);
      FILE = shell::s2S(shell::getPath() + result.str(0));
      std::cout << "\n";
      pFile = SPIFFS.open(FILE);
      while (pFile.available())
      {
        std::cout << shell::S2s(pFile.readStringUntil('\n')) << "\n";
      }
      pFile.close();
      cfgshell.__row__ = "";
    }

    else if (result.str(0) == "grep")
    { // esecuzione del comando ( comando interno )
      shell::flag("((--(reg|num))|<<|>>|>)", cfgshell.__row__);
      shell::grep();
      cfgshell.__row__ = "";
      shell::cleanFlag();
    }

    else if (result.str(0) == "lora")
    { // esecuzione del comando ( comando interno )
      shell::flag("((--(mode|tx))|<<)", cfgshell.__row__);
      shell::lora();
      cfgshell.__row__ = "";
      shell::cleanFlag();
    }

    else if (result.str(0) == "exit")
    { // esecuzione del comando ( comando interno )
      shell::setMode(false);
      (aReg[0]).iPos = 0;
    }
  }

  return false;
}

//************************************************************/
//
//       ID :
// Descrive : pulisce la matrice dei flag
//     Date :
//   Author : Andrea Venuti
//   return :
//
//************************************************************/
void shell::cleanFlag()
{
  for (int iAux = 0; iAux < 6; iAux++)
  {
    shell::setFlag("", iAux);
    shell::setPos(0, iAux);
    shell::setValue("", iAux);
  }
}

//************************************************************/
//
//       ID :
// Descrive : gestisce una scheda lora
//     Date :
//   Author : Andrea Venuti
//   return :
//
//************************************************************/
void shell::lora()
{

  int iM0; // impostazione del pin M0 al valore di default
  int iM1; // impostazione del pin M1 al valore di default

  File pFileIN;
  String FO;

  string sRow;
  string sMode;
  string sTX;

  smatch result;

  if ((shell::readFlag("<<")).size() > 0)
  {
    /* apertura del file in modo sovrascrittura */
    FO = shell::s2S(shell::getPath() + shell::readFlag("<<"));
    __PRTVAR__("<<", shell::getPath() + shell::readFlag("<<"))

    if (SPIFFS.exists(FO))
    {
      pFileIN = SPIFFS.open(FO);
      regex m("(mode)( ){0,}(=)( ){0,}[psave|program|txrx|wakeup]");
      regex t("(tx)( ){0,}(=)( ){0,}[\\w ]+");

      while (pFileIN.available())
      {
        sRow = S2s(pFileIN.readStringUntil('\n'));

        regex_search(sRow, result, m);
        if ((result.str(0)).size())
        {
          sMode = result.str(0);
        }

        regex_search(sRow, result, t);
        if ((result.str(0)).size())
        {
          sTX = result.str(0);
        }
      }
    }
  }
  else
  {
    sMode = shell::readFlag("--mode");
    sTX = shell::readFlag("--tx");
    __PRTDBG__
  }

  __PRTVAR__("mode", sMode)
  __PRTVAR__("tx", sTX)

  if (sMode.size() > 0)
  {
    /* ricerca il modo all'interno dell'array di LoRa mode */
    boolean bFound = false;
    auto it = LoRaMode.begin();
    int iIndex = 0;
    while (it != LoRaMode.end() && !bFound)
    {
      if (it->sName == sMode)
      {

        bFound = true;
        myLoRa.iM0 = it->iM0;
        myLoRa.iM1 = it->iM1;

        /* impostazione dei PIN di M0 e M1 */

        if (myLoRa.iM0 == 1)
          digitalWrite(GPIO_NUM_2, HIGH);
        else
          digitalWrite(GPIO_NUM_2, LOW);
        
        if (myLoRa.iM1 == 1)
          digitalWrite(GPIO_NUM_0 , HIGH);
        else
          digitalWrite(GPIO_NUM_0, LOW);
      
      }
      else
      {
        it++;
      }
    }
  }

  __PRTVAR__("myLoRa.M0", myLoRa.iM0)
  __PRTVAR__("myLoRa.M1", myLoRa.iM1)

  if (sTX.size() > 0)
  {
    __PRTVAR__("...invio messaggio...", "")
  }
}
