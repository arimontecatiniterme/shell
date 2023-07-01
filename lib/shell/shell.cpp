/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Definisce i metodi per la classe shell che modella l'omonimo ambiente da utilizzare su porta seriale
 *
 */

/*
 * include le librerie personalizzate
 */
#include "shell.h" /* libreia che contiene le definizioni per i metodi implementati in questo file */
#include "input.h" /* libreria per la gestione dell'input su seriale */

/*
 * definisce le costanti della classe
 */
#define __MACHINE__ ARDUINO
#define DBG 1

/*
 * Macro che stampa le informazioni su file, linea e metodo da dove viene richiamata quando il metodo della classe ritorna true.
 * Il meotodo ritorna true se la variabile debug=1
 */
#define __PRTDBG__                                                                               \
  if (shell::dbg())                                                                              \
  {                                                                                              \
    std::cout << "\n..." << __FILE__ << " - " << __LINE__ << " - " << __FUNCTION__ << std::endl; \
  }

/*
 * Macro che stampa le informazioni su file, linea e metodo nonche il valore delle variabili (x,y) da dove viene richiamata quando
 * il metodo della classe ritorna true. Il meotodo ritorna true se la variabile debug=1
 */
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
  cfgshell.__btf__ = "";

  // pulisce il vettore delle variabili
  aVar.clear();
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
boolean shell::start(string __cmd__)
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
  regex r("^([ ]{0,}(ls|cp|grep|set|echo|format|edlin|rm|mv|exit|cls|ifup|help|cat|lora|bt))");
  /*                  |  |   |    |  |   |  |   |     |   |   |    |   |   |    |   |    |  */
  /*                 OK  |   |    | OK   |  |   |     |   |   |    OK  |   |    OK  |    OK */
  /*                    OK   |    |     OK  |   |     |   |   OK       |   OK       OK      */
  /*                        OK    |        OK  OK     |   OK           |                    */
  /*                              OK                  OK               OK                   */

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

        if (__cmd__.size() == 0)
        {
          std::cin >> cfgshell.__inpRow__;
          cfgshell.__inpRow__.trim();
          cfgshell.__row__ = cfgshell.__inpRow__;
        }
        else
        {
          cfgshell.__row__ = __cmd__;

          /*
           * quando la shell viene avviata in modo trasparente e vi e' un unico comando da eseguire
           * si disabilita l'ambiente di shell di modo che alla fine dell'esecuzione del comando il metodo viene
           * abbandonato.
           */
          cfgshell.__mode__ = false;
        }

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

          /*
           * quando la shell viene avviata in modo trasparente e la catena della pipe
           * e' esaurita allora si renede necessario terminare l'ambiente di shell.
           */
          if (__cmd__.size() > 0)
            cfgshell.__mode__ = false;

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
      __PRTDBG__;
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
      shell::flag("((--(set|rconf|mode|username))|<<|>>|>)", cfgshell.__row__);
      shell::lora();
      cfgshell.__row__ = "";
      shell::cleanFlag();
    }

    else if (result.str(0) == "bt")
    { // esecuzione del comando ( comando interno )
      shell::flag("((--(set|send|listening|rconf)))", cfgshell.__row__);
      shell::bt();
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

  File pFileIN;
  String sFO, sFI;

  string sRow;
  string sMode;
  string sAUX;
  string sTX;

  smatch result;

  __PRTDBG__

  if ((shell::readFlag("<<")).size() > 0)
  {

    __PRTVAR__("file", shell::cfgshell.__cur_path__ + shell::readFlag("<<"))
    sFI = s2S(shell::cfgshell.__cur_path__ + shell::readFlag("<<"));

    if (SPIFFS.exists(sFI))
    {
      __PRTDBG__

      pFileIN = SPIFFS.open(sFI);

      __PRTDBG__

      while (pFileIN.available())
      {
        sRow = S2s(pFileIN.readStringUntil('\n'));
        __PRTVAR__("row file", sRow)
        shell::LoRaParseConf(sRow);
        __PRTDBG__
      }

      pFileIN.close();
    }
  }
  else if ((shell::readFlag("--set")).size() > 0)
  {
    __PRTDBG__("flag", shell::readFlag("--set"));
    shell::LoRaParseConf(shell::readFlag("--set"));
  }

  /*
   * configurazione della scheda
   */
  if ((shell::readFlag("--rconf")).size() > 0)
  {
    __PRTDBG__("flag", shell::readFlag("--rconf"));
    shell::LoRaReadConf();
  }

  /*
   * Imposta il nome utente di un messaggio LoRa
   */
  if ((shell::readFlag("--username")).size() > 0)
  {
    shell::set(" username=" + shell::readFlag("--username"));
  }
}

/*
 * Legge la configurazione della scheda LoRa
 */
void shell::LoRaReadConf()
{

  String FO;
  String sMODE;
  File pFILE;

  Serial2.setPins(shell::myPIN["TX"], shell::myPIN["RX"]); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
  LoRa_E220 e220ttl(&Serial2, shell::myPIN["AUX"], shell::myPIN["M0"], shell::myPIN["M1"]);

  e220ttl.begin();

  ResponseStructContainer c;
  c = e220ttl.getConfiguration();

  // It's important get configuration pointer before all other operation
  Configuration configuration = *(Configuration *)c.data;

  ResponseStructContainer cMi;
  cMi = e220ttl.getModuleInformation();
  // It's important get information pointer before all other operation
  ModuleInformation mi = *(ModuleInformation *)cMi.data;

  if ((shell::readFlag(">>")).size() > 0)
  {
    __PRTDBG__
    FO = shell::s2S(shell::getPath() + shell::readFlag(">>"));
    pFILE = SPIFFS.open(FO.c_str(), "a+");
  }
  else if ((shell::readFlag(">")).size() > 0)
  {
    __PRTDBG__
    FO = shell::s2S(shell::getPath() + shell::readFlag(">"));
    pFILE = SPIFFS.open(FO.c_str(), "w");
  }

  __PRTVAR__("pFILE", pFILE)

  if (pFILE)
  {
    __PRTDBG__

    pFILE.println("");
    pFILE.println(c.status.getResponseDescription());
    pFILE.println(c.status.code);
    pFILE.println("----------------------------------------");
    pFILE.print(F("HEAD : "));
    pFILE.print(configuration.COMMAND, DEC);
    pFILE.print(" ");
    pFILE.println(configuration.STARTING_ADDRESS, DEC);
    pFILE.println(" ");
    pFILE.println(configuration.LENGHT, HEX);
    pFILE.println(F(" "));
    pFILE.print(F("AddH : "));
    pFILE.println(configuration.ADDH, DEC);
    pFILE.print(F("AddL : "));
    pFILE.println(configuration.ADDL, DEC);
    pFILE.println(F(" "));
    pFILE.print(F("Chan : "));
    pFILE.print(configuration.CHAN, DEC);
    pFILE.print(" -> ");
    pFILE.println(configuration.getChannelDescription());
    pFILE.println(F(" "));
    pFILE.print(F("SpeedParityBit     : "));
    pFILE.print(configuration.SPED.uartParity, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.SPED.getUARTParityDescription());
    pFILE.print(F("SpeedUARTDatte     : "));
    pFILE.print(configuration.SPED.uartBaudRate, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.SPED.getUARTBaudRateDescription());
    pFILE.print(F("SpeedAirDataRate   : "));
    pFILE.print(configuration.SPED.airDataRate, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.SPED.getAirDataRateDescription());
    pFILE.println(F(" "));
    pFILE.print(F("OptionSubPacketSett: "));
    pFILE.print(configuration.OPTION.subPacketSetting, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.OPTION.getSubPacketSetting());
    pFILE.print(F("OptionTranPower    : "));
    pFILE.print(configuration.OPTION.transmissionPower, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.OPTION.getTransmissionPowerDescription());
    pFILE.print(F("OptionRSSIAmbientNo: "));
    pFILE.print(configuration.OPTION.RSSIAmbientNoise, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
    pFILE.println(F(" "));
    pFILE.print(F("TransModeWORPeriod : "));
    pFILE.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
    pFILE.print(F("TransModeEnableLBT : "));
    pFILE.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
    pFILE.print(F("TransModeEnableRSSI: "));
    pFILE.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
    pFILE.print(F("TransModeFixedTrans: "));
    pFILE.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);
    pFILE.print(" -> ");
    pFILE.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());
    pFILE.println("----------------------------------------");
    pFILE.print(F("HEAD: "));
    pFILE.print(mi.COMMAND, DEC);
    pFILE.print(" ");
    pFILE.print(mi.STARTING_ADDRESS, DEC);
    pFILE.print(" ");
    pFILE.println(mi.LENGHT, DEC);
    pFILE.print(F("Model no.: "));
    pFILE.println(mi.model, DEC);
    pFILE.print(F("Version  : "));
    pFILE.println(mi.version, DEC);
    pFILE.print(F("Features : "));
    pFILE.println(mi.features, DEC);
    pFILE.println("----------------------------------------");

    pFILE.close();
  }
  else
  {
    __PRTDBG__

    Serial.println();
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);
    Serial.println("----------------------------------------");
    Serial.print(F("HEAD : "));
    Serial.print(configuration.COMMAND, DEC);
    Serial.print(" ");
    Serial.print(configuration.STARTING_ADDRESS, DEC);
    Serial.print(" ");
    Serial.println(configuration.LENGHT, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));
    Serial.println(configuration.ADDH, DEC);
    Serial.print(F("AddL : "));
    Serial.println(configuration.ADDL, DEC);
    Serial.println(F(" "));
    Serial.print(F("Chan : "));
    Serial.print(configuration.CHAN, DEC);
    Serial.print(" -> ");
    Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));
    Serial.print(configuration.SPED.uartParity, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte     : "));
    Serial.print(configuration.SPED.uartBaudRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getUARTBaudRateDescription());
    Serial.print(F("SpeedAirDataRate   : "));
    Serial.print(configuration.SPED.airDataRate, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.SPED.getAirDataRateDescription());
    Serial.println(F(" "));
    Serial.print(F("OptionSubPacketSett: "));
    Serial.print(configuration.OPTION.subPacketSetting, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getSubPacketSetting());
    Serial.print(F("OptionTranPower    : "));
    Serial.print(configuration.OPTION.transmissionPower, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getTransmissionPowerDescription());
    Serial.print(F("OptionRSSIAmbientNo: "));
    Serial.print(configuration.OPTION.RSSIAmbientNoise, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.OPTION.getRSSIAmbientNoiseEnable());
    Serial.println(F(" "));
    Serial.print(F("TransModeWORPeriod : "));
    Serial.print(configuration.TRANSMISSION_MODE.WORPeriod, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.TRANSMISSION_MODE.getWORPeriodByParamsDescription());
    Serial.print(F("TransModeEnableLBT : "));
    Serial.print(configuration.TRANSMISSION_MODE.enableLBT, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.TRANSMISSION_MODE.getLBTEnableByteDescription());
    Serial.print(F("TransModeEnableRSSI: "));
    Serial.print(configuration.TRANSMISSION_MODE.enableRSSI, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.TRANSMISSION_MODE.getRSSIEnableByteDescription());
    Serial.print(F("TransModeFixedTrans: "));
    Serial.print(configuration.TRANSMISSION_MODE.fixedTransmission, BIN);
    Serial.print(" -> ");
    Serial.println(configuration.TRANSMISSION_MODE.getFixedTransmissionDescription());
    Serial.println("----------------------------------------");
    Serial.print(F("HEAD: "));
    Serial.print(mi.COMMAND, DEC);
    Serial.print(" ");
    Serial.print(mi.STARTING_ADDRESS, DEC);
    Serial.print(" ");
    Serial.println(mi.LENGHT, DEC);
    Serial.print(F("Model no.: "));
    Serial.println(mi.model, DEC);
    Serial.print(F("Version  : "));
    Serial.println(mi.version, DEC);
    Serial.print(F("Features : "));
    Serial.println(mi.features, DEC);
    Serial.println("----------------------------------------");
  }
}

/*
 * void LoraSetConf()  imposta una configurazione per la board LoRa
 *
 * Parametri ammessi :
 *
 *
 */
void shell::LoRaParseConf(string __cmd__)
{

  int iPOS;
  string sValue;
  string sName;

  __PRTVAR__("__cmd__", __cmd__)

  // pattern di ricerca
  regex patternSET("((addl|addh|ch|uartbr|uartp|airdata|packet|rnoise|tpow|re|ftx|lbt|wor|save)=[\\w]*)");

  // Oggetti iterator per iterare sulle corrispondenze
  std::sregex_iterator iteratore(__cmd__.begin(), __cmd__.end(), patternSET);
  std::sregex_iterator fineIteratore;

  // Iterare sulle corrispondenze
  while (iteratore != fineIteratore)
  {
    std::smatch corrispondenza = *iteratore;
    std::string parola = corrispondenza.str();

    iPOS = parola.find("=", 0);
    sName = parola.substr(0, iPOS);
    sValue = parola.substr(iPOS + 1, parola.length());
    shell::LoRaSetConf(sName, sValue);

    __PRTVAR__("sName", sName)
    __PRTVAR__("sValue", sValue)

    ++iteratore;
  }
}

/*
 * Imposta la configurazione della board LoRa
 */
void shell::LoRaSetConf(string sVar, string sVal)
{

  /*
   * Definisce la mappa dei valori della velocita' della UART
   */
  std::map<std::string, int> uartb;

  uartb["UART_BPS_1200"] = UART_BPS_1200;
  uartb["UART_BPS_2400"] = UART_BPS_2400;
  uartb["UART_BPS_4800"] = UART_BPS_4800;
  uartb["UART_BPS_9600"] = UART_BPS_9600;
  uartb["UART_BPS_19200"] = UART_BPS_19200;
  uartb["UART_BPS_38400"] = UART_BPS_38400;
  uartb["UART_BPS_57600"] = UART_BPS_57600;
  uartb["UART_BPS_115200"] = UART_BPS_115200;

  /*
   * Definisce la mappa della parita' della porta UART
   */
  std::map<std::string, int> uartp;

  uartp["MODE_00_8N1"] = MODE_00_8N1;
  uartp["MODE_01_8O1"] = MODE_01_8O1;
  uartp["MODE_10_8E1"] = MODE_10_8E1;
  uartp["MODE_11_8N1"] = MODE_11_8N1;

  /*
   * definisce la mappa delle opzioni della vecolita' di trasmissione
   */

  std::map<std::string, int> air;

  air["AIR_DATA_RATE_000_24"] = AIR_DATA_RATE_000_24;
  air["AIR_DATA_RATE_001_24"] = AIR_DATA_RATE_001_24;
  air["AIR_DATA_RATE_010_24"] = AIR_DATA_RATE_010_24;
  air["AIR_DATA_RATE_011_48"] = AIR_DATA_RATE_011_48;
  air["AIR_DATA_RATE_100_96"] = AIR_DATA_RATE_100_96;
  air["AIR_DATA_RATE_101_192"] = AIR_DATA_RATE_101_192;
  air["AIR_DATA_RATE_110_384"] = AIR_DATA_RATE_110_384;
  air["AIR_DATA_RATE_111_625"] = AIR_DATA_RATE_111_625;

  /*
   * Imposta la dimensione del pacchetto trasmesso
   */
  std::map<std::string, int> packet;

  packet["SPS_200_00"] = SPS_200_00;
  packet["SPS_128_01"] = SPS_128_01;
  packet["SPS_064_10"] = SPS_064_10;
  packet["SPS_032_11"] = SPS_032_11;

  /*
   * imposta la gestione del rumore di fondo
   */
  std::map<std::string, int> rssi;

  rssi["RSSI_AMBIENT_NOISE_ENABLED"] = RSSI_AMBIENT_NOISE_ENABLED;
  rssi["RSSI_AMBIENT_NOISE_DISABLED"] = RSSI_AMBIENT_NOISE_DISABLED;

  /*
   * Impost la potenza di trasmissione
   */
  std::map<std::string, int> ptx;

  ptx["POWER_22"] = POWER_22;
  ptx["POWER_17"] = POWER_17;
  ptx["POWER_13"] = POWER_13;
  ptx["POWER_10"] = POWER_10;

  /*
   * Imposta il modo di funzionamento della scheda
   */
  std::map<std::string, int> mode;

  mode["FT_FIXED_TRANSMISSION"] = FT_FIXED_TRANSMISSION;
  mode["FT_TRANSPARENT_TRANSMISSION"] = FT_TRANSPARENT_TRANSMISSION;

  /*
   * Monitoraggio dei dati in trasmissione
   */
  std::map<std::string, int> mtx;

  mtx["LBT_ENABLED"] = LBT_ENABLED;
  mtx["LBT_DISABLED"] = LBT_DISABLED;

  /*
   * Impostazione della modalita wor
   */
  std::map<std::string, int> wor;

  // wor["WAKE_UP_500"] = WAKE_UP_500;
  // wor["WAKE_UP_1000"] = WAKE_UP_1000;
  // wor["WAKE_UP_1500"] = WAKE_UP_1500;
  // wor["WAKE_UP_2000"] = WAKE_UP_2000;
  // wor["WAKE_UP_2500"] = WAKE_UP_2500;
  // wor["WAKE_UP_3000"] = WAKE_UP_3000;
  // wor["WAKE_UP_3500"] = WAKE_UP_3500;
  // wor["WAKE_UP_4000"] = WAKE_UP_4000;

  /*
   * Bit di parita'
   */
  std::map<std::string, int> re;
  re["RSSI_ENABLED"] = RSSI_ENABLED;
  re["RSSI_DISABLED"] = RSSI_DISABLED;

  /*
   * Salva la configurazione
   */
  std::map<std::string, int> save;
  save["YES"] = 1;
  save["NO"] = 0;

  __PRTDBG__

  Serial2.setPins(shell::myPIN["TX"], shell::myPIN["RX"]); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX

  __PRTDBG__

  LoRa_E220 e220ttl(&Serial2, shell::myPIN["AUX"], shell::myPIN["M0"], shell::myPIN["M1"]);

  __PRTDBG__

  e220ttl.begin();

  __PRTDBG__

  ResponseStructContainer c;
  c = e220ttl.getConfiguration();
  // It's important get configuration pointer before all other operation
  Configuration config = *(Configuration *)c.data;

  Serial.println(c.status.getResponseDescription());
  Serial.println(c.status.code);

  if (sVar == "addl")
  {
    config.ADDL = stoi(sVal);
  }
  else if (sVar == "addh")
  {
    config.ADDH = stoi(sVal);
  }
  else if (sVar == "ch")
  {
    config.CHAN = stoi(sVal);
  }
  else if (sVar == "uartb")
  {
    config.SPED.uartBaudRate = uartb[sVar];
  }
  else if (sVar == "uartp")
  {
    config.SPED.uartParity = uartp[sVal];
  }
  else if (sVar == "airdata")
  {
    config.SPED.airDataRate = air[sVal];
  }
  else if (sVar == "paket")
  {
    config.OPTION.subPacketSetting = packet[sVal];
  }
  else if (sVar == "rnoise")
  {
    config.OPTION.RSSIAmbientNoise = rssi[sVal];
  }
  else if (sVar == "tpow")
  {
    config.OPTION.transmissionPower = ptx[sVal];
  }
  else if (sVar == "re")
  {
  }
  else if (sVar == "ftx")
  {
    config.TRANSMISSION_MODE.fixedTransmission = mode[sVal];
  }
  else if (sVar == "lbt")
  {
    config.TRANSMISSION_MODE.enableLBT = mtx[sVal];
  }
  else if (sVar == "wor")
  {
    config.TRANSMISSION_MODE.WORPeriod = wor[sVal];
  }

  // Set configuration changed and set to not hold the configuration
  if (save[sVal])
  {
    ResponseStatus rs = e220ttl.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    Serial.println(rs.getResponseDescription());
    Serial.println(rs.code);
  }
  else
  {
    ResponseStatus rs = e220ttl.setConfiguration(config, WRITE_CFG_PWR_DWN_LOSE);
    Serial.println(rs.getResponseDescription());
    Serial.println(rs.code);
  }
}

/*
 * void bt()  gestione del bt
 *
 * Parametri ammessi :
 *
 *
 */
void shell::bt()
{

  char cCH;
  string sMode;
  string sFile;
  File pFile;

  /*
   * Imposta la scheda bluetooth
   */
  if ((shell::readFlag("--set")).size() > 0)
  {
    // shell::BTSerial.begin(9600);
    shell::BTSerial.begin(s2S(shell::readFlag("--set")));
  }

  /*
   * Invia un messaggio sulla seriale bluetooth
   */
  if ((shell::readFlag("--send")).size() > 0)
  {
    shell::BTSerial.println(s2S(shell::readFlag("--send")).c_str());
  }

  /*
   * Legge i dati ricevuti
   */
  __PRTVAR__("file", shell::readFlag("--listening"))

  if ((shell::readFlag("--listening")).size() > 0)
  {
    __PRTDBG__

    smatch result;
    regex fo("((>>)( )*[\\w.-_!~]+)");

    /*
     * controlla se l'output deve essere accodato a file
     */
    if (regex_search(sFile, result, fo))
    {
      __PRTDBG__
      sMode = "+a";
      regex file("(([\\w.-_!~]+)$)");
      string sData = result.str(0);
      __PRTVAR__("sData", sData)
      regex_search(sData, result, file);
      sFile = result.str(0);
    }
    else
    {
      /*
       * Controlla se l'output deve sovrascrivere il contenuto di un file
       */
      regex fo("((>)( )*[\\w.-_!~]+)");
      if (regex_search(sFile, result, fo))
      {
        __PRTDBG__
        regex file("(([\\w.-_!~]+)$)");
        string sData = result.str(0);
        regex_search(sData, result, file);
        sMode = "w";
        sFile = result.str(0);
      }
    }
    __PRTVAR__("file.size", sFile.size())

    if (sFile.size() > 0)
    {
      String sFILE = s2S(shell::cfgshell.__cur_path__ + sFile);
      pFile = SPIFFS.open(sFILE, sMode.c_str());
    }


    if (shell::BTSerial.available())
    {
      
      std::cout << "\n";

      while (shell::BTSerial.available())
      {
        String messaggio = shell::BTSerial.readStringUntil('\n');
        if (pFile)
        {
          pFile.println(messaggio);
        }
        else
        {
          std::cout << S2s(messaggio) << "\n";
        }
      }
    }
  }
}