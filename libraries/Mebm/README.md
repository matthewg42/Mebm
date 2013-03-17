Arduino MEBM Messaging Library
==============================

MEBM stands for: Mouse's Ethernet Based Messaging, and is a super simple protocol for 
sending simple messages between Arduinos over Ethernet (i.e. Arduinos with Ethernet
shields on them).

MEBM sits on top of the Arduino Ethernet library, using TCP to connect / listen.

MEBM Protocol
-------------

MEBM messages are fixed size structures with the following fields:

- MEBM Magic (4 bytes)		To help identify MEBM messages
- MessageFrom (12 bytes)	An ID of the Node which sent the message
- MessageType (12 bytes)	Some arbitray string to identify the purpose of the message
- MessageData (48 bytes)	Arbitrary message data.

A typical message might be that a magstripe has been read by a MebmNode called "MagReader":
- MessageFrom = "MagReader"
- MessageType = "ReadCard"
- MessageDaya = ;1234567890ABCDEF;00000000001

Fields are NULL padded up to their length.

Usage
-----

See the Doxygen documentation for API and tutorial examples.
