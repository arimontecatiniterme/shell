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

/* associa una stringa ad un url */
void webs::onText(String sURL, String sText, int iMode, AsyncWebServer *aws)
{
    aws->on(sURL.c_str(), iMode, [sText](AsyncWebServerRequest *request)
            {
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
void webs::onLS(String sURL, int iMode)
{
    this->websrv.on(sURL.c_str(), iMode, [](AsyncWebServerRequest *request)
                    {
        // Gestore per la URl del sito
        // associa una un file
        shell sh;
        sh.start("ls > shell.txt");
        request->send(SPIFFS, "/ls.html"); });
}

/* associa ad URL il comando ls della shell */
void webs::onDownload(String sURL)
{
    this->websrv.on(sURL.c_str(), HTTP_GET, [](AsyncWebServerRequest *request)
                    {
        // Gestore per la URl del sito
        // associa una un file
        shell sh;
        sh.start("ls > shell.txt");
        request->send(SPIFFS, "/ls.html"); });
}