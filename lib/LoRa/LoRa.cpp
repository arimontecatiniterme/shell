#include "LoRa.h"

/* costruttore di default */
LoRa::LoRa() : e220ttl(&Serial2, GPIO_NUM_18, GPIO_NUM_21, GPIO_NUM_19)
{
    /* inizializza l'oggetto LoRa */
    // LoRa_E220 e220ttl(&Serial2, AUX, M0, M1);
}

/* costruttore personalizzato */
LoRa::LoRa(HardwareSerial *mySerial, int iAux, int iM0, int iM1) : e220ttl(mySerial, iAux, iM0, iM1)
{
    /* inizializza l'oggetto LoRa */
    // LoRa_E220 e220ttl(&Serial2, AUX, M0, M1);
}

/* Inizializza l'oggetto LoRa */
void LoRa::init()
{
    e220ttl.begin();
}

/* Stampa su file la configurazione della board LoRa */
void LoRa::rlConf(String sFILE, String sMODE)
{

    File pFILE;

    ResponseStructContainer c;
    c = e220ttl.getConfiguration();

    // It's important get configuration pointer before all other operation
    Configuration configuration = *(Configuration *)c.data;

    ResponseStructContainer cMi;
    cMi = e220ttl.getModuleInformation();

    // It's important get information pointer before all other operation
    ModuleInformation mi = *(ModuleInformation *)cMi.data;

    if (SPIFFS.exists(sFILE))
    {
        pFILE = SPIFFS.open(sFILE, sMODE.c_str());

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

} // end rlConf

/* Stampa a video la configurazione della board LoRa */
void LoRa::rlConf()
{

    ResponseStructContainer c;
    c = e220ttl.getConfiguration();

    // It's important get configuration pointer before all other operation
    Configuration configuration = *(Configuration *)c.data;

    ResponseStructContainer cMi;
    cMi = e220ttl.getModuleInformation();

    // It's important get information pointer before all other operation
    ModuleInformation mi = *(ModuleInformation *)cMi.data;

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

/* imposta la configurazione della board LoRa */
void LoRa::slConf(std::string sVal, std::string sVar)
{
    /* Definisce la mappa dei valori della velocita' della UART */
    std::map<std::string, int> uartb;

    uartb["UART_BPS_1200"] = UART_BPS_1200;
    uartb["UART_BPS_2400"] = UART_BPS_2400;
    uartb["UART_BPS_4800"] = UART_BPS_4800;
    uartb["UART_BPS_9600"] = UART_BPS_9600;
    uartb["UART_BPS_19200"] = UART_BPS_19200;
    uartb["UART_BPS_38400"] = UART_BPS_38400;
    uartb["UART_BPS_57600"] = UART_BPS_57600;
    uartb["UART_BPS_115200"] = UART_BPS_115200;

    /* Definisce la mappa della parita' della porta UART */
    std::map<std::string, int> uartp;

    uartp["MODE_00_8N1"] = MODE_00_8N1;
    uartp["MODE_01_8O1"] = MODE_01_8O1;
    uartp["MODE_10_8E1"] = MODE_10_8E1;
    uartp["MODE_11_8N1"] = MODE_11_8N1;

    /* definisce la mappa delle opzioni della vecolita' di trasmissione */
    std::map<std::string, int> air;

    air["AIR_DATA_RATE_000_24"] = AIR_DATA_RATE_000_24;
    air["AIR_DATA_RATE_001_24"] = AIR_DATA_RATE_001_24;
    air["AIR_DATA_RATE_010_24"] = AIR_DATA_RATE_010_24;
    air["AIR_DATA_RATE_011_48"] = AIR_DATA_RATE_011_48;
    air["AIR_DATA_RATE_100_96"] = AIR_DATA_RATE_100_96;
    air["AIR_DATA_RATE_101_192"] = AIR_DATA_RATE_101_192;
    air["AIR_DATA_RATE_110_384"] = AIR_DATA_RATE_110_384;
    air["AIR_DATA_RATE_111_625"] = AIR_DATA_RATE_111_625;

    /* Imposta la dimensione del pacchetto trasmesso */
    std::map<std::string, int> packet;

    packet["SPS_200_00"] = SPS_200_00;
    packet["SPS_128_01"] = SPS_128_01;
    packet["SPS_064_10"] = SPS_064_10;
    packet["SPS_032_11"] = SPS_032_11;

    /* imposta la gestione del rumore di fondo */
    std::map<std::string, int> rssi;

    rssi["RSSI_AMBIENT_NOISE_ENABLED"] = RSSI_AMBIENT_NOISE_ENABLED;
    rssi["RSSI_AMBIENT_NOISE_DISABLED"] = RSSI_AMBIENT_NOISE_DISABLED;

    /* Imposta la potenza di trasmissione */
    std::map<std::string, int> ptx;

    ptx["POWER_22"] = POWER_22;
    ptx["POWER_17"] = POWER_17;
    ptx["POWER_13"] = POWER_13;
    ptx["POWER_10"] = POWER_10;

    /* imposta il modo di funzionamento della scheda */
    std::map<std::string, int> mode;

    mode["FT_FIXED_TRANSMISSION"] = FT_FIXED_TRANSMISSION;
    mode["FT_TRANSPARENT_TRANSMISSION"] = FT_TRANSPARENT_TRANSMISSION;

    /* Monitoraggio dei dati in trasmissione */
    std::map<std::string, int> mtx;

    mtx["LBT_ENABLED"] = LBT_ENABLED;
    mtx["LBT_DISABLED"] = LBT_DISABLED;

    /* Impostazione della modalita wor */
    std::map<std::string, int> wor;

    // wor["WAKE_UP_500"] = WAKE_UP_500;
    // wor["WAKE_UP_1000"] = WAKE_UP_1000;
    // wor["WAKE_UP_1500"] = WAKE_UP_1500;
    // wor["WAKE_UP_2000"] = WAKE_UP_2000;
    // wor["WAKE_UP_2500"] = WAKE_UP_2500;
    // wor["WAKE_UP_3000"] = WAKE_UP_3000;
    // wor["WAKE_UP_3500"] = WAKE_UP_3500;
    // wor["WAKE_UP_4000"] = WAKE_UP_4000;

    /* Bit di parita' */
    std::map<std::string, int> re;
    re["RSSI_ENABLED"] = RSSI_ENABLED;
    re["RSSI_DISABLED"] = RSSI_DISABLED;

    /* Salva la configurazione */
    std::map<std::string, int> save;
    save["YES"] = 1;
    save["NO"] = 0;

    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    // It's important get configuration pointer before all other operation
    Configuration config = *(Configuration *)c.data;

    Serial.print(c.status.getResponseDescription());
    Serial.print("  ");
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
    else if (sVar == "uartbr")
    {
        config.SPED.uartBaudRate = uartb[sVar];
    }
    else if (sVar == "uartp")
    {
        config.SPED.uartParity = uartp[sVal];
    }
    else if (sVar == "airdr")
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
        Serial.print(rs.getResponseDescription());
        Serial.print("   ");
        Serial.println(rs.code);
    }
    else
    {
        ResponseStatus rs = e220ttl.setConfiguration(config, WRITE_CFG_PWR_DWN_LOSE);
        Serial.println(rs.getResponseDescription());
        Serial.println(rs.code);
    }
} // end slConf()

/* imposta la configurazione della board LoRa da file */
void LoRa::slConf(String sFILE)
{

    std::smatch result;
    std::regex row("( )*(addl|addh|ch|uartbr|uartp|airdr|packet|rnoise|tpow|re|ftx|lbt|wor|save)( )*=( )*[\\w\\.@]*");
    std::regex var("(addl|addh|ch|uartbr|uartp|airdr|packet|rnoise|tpow|re|ftx|lbt|wor|save)");
    std::regex val("([\\w\\.@]*)$");

    if (SPIFFS.exists(sFILE))
    {
        String sRROW;
        String sVAR;
        String sVAL;

        std::string sVar;
        std::string sVal;
        std::string srrow;

        File pFILE;
        pFILE = SPIFFS.open(sFILE);

        while (pFILE.available())
        {
            sRROW = pFILE.readStringUntil('\n');
            srrow = LoRa::S2s(sRROW);

            if (std::regex_search(srrow, result, row))
            {
                std::regex_search(srrow, result, var);
                sVAR = LoRa::s2S(result.str(0));
                sVAR.trim();
                sVar = LoRa::S2s(sVAR);

                std::regex_search(srrow, result, val);
                sVAL = LoRa::s2S(result.str(0));
                sVAL.trim();
                sVal = LoRa::S2s(sVAL);

                std::cout << sVar << "=" << sVal << std::endl;

                LoRa::slConf(sVal, sVar);
            }
        }
    }
}

/* invia un messaggio lora di tipo struct */
/* slMsg (MESSAGE, USERNAME, ch ) */
void LoRa::slMsg(String sMESSAGE, String sUSERNAME, int iCH)
{

    struct ChatMessage messageTemp;

    strcpy(messageTemp.message, sMESSAGE.c_str());
    strcpy(messageTemp.user, sUSERNAME.c_str());

    /* calola la dimesione reale del messaggio e del nome utente */
    int strLenByteMessage = (sMESSAGE.length() + 1) * sizeof(char); // null char
    int strLenByteUser = (sUSERNAME.length() + 1) * sizeof(char);   // null char

    /* controlla se la dimensione del messaggio non ecceda quella riservata in struttura */
    if (strLenByteMessage > (sizeof(messageTemp.message) / sizeof(messageTemp.message[0])))
    {
        // message is longher than the buffer
        sMESSAGE.toCharArray(messageTemp.message, ((sizeof(messageTemp.message) / sizeof(messageTemp.message[0])) - 1));
    }
    else
    {
        // message is SHORTER than the buffer
        sMESSAGE.toCharArray(messageTemp.message, sMESSAGE.length() + 1);
    }

    /* controlla se la dimensione dell username non ecceda quella riservata in struttura */
    if (strLenByteUser > (sizeof(messageTemp.user) / sizeof(messageTemp.user[0])))
    {

        // username is longher than the buffer
        sUSERNAME.toCharArray(messageTemp.user, ((sizeof(messageTemp.user) / sizeof(messageTemp.user[0])) - 1));
    }
    else
    {

        // username is SHORTER than the buffer
        sUSERNAME.toCharArray(messageTemp.user, sMESSAGE.length() + 1);
    }

    /* invia il messaggio */
    ResponseStatus rs = e220ttl.sendBroadcastFixedMessage((int8_t)iCH, &messageTemp, (uint8_t)sizeof(struct ChatMessage));
    rs.getResponseDescription();
    std::cout << ("\nMessage send: " + rs.getResponseDescription()).c_str();
}

/* invia un messaggio non strutturato */
/* slMsg (MESSAGE, ch ) */
void LoRa::slMsg(String sMESSAGE, int iCH)
{
    ResponseStatus rs = e220ttl.sendBroadcastFixedMessage((int8_t)iCH, sMESSAGE);
    rs.getResponseDescription();
    std::cout << ("\nMessage send: " + rs.getResponseDescription()).c_str();
}

/* invia un messaggio in modo FIXED */
/* slMsg (addh, addl, ch, message ) */
void LoRa::slMsg(int iAddh, int iAddl, int iCH, String sMsg)
{
    ResponseStatus rs = e220ttl.sendFixedMessage(iAddh, iAddl, iCH, sMsg);
    std::cout << rs.getResponseDescription() << std::endl;
}

/* legge un messaggio LoRa */
void LoRa::rlMsg()
{
    if (e220ttl.available() > 1)
    {
        ResponseContainer rc = e220ttl.receiveMessage();

        if (rc.status.code != 1)
        {
            rc.status.getResponseDescription();
        }
        else
        {
            // Print the data received
            std::cout << "Received: " + rc.data;
        }
    }
    else
    {
        std::cout << "Nothing to read on Lora";
    }
}

/* converte da std::string a String */
String LoRa::s2S(std::string str)
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
std::string LoRa::S2s(String STR)
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
