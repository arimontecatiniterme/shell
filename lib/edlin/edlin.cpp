/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Dichiara i metodi per la classe che modella l'omonimo ambiente da utilizzare su porta seriale
 *
 */

#include "edlin.h"

#define DBG 0
#define PAGE 20

/*
 * Macro che stampa le informazioni su file, linea e metodo da dove viene richiamata quando il metodo della classe ritorna true.
 * Il meotodo ritorna true se la variabile debug=1
 */
#define __PRTDBG__                                                                                   \
    if (DBG)                                                                                         \
    {                                                                                                \
        std::cout << "\n..." << __FILE__ << " - " << __LINE__ << " - " << __FUNCTION__ << std::endl; \
    }

/*
 * Macro che stampa le informazioni su file, linea e metodo nonche il valore delle variabili (x,y) da dove viene richiamata quando
 * il metodo della classe ritorna true. Il meotodo ritorna true se la variabile debug=1
 */
#define __PRTVAR__(x, y)                                                                \
    if (DBG)                                                                            \
    {                                                                                   \
        std::cout << "\n..." << __FILE__ << " - " << __LINE__ << " - " << __FUNCTION__; \
        std::cout << "\t" << x << "=\t[" << y << "]" << std::endl;                      \
    }

/* costruttore di default */
edlin::edlin(String sFILE)
{

    __PRTVAR__("sFILE", S2s(sFILE))

    // inizializza il file system
    SPIFFS.begin();

    // imposta il nome del file
    myFile.sFile = S2s(sFILE);

    /* apre il file da editare */
    if (SPIFFS.exists(sFILE))
    {

        myFile.pFILE = SPIFFS.open(sFILE);

        if (myFile.pFILE)
        {
            __PRTDBG__

            // se l'apertura ha funzionato allora si attiva la modalita di editing
            myFile.bEmode = true;

            /* carica il buffer del file */
            while (myFile.pFILE.available())
            {
                myFile.sBuffer.push_back(S2s(myFile.pFILE.readStringUntil('\n')));
            }

            /* imposta i parametri per la prima schermata */
            myFile.startView = 0;
        }
    }

    __PRTVAR__("myFile.sFile", myFile.sFile)
    __PRTVAR__("myFile.pFile", myFile.pFILE)
    __PRTVAR__("myFile.startView", myFile.startView)
}

/* distruttore di default */
edlin::~edlin()
{
    /* chiude il file editato */
    myFile.pFILE.close();
}

/* visualizza un blocco del file */
void edlin::view(int iShift)
{
    int iAux = 0;
    int iRow = 0;
    int iTemp = 0;
    string sRow;

    // pulisce lo schermo
    std::cout << "\033[2J\033[1;1H";

    // imposta l'inizio del blocco da visualizzare
    iTemp = myFile.startView;

    // gestisce lo sconfinamento verso l'alto
    myFile.startView += iShift;
    if (myFile.startView < 0)
        myFile.startView = 0;

    // gestisce lo sconfinamento verso il basso
    if (myFile.startView > myFile.sBuffer.size())
        myFile.startView = iTemp;

    __PRTVAR__("myFile.startView", myFile.startView)
    __PRTVAR__("myFile.endView", myFile.endView)
    __PRTVAR__("myFile.maxRow", myFile.maxRow)

    // stampa l'intestazione
    std::cout << "***********************************************************************************************" << std::endl;
    std::cout << std::endl;

    // inizia il ciclo di visualizzazione se c'e' qualcosa da visualizzare
    if (myFile.sBuffer.size() > 0)
    {
        while ((myFile.startView + iAux) < (myFile.sBuffer.size()) && iAux <= PAGE)
        {
            iRow = myFile.startView + iAux;
            __PRTVAR__("iRow", iRow)

            if (iRow >= 0 && iRow < 10)
                std::cout
                    << "00";
            else if (iRow > 9 && iRow < 100)
                std::cout << "0";

            std::cout << iRow << " - " << myFile.sBuffer[iRow] << "\n";

            iAux++;
        }

        myFile.endView = myFile.startView + iAux;
    }
    // stampa il pie pagina
    std::cout << std::endl;
    std::cout << "***********************************************************************************************" << std::endl;
    std::cout << std::endl;
}

/* gestione delle scelte del menu di editing */
void edlin::choice()
{

    __PRTDBG__

    std::regex regMenu("^(((p|n|q|s|){1,1})|((\\d)(i|e|d)))");
    std::regex regWord("[p|n|q|s|i|d|e]{1}$");
    char cChoice;

    std::smatch result;
    std::string sChoice;
    std::string sChTemp;

    input inpChoice;

    Serial.print("[p] PgUP - [n] PgDown - [s] Save - [q] Exit - [num. Linea]( [e] Edit | [i] Ins | [d] Del. )  : ");

    do
    {
        __PRTDBG__

        inpChoice.clear();
        sChoice.clear();

        std::cin >> inpChoice;

        sChoice = inpChoice;
        __PRTVAR__("sChoice", sChoice)

    } while (!regex_search(sChoice, result, regMenu));

    if (regex_search(sChoice, result, regWord))
    {

        cChoice = (result.str(0))[0];
        __PRTVAR__("cChoice", cChoice)

        switch (cChoice)
        {
        case 'n' /* next page */:
        { /* code */
            __PRTVAR__("myFile.startView", myFile.startView)

            edlin::view(PAGE);

            __PRTVAR__("myFile.startView", myFile.startView)

            break;
        }

        case 'p' /* prev page */:
        { /* code */
            __PRTVAR__("myFile.startView", myFile.startView)

            edlin::view(-PAGE);

            __PRTVAR__("myFile.startView", myFile.startView)

            break;
        }

        case 's' /* save */:
        { /* code */

            __PRTDBG__

            size_t iAux = 0;
            input inpYN;
            string sYN;
            smatch result;
            regex regYN("(y|Y|s|S){1}");

            myFile.pFILE.seek(0);

            inpYN.clear();
            sYN.clear();

            std::cout << std::endl;
            std::cout << "Sovrascrivo il file ? [s/N]... ";

            std::cin >> inpYN;
            sYN = inpYN;

            if (regex_search(sYN, result, regYN))
            {
                __PRTDBG__
                myFile.pFILE = SPIFFS.open(s2S(myFile.sFile), "w");

                while (iAux < myFile.sBuffer.size())
                {
                    __PRTVAR__("myFile.sBuffer[iAux]", myFile.sBuffer[iAux])

                    myFile.pFILE.println(s2S(myFile.sBuffer[iAux]));
                    iAux++;
                }
            }
            break;
        }

        case 'q' /* exit */:
        { /* code */

            __PRTDBG__

            myFile.bEmode = false;
            myFile.pFILE.close();
            break;
        }

        case 'e' /* edit row */:
        { /* code */
            __PRTDBG__
            regex regRow("([0-9]{1,})");
            if (regex_search(sChoice, result, regRow))
            {
                int iRowEdit = stoi(result.str(0));

                __PRTVAR__("iRowEdit", iRowEdit)
                __PRTVAR__("myFile.startView", myFile.startView)

                if ((myFile.startView <= iRowEdit) && (iRowEdit <= myFile.endView))
                    edlin::editROW(iRowEdit);
            }

            break;
        }

        case 'i' /* insert row */:
        { /* code */
            __PRTDBG__
            regex regRow("([0-9]{1,})");
            if (regex_search(sChoice, result, regRow))
            {
                int iRowEdit = stoi(result.str(0));

                __PRTVAR__("iRowEdit", iRowEdit)
                __PRTVAR__("myFile.startView", myFile.startView)
                __PRTVAR__("myFile.endView", myFile.endView)

                if ((myFile.startView <= iRowEdit) && (iRowEdit <= myFile.endView + 1))
                    edlin::insROW(iRowEdit);
            }

            break;
        }

        case 'd' /* delete row */:
        { /* code */
            __PRTDBG__

            regex regRow("([0-9]{1,})");
            if (regex_search(sChoice, result, regRow))
            {
                int iRowEdit = stoi(result.str(0));

                __PRTVAR__("iRowEdit", iRowEdit)
                __PRTVAR__("myFile.startView", myFile.startView)

                // controlla se la riga da cancellare e' compresa dopo nella pagina visualizzata
                if ((myFile.startView <= iRowEdit) && (iRowEdit <= (myFile.endView)))
                    edlin::delROW(iRowEdit);
            }

            break;
        }
        }
    }

    __PRTDBG__

} //  END choice

/* si entra in modalita' edit mode */
void edlin::edit()
{

    input inpTemp;
    string sTemp;
    string sYN;
    smatch result;
    regex regYN("(y|Y|s|S){1}");

    if (!myFile.bEmode)
    {
        std::cout << std::endl;
        std::cout << "Il file " << myFile.sFile << " non esiste lo creo ? [s/N]...";

        inpTemp.clear();
        sYN.clear();

        std::cin >> inpTemp;
        sYN = inpTemp;

        if (regex_search(sYN, result, regYN))
        {
            myFile.pFILE = SPIFFS.open(s2S(myFile.sFile), "w");
            if (myFile.pFILE)
            {

                __PRTDBG__

                myFile.bEmode = true;
                myFile.maxRow = 0;
                myFile.startView = 0;
                myFile.endView = 0;
            }
        }
    }

    __PRTVAR__("bEmode", myFile.bEmode)
    __PRTVAR__("myFile.startView", myFile.startView)
    __PRTVAR__("myFile.endView", myFile.endView)
    __PRTVAR__("myFile.maxRow", myFile.maxRow)

    while (myFile.bEmode)
    {
        __PRTDBG__
        edlin::view(0);

        __PRTDBG__
        edlin::choice();

        __PRTDBG__
    }

} // END edit()

/* converte da std::string a String */
String edlin::s2S(string str)
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
string edlin::S2s(String STR)
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

/* modifica una riga */
void edlin::editROW(int iRow)
{
    input inpTemp;
    string sTemp;
    string sYN;
    smatch result;
    regex regYN("(y|Y|s|S){1}");

    inpTemp.clear();
    sTemp.clear();

    std::cout << std::endl;
    std::cout << myFile.sBuffer[iRow];
    inpTemp.clear();
    inpTemp = myFile.sBuffer[iRow];

    std::cin >> inpTemp;
    sTemp = inpTemp;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "...Modifico la riga ? [sN]...";

    inpTemp.clear();
    sYN.clear();

    std::cin >> inpTemp;
    sYN = inpTemp;

    if (regex_search(sYN, result, regYN))
    {
        myFile.sBuffer[iRow].clear();
        myFile.sBuffer[iRow] = sTemp;
    }
}

/* cancella la riga passata come parametro */
void edlin::delROW(int iRow)
{

    input inpTemp;
    string sTemp;
    string sYN;
    smatch result;
    regex regYN("(y|Y|s|S){1}");

    inpTemp.clear();
    sTemp.clear();

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "...Cancello la riga " << iRow << " ? [sN]...";

    inpTemp.clear();
    sYN.clear();

    std::cin >> inpTemp;
    sYN = inpTemp;

    if (regex_search(sYN, result, regYN))
    {
        __PRTVAR__("buffer size", myFile.sBuffer.size())

        if (myFile.sBuffer.size() >= 1)
        {
            __PRTDBG__

            std::vector<string>::iterator it = myFile.sBuffer.begin() + iRow;
            myFile.sBuffer.erase(it);
            myFile.maxRow -= 1;

            if (myFile.sBuffer.size() < PAGE)
            {
                __PRTDBG__
                myFile.endView -= 1;
            }
        }
    }

    __PRTVAR__("buffer size", myFile.sBuffer.size())
}

/* inserisce una riga al posto di quella passata come parametro */
void edlin::insROW(int iRow)
{

    __PRTDBG__

    input inpTemp;
    string sTemp;
    string sYN;
    smatch result;
    regex regYN("(y|Y|s|S){1}");

    inpTemp.clear();
    sTemp.clear();

    std::cout << std::endl;
    std::cout << "Inserisci il contenuto della nuova riga " << std::endl;

    std::cin >> inpTemp;
    sTemp = inpTemp;

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "...Aggiungo la riga ? [sN]...";

    inpTemp.clear();
    sYN.clear();

    std::cin >> inpTemp;
    sYN = inpTemp;

    if (regex_search(sYN, result, regYN))
    {

        myFile.sBuffer.insert(myFile.sBuffer.begin() + iRow, sTemp);
        myFile.maxRow += 1;

        __PRTVAR__("buffer size", myFile.sBuffer.size())

        if (myFile.sBuffer.size() < PAGE)
        {
            __PRTDBG__
            myFile.endView = myFile.maxRow;
        }
    }
}
