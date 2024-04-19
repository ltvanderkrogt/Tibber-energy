// https://github.com/ltvanderkrogt/Tibber-energy

#include <ESP8266WiFi.h>

const char* ssid = "ssid";  // Voer hier je SSID in
const char* password = "password";  // Voer hier je wachtwoord in
const char* gqlEndpoint = "api.tibber.com";
const int gqlPort = 443;
const char* accessToken = "accessToken";  // Voer hier je Tibber API-toegangstoken in

// Initialisatie van de Wi-Fi-verbinding
void initWiFi() {
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connection successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Maak en voer een GraphQL-query uit
void executeGraphQLQuery(String query) {
  WiFiClientSecure client;
  client.setInsecure();

  Serial.println("Connecting to GraphQL endpoint...");
  if (!client.connect(gqlEndpoint, gqlPort)) {
    Serial.println("Connection failed");
    return;
  }

  Serial.println("Sending query...");
  client.println("POST /v1-beta/gql HTTP/1.1");
  client.println("Host: api.tibber.com");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Authorization: Bearer ");
  client.println(accessToken);
  client.print("Content-Length: ");
  client.println(query.length());
  client.println();
  client.println(query);

  Serial.println("Reading response...");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line); // Print elke responsregel voor debugging
    if (line == "\r") {
      break;
    }
  }

  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("");
  client.stop();
}

void setup() {
  Serial.begin(9600);
  initWiFi();

  // "query": "{__schema { types { name kind description fields { name description}}}}" 
  // "query": "{viewer {name }}"
  // "query": "{viewer {homes {currentSubscription {priceInfo {current {total}}}}}}"
  // "query": "{viewer {homes {currentSubscription {priceInfo {current {total energy tax startsAt }}}}}}"
  // "query": "{viewer {homes {address{address1 address2 address3 postalCode city country latitude longitude}}}}"
  // "query": "{viewer {homes {currentSubscription {priceInfo {current{total energy tax startsAt }today {total energy tax startsAt}tomorrow {total energy tax startsAt}}}}}}"
 
 String query = R"(
     {
     "query": "{viewer {homes {currentSubscription {priceInfo {current {total}}}}}}"
      }
  )";
         
  Serial.print("***************************");
  Serial.print(query);
  Serial.println("***************************");
  executeGraphQLQuery(query);
}

void loop() {}
