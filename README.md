nrf51-simple-radio
==================

A simple radio protocol for the nRF51 series, with full BSD-licensed source code. 

This is very much a work in progress, but could possibly serve as a useful base for anyone looking to understand how to use the radio on the nRF51 series for a custom protocol.

What is done here may or may not be good things to do, and the entire protocol is done primarily for my own understanding of the radio peripheral. 

Usage
-----
To compile, run the Makefiles in app/tx/pure-gcc/ and app/rx/pure-gcc. You will need the official nRF51 SDK, available from www.nordicsemi.com and my pure-gcc setup from https://github.com/hlnd/nrf51-pure-gcc-setup. Set correct paths in the project makefiles. 

To run the few unit tests for the queue, you'd have to use CppUTest and set its path in the test makefile. 

This has been developed on Linux and not tested on any other platform. 

Current features
----------------
- 64 byte packets (configurable).
- ACKs on successful reception.
- Event on packet sent, packet lost (configurable number of retransmits) and packet received. 
- RX always on, TX only on when sending.
- Queues for packets (configurable size); will send all as fast as possible when actually sending.

Future development could include anything. Feature requests will (most likely) not be implemented, but pull requests will be considered!
