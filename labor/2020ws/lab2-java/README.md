# Repository lab2-java

This repository contains the source code in Java for a client-server application that is used for teaching Internet protocols.

## Overview

The application has been developed for the Computer Network Lab 2 in Winter Term 2020/2021 ("Rechnernetze Labor 2") at the University of Applied Sciences (www.hs-esslingen.de).

More information about the lab can be found at www.ktlab.de.

A specification of the SIMPLE application protocol between client and server can be found below.

Contact: michael.scharf@ktlab.de

## Usage

The ANT build.xml file builds one JAR for each application:

`$ ant`

Applications can be launched from a shell as follows:

`$ java -jar dist/<ClassName>.jar`


# String Interactive Messaging Protocol with Length Encoding (SIMPLE)

## SIMPLE Protocol Specification

### Protocol Design

String Interactive Messaging Protocol with Length Encoding (SIMPLE) is a straightforward client-server protocol based on the pre-standard HTTP version HTTP/0.9.

The objective of SIMPLE is to perform client-server text message exchanges with defined message length. The protocol encodes the message length in bytes inside the message body. Otherwise, the message only contains empty text, but the text is conveniently formated to facilitate analysis. 

SIMPLE uses a small subset of the HTTP/0.9 functions. A SIMPLE server can be queried from a HTTP-based Web browser, and a SIMPLE client can communicate with a HTTP server.
* SIMPLE only consists of GET requests and corresponding responses
* Messages are always encoded in ASCII text
* SIMPLE always uses CRLF ("\r\n")

URIs in SIMPLE only consist of one, two, or three parameter using following format:

	/<length>[?<iterations>[+<delay>]]

In order to be compatible with Web browsers, SIMPLE servers support both HTTP/0.9 and HTTP/1.0: SIMPLE servers send a reply with HTTP/1.0 header if the request uses HTTP/1.0 syntax. Otherwise, the reply uses no HTTP/1.0 header.


### Request

Requests consists of a single request line with the following formate:

	GET /<length>[?<iterations>[+<delay>]] [further characters]\r\n

The maximum length of a request line is 4095 byte.

Valid values are: 
* Length (in byte): 13 <= length <= 10000000
  - Requests length > 0 are accepted, but the minimum response size is always 13 byte
  - A request with length == 0 results in an error
* Iterations: iterations > 0 with the default iterations = 1
* Delay (in ms): delay >= 0 with the default delay = 0

Example for valid requests are:

Minimal request: `GET /13\r\n`

Longer request: `GET /400\r\n`

Request for 4 messages, each of 5000 byte: `GET /5000?4\r\n`

Complete request asking for 5 messages with a sleep duration of 2000 ms = 2 s: `GET /200?3+2000\r\n`

HTTP/1.0 compatibility: `GET /1000 HTTP/1.0\r\n`


### Response 

SIMPLE formats the text to an alignment of 80 bytes per line. Every 10 bytes are highlighted:

	12345678901234567890123456789012345678901234567890123456789012345678901234567890
	........._........._........._........._........._........._........._........\r\n

The last two chars on every line are CRLF ("\r\n").

The last 13 chars of every message encode the length and terminate by a '!': `00000xxxxx!\r\n`

If a HTTP/1.0 query is recieved, a SIMPLE server replies with a HTTP/1.0 header that includes 80 bytes.

### Errors

Errors are reported by `HTTP/1.0 400 Bad Request\r\n\r\n<Explanation>\r\n`


## Examples

### Example 1

Request:

	GET /400 

Reply:

	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._.......0000000400!

### Example 2

Request:

	GET /400 HTTP/1.0

Reply:

	HTTP/1.0 200 OK
	Server: Rechnernetze SimpleServer
	Content-type: text/plain
	
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._.......0000000400!

### Example 3

Request:

	GET /240?3+2000 HTTP/1.0

Reply:

	HTTP/1.0 200 OK
	Server: Rechnernetze SimpleServer
	Content-type: text/plain
	
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._.......0000000240!
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._.......0000000240!
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._........._........
	........._........._........._........._........._........._.......0000000240!


## References

* RFC 1949

* https://www.w3.org/Protocols/HTTP/AsImplemented.html

* https://www.w3.org/Addressing/BNF.html
