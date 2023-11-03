/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Dichiara i metodi per la classe shell che modella l'omonimo ambiente da utilizzare su porta seriale
 *
 */

#ifndef __EDLIN__
#define __EDLIN__

/* librerie di sistema */
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <cstddef>
#include <iostream>
#include <regex.h>
#include <string>
#include <vector>

/* librerie personali */
#include "input.h"

class edlin
{

private:
    int iFirstRow = 0; // indica la prima riga da visualizzare

    /* struttura per la gestione dell'editing di un file */
    struct editFile
    {
        File pFILE;                       // puntatore al file da editare
        std::string sFile;                     // nome del file
        int maxRow = 0;                   // numero massimo di righe del file
        int posixBuffer = 0;              // indice della posizione all'interno del buffer ( ultima riga visualizzata )
        int startView = 0;                // prima riga del buffer visualizzata
        int endView = 0;                  // ultima riga visualizzata
        boolean bEmode = false;           // indica se e' attiva la modalita di editing del file
        std::vector<std::string> sBuffer; // buffer che memorizza il contenuto del file
    };

    /* dichiara una specifica struttura di editing di un file */
    struct editFile myFile;

    void view(int);       // visualizza un blocco del file
    void choice();        // ciclo di input delle scelte del menu
    void editROW(int);    // modifica la riga passata come parametro;
    void delROW(int);     // elimina la riga passata come parametro
    void insROW(int);     // inserisce una riga al posto di quella passata come parametro
    void save();          // salva il file
    void newFile(String); // crea un nuovo file
    String s2S(std::string);   // converte da String a string
    std::string S2s(String);   // converte da string a String

public:
    edlin(String); // costruttore di default
    ~edlin();      // distrottore di default
    void edit();   // passa il file in modalita' edit
};

#endif // end __EDLIN
