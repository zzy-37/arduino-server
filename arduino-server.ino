#include <ESPmDNS.h>
#include <WebServer.h>

WebServer server(80);

#include "assets.h"

const int led = 2;
WiFiClient client;

int remain_time = 20 * 1000;

void sse_send_time_event() {
  if (client.connected())
    client.printf("event: time\ndata: %d\n\n", remain_time);
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);

#if 1
  const char* ssid = "my board";
  const char* password = "12345679";
  if (!WiFi.softAP(ssid)) {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
#else
  const char* ssid = "FORMSHOP";
  const char* password = "formshop";
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", [] {
    server.send_P(200, "text/html", index_page, sizeof(index_page));
  });

  server.on("/on", [] {
    long time = server.arg(0).toInt();
    digitalWrite(led, HIGH);
    server.send(200, "text/plain", "OK");
  });

  server.on("/off", [] {
    digitalWrite(led, LOW);
    server.send(200, "text/plain", "OK");
  });

  server.on("/page2", [] {
    server.send_P(200, "text/html", page2_html, sizeof(page2_html));
  });

  server.on("/cat2.jpg", [] {
    server.send_P(200, "image/jpg", cat2_jpg, sizeof(cat2_jpg));
  });

  server.on("/events", [] {
    client = server.client();
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.sendContent_P(
      "HTTP/1.1 200 OK\n"
      "Content-Type: text/event-stream;\n"
      "Connection: keep-alive\n"
      "\n");
  });

  server.onNotFound([] {
    server.send(404, "text/html", "<h1>Not Found</h1>");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  unsigned long now = millis();
  static unsigned long last_send_time;
  if (now > last_send_time + 2000) {
    last_send_time = now;
    sse_send_time_event();
  }
}