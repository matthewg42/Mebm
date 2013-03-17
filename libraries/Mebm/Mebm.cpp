// MebmClass.cpp
// Copyright (C) 2013 Matthew Gates <matthewg42@gmail.com> (@mousefad)
// This software is released under the terms of the GNU LGPL version 3
// See this URL for more details: http://www.gnu.org/licenses/lgpl.txt

#include <Arduino.h>
#include <Ethernet.h>
#include <Mebm.h>
#include <stdlib.h>
#include <stdio.h>

MebmClass::MebmClass() : 
    _responderCount(0),
    _maxResponders(0)
{
    _responders = NULL;
    _listener = NULL;
}

MebmClass::~MebmClass()
{
    if (_listener != NULL)
    {
        delete _listener;
        _listener = NULL;
    }

    if (_responders) { 
        free(_responders); 
        _responders = NULL;
    }
}

void MebmClass::begin(const char* name, int numResponders, bool defaultResponder)
{
    strncpy(_id, name, MEBM_NODEID_MAX_SZ);
    _id[MEBM_NODEID_MAX_SZ] = 0; // ensure null termination

    // protect against calling more than once...
    if (_responders != NULL) {
        free(_responders);
        _responders = NULL;
    }

    if (numResponders > 0) {
        _responders = (t_mebmResponder*)malloc(sizeof(t_mebmResponder)*numResponders);
    }

    _maxResponders = numResponders;
    _responderCount = 0;

    if (defaultResponder)
        addResponder("*", "WhoDat", _mebmReplyWithID);

}

int MebmClass::sendToIP(const IPAddress& toIP, const char* messageType, const char* data)
{
    int i;
    byte* b = (byte*)&_buf;
    EthernetClient client;

#ifdef MEBM_SERIAL_DEBUG
    char ipStr[16];
    ip2str(toIP, ipStr);

    Serial.print("MEBM toip=");
    Serial.print(ipStr);
    Serial.print(", t=");
    Serial.print(messageType);
    Serial.print(", d=");
    Serial.print(data);
    Serial.print(" ");
#endif

    memset((void*)&_buf, 0, sizeof(t_mebmMessage));
    strncpy(_buf.magic, MEBM_MAGIC_STR, MEBM_MAGIC_SZ);
    strncpy(_buf.msgFrom, _id, MEBM_NODEID_MAX_SZ);
    strncpy(_buf.msgType, messageType, MEBM_MSGTYP_MAX_SZ);
    strncpy(_buf.msgData, data, MEBM_MSGDAT_MAX_SZ);


    // Open TCP connection, send data packet, close connection.
    if (!client.connect(toIP, MEBM_PORT)) {
#ifdef MEBM_SERIAL_DEBUG
        Serial.println("MEBM ERROR connect");
#endif
        return MEBM_ERR_CANNOT_CONNECT;
    }

    if (client.connected()) {
        for(int i=0; i<sizeof(t_mebmMessage); i++)
        {
            client.write(b[i]);
        }
        client.stop();
    }
    else {
#ifdef MEBM_SERIAL_DEBUG
        Serial.println("MEBM ERROR connect");
#endif
        return MEBM_ERR_CANNOT_CONNECT;
    }

#ifdef MEBM_SERIAL_DEBUG
    Serial.println("MEBM OK");
#endif
    return MEBM_MESSAGE_SENT_OK;
}

void MebmClass::addResponder(const char* fromFilter, const char* typeFilter, void (*callback)(MebmClass&, const t_mebmMessage*))
{
    // Don't allow too many responders
    if (_responderCount >= _maxResponders)
    {
#ifdef MEBM_SERIAL_DEBUG
        Serial.println("MEBM ERROR: too many responders");
#endif
        return;
    }

    strncpy(_responders[_responderCount].fromFilter, fromFilter, MEBM_NODEID_MAX_SZ);
    _responders[_responderCount].fromFilter[MEBM_NODEID_MAX_SZ] = 0; // ensure NULL termination
    strncpy(_responders[_responderCount].typeFilter, typeFilter, MEBM_MSGTYP_MAX_SZ);
    _responders[_responderCount].typeFilter[MEBM_MSGTYP_MAX_SZ] = 0; // ensure NULL termination
    _responders[_responderCount++].callback = callback;

    // Once we've added the first responder, we should create the server object which we can
    // use to listen for incoming connections.
    if (_listener == NULL)
        _listener = new EthernetServer(MEBM_PORT);

}

int MebmClass::listen()
{
    int i;
    bool fromMatch;
    bool typeMatch;
    char* buf = (char*)(&_buf);
    int handled = 0;

    // do nothing if we haven't added a responder yet (which is when _listener is created)
    if (_listener == NULL)
        return 0;

    EthernetClient client = _listener->available();
    if (client) {
        // read the message into _buf
        i=0;
        while (client.connected()) {
            if (client.available()) {
                if (i<sizeof(t_mebmMessage)) {
                    buf[i++] = client.read();
                }
                else
                    client.read(); // read data to clear client buffers, but don't do anything with it.
            }
        }

#ifdef MEBM_SERIAL_DEBUG
        Serial.print("MEBM read ");
        Serial.println(i);
#endif

        // close the connection:
        client.flush();
        client.stop();

        if (strncmp(_buf.magic, MEBM_MAGIC_STR, MEBM_MAGIC_SZ) != 0) {
#ifdef MEBM_SERIAL_DEBUG
            Serial.println("MEBM listen bad magic");
#endif
            return -1;
        }

#ifdef MEBM_SERIAL_DEBUG
        Serial.print("MEBM listen n=");
        Serial.print(_buf.msgFrom);
        Serial.print(" t=");
        Serial.print(_buf.msgType);
        Serial.print(" d=");
        Serial.println(_buf.msgData);
#endif

        // Decide if one or more responders need to be called for this message
        for(i=0; i<_responderCount; i++) {
            fromMatch = false;
            if (strncmp(_responders[i].fromFilter, "*", 1) == 0)
                fromMatch = true;
            else if (strncmp(_buf.msgFrom, _responders[i].fromFilter, strnlen(_responders[i].fromFilter, MEBM_NODEID_MAX_SZ)) == 0)
                fromMatch = true;

            typeMatch = false;
            if (strncmp(_responders[i].typeFilter, "*", 1) == 0)
                typeMatch = true;
            else if (strncmp(_buf.msgType, _responders[i].typeFilter, strnlen(_responders[i].typeFilter, MEBM_MSGTYP_MAX_SZ)) == 0)
                typeMatch = true;

#ifdef MEBM_SERIAL_DEBUG
            Serial.print("MEBM listen(");
            Serial.print(_id);
            Serial.print(") res?: ");
            Serial.print(_responders[i].fromFilter);
            Serial.print("/");
            Serial.print(_responders[i].typeFilter);
            Serial.print(" : ");
            Serial.print(fromMatch);
            Serial.print("/");
            Serial.println(typeMatch);
#endif
            if (fromMatch && typeMatch) {
                // call the responder
#ifdef MEBM_SERIAL_DEBUG
                Serial.print("MEBM listen(");
                Serial.print(_id);
                Serial.println(") calling");
#endif
                _responders[i].callback(*this, &_buf);
                handled++;
            }
        }
#ifdef MEBM_SERIAL_DEBUG
        Serial.print("MEBM listen(");
        Serial.print(_id);
        Serial.print(") ok : ");
        Serial.println(handled);
#endif

        if (handled == 0)
            return handled;
    }
    else
        return -1;
}

void MebmClass::_mebmReplyWithID(MebmClass& node, const t_mebmMessage* mes)
{
    IPAddress srcIP;
    char buf[16];

    srcIP = str2ip(mes->msgData);
    ip2str(Ethernet.localIP(), buf);
    node.sendToIP(srcIP, "IHaveIP", buf);
}

void MebmClass::ip2str(const IPAddress& ip, char* str)
{
    memset((void*)(str), 0, sizeof(char)*16);
    snprintf(str, 16, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

IPAddress MebmClass::str2ip(const char* str)
{
    int octet[4];
    sscanf(str, "%d.%d.%d.%d", &octet[0], &octet[1], &octet[2], &octet[3]);
    return IPAddress(octet[0], octet[1], octet[2], octet[3]);
}

MebmClass Mebm;

