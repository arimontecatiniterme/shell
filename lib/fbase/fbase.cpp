/*
 * ARI Montecatini - PTLUG Pistoia
 *
 * https://arimontecatini.it
 * https://ptlug2.altervista.org
 *
 * IU5HLS - Andrea Venuti
 *
 * Gestione oggetto firebase
 *
 *

*/

#include "fbase.h"
#include "addons/RTDBHelper.h"
#include "addons/TokenHelper.h"
#include <Firebase_ESP_Client.h>

/* data */
FirebaseData fbData;
FirebaseAuth fbAuth;
FirebaseConfig fbConf;

/* costruttore di default */
fbase::fbase()
{

    /* Assign the api key (required) */
    fbConf.api_key = "AIzaSyBVK2Z1-sN9JYW9356lWlN5MAAcsNi4kGw";

    /* Assign the RTDB URL (required) */
    fbConf.database_url = "https://labint-ef3a3-default-rtdb.europe-west1.firebasedatabase.app/";

    /* Sign up */
    if (Firebase.signUp(&fbConf, &fbAuth, "", ""))
    {
        Serial.println("ok");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", fbConf.signer.signupError.message.c_str());
    }

    /* Assign the callback function for the long running token generation task */
    fbConf.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    Firebase.begin(&fbConf, &fbAuth);
    Firebase.reconnectWiFi(true);

} /* end fbase() */

/* costruttore personalizzato */
fbase::fbase(String sTOKEN, String sURL)
{
    /* Assign the api key (required) */
    fbConf.api_key = sTOKEN;

    /* Assign the RTDB URL (required) */
    fbConf.database_url = sURL;

    /* Sign up */
    if (Firebase.signUp(&fbConf, &fbAuth, "", ""))
    {
        Serial.println("ok");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", fbConf.signer.signupError.message.c_str());
    }

    /* Assign the callback function for the long running token generation task */
    fbConf.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    Firebase.begin(&fbConf, &fbAuth);
    Firebase.reconnectWiFi(true);

} /* end fbase(String, String) */

/* effettua la conversione di tipo e inserisce un valore nel database di fireBase */
void fbase::set(String sPATH, std::string sType, std::string sVal, String sSTR)
{

    if (sVal.length() > 0)
    {
        
        if (sType == "(int)")
        {
            
            fbase::setInt(sPATH, std::stoi(sVal));
        }
        else if (sType == "(float)")
        {
            
            fbase::setFloat(sPATH, std::stof(sVal));
        }
        else if (sType == "(string)")
        {
            
            fbase::setString(sPATH, sSTR);
        }
    }
}

/* metodo per l'inserimento di interi */
void fbase::setInt(String sPATH, int iVal)
{

    if (Firebase.RTDB.setInt(&fbData, sPATH, iVal))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbData.dataPath());
        Serial.println("TYPE: " + fbData.dataType());
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbData.errorReason());
    }

} /* end template fbIns(String sPATH, tVal tV) */

/* metodo per l'inserimento di stringhe */
void fbase::setString(String sPATH, String sVAL)
{

    if (Firebase.RTDB.setString(&fbData, sPATH, sVAL))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbData.dataPath());
        Serial.println("TYPE: " + fbData.dataType());
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbData.errorReason());
    }

} /* end template fbIns(String sPATH, tVal tV) */

/* metodo per l'inserimento di float */
void fbase::setFloat(String sPATH, float fVal)
{

    if (Firebase.RTDB.setFloat(&fbData, sPATH, fVal))
    {
        Serial.println("PASSED");
        Serial.println("PATH: " + fbData.dataPath());
        Serial.println("TYPE: " + fbData.dataType());
    }
    else
    {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbData.errorReason());
    }

} /* end template fbIns(String sPATH, tVal tV) */

/* stampa a video il valore letto */
void fbase::readFB(String sPATH /* valore da leggere */)
{

    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();
        if (Firebase.RTDB.getInt(&fbData, sPATH))
        {
            if (fbData.dataType() == "int")
            {
                int intValue = fbData.intData();
                Serial.println(intValue);
            }

            if (fbData.dataType() == "float")
            {
                float floatValue = fbData.floatData();
                Serial.println(floatValue);
            }

            if (fbData.dataType() == "String")
            {
                String StringValue = fbData.stringData();
                Serial.println(StringValue);
            }
        }
        else
        {
            Serial.println(fbData.errorReason());
        }
    }
}

/* stampa a video il valore letto */
void fbase::readFBtoPrintFile(String sPATH /* valore da leggere */,
                              String sFILE /* file */,
                              String sMODE /* modo di scrittura */)
{

    fbase::pFILE = SPIFFS.open(sFILE, sMODE.c_str());

    if (pFILE)
    {
        if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
        {
            sendDataPrevMillis = millis();
            if (Firebase.RTDB.getInt(&fbData, sPATH))
            {
                if (fbData.dataType() == "int")
                {
                    int intValue = fbData.intData();
                    pFILE.print(sPATH + "=");
                    pFILE.println(intValue);
                }

                if (fbData.dataType() == "float")
                {
                    float floatValue = fbData.floatData();
                    pFILE.print(sPATH + "=");
                    pFILE.println(floatValue);
                }

                if (fbData.dataType() == "String")
                {
                    String StringValue = fbData.stringData();
                    pFILE.print(sPATH + "=");
                    pFILE.println(StringValue);
                }
            }
            else
            {
                Serial.println(fbData.errorReason());
            }
            pFILE.close();
        }
    }
}