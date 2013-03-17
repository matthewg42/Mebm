// MebmConfig.h
// Copyright (C) 2013 Matthew Gates <matthewg42@gmail.com> (@mousefad)
// This software is released under the terms of the GNU LGPL version 3
// See this URL for more details: http://www.gnu.org/licenses/lgpl.txt
//
// M - Mouse's
// E - Ethernet
// B - Based
// M - Messaging
//

#ifndef MEBMCONFIG_H
#define MEBMCONFIG_H

//! @file MebmConfig.h
//! @brief Define constants for an installation of Mebm
//!

// define MEBM_SERIAL_DEBUG if you want serial debugging else comment this line out
// WARNING - takes about 182 bytes of RAM, so if you're low, beware of enabling this!
//#define MEBM_SERIAL_DEBUG           1

//! The port on which all MEBM messages are sent.
#define MEBM_PORT                   2300    //!< The TCP port all MEBM traffic uses.

//! Status codes for send commands
#define MEBM_MESSAGE_SENT_OK        0       //!< Status code returned by send() when message was sent OK.
#define MEBM_ERR_NO_DESTINATION     1       //!< Status code returned by send() when message failed because destination IP was not found.
#define MEBM_ERR_CANNOT_CONNECT     2       //!< Status code returned by send() when message failed for some other reason.

//! Sizes for the three parts of the MEBM protocol messages.
#define MEBM_MAGIC_SZ	            4       //!< MEBM protocol Magic field size.
#define MEBM_NODEID_MAX_SZ	        12      //!< MEBM protocol NodeID field size.
#define MEBM_MSGTYP_MAX_SZ	        12      //!< MEBM protocol MessageType field size.
#define MEBM_MSGDAT_MAX_SZ	        48      //!< MEBM protocol MessageData field size.
#define MEBM_MAGIC_STR              "MeBm"  //!< MEBM protocol Magic data (for identifying MEBM messages).

#endif // MEBMCONFIG_H

