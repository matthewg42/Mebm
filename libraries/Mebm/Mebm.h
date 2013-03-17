// Mebm.h
// Copyright (C) 2013 Matthew Gates <matthewg42@gmail.com> (@mousefad)
// This software is released under the terms of the GNU LGPL version 3
// See this URL for more details: http://www.gnu.org/licenses/lgpl.txt
//
// M - Mouse's
// E - Ethernet
// B - Based
// M - Messaging
//

#ifndef MEBM_H
#define MEBM_H

#include <Arduino.h>
#include <Ethernet.h>
#include <MebmConfig.h>

//! @file Mebm.h
//! @brief Define Mebm protocol structres and the MebmClass and extern global.

//! @brief Mebm message structure 
typedef struct {
    char magic[MEBM_MAGIC_SZ];              //!< The "magic" header used to help identify the data as a MEBM message.
    char msgFrom[MEBM_NODEID_MAX_SZ];       //!< The name of the node where the message is originating (human-readable ID).
    char msgType[MEBM_MSGTYP_MAX_SZ];       //!< The type of the message, typically named for the sort of event it described, e.g. "TempSensor".
    char msgData[MEBM_MSGDAT_MAX_SZ];       //!< The per-message varying data for the message, e.g. an actual temperature sensor output value.
} t_mebmMessage;

class MebmClass;

//! @brief Responder filter and callback structure
typedef struct {
    char fromFilter[MEBM_NODEID_MAX_SZ+1];              //!< Either "*" (meaning "any"), or a specific node name to pass an incoming message.
    char typeFilter[MEBM_MSGTYP_MAX_SZ+1];              //!< Either "*" (meaning "any"), or a specific message type to pass an incoming message.
    void (*callback)(MebmClass&, const t_mebmMessage*);      //!< A pointer to a function to call if the from and type filters both match the incoming message.
} t_mebmResponder;

//! @class MebmClass
//! @brief A node for sending and receiving MEBM protocol messages
//!
//! A MebmClass object has an ID, can send and receive messages in the
//! super-simple MEBM protocol. A MEBM protocol message has three parts:
//! 1. FromNode ID (nodes are expected to be honest - there no spoofing protection)
//! 2. MessageType (arbitrary string with maximum length MEBM_MSGTYP_MAX_SZ)
//! 3. MessageData (arbitrary string with maximum length MEBM_MSGDAT_MAX_SZ)
//!
//! Messages are sent to listening nodes, which may be other arduinos, or
//! python scripts like the central control node script.
//!
//! A MebmClass object can have a server associated with it, which is where it sends outbound
//! messages to by default.  Typically this will be the central control node.
//!
//! A MebmClass object can also listen for and react to incoming messages from other nodes.
//! To do this, <i>responders</i> are added.  A responder is a FromNode pattern (* or a
//! fixed name), a message type pattern (* or a fixed string), and a callback function
//! which will be triggered when an incoming message's FromNode and MessageType match
//! the specified pattern.
//!
//! In the style of the Arduino Ethernet and Serial libraries, we also create an instance
//! of MebmClass called Mebm which you can access from within your code.
//!
class MebmClass
{
public:
    //! @brief Create a new MebmClass object
    MebmClass();

    //! @brief Destructor for a MebmClass object
    ~MebmClass();

    //! @brief Configure the MebmClass object
    //!
    //! @param name use this name in outgoing messages to identify this MebmClass as the source of the message
    //! @param numResponders allocate enough memory to have up to this many responders
    //! @param defaultResponder if true, add a responder for WhoDat messages (kindof like ping), note that
    //!        this will take up one of the slots from the numResponders parameter, so if numResponders is 0
    //!        having defaultResponder=true will do nothing.
    void begin(const char* name, int numResponders=0, bool defaultResponder=false);

    //! @brief Send a MEBM message to a specific IP address
    //!
    //! @param toIP the IP address where this message should be sent.
    //! @param messageType set the message type in the Mebm protocol message.
    //!                    Note that only MEBM_MSGTYP_MAX_SZ characters are used - anything else is ignored
    //! @param data the data payload to send.
    //!             Note that only MEBM_MSGDAT_MAX_SZ characters are used - anything else is ignored
    //! @return 0 on successful send, non-0 otherwise.
    int sendToIP(const IPAddress& toIP, const char* messageType, const char* data);

    //! @brief Add a resonder for incoming messages. 
    //! 
    //! Responders are callback function which are called when an incoming message matches the
    //! source node and/or the message type for the responder.
    //! @param msgFrom Only call callBack when the source node matches msgFromID in the incoming message (use * to match any source node).
    //! @param messageType Only call callBack when the messageType matches the message type in the inoming message (use * to match any message type).
    //! @param callBack The function to be called when a matching message is received,  The data part of the message will be used
    //!                 as the parameter to the callBack (it should accept a char* argument).
    //! For example:
    //!  node.addResponder("*", "*", anyMessage);
    //! will add a responder which is triggered for any incoming message (matches all source nodes and message types) and calls anyMessage()
    //!
    //! Another example:
    //!  node.addResponder("*", "SwitchLights", lightSwitch);
    //! will add a responder which works when and node (*) sends a "SwitchLights" message type, calling lightSwitch() in this case.
    void addResponder(const char* msgFrom, const char* messageType, void (*callBack)(MebmClass&, const t_mebmMessage*));

    //! @brief Check for incoming messages.  
    //!
    //! This should be called frequently from the loop() function to catch incoming messages
    //! Returns -1 if there was no incoming message to process, else how many handlers matched the message.
    int listen();

    //! @brief Get the Node ID string for this node.
    const char* id() {
        return _id;
    }

    //! @brief This is a built-in responder callback which sends a Mebm's ID back to the sender of a message (a reply to WhoDat messages)
    //! @param node a reference to the node which is doing the responding. Necessary for this static function.
    //! @param message a pointer to the incoming WhoDat message which prompted the response.  The IP address is expected to be string encoded in the payoad of the incoming message.
    static void _mebmReplyWithID(MebmClass& node, const t_mebmMessage* message);

    //! @brief Convert an IPAddress object to a string IP address.
    //!
    //! Given an Arduino Ethernet library IPAddress object, convert it to a string of the form "aaa.bbb.ccc.ddd"
    //! @param ip the IP address to convert to a string
    //! @param str a char* with at least 16 characters allocated which will be written to
    static void ip2str(const IPAddress& ip, char* str);

    //! @brief Convert a string IP address to an IPAddress object
    //! 
    //! Given a string of the form "aaa.bbb.ccc.ddd" representing an IP address, convert it into 
    //! An Arduino Ethernet Library IPAddress object. 
    //! @param str a pointer to a string containing a representation of an IP address.
    //! @return an IPAddress object containing the address represented by str.
    static IPAddress str2ip(const char* str);

private:
    char _id[MEBM_NODEID_MAX_SZ+1];
    t_mebmResponder *_responders;
    int _responderCount;
    int _maxResponders;
    EthernetServer* _listener;
    int _listenerPort;
    t_mebmMessage _buf;

};

//! The global Mebm object is availabae when the Mebm.h file is included.  This object should be used for all
//! Mebm operations.
extern MebmClass Mebm;

#endif

