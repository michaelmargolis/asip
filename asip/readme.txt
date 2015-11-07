The Arduino Service Interface Protocol (ASIP) is a protocol that provides 
communication between a computer and a microcontroller, typically Arduino.

ASIP  enables a computer to discover, configure, read and write the
microcontroller’s general purpose IO pins. In this sense it is similar in
spirit to Firmata, but differs in that it is designed to easily expose
higher level service that can be implemented on Arduino.

The standard ASIP implementation uses a serial connection to the
remote computer but other links using a stream interface, such as Ethernet
can be used.
