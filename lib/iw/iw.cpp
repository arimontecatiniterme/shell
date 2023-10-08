/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Definisce i metodi per la classe iw per la gestione di una rete wifi
 *
 */

#include "iw.h"

/* costruttore di defaul */
iw::iw()
{
}

/* esegue la scansione della rete */
void iw::scan()
{

    // Inizia la scansione delle reti Wi-Fi
    int numberOfNetworks = WiFi.scanNetworks();

    // Verifica se sono state trovate reti Wi-Fi
    if (numberOfNetworks == 0)
    {
        Serial.println("Nessuna rete Wi-Fi trovata.");
    }
    else
    {
        Serial.print("Numero di reti Wi-Fi trovate: ");
        Serial.println(numberOfNetworks);

        // Ciclo attraverso le reti Wi-Fi rilevate
        for (int i = 0; i < numberOfNetworks; ++i)
        {
            // Ottieni l'SSID della rete
            String ssid = WiFi.SSID(i);

            // Ottieni l'intensità del segnale (RSSI) della rete
            int rssi = WiFi.RSSI(i);

            // Stampa i dettagli della rete sulla console Seriale
            Serial.print("Rete ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(ssid);
            Serial.print(" (RSSI ");
            Serial.print(rssi);
            Serial.println(" dBm)");
        }
    }
}

/* connette si connette ad una rete */
int iw::connect(String sSID, String sPASSWD)
{
    int iAux = 0;

    // Connessione a una rete Wi-Fi
    WiFi.begin(sSID.c_str(), sPASSWD.c_str());

    std::cout << std::endl
              << "Connessione in corso";
    while ((WiFi.status() != WL_CONNECTED) && (iAux < 20))
    {
        delay(500);
        std::cout << ".";
        iAux++;
    }

    std::cout << std::endl;

    return WiFi.status();
}

/* stampa a video lo stato della scheda */
void iw::status()
{
    
   
}