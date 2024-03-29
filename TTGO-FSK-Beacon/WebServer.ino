
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const byte DNS_PORT=53;
IPAddress apIP(8,8,8,8); // The default android DNS
DNSServer dnsserver;

AsyncWebServer webserver(80);

const char *apname="TTGO-FSK-BEACON";
const char *password="marsflightcrew";

int StartWebServer(void)
{ 
	WiFi.mode(WIFI_AP);
	WiFi.softAP(apname,password);
	WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
	
	// if DNSServer is started with "*" for domain name, it will reply with
	// provided IP to all DNS request
	dnsserver.start(DNS_PORT,"*",apIP);
	
	webserver.begin();

	webserver.on("/",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.print("Redirecting to config.html\r\n");
		request->redirect("/config.html");
	});	
	
	webserver.on("/config.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.println("Returning /config.html");
		request->send(SPIFFS,"/config.html");
	});	
	
	webserver.on("/config.css",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.println("Returning /config.css");
		request->send(SPIFFS,"/config.css");
	});	
	
	webserver.on("/config.js",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.println("Returning /config.js");
		request->send(SPIFFS,"/config.js");
	});
	
	webserver.on("/heartbeat.html",HTTP_GET,[](AsyncWebServerRequest *request)
	{
		Serial.println("Returning /heartbeat.html");
		request->send(SPIFFS,"/heartbeat.html");
	});	
	
	return(0);
}

void StopWebServer(void)
{
	dnsserver.stop();
	webserver.end();
	WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);
}

void PollWebServer(void)
{
	dnsserver.processNextRequest();
}
