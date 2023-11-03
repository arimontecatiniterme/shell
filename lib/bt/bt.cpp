#include "bt.h"

/* costruttore di default */
bt::bt()
{
    BTSerial.begin("bt-serial");
}

/* overload del costruttore */
bt::bt(String sNAME)
{
    BTSerial.begin(sNAME);
}

/* stampa a video il contenuto del buffer del BT */
void bt::readBT()
{
    String sMSG;

    Serial.println("");
    while (BTSerial.available())
    {
        sMSG = BTSerial.readStringUntil('\n');
        Serial.println(sMSG);
    }
    Serial.println("");
}

/* stampa su file il contenuto del buffer del BT */
void bt::readBT(String sFILE, String sMODE)
{

    File pFILE;
    pFILE = SPIFFS.open(sFILE, sMODE.c_str());
    if (pFILE)
    {
        while (BTSerial.available())
        {
            pFILE.print(BTSerial.readStringUntil('\n'));
        }
    }
    pFILE.close();
}

/* converte da std::string a String */
String bt::s2S(std::string str)
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
std::string bt::S2s(String STR)
{
    int len = 0;
    int pos;
    char rc;
    std::string str;

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

/* invia un messaggio sulla scheda BT */
void bt::sendBT(String sMESSAGE)
{
    bt::BTSerial.println(sMESSAGE);
}

/* imposta il nome della scheda BT */
void bt::setNameBT(String sNAME_BT)
{
    bt::BTSerial.begin(sNAME_BT);
}