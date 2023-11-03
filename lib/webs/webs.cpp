/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Definisce i metodi per la classe del webserver
 *
 */
#include "webs.h"

/* costruttore di defaul del web server - porta 80 */
webs::webs() : websrv(80)
{

} /* end ws() */

/* costruttore  personalizzato - permette di scegliere la porta di ascolto */
webs::webs(int iPort) : websrv(iPort)
{

} /* end ws(int) */

/* avvia il webserver */
void webs::begin()
{
    this->websrv.begin();
}

/* associa un file html ad un url */
void webs::onFile(String sURL, String sFile, int iMode)
{
    this->websrv.on(sURL.c_str(), iMode, [sFile](AsyncWebServerRequest *request)
                    {
        // Gestore per la URl del sito
        // invia il file sFILE
        request->send(SPIFFS, sFile, "text/html"); });
}

/* associa un file html ad un url ma prima esegue un comando shell */
void webs::onFile(String sURL, String sFILE, int iMode, shell* sh , String sCMD)
{

    this->websrv.on(sURL.c_str(), iMode, [sFILE, sCMD, sh](AsyncWebServerRequest *request)
                    {
        // esecuzione del comando
        sh->start(sCMD);

        // Gestore per la URl del sito
        // invia il file sFILE
        request->send(SPIFFS, sFILE, "text/html"); });
}

/* associa una stringa ad un url */
void webs::onText(String sURL, String sText, int iMode, AsyncWebServer *aws)
{
    aws->on(sURL.c_str(), iMode, [sText](AsyncWebServerRequest *request)
            {
        // Gestore per la URl del sito
        // associa una stringa 
        request->send(200, "text/html", sText); });
}

/* associa una stringa ad un url con l'esecuzione di un comando shell*/
void webs::onText(String sURL, String sText, int iMode, shell* sh, String sCMD)
{
    this->websrv.on(sURL.c_str(), iMode, [sText, sCMD, sh](AsyncWebServerRequest *request)
                    {

        // esecuzione del comando di shell
        sh->start(sCMD);

        // Gestore per la URl del sito
        // associa una stringa 
        request->send(200, "text/html", sText); });
}

/* associa un file ad un url */
void webs::onMedia(String sURL, String sFILE, String sMIMET, int iMode)
{
    this->websrv.on(sURL.c_str(), iMode, [sFILE, sMIMET](AsyncWebServerRequest *request)
                    {
        // Gestore per la URl del sito
        // associa una un file
        request->send(SPIFFS, sFILE, sMIMET); });
}


    /* associa ad URL il comando ls della shell */
    void
    webs::onDownload(String sURL)
{
    this->websrv.on(sURL.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
                    {
        // Gestore per la URl del sito
        // associa una un file
        shell sh;
        
        request->send(SPIFFS, "/ls.html"); });
}

/* gestisce una form trasferendo in memoria le variabili dei campi */
void webs::onForm_FORM_Text(String sURL, String sTEXT, int iMode, std::string sCmd)
{

    /* definisce le variabili per la gestione della form */

    this->websrv.on(sURL.c_str(), iMode, [sTEXT, sCmd](AsyncWebServerRequest *request)
                    {
        /* definisce le variabili per la gestione della form */
        shell sh;                           /* crea un oggetto di tipo shell */
        int iNumParams = request->params(); /* numero dei parametri */
        String sNAME;                       /* nome del parametro */
        String sVALUE;                      /* valore del parametro */

        /* ciclo di trasferimento in memoria dei parametri */
        for (int i = 0; i < iNumParams; i++)
            {
                sNAME = request->getParam(i)->name();
                sVALUE = request->getParam(i)->value();

                /* trasferisce in memoria i parametri della form */
                sh.start("set " + sNAME +"=" + sVALUE);
            }

        /* esegue il comando di shell assegnato all'URL */
        sh.start(sCmd);

        // Gestore per la URl del sito
        // associa una stringa 
        request->send(200, "text/html", sTEXT); });
}


void webs::clear(){
    this->clear();
}