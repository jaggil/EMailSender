/*
 * EMail Sender Arduino, esp8266 and esp32 library to send email
 *
 * AUTHOR:  Renzo Mischianti
 * VERSION: 2.0.0
 *
 * https://www.mischianti.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Renzo Mischianti www.mischianti.org All right reserved.
 *
 * You may copy, alter and reuse this code in any way you like, but please leave
 * reference to www.mischianti.org in your comments if you redistribute this code.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef EMailSender_h
#define EMailSender_h

// Uncomment if you use esp8266 core <= 2.4.2
// #define ARDUINO_ESP8266_RELEASE_2_4_2
//#define ESP8266_GT_2_4_2_SD_STORAGE_SELECTED

#define ENABLE_ATTACHMENTS

// Uncomment to enable printing out nice debug messages.
 #define EMAIL_SENDER_DEBUG

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

#define NETWORK_ESP8266_ASYNC (0)
#define NETWORK_ESP8266 (1)
#define NETWORK_W5100 (2)
#define NETWORK_ENC28J60 (3)
#define NETWORK_ESP32 (4)
#define NETWORK_ESP32_ETH (5)

#define DEFAULT_EMAIL_NETWORK_TYPE_ESP8266 	NETWORK_ESP8266
#define DEFAULT_EMAIL_NETWORK_TYPE_ESP32 	NETWORK_ESP32
#define DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO 	NETWORK_ENC28J60

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#if !defined(EMAIL_NETWORK_TYPE)
// select Network type based
	#if defined(ESP8266) || defined(ESP31B)
		//#define STORAGE_SPIFFS_ENABLED
		#define EMAIL_NETWORK_TYPE DEFAULT_EMAIL_NETWORK_TYPE_ESP8266
	#elif defined(ESP32)
		#define EMAIL_NETWORK_TYPE DEFAULT_EMAIL_NETWORK_TYPE_ESP32
	#else
		#define EMAIL_NETWORK_TYPE DEFAULT_EMAIL_NETWORK_TYPE_ARDUINO
	//	#define STORAGE_SD_ENABLED
	#endif
#endif

#if defined(ESP8266) || defined(ESP31B)
	#define STORAGE_SPIFFS_ENABLED

	#ifdef ARDUINO_ESP8266_RELEASE_2_4_2
		#define STORAGE_SD_ENABLED
	#else

		#ifdef ESP8266_GT_2_4_2_SD_STORAGE_SELECTED
			#define STORAGE_SD_ENABLED
		#else
			#define STORAGE_SPIFFS_ENABLED
		#endif

	#endif

#elif defined(ESP32)
	#define STORAGE_SPIFFS_ENABLED
	#define STORAGE_SD_ENABLED
#else
	#define STORAGE_SD_ENABLED
#endif


// Includes and defined based on Network Type
#if(EMAIL_NETWORK_TYPE == NETWORK_ESP8266_ASYNC)

// Note:
//   No SSL/WSS support for client in Async mode
//   TLS lib need a sync interface!

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define EMAIL_NETWORK_CLASS WiFiClient
#define EMAIL_NETWORK_SSL_CLASS WiFiClientSecure
#define EMAIL_NETWORK_SERVER_CLASS WiFiServer

#elif defined(ESP31B)
#include <ESP31BWiFi.h>
#else
#error "network type ESP8266 ASYNC only possible on the ESP mcu!"
#endif
//
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncTCPbuffer.h>
//#define EMAIL_NETWORK_CLASS AsyncTCPbuffer
//#define EMAIL_NETWORK_SERVER_CLASS AsyncServer

#elif(EMAIL_NETWORK_TYPE == NETWORK_ESP8266)

#if !defined(ESP8266) && !defined(ESP31B)
#error "network type ESP8266 only possible on the ESP mcu!"
#endif

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <ESP31BWiFi.h>
#endif
#define EMAIL_NETWORK_CLASS WiFiClient
#define EMAIL_NETWORK_SSL_CLASS WiFiClientSecure
#define EMAIL_NETWORK_SERVER_CLASS WiFiServer

#elif(EMAIL_NETWORK_TYPE == NETWORK_W5100)

#ifdef STM32_DEVICE
#define EMAIL_NETWORK_CLASS TCPClient
#define EMAIL_NETWORK_SERVER_CLASS TCPServer
#else
#include <Ethernet.h>
#include <SPI.h>
#define EMAIL_NETWORK_CLASS EthernetClient
#define EMAIL_NETWORK_SERVER_CLASS EthernetServer
#endif

#elif(EMAIL_NETWORK_TYPE == NETWORK_ENC28J60)

#include <UIPEthernet.h>

#define EMAIL_NETWORK_CLASS UIPClient
#define EMAIL_NETWORK_SERVER_CLASS UIPServer

//#include <UIPEthernet.h>
//UIPClient base_client;
//SSLClient client(base_client, TAs, (size_t)TAs_NUM, A5);
//
//#define EMAIL_NETWORK_CLASS SSLClient
//#define EMAIL_NETWORK_SERVER_CLASS UIPServer

#elif(EMAIL_NETWORK_TYPE == NETWORK_ESP32)

#include <WiFi.h>
#include <WiFiClientSecure.h>
#define EMAIL_NETWORK_CLASS WiFiClient
#define EMAIL_NETWORK_SSL_CLASS WiFiClientSecure
#define EMAIL_NETWORK_SERVER_CLASS WiFiServer

#elif(EMAIL_NETWORK_TYPE == NETWORK_ESP32_ETH)

#include <ETH.h>
#define EMAIL_NETWORK_CLASS WiFiClient
#define EMAIL_NETWORK_SERVER_CLASS WiFiServer

#else
#error "no network type selected!"
#endif

#ifdef ENABLE_ATTACHMENTS
	#ifdef STORAGE_SPIFFS_ENABLED
		#if defined(ESP32)
			#include <SPIFFS.h>
		#else
			#define FS_NO_GLOBALS
			#include "FS.h"
		#endif
	#endif

	#ifdef STORAGE_SD_ENABLED
		#include "SD.h"
	#endif
#endif

#ifdef EMAIL_NETWORK_SSL_CLASS
#define EMAIL_NETWORK_CLASS EMAIL_NETWORK_SSL_CLASS
#endif

#define OPEN_CLOSE_SPIFFS
#define OPEN_CLOSE_SD

// Setup debug printing macros.
#ifdef EMAIL_SENDER_DEBUG
	#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
#endif

class EMailSender {
public:
	EMailSender(const char* email_login, const char* email_password, const char* email_from, const char* smtp_server, uint16_t smtp_port, bool isSecure = false);
	EMailSender(const char* email_login, const char* email_password, const char* email_from, bool isSecure = false);
	EMailSender(const char* email_login, const char* email_password, bool isSecure = false);

	enum StorageType {
		EMAIL_STORAGE_TYPE_SPIFFS,
		EMAIL_STORAGE_TYPE_SD
	};

#define MIME_TEXT_HTML F("text/html")
#define MIME_TEXT_PLAIN F("text/plain")
#define MIME_IMAGE_JPG F("image/jpg")
#define MIME_IMAGE_PNG F("image/png")

	typedef struct {
		String mime = "text/html";
		String subject;
		String message;
	} EMailMessage;

	typedef struct {
		StorageType storageType = EMAIL_STORAGE_TYPE_SD;
		String mime;
		bool encode64 = false;
		String filename;
		String url;
	} FileDescriptior;

	typedef struct {
		byte number;
		FileDescriptior *fileDescriptor;
	} Attachments;

	typedef struct {
		String code;
		String desc;
		bool status = false;
	} Response;

	void setSMTPPort(uint16_t smtp_port);
	void setSMTPServer(const char* smtp_server);
	void setEMailLogin(const char* email_login);
	void setEMailFrom(const char* email_from);
	void setEMailPassword(const char* email_password);

	EMailSender::Response send(const char* to, EMailMessage &email, Attachments att = {0, {}});

	void setIsSecure(bool isSecure = false);

	void setUseAuth(bool useAuth = true) {
		this->useAuth = useAuth;
	}

	void setPublicIpDescriptor(const char *publicIpDescriptor = "mischianti") {
		publicIPDescriptor = publicIpDescriptor;
	}

private:
	uint16_t smtp_port = 465;
	char* smtp_server = strdup("smtp.gmail.com");
	char* email_login = 0;
	char* email_from  = 0;
	char* email_password = 0;

	const char* publicIPDescriptor = "mischianti";

	bool isSecure = false;

	bool useAuth = true;

    String _serverResponce;

    Response awaitSMTPResponse(EMAIL_NETWORK_CLASS &client, const char* resp = "", const char* respDesc = "", uint16_t timeOut = 10000);
};

#endif
