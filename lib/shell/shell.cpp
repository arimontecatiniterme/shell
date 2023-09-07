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

#include "shell.h"

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
  if (stoi(mVar["debug"]))                                                                       \
  {                                                                                              \
    std::cout << "\n..." << __FILE__ << " - " << __FUNCTION__ << " - " << __LINE__ << std::endl; \
  }

/*
 * Macro che stampa le informazioni su file, linea e metodo nonche il valore delle variabili (x,y) da dove viene richiamata quando
 * il metodo della classe ritorna true. Il meotodo ritorna true se la variabile debug=1
 */
#define __PRTVAR__(x, y)                                                            \
  if (stoi(mVar["debug"]))                                                          \
  {                                                                                 \
    std::cout << "\n..." << __FILE__ << " - " << __FUNCTION__ << " - " << __LINE__; \
    std::cout << "\t" << x << "=\t[" << y << "]" << std::endl;                      \
  }

using namespace std;

/* costruttore di default */
shell::shell()
{

  /* inizializza la struttura che modella la board */
  myBoard.sNAME = "IQ5MT";
  myBoard.sMODEL = "esp32";
  myBoard.iTimeout = 3000;

  /* pulisce il vettore delle variabili e lo inizializza con i valori di default */
  mVar.clear();

  mVar["path"] = "/";                         // path attuale
  mVar["debug"] = "0";                        // 1 attiva il modo debug
  mVar["shell"] = mVar["path"] + "shell.ini"; // file di configurazione della shell
  mVar["echochar"] = "1";                     // 1 attiva l'echo dei caratteri
}

/* controlla se e' attivo il debug  */
int shell::dbg()
{
  int debug = 0;

  if (shell::mVar["debug"] == "1")
  {
    debug = 1;
  }

  return debug;
}

/* converte da std::string a String */
String shell::s2S(string str)
{
  int len = 0;
  int pos;
  char rc;
  String sSTR = "";

  len = str.length();
  char buffer[len + 1];

  strcpy(buffer, str.c_str());

  pos = 0;

  sSTR.clear();
  while (buffer[pos] != '\0')
  {
    rc = '\0';
    rc = buffer[pos];
    sSTR += rc;
    pos++;
  }

  return sSTR;

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
//     Date : 15.07.2023
//   Author : Andrea Venuti
//
/************************************************************/
boolean shell::cp(String __files__, String __filed__)
{
  boolean bTest = true;
  std::smatch result;

  __PRTVAR__("__files__", S2s(__files__))
  __PRTVAR__("__filed__", S2s(__filed__))

  if (SPIFFS.exists(__filed__))
  {

    regex rAnswer("(s|S|y|Y){1}"); // tutti i caratteri diversi da quelli indicati signoficano no nella risposta
    input inpRow;

    std::cout << "\n...il file di destinazione esiste! Lo cancello ? [sN]...";
    std::cin >> inpRow;
    string sRow = inpRow;
    if (regex_search(sRow, result, rAnswer))
      bTest = shell::rm(__filed__);
    else
      bTest = false;
  }

  if (bTest)
  {

    // imposta a false in quanto se qualcosa va storto
    // il metodo ritorna false
    bTest = false;

    // crea i puntatori dei file source e dest
    File pSourceFile = SPIFFS.open(__files__, "r");
    File pDestFile = SPIFFS.open(__filed__, "w");

    if (pSourceFile && pDestFile)
    {
      while (pSourceFile.available())
      {
        pDestFile.write(pSourceFile.read());
      }
      bTest = true;
    }
    else
    {
      std::cout << "\n...errore... ";
      std::cout << "il file sorgente non esite ";
      std::cout << "oppure quello di destinazione non puo' essere creato...\n";
    }

    // Chiudo i file
    pSourceFile.close();
    pDestFile.close();
  }

  return bTest;

} // END cp

//************************************************************/
//
// Descrive : cancella un file
//     Date : 15.07.2023
//   Author : Andrea Venuti
//
//************************************************************/
boolean shell::rm(String __file__)
{
  smatch result;
  boolean bResult = false;

  if (SPIFFS.exists(__file__))
  {
    regex rAnswer("(s|S|y|Y){1}");
    input inpAnswer;

    Serial.print("...Cancello il file " + __file__ + " ? [sN]...");
    std::cin >> inpAnswer;
    string sAnswer = inpAnswer;
    String sANSWER = shell::s2S(sAnswer);
    if (std::regex_search(sAnswer, result, rAnswer))
    {
      if (SPIFFS.remove(__file__))
        bResult = true;
    }
  }
  else
  {
    Serial.println("...il file non esiste " + __file__ + " oppure e' inaccessibile...");
  }

  return bResult;

} // END rm

//************************************************************/
//
// Descrive : formatta il FS
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
boolean shell::format()
{
  input inpAnswer;
  smatch result;
  string sAnswer;

  boolean bResult = false;

  std::regex rAnswer("(sRow;|y|Y|S|s){1}");

  do
  {
    std::cout << "\n\n\tFormatto il FS ? [sN]....";
    std::cin >> inpAnswer;
    sAnswer = inpAnswer;
  } while (!regex_search(sAnswer, result, rAnswer));

  if ((result.str(0)).length() > 0)
    if (SPIFFS.format())
    {
      std::cout << "..Formattazione eseguita correttamente..."
                << "\n";
      bResult = true;
    }
    else
    {
      bResult = false;
    }

  return bResult;
} // end format

//************************************************************/
//
// Descrive : sposta un file
//     Date : 16.07.20323
//   Author : Andrea Venuti
//
//************************************************************/
boolean shell::mv(String __files__, String __filed__)
{

  boolean bAnswer = false;
  boolean bResult = false;

  /*
   * TEST sull'esistenza dei file

          |__files__ | __filed__ |
          |    1     |     1     | chiedo conferma della cancellazione del __filed__ e copio i file
          |    0     |     1     | operazione fallita
          |    0     |     0     | operazione fallita
          |    1     |     0     | copio il file
   */

  if (SPIFFS.exists(__files__))
  {
    bAnswer = true;

    if (SPIFFS.exists(__filed__))
    {
      string sAnswer;
      input inpAnswer;
      smatch result;
      regex rAnswer("(y|Y|s|S){1}"); // si intende no alla cancellazione ogni caratteri diverso da quelli indicati
      std::cout << "\n...il file di destinazione esiste! Lo cancello ? [sN]...";
      std::cin >> inpAnswer;
      sAnswer = inpAnswer;
      if (std::regex_search(sAnswer, result, rAnswer))
        bAnswer = true;
      else
        bAnswer = false;
    }
  }
  else
  {
    std::cout << "\n...il files " + __files__ + " non esiste...\n";
  }

  if (bAnswer)
    if (SPIFFS.exists(__filed__))
      // cancella il file di destinazione
      shell::rm(__filed__);

  // copia il file
  if (shell::cp(__files__, __filed__))
  {
    bResult = true;
    shell::rm(__files__);
  }
  return bResult;

} // END mv()

//************************************************************/
//
// Descrive : elenca i file della directory passata come
//            parametro e stampa a video
//     Date : 16.07.2023
//   Author : Andrea Venuti
//
//************************************************************/
void shell::ls(String __dir__)
{

  __PRTVAR__("__dir__", S2s(__dir__))

  File root = SPIFFS.open(__dir__);
  File file = root.openNextFile();

  Serial.println("");

  while (file)
  {
    __PRTDBG__
    Serial.print(file.name());
    Serial.print("  ");
    Serial.println(file.size());
    file = root.openNextFile();
  }

  Serial.println("");

} // END ls(string)

//************************************************************/
//
// Descrive : elenca i file della directory passata come
//            parametro e stampa su file
// Parameter : ls(__dir__,__file__,__mode__)
//     Date : 16.07.2023
//   Author : Andrea Venuti
//
//************************************************************/
void shell::ls(String __dir__, String __file__, String __mode__)
{

  File pROOT = SPIFFS.open(__dir__);
  File pFILE_OUT = SPIFFS.open(__file__, __mode__.c_str());

  Serial.println("");

  if (pFILE_OUT)
  {
    File pFILE = pROOT.openNextFile();
    while (pFILE)
    {
      pFILE_OUT.print(pFILE.name());
      pFILE_OUT.print("  ");
      pFILE_OUT.println(pFILE.size());
      pFILE = pROOT.openNextFile();
    }

    pFILE_OUT.close();
  }
  else
  {
    Serial.println("...impossibile stampare sul file " + __file__);
  }

  Serial.println();

} // END ls(string, string)

//************************************************************/
//
// Descrive : applica una regexp al contenuto di un file con stampa a video
//   Retrun : void
//   Author : Andrea Venuti
//************************************************************/
void shell::grep(string sReg, String sFILE)
{

  __PRTVAR__("sReg", sReg)
  __PRTVAR__("sFILE", S2s(sFILE))

  smatch result;
  std::regex regVal(sReg);

  if (SPIFFS.exists(sFILE))
  {
    // se il file esiste lo apre
    File pFILE = SPIFFS.open(sFILE);
    if (pFILE)
    {
      // se l'apertura va a buon fine si applica la regexp a tutte le righe

      __PRTDBG__

      string sRow;
      while (pFILE.available())
      {
        sRow = S2s(pFILE.readStringUntil('\n'));

        __PRTVAR__("sRow", sRow)

        if (regex_search(sRow, result, regVal))
          std::cout << "\n"
                    << result.str(0);
      }
      pFILE.close();
      std::cout << "\n";
    }
  }

} // end grep(std::string, String)

//************************************************************/
//
// Descrive : applica una regexp al contenuto di un file con stampa su file
//   Retrun : void
//   Author : Andrea Venuti
//************************************************************/
void shell::grep(std::string sReg, String sFILE_IN, String sFILE_OUT, String sMODE)
{
  smatch result;
  std::regex regVal(sReg);

  __PRTVAR__("sReg", sReg)
  __PRTVAR__("sFILE_IN", S2s(sFILE_IN))
  __PRTVAR__("sFILE_OUT", S2s(sFILE_OUT))
  __PRTVAR__("sMODE", S2s(sMODE))

  if (SPIFFS.exists(sFILE_IN))
  {

    __PRTDBG__

    // se il file esiste lo apre
    File pFILE_IN = SPIFFS.open(sFILE_IN);
    if (pFILE_IN)
    {
      __PRTDBG__

      // se l'apertura del file di input va a buon fine si apre di output
      File pFILE_OUT = SPIFFS.open(sFILE_OUT, sMODE.c_str());
      // se l'apertura va a buon fine si applica la regexp a tutte le ricghe
      while (pFILE_IN.available())
      {
        string sRow = S2s(pFILE_IN.readStringUntil('\n'));
        if (regex_search(sRow, result, regVal))
          pFILE_OUT.println(s2S(result.str(0)));
      }
      pFILE_IN.close();
      pFILE_OUT.close();
      std::cout << "\n";
    }
  }

} // end grep(std::string, String, String, String)

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

  __PRTVAR__("__cmd__", __cmd__)

  smatch result;
  File pFile;

  int iPos;
  string sVar;
  string sVal;
  string sFile;

  regex r("(<< ([\\w+\\.-@$&]*))");
  if (regex_search(__cmd__, result, r))
  {
    __PRTDBG__

    __cmd__ = result.str(0);

    __PRTVAR__("__cmd__", __cmd__)

    regex f("(([\\w+\\.-@$&]*)$)");
    if (regex_search(__cmd__, result, f))
      sFile = result.str(0);

    __PRTVAR__("sFile", sFile)

    if (SPIFFS.exists(s2S(mVar["path"] + sFile)))
    {
      pFile = SPIFFS.open(s2S(mVar["path"] + sFile));
    }
  }

  regex vv("([\\w_\\-]+)( )*(=)( )*([\\w+\\.\\-@&_/ :;\\[\\]=\\(\\)\\|\\#\\.\\$\\{\\}\\?]*)");

  do
  {
    if (pFile)
    {
      __cmd__.clear();
      __cmd__ = S2s(pFile.readStringUntil('\n'));
      shell::trim(__cmd__);
      __PRTVAR__("riga letta", __cmd__)
    }

    if (regex_search(__cmd__, result, vv))
    {
      __cmd__ = result.str(0);

      __PRTVAR__("__cmd__", __cmd__)

      iPos = __cmd__.find("=", 0);

      __PRTVAR__("iPos", iPos)

      sVar = shell::trim(__cmd__.substr(0, iPos));
      sVal = shell::trim(__cmd__.substr(iPos + 1, __cmd__.length()));

      __PRTVAR__("sVar", sVar)
      __PRTVAR__("sVal", sVal)

      auto insert = mVar.insert(std::make_pair(sVar, sVal));

      if (!insert.second)
      {
        /* code */
        __PRTVAR__("sVal.size()", sVal.size())
        if (sVal.size() > 0)
        {
          __PRTDBG__
          mVar[sVar] = sVal;
        }
        else
        {
          __PRTDBG__

          mVar.erase(sVar);
        }
      }
    }
  } while (pFile.available());
} // end set

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
  String sMode = "a+";

  string sOut;
  string sVar, sFile;
  string sPattern = ">>";

  boolean bTest = true;
  boolean bAll = false;

  smatch result;
  File pFILE;

  int iAux = 0;

  while (bTest && iAux <= 1)
  {
    regex f("(" + sPattern + ")( )*([\\w+\\.-@&_]*)");

    __PRTVAR__("f", "(" + sPattern + ")( )*([\\w+\\.-@&_]*)")

    if (regex_search(__cmd__, result, f))
    {
      regex file("([\\w+\\.-@&_]*)$");
      if (regex_search(__cmd__, result, file))
      {
        __PRTVAR__("result.str(0)", result.str(0))

        sFILE = s2S(mVar["path"] + result.str(0));
        pFILE = SPIFFS.open(sFILE, sMode.c_str());
        bTest = false;
        bAll = true;
      }
    }
    sPattern = ">";
    sMode = "w";
    iAux++;
  }

  regex r("[$]([\\w+\\.-@&]*)");
  if (regex_search(__cmd__, result, r) && !bAll)
  {
    __PRTDBG__

    regex v("([\\w+\\.-@&]*)$");
    if (regex_search(__cmd__, result, v))
    {
      __PRTDBG__

      if (pFILE)
      {
        pFILE.println(s2S(result.str(0) + "=" + mVar[result.str(0)]));
      }
      else
      {
        std::cout << "\n"
                  << result.str(0) << "=" << mVar[result.str(0)] << "\n";
      }
    }
  }
  else
  {
    __PRTDBG__
    std::cout << "\n";
    for (const auto &pair : mVar)
    {

      if (pFILE)
      {
        pFILE.println(s2S(pair.first + "=" + pair.second));
      }
      else
      {
        std::cout << pair.first << "=" << pair.second << std::endl;
      }
    }
  }

} // end echo

//************************************************************/
//
// Descrive : avvia una sessione di shell in modo trasparente
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::start(string sRow)
{

  sRow = shell::trim(sRow);
  __PRTVAR__("sRow", sRow)

  boolean bPipe = false; // indica se la shell sta processando una catena di pipe
  string sCommand;       // comando digitato
  string sTemp;          // comando temporaneo
  string sFileIN;        // nome temporaneo del file di out di scambio di dati
  string sFileOUT;       // nome temporaneo del file di input di scambio di dati
  smatch result;
  int iStep = 0;
  int iMaxStep = 0;
  int iAux = 0;

  std::regex regCommand("^(( )*(ls|cp|grep|set|echo|format|edlin|rm|mv|exit|cls|ifup|help|cat|lora|bt))");
  std::regex delimiter("\\|");

  // rilevamento della catena di pipe
  // ls | grep --reg .... | set | echo
  std::sregex_token_iterator it(sRow.begin(), sRow.end(), delimiter, -1);
  std::sregex_token_iterator end;

  while ((iAux = sRow.find("|", iAux)) != std::string::npos)
  {
    __PRTDBG__
    iMaxStep++;
    iAux += 1;
  }

  __PRTVAR__("iMaxStep", iMaxStep)

  if (sRow.find("|") != std::string::npos)
  {
    bPipe = true;
  }
  else
  {

    bPipe = false;
  }

  iStep = 0;

  while (it != end)
  {
    sTemp = *it;
    ++it;

    // Esempio di PIPE
    // ls | grep | set | cat
    //
    // ls        > 1
    // grep << 1 > 2
    // set  << 2 > 3
    // cat  << 3

    __PRTVAR__("iStep", iStep)

    if (bPipe)
    {
      __PRTDBG__

      if (iStep == 0)
      {
        __PRTDBG__

        sFileOUT = std::to_string(iStep) + ".tmp";
        sTemp = sTemp + " > " + sFileOUT;
      }
      else
      {
        __PRTDBG__

        sFileIN = sFileOUT;
        sTemp = sTemp + " << " + sFileIN;
        sFileOUT = std::to_string(iStep) + ".tmp";
        sTemp = sTemp + " > " + sFileOUT;
      }
    }

    if (bPipe && (iStep == iMaxStep))
    {
      __PRTDBG__
      sTemp.replace(sTemp.find(" > " + sFileOUT), 3 + sFileOUT.length(), "");
    }

    __PRTVAR__("sTemp", sTemp)

    if (regex_search(sTemp, result, regCommand))
    {
      __PRTDBG__

      sCommand = result.str(0);
      __PRTVAR__("sCommand", sCommand)

      shell::exec(sCommand, sTemp);

      // if (bPipe && (iStep > 1))
      // SPIFFS.remove(s2S(mVar["path"] + sFileIN));
    }

    iStep++;
  }
}

//************************************************************/
//
// Descrive : avvia una sessione di shell in modo interattivo
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::start()
{

  input inpCommand; // oggetto di tipo input per l'inserimento dei comandi
  string sRow;      // intero comando di riga

  // ciclo di lettura dei comandi
  myBoard.bShell = true;
  while (myBoard.bShell)
  {
    std::cout << "\n...digita un comando valido oppure help per aiuto...\n";
    std::cout << mVar["path"];
    sRow.clear();
    inpCommand.clear();
    std::cin >> inpCommand;
    sRow = inpCommand;
    shell::start(sRow);
  }

} // END start

//************************************************************/
//
// Descrive : esegue un comando di shell
//     Date :
//   Author : Andrea Venuti
//
//************************************************************/
void shell::exec(string sCommand, string sRow)
{

  String sPTH;
  smatch result;

  // ╔╗      ╔╗
  // ║║      ║║
  // ║╚═╗╔══╗║║ ╔══╗
  // ║╔╗║║╔╗║║║ ║╔╗║
  // ║║║║║║═╣║╚╗║╚╝║
  // ╚╝╚╝╚══╝╚═╝║╔═╝
  //            ║║
  //            ╚╝
  if (sCommand == "help")
  {
    edlin myEdit("/help.txt");
    myEdit.edit();
    sRow.clear();
  }

  // ╔╗
  // ║║
  // ║║ ╔══╗
  // ║║ ║══╣
  // ║╚╗╠══║
  // ╚═╝╚══╝
  if (sCommand == "ls")
  {

    __PRTVAR__("sRow", sRow)

    regex regFile("(>>|>)( )*([(\\w)-\\._]{1,}){1,}");

    // controlla se il commando contiene opzioni
    if (regex_search(sRow, result, regFile))
    {

      // estrae la sottostringa corripondente all regFile
      sCommand = result.str(0);
      __PRTVAR__("sCommand", sCommand)

      // l'output e' verso un file
      String sFILE; // nome del file
      String sMODE; // modo di scrittura

      // stabilisce il modo di scrittura
      regex regFileAppend("(>>)");
      if (regex_search(sCommand, result, regFileAppend))
        sMODE = "a"; // modo append
      else
        sMODE = "w+"; // modo overwrite

      // estrae il nome del file di output
      regex regFileName("(([(\\w)-\\._]{0,}){1,})$");
      if (regex_search(sCommand, result, regFileName))
      {
        sFILE = s2S(result.str(0));
      }

      __PRTVAR__("sFILE", S2s(sFILE))
      __PRTVAR__("sMODE", S2s(sMODE))

      shell::ls(s2S(mVar["path"]), s2S(mVar["path"]) + sFILE, sMODE);
    } // end blocco che processa le opzioni
    else
    {
      // il comando non contiene opzioni
      __PRTVAR__("path", mVar["path"])
      shell::ls(s2S(mVar["path"]));
    } // end

    // esecuzione del comando ls ( comando interno )
    sRow.clear();
  }

  //  ╔═╗                 ╔╗
  //  ║╔╝                ╔╝╚╗
  // ╔╝╚╗╔══╗╔═╗╔╗╔╗╔══╗ ╚╗╔╝
  // ╚╗╔╝║╔╗║║╔╝║╚╝║╚ ╗║  ║║
  //  ║║ ║╚╝║║║ ║║║║║╚╝╚╗ ║╚╗
  //  ╚╝ ╚══╝╚╝ ╚╩╩╝╚═══╝ ╚═╝
  else if (sCommand == "format")
  { // esecuzione del comando format ( comando interno )
    shell::format();
    sRow.clear();
  }

  // ╔╗╔╗╔╗╔╗
  // ║╚╝║║╚╝║
  // ║║║║╚╗╔╝
  // ╚╩╩╝ ╚╝
  else if (sCommand == "mv")
  { // muove un file

    String sFILES;
    String sFILED;

    regex regMV("[^mv( )*](([(\\w)-\\._]{0,}){1,})( )*(([(\\w)-\\._]{0,}){1,})");
    sCommand.clear();
    if (regex_search(sRow, result, regMV))
    {
      sCommand = result.str(0);

      // regole di estrazione dei nomi dei file
      regex regFileS("^(([(\\w)-\\._]{0,}){1,})");
      regex regFileD("(([(\\w)-\\._]{0,}){1,})$");

      // estrazione del nome file origine
      if (regex_search(sCommand, result, regFileS))
        sFILES = s2S(result.str(0));

      // estrazione del nome file origine
      if (regex_search(sCommand, result, regFileD))
        sFILED = s2S(result.str(0));

      shell::mv(s2S(mVar["path"]) + sFILES, s2S(mVar["path"]) + sFILED);
    }

    sRow.clear();
  }

  // ╔═╗╔╗╔╗
  // ║╔╝║╚╝║
  // ║║ ║║║║
  // ╚╝ ╚╩╩╝
  else if (sCommand == "rm")
  { // muove un file
    String sFILE;

    regex regFile("(([(\\w)-\\._]{0,}){1,})$");

    if (regex_search(sRow, result, regFile))
      shell::rm(s2S(mVar["path"]) + s2S(result.str(0)));

    sRow.clear();
  }

  // ╔══╗╔══╗
  // ║╔═╝║╔╗║
  // ║╚═╗║╚╝║
  // ╚══╝║╔═╝
  //     ║║
  //     ╚╝
  else if (sCommand == "cp")
  {

    String sFILES;
    String sFILED;

    regex regMV("[^cp( )*](([(\\w)-\\._]{0,}){1,})( )*(([(\\w)-\\._]{0,}){1,})");
    sCommand.clear();
    if (regex_search(sRow, result, regMV))
    {
      sCommand = result.str(0);

      // regole di estrazione dei nomi dei file
      regex regFileS("^(([(\\w)-\\._]{0,}){1,})");
      regex regFileD("(([(\\w)-\\._]{0,}){1,})$");

      // estrazione del nome file origine
      if (regex_search(sCommand, result, regFileS))
        sFILES = s2S(result.str(0));

      // estrazione del nome file origine
      if (regex_search(sCommand, result, regFileD))
        sFILED = s2S(result.str(0));

      shell::cp(s2S(mVar["path"]) + sFILES, s2S(mVar["path"]) + sFILED);
      sRow.clear();
    }
  }

  //         ╔╗
  //         ║║
  // ╔══╗╔══╗║╚═╗╔══╗
  // ║╔╗║║╔═╝║╔╗║║╔╗║
  // ║║═╣║╚═╗║║║║║╚╝║
  // ╚══╝╚══╝╚╝╚╝╚══╝
  else if (sCommand == "echo")
  { // imposta una variabile
    __PRTDBG__;
    shell::echo(sRow);
    sRow.clear();
  }

  //          ╔╗
  //         ╔╝╚╗
  // ╔══╗╔══╗╚╗╔╝
  // ║══╣║╔╗║ ║║
  // ╠══║║║═╣ ║╚╗
  // ╚══╝╚══╝ ╚═╝
  else if (sCommand == "set")
  { // imposta una variabile
    __PRTDBG__;
    shell::set(sRow);
    sRow.clear();
  }

  //     ╔╗
  //     ║║
  // ╔══╗║║ ╔══╗
  // ║╔═╝║║ ║══╣
  // ║╚═╗║╚╗╠══║
  // ╚══╝╚═╝╚══╝
  else if (sCommand == "cls")
  { // cancella lo schermo
    std::cout << "\033[2J\033[1;1H";
    sRow.clear();
  }

  //       ╔╗╔╗
  //       ║║║║
  // ╔══╗╔═╝║║║ ╔╗╔══╗
  // ║╔╗║║╔╗║║║ ╠╣║╔╗║
  // ║║═╣║╚╝║║╚╗║║║║║║
  // ╚══╝╚══╝╚═╝╚╝╚╝╚╝
  else if (sCommand == "edlin")
  { // entra in modalita edit
    regex regFile("(([(\\w)-\\._]{0,}){1,})$");
    if (regex_search(sRow, result, regFile))
    {
      __PRTVAR__("file", result.str(0))
      edlin myEdit(s2S(mVar["path"]) + s2S(result.str(0)));
      myEdit.edit();
    }
    sRow.clear();
  }

  //           ╔╗
  //          ╔╝╚╗
  // ╔══╗╔══╗ ╚╗╔╝
  // ║╔═╝╚╗ ║  ║║
  // ║╚═╗║╚╝╚╗ ║╚╗
  // ╚══╝╚═══╝ ╚═╝
  else if (sCommand == "cat")
  { // esecuzione del comando ( comando interno )
    regex regFile("(([(\\w)-\\._]{0,}){1,})$");
    if (regex_search(sRow, result, regFile))
    {
      __PRTVAR__("file", result.str(0))
      if (SPIFFS.exists(s2S("/" + result.str(0))))
      {
        File pFILE = SPIFFS.open(s2S("/" + result.str(0)));

        Serial.println("");

        while (pFILE.available())
        {
          Serial.println(pFILE.readStringUntil('\n'));
        }
        pFILE.close();
      }
    }
    sRow.clear();
  }

  // ╔═══╗
  // ║╔═╗║
  // ║║ ╚╝╔═╗╔══╗╔══╗
  // ║║╔═╗║╔╝║╔╗║║╔╗║
  // ║╚╩═║║║ ║║═╣║╚╝║
  // ╚═══╝╚╝ ╚══╝║╔═╝
  //             ║║
  //             ╚╝
  else if (sCommand == "grep")
  { // esecuzione del comando ( comando interno )
    string sReg;
    string sFileIN;
    string sFileOUT;
    String sMODE;

    int iStart = 0;
    int iEndIN = 0;
    int iEndOUT = 0;
    int iEndMax = 0;
    int iEndMin = 0;
    int iEnd = sRow.length();

    std::regex regPattern("--reg\\s+(\\S+)|<<(\\s*\\S+)|>>(\\s*\\S+)|>(\\s*\\S+)");
    std::sregex_iterator it(sRow.begin(), sRow.end(), regPattern);
    std::sregex_iterator end;

    __PRTVAR__("sRow", sRow)

    while (it != end)
    {
      std::smatch match = *it;

      if ((*it)[1].matched)
      {
        iStart = match.position() + 5;
        __PRTVAR__("iStart", iStart)
      }
      if ((*it)[2].matched)
      {
        iEndIN = match.position();
        sFileIN = shell::trim((*it)[2].str());

        __PRTVAR__("sFileIN", sFileIN)
        __PRTVAR__("iEndIN", iEndIN)
      }
      if ((*it)[3].matched)
      {
        iEndOUT = match.position();
        sFileOUT = shell::trim((*it)[3].str());
        sMODE = "a+";

        __PRTVAR__("sFileOUT", sFileOUT)
        __PRTVAR__("iEndOUT", iEndOUT)
      }
      if ((*it)[4].matched)
      {
        iEndOUT = match.position();
        sFileOUT = shell::trim((*it)[4].str());
        sMODE = "w";

        __PRTVAR__("sFileOUT", sFileOUT)
        __PRTVAR__("iEndOUT", iEndOUT)
      }
      ++it;
    }

    __PRTDBG__

    /* ricerca del massimo e del minimo delle posizioni dei redirect */
    if (iEndIN < iEndOUT)
    {
      iEndMin = iEndIN;
      iEndMax = iEndOUT;
    }
    else
    {
      iEndMin = iEndOUT;
      iEndMax = iEndIN;
    }

    __PRTVAR__("iEndMax", iEndMax)
    __PRTVAR__("iEndMin", iEndMin)

    /* ricerca della posizione della reg rispetto ai redirect */
    if (iStart > iEndMin && iStart < iEndMax)
      iEnd = iEndMax;
    else if (iStart > iEndMax)
      iEnd = sRow.length();
    else if (iStart < iEndMin)
      iEnd = iEndMin;

    sReg = shell::trim(sRow.substr(iStart, iEnd - iStart));
    __PRTVAR__("sReg", sReg)

    if (sFileOUT.length() > 0)
    {
      __PRTDBG__
      shell::grep(sReg, s2S(mVar["path"] + sFileIN), s2S(mVar["path"] + sFileOUT), sMODE);
    }
    else
    {
      __PRTDBG__
      String sFILEIN = s2S(mVar["path"] + sFileIN);
      shell::grep(sReg, sFILEIN);
      __PRTDBG__
    }

    sRow.clear();
  }

  // ╔╗       ╔═══╗
  // ║║       ║╔═╗║
  // ║║   ╔══╗║╚═╝║╔══╗
  // ║║ ╔╗║╔╗║║╔╗╔╝╚ ╗║
  // ║╚═╝║║╚╝║║║║╚╗║╚╝╚╗
  // ╚═══╝╚══╝╚╝╚═╝╚═══╝
  else if (sCommand == "lora")
  {
    __PRTDBG__

#ifndef __MYLORA__
#define __MYLORA__
    static LoRa lr;
    lr.init();
#endif

    std::string sFlag;
    std::string sOpt;
    std::string sVar, sValue;

    boolean bManualInput = true;

    String sFILE, sMSG;
    File pFILE;

    input inpmsg;

    /*
     * --rconf : legge la configurazione della scheda
     * --set   : imposta i singoli parametri della scheda
     * --send  : invia un messaggio da console
     * --lconf : carica una configurazione da file
     */
    std::regex regFlag("(--(rconf|set|send|read)\\s+(.*?))");
    // std::regex regOpt("(addh|addl|ch|uartbr|uartp|airdr|packetftx|rnoise|tpow|re|ftx|lbt|wor|save)(=)(\\s+(\\S+))");

    /*
      In questa versione dell'espressione regolare (\w+)=(\w+(\.\w+)?),
      la parte (\w+(\.\w+)?) cattura il valore. La parte \w+ cattura una
      sequenza di caratteri alfanumerici, e (\.\w+)? indica che può esserci
      una parte opzionale con un punto seguito da una sequenza di caratteri
      alfanumerici. In questo modo, i valori che contengono il carattere punto
      saranno catturati correttamente.
    */
    std::regex regOpt("(\\w+)=(\\w+(\\.\\w+)?)");

    /*
       In questa versione dell 'espressione regolare, (\\>{1,2}) cattura
       l' operatore di ridirezione singola(>) o doppia(>>) e(\\w +\\.\\w +)
       cattura il nome del file.La parte \\> {1, 2} corrisponde a uno o due
       caratteri di maggiore >.
    */
    std::regex regFlin("(<<)\\s*(\\w+\\.\\w+)");
    std::regex regFlout("(\\>{1,2})\\s*(\\w+\\.\\w+)");

    // sRow = sRow + " --";
    __PRTVAR__("sRow", sRow)

    /* pulisce la variabile file */
    sFILE.clear();

    if (std::regex_search(sRow, result, regFlag))
    {

      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])

      /* aggiorna la variabile */
      sFlag = result[2];

      /* Imposta la configurazione della board LoRa*/
      if (sFlag == "set")
      {
        __PRTDBG__

        /* ciclo di estrazione delle opzioni del comando */
        std::sregex_iterator itOpt(sRow.begin(), sRow.end(), regOpt);
        std::sregex_iterator endOpt;

        while (itOpt != endOpt)
        {
          std::smatch matchOpt = *itOpt;

          __PRTVAR__("matchOpt[0]", matchOpt[0])
          __PRTVAR__("matchOpt[1]", matchOpt[1])
          __PRTVAR__("matchOpt[2]", matchOpt[2])

          /* impostazione della configurazione */
          sVar = matchOpt[1];
          sValue = matchOpt[2];
          shell::set("set " + sVar + "=" + sValue);
          lr.slConf(sValue, sVar);

          ++itOpt;
        }
      }
      /* end set board config */

      /* legge la configurazione della board */
      if (sFlag == "rconf")
      {
        __PRTDBG__
        std::cout << std::endl;
        lr.rlConf();
      }
      /* end read board config */

      /* legge il buffer della board */
      if (sFlag == "read")
      {
        __PRTDBG__
        std::cout << std::endl;
        lr.rlConf();
      }
      /* end read board buffer */
    }
    else if (std::regex_search(sRow, result, regFlout))
    {
      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])
    }
    else if (std::regex_search(sRow, result, regFlin))
    {
      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])

      sFILE = s2S(mVar["path"]);
      sFILE = sFILE + s2S(result[2]);
      bManualInput = false;

      if (SPIFFS.exists(sFILE))
      {
        pFILE=SPIFFS.open(sFILE);
      }
    }

    if (bManualInput)
    {
      /* digita il messaggio */
      inpmsg.clear();
      std::cout << std::endl
                << "Digita il messaggio da inviare : ";
      std::cin >> inpmsg;
      sMSG = inpmsg;
    }

    /* inizia la trasmissione */
    do
    {
      __PRTDBG__

      if (pFILE.available())
      {
        sMSG = pFILE.readStringUntil('\n');
        __PRTVAR__("sMSG", S2s(sMSG))
      }

      if (mVar["mode"] == "fixed")
      {
        __PRTDBG__
        lr.slMsg(stoi(mVar["addh"]), stoi(mVar["addl"]), stoi(mVar["ch"]), sMSG);
      }
      else if (mVar["mode"] == "simple")
      {
        __PRTDBG__
        lr.slMsg(sMSG, stoi(mVar["ch"]));
      }
      else if (mVar["mode"] == "struct")
      {
        __PRTDBG__
        lr.slMsg(sMSG, s2S(mVar["username"]), stoi(mVar["ch"]));
      }

    } while (pFILE.available());

    /* ------------------------------------------------ */

    sRow.clear();
  }

  // ╔╗   ╔╗
  // ║║  ╔╝╚╗
  // ║╚═╗╚╗╔╝
  // ║╔╗║ ║║
  // ║╚╝║ ║╚╗
  // ╚══╝ ╚═╝
  else if (sCommand == "bt") /* gestione del comando lora */
  {

    String sFileIN;
    String sFileOUT;
    String sMODE;
    String sMSG;

#ifndef __BTSERIAL__
#define __BTSERIAL__
    static bt btSerial;
#endif

    std::regex regFlag("--(name|send|read)\\s+(\\S+)");

    /*
       In questa versione dell 'espressione regolare, (\\>{1,2}) cattura
       l' operatore di ridirezione singola(>) o doppia(>>) e(\\w +\\.\\w +)
       cattura il nome del file.La parte \\> {1, 2} corrisponde a uno o due
       caratteri di maggiore >.
    */
    std::regex regRead("(\\<{1,2})\\s*(\\w+\\.\\w+)");
    std::regex regWrite("(\\>{1,2})\\s*(\\w+\\.\\w+)");

    __PRTVAR__("sRow", sRow)

    /* Estrae il flag e il suo valore */
    if (std::regex_search(sRow, result, regFlag))
    {
      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])

      if (result[1] == "name")
      {
        String sNAME = s2S(result[2]);
        btSerial.sBT(sNAME);
      }

      if (result[1] == "send")
      {
        __PRTDBG__

        /* digita il messaggio */
        input inpmsg;
        inpmsg.clear();
        std::cout << std::endl
                  << "Digita il messaggio da inviare : ";
        std::cin >> inpmsg;
        sMSG = inpmsg;

        btSerial.sBT(sMSG);
      }

      /* legge il buffer del BT */
      if (result[1] == "read")
      {
        __PRTDBG__
        btSerial.rBT();
      }
    }
    else if (std::regex_search(sRow, result, regRead))
    {
      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])
    }
    else if (std::regex_search(sRow, result, regWrite))
    {
      __PRTVAR__("result[0]", result[0])
      __PRTVAR__("result[1]", result[1])
      __PRTVAR__("result[2]", result[2])

      sFileOUT = s2S(result[2]);
      sFileOUT = s2S(mVar["path"].c_str()) + sFileOUT;

      if (result[1] == ">")
      {
        sMODE = "w";
      }
      else
      {
        sMODE = "a+";
      }

      btSerial.rBT(sFileOUT, sMODE);
    }

    __PRTDBG__

    sRow.clear();
  }

  // ╔═══╗╔═╗╔═╗╔══╗╔════╗
  // ║╔══╝╚╗╚╝╔╝╚╣╠╝║╔╗╔╗║
  // ║╚══╗ ╚╗╔╝  ║║ ╚╝║║╚╝
  // ║╔══╝ ╔╝╚╗  ║║   ║║
  // ║╚══╗╔╝╔╗╚╗╔╣╠╗ ╔╝╚╗
  // ╚═══╝╚═╝╚═╝╚══╝ ╚══╝
  else if (sCommand == "exit")
  { // esecuzione del comando ( comando interno )
    myBoard.bShell = false;
  }

} // end exec
