/* Pinos correspondente a ligação do ESP */
#define temperaturaSolo 15
#define dht11 4
#define umidadeSolo 2
#define bombaAgua 5
#define lampada 18

/* Pertinentes a conexão WIFI */
#include<WiFi.h>
char ssid[] = "Moto";
char password[] = "33333333";
WiFiServer server(80);

#include <dht.h>
#include <iostream>
#include <string>

/* Bibliotecas para o DS18B20 */
#include <OneWire.h> 
#include <DallasTemperature.h>

OneWire ourWire(temperaturaSolo);     // CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR DE TEMPERATURA A PROVA D'ÁGUA
DallasTemperature sensors(&ourWire); // BIBLIOTECA DallasTemperature UTILIZA A OneWire

dht DHT;

void setup() {

  pinMode(dht11, INPUT);
  pinMode(temperaturaSolo, INPUT);
  pinMode(umidadeSolo, INPUT);

  pinMode(bombaAgua, OUTPUT);
  pinMode(lampada, OUTPUT);
  digitalWrite(bombaAgua, LOW);
  digitalWrite(lampada, LOW);
  
  sensors.begin(); //INICIA O SENSOR
  Serial.begin(115200);

  /* Pertinentes a conexão wifi */
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi conectada");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
  delay(3000);
  
}

int temperaturaDoSoloLida = 0;
int temperaturaDoArLida = 0;
int umidadeDoSoloLida = 0;
int umidadeDoArLida = 0;

void loop() {
  DHT.read11(dht11);
  
  sensors.requestTemperatures();
  
  /* Realizando a Leitura da Temperatura do Solo */
  temperaturaDoSoloLida = sensors.getTempCByIndex(0);

  /* Realizando a Leitura da Temperatura do Ar */
  temperaturaDoArLida = DHT.temperature;

  /* Realizando a Leitura da Umidade do Solo */
  umidadeDoSoloLida = constrain(analogRead(umidadeSolo),0,2000);
  umidadeDoSoloLida = map(umidadeDoSoloLida,0,2000,100,0);

  /* Realizando a Leitura da Umidade do Ar */
  umidadeDoArLida = DHT.humidity;
    
  WiFiClient client = server.available();
    if(client){
    Serial.println("Novo cliente.");
    String linhaCorrente="";
    
    while (client.connected()) {
      
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        
        if (c == '\n') {
        
          if(linhaCorrente.length() == 0){
            run_html(client);
            break;
          } else {
            linhaCorrente= "";            
          }
          
         } else if (c != '\r') {
            linhaCorrente += c;
         }

        if (linhaCorrente.endsWith("GET /bomba")){
          digitalWrite(bombaAgua, !digitalRead(bombaAgua));
        }

        if (linhaCorrente.endsWith("GET /lampada")){
          digitalWrite(lampada, !digitalRead(lampada)); 
        }
      
         /* Para o acinamento automático do relé */
         
        /*
        if(temperaturaDoSoloLida < 20){
          digitalWrite(lampada, HIGH);
        }
        else{
          digitalWrite(lampada, LOW);
        }
        
        if(umidadeDoSoloLida <50){
          digitalWrite(bombaAgua, HIGH);
        }else{
          digitalWrite(bombaAgua, LOW);
        }*/
      }
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}

void run_html(WiFiClient client){
  
    /* Sensor de umidade e temperatura (DHT11) */
    DHT.read11(dht11);
    
    sensors.requestTemperatures();
    /* Realizando a Leitura da Temperatura do Solo */
    temperaturaDoSoloLida = sensors.getTempCByIndex(0);
  
    /* Realizando a Leitura da Temperatura do Ar */
    temperaturaDoArLida = DHT.temperature;
  
    /* Realizando a Leitura da Umidade do Solo */
    umidadeDoSoloLida = constrain(analogRead(umidadeSolo),0,2000);
    umidadeDoSoloLida = map(umidadeDoSoloLida,0,2000,100,0);
  
    /* Realizando a Leitura da Umidade do Ar */
    umidadeDoArLida = DHT.humidity;

  
    String html = 
    
    "<!DOCTYPE html>" \
    "<html lang='en'>" \
    "<head>" \
    "<meta charset='UTF-8'>" \
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>" \
    "<script src = 'script.js'></script>" \
    "<meta http-equiv='X-UA-Compatible' content='ie=edge'>" \
    "<link href='https://fonts.googleapis.com/css?family=Montserrat:400,600,700&display=swap' rel='stylesheet'>" \
    "<link rel='stylesheet' href='css/style.css'>" \
    "<link rel='shortcut icon' type='image/png' href='data:image/png;base64,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'>" \
    
    "<title>Plantação automática</title>" \
    "<style>" \
    ":root {" \
     " --color-primary-greenblue: #9be3de;" \
     "--color-secondary-lightblue: #beebe9;" \
     "--color-accent-pink: #ffe3ed;" \
     " --color-accent-yellow: #fffdf9;" \
     " --color-grey-light-1: #faf9f9;" \
     " --color-grey-light-2: #f4f2f2;" \
     " --color-grey-light-3: #f0eeee;" \
     " --shadow-dark: 0 2rem 6rem rgba(0, 0, 0, .3);" \
     " --shadow-light: 0 2rem 5rem rgba(0, 0, 0, .06);" \
     " --line: 1px solid var(--color-grey-light-2); }" \
    
    "* {" \
    "  margin: 0;" \
     " padding: 0; }" \
    
    "*," \
    "*::before," \
    "*::after {" \
    "  box-sizing: inherit; }" \
    
    "::selection {" \
    "  background-color: var(--color-accent-pink); }" \
    
    "html {" \
    "  box-sizing: border-box;" \
    "  font-size: 62.5%; }" \
    "  @media only screen and (max-width: 68.75em) {" \
    "    html {" \
    "      font-size: 50%; } }" \
    
    "body {" \
    "  font-family: 'Montserrat', sans-serif;" \
    "  font-weight: 400;" \
    "  background-image: linear-gradient(to right bottom, var(--color-secondary-lightblue), var(--color-primary-greenblue));" \
    "  background-size: cover;" \
    "  background-repeat: no-repeat;" \
    "  min-height: 100vh; }" \
    
    ".container {" \
    "  max-width: 120rem;" \
    "  margin: 8rem auto;" \
    "  background-color: var(--color-grey-light-1);" \
    "  box-shadow: var(--shadow-dark);" \
    "  min-height: 50rem; }" \
    "  @media only screen and (max-width: 75em) {" \
    "    .container {" \
    "      margin: 0;" \
    "      max-width: 100%;" \
    "      width: 100%; } }" \
    
    ".header {" \
      "font-size: 1.4rem;" \
      "height: 7rem;" \
      "background-color: #fff;" \
      "border-bottom: var(--line);" \
      "display: flex;" \
      "justify-content: space-between;" \
      "align-items: center; }" \
    
    ".content {" \
    "  display: flex; }" \
    "  @media only screen and (max-width: 56.25em) {" \
    "    .content {" \
    "      flex-direction: column; } }" \
    
    ".plantation-view {" \
    "  background-color: #fff;" \
    "  flex: 1; }" \
    
    ".detail {" \
    "  display: flex;" \
    "  padding: 2rem;" \
    "  background-color: var(--color-grey-light-1); }" \
    "  @media only screen and (max-width: 56.25em) {" \
    "    .detail {" \
    "      padding: 3rem;" \
    "      flex-direction: column; } }" \
    
    ".data-reviews {" \
    "  border-right: var(--line);" \
    "  display: flex;" \
    "  flex-direction: column;" \
    "  align-items: center;" \
    "  flex-wrap: wrap;" \
    "  max-height: 50rem;" \
    "  flex: 0 0 55%; }" \
    "  @media only screen and (max-width: 56.25em) {" \
    "    .data-reviews {" \
    "      border-right: none;" \
    "      border-bottom: var(--line);" \
    "      margin-bottom: 2rem; } }" \
    "  @media only screen and (max-width: 37.5em) {" \
    "    .data-reviews {" \
    "      max-height: 100%; } }" \
    
    ".user-controls {" \
    "  flex: 1;" \
    "  display: flex;" \
    "  flex-direction: column;" \
    "  align-items: center; }" \
    
    ".logo {" \
    "  height: 4.25rem;" \
    "  margin-left: 2rem; }" \
    
    ".user-nav {" \
    "  align-self: stretch;" \
    "  display: flex;" \
    "  align-items: center; }" \
    "  .user-nav > * {" \
    "    padding: 0 2rem;" \
    "    cursor: pointer;" \
    "    height: 100%;" \
    "    display: flex;" \
    "    align-items: center; }" \
    " .user-nav > *:hover {" \
    "  background-color: var(--color-grey-light-2); }" \
    ".user-nav__icon-box {" \
    " position: relative; }" \
    ".user-nav__icon {" \
    " height: 4.25rem;" \
        "width: 4.25rem; }" \
    
    ".review {" \
     " background-color: #fff;" \
     " box-shadow: var(--shadow-light);" \
     " padding: 3rem;" \
     " width: 25rem;" \
     " margin-bottom: 3.5rem;" \
     " position: relative;" \
     " overflow: hidden; }" \
     " .review__svg {" \
     "   height: 8rem;" \
     "   width: 8rem;" \
     "   margin-bottom: 1.5rem; }" \
      ".review__data {" \
        "display: flex;" \
        "flex-direction: column;" \
        "align-items: initial;" \
        "justify-content: center; }" \
      ".review__box {" \
      "  margin-right: auto; }" \
      ".review__name {" \
        "font-size: 1.5rem;" \
        "margin-bottom: 1.2rem; }" \
      ".review__estimate {" \
      "  font-size: 2.2rem;" \
      "  font-weight: 600;" \
      "  color: var(--color-primary-greenblue); }" \
    
    ".control {" \
      "background-color: #fff;" \
      "box-shadow: var(--shadow-light);" \
      "padding: 3rem;" \
      "margin-bottom: 3.5rem;" \
      "position: relative;" \
      "overflow: hidden;" \
      "width: 21rem;" \
      "height: 21rem;" \
      "border-radius: 100%; }" \
      ".control__user {" \
        "display: flex;" \
        "flex-direction: column;" \
        "align-items: center;" \
        "justify-content: center; }" \
      ".control__svg {" \
         "height: 8rem;" \
         "width: 8rem;" \
         "margin-bottom: 1.5rem; }" \
      ".control__name {" \
        "font-size: 1.5rem;" \
        "margin-bottom: 1.2rem; }" \
      ".control__estimate {" \
        "font-size: 2.2rem;" \
        "font-weight: 600;" \
        "color: var(--color-primary-greenblue); }" \
    
    ".legal {" \
      "display: flex;" \
      "justify-content: center;" \
      "font-size: 1.5rem;" \
      "font-weight: 500; }" \
      "@media only screen and (max-width: 56.25em) {" \
        ".legal { " \
           "display: none; } }" \    
    "</style>" \
    "</head>" \
    "<body>" \
    "    <div class='container'>" \
    "        <header class='header'>" \
    "            <img src='data:image/png;base64,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' alt='Logo' class='logo'>" \
    
    "            <nav class='user-nav'>" \
     "               <div class='user-nav__icon-box'>          " \      
      "                  <img src='data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA4MCA4MCIgd2lkdGg9IjE2MHB4IiBoZWlnaHQ9IjE2MHB4Ij48cGF0aCBmaWxsPSIjZTFlYmYyIiBkPSJNNjYuNSA1LjVINzcuNVY3NC41SDY2LjV6Ii8+PHBhdGggZmlsbD0iI2EzYjdjYyIgZD0iTTc3LDZ2NjhINjdWNkg3NyBNNzgsNUg2NnY3MGgxMlY1TDc4LDV6Ii8+PHBhdGggZmlsbD0iI2UxZWJmMiIgZD0iTTUwLjUgMjEuNUg2MS41Vjc0LjVINTAuNXoiLz48cGF0aCBmaWxsPSIjYTNiN2NjIiBkPSJNNjEsMjJ2NTJINTFWMjJINjEgTTYyLDIxSDUwdjU0aDEyVjIxTDYyLDIxeiIvPjxwYXRoIGZpbGw9IiNiYWUwYmQiIGQ9Ik0zNC41IDM1LjVINDUuNVY3NC41SDM0LjV6Ii8+PHBhdGggZmlsbD0iIzVlOWM3NiIgZD0iTTQ1LDM2djM4SDM1VjM2SDQ1IE00NiwzNUgzNHY0MGgxMlYzNUw0NiwzNXoiLz48Zz48cGF0aCBmaWxsPSIjYmFlMGJkIiBkPSJNMTguNSA0OS41SDI5LjVWNzQuNUgxOC41eiIvPjxwYXRoIGZpbGw9IiM1ZTljNzYiIGQ9Ik0yOSw1MHYyNEgxOVY1MEgyOSBNMzAsNDlIMTh2MjZoMTJWNDlMMzAsNDl6Ii8+PC9nPjxnPjxwYXRoIGZpbGw9IiNiYWUwYmQiIGQ9Ik0yLjUgNTkuNUgxMy41Vjc0LjVIMi41eiIvPjxwYXRoIGZpbGw9IiM1ZTljNzYiIGQ9Ik0xMyw2MHYxNEgzVjYwSDEzIE0xNCw1OUgydjE2aDEyVjU5TDE0LDU5eiIvPjwvZz48L3N2Zz4=' class='user-nav__icon'>" \
       "             </div>" \
        "        </nav>" \
         "   </header>" \
            
     "       <div class='content'>" \
      "          <main class='plantation-view'>" \
       "             <div class='detail'>" \
        "                <div class='data-reviews'>" \
         "                   <figure class='review'>" \
          "                      <figcaption class='review__data'>" \
           "                         <img src='data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA4MCA4MCIgd2lkdGg9IjE2MHB4IiBoZWlnaHQ9IjE2MHB4Ij48cGF0aCBmaWxsPSIjOGJiN2YwIiBkPSJNMzMsNjQuNUMxNS42MzEsNjQuNSwxLjUsNTAuMzY5LDEuNSwzM1MxNS42MzEsMS41LDMzLDEuNVM2NC41LDE1LjYzMSw2NC41LDMzUzUwLjM2OSw2NC41LDMzLDY0LjUgeiIvPjxwYXRoIGZpbGw9IiM0ZTdhYjUiIGQ9Ik0zMywyYzE3LjA5MywwLDMxLDEzLjkwNywzMSwzMVM1MC4wOTMsNjQsMzMsNjRTMiw1MC4wOTMsMiwzM1MxNS45MDcsMiwzMywyIE0zMywxIEMxNS4zMjcsMSwxLDE1LjMyNywxLDMzczE0LjMyNywzMiwzMiwzMnMzMi0xNC4zMjcsMzItMzJTNTAuNjczLDEsMzMsMUwzMywxeiIvPjxwYXRoIGZpbGw9IiNmZmYiIGQ9Ik0zMyw1Ny41QzE5LjQ5MSw1Ny41LDguNSw0Ni41MDksOC41LDMzUzE5LjQ5MSw4LjUsMzMsOC41UzU3LjUsMTkuNDkxLDU3LjUsMzNTNDYuNTA5LDU3LjUsMzMsNTcuNSB6Ii8+PHBhdGggZmlsbD0iIzRlN2FiNSIgZD0iTTMzLDljMTMuMjU1LDAsMjQsMTAuNzQ1LDI0LDI0UzQ2LjI1NSw1NywzMyw1N1M5LDQ2LjI1NSw5LDMzUzE5Ljc0NSw5LDMzLDkgTTMzLDggQzE5LjIxNSw4LDgsMTkuMjE1LDgsMzNzMTEuMjE1LDI1LDI1LDI1czI1LTExLjIxNSwyNS0yNVM0Ni43ODUsOCwzMyw4TDMzLDh6Ii8+PHBhdGggZmlsbD0iI2MyZThmZiIgZD0iTTY0LjUsNzguNWMtNy43MiwwLTE0LTYuNDY0LTE0LTE0LjQwOWMwLTguMzcsMTEuNzIyLTIyLjYzLDEzLjk5Ny0yNS4zMTggQzY2Ljc2Niw0MS40OTEsNzguNSw1NS45NDUsNzguNSw2NC4wOTFDNzguNSw3Mi4wMzYsNzIuMjIsNzguNSw2NC41LDc4LjV6Ii8+PHBhdGggZmlsbD0iIzc0OTZjNCIgZD0iTTY0LjQ5NSwzOS41NTNDNjcuNDM4LDQzLjEyOCw3OCw1Ni40OSw3OCw2NC4wOTFDNzgsNzEuNzYsNzEuOTQ0LDc4LDY0LjUsNzhTNTEsNzEuNzYsNTEsNjQuMDkxIEM1MSw1Ni4yODEsNjEuNTQ0LDQzLjA5LDY0LjQ5NSwzOS41NTMgTTY0LjUsMzhjMCwwLTE0LjUsMTYuNTcxLTE0LjUsMjYuMDkxQzUwLDcyLjMyNSw1Ni40OTIsNzksNjQuNSw3OVM3OSw3Mi4zMjUsNzksNjQuMDkxIEM3OSw1NC44MjcsNjQuNSwzOCw2NC41LDM4TDY0LjUsMzh6Ii8+PHBhdGggZmlsbD0iI2I2YzlkNiIgZD0iTTE5LjU5LDQzLjQxQzE3LjM0OSw0MC41MywxNiwzNi45MjMsMTYsMzNjMC05LjM3NCw3LjYyNi0xNywxNy0xN3MxNyw3LjYyNiwxNywxNwljMCwzLjkyMy0xLjM0OSw3LjUzLTMuNTksMTAuNDFsMS40MzMsMS40MzNDNTAuNDM5LDQxLjU5NCw1MiwzNy40ODIsNTIsMzNjMC0xMC40OTMtOC41MDctMTktMTktMTlzLTE5LDguNTA3LTE5LDE5CWMwLDQuNDgyLDEuNTYxLDguNTk0LDQuMTU3LDExLjg0M0wxOS41OSw0My40MXoiLz48cGF0aCBmaWxsPSIjMzY0MDRkIiBkPSJNMjQuMDkyIDE2LjkzMUwyNC41MDMgMTUuMjM4IDI2LjA3NiAxNS45ODEgMzguMzA1IDQxLjU0OCAzNi4zMiA0Mi40OTd6Ii8+PGc+PHBhdGggZmlsbD0iIzhiYjdmMCIgZD0iTTMzIDI5LjVBMy41IDMuNSAwIDEgMCAzMyAzNi41QTMuNSAzLjUgMCAxIDAgMzMgMjkuNVoiLz48cGF0aCBmaWxsPSIjNGU3YWI1IiBkPSJNMzMsMzBjMS42NTQsMCwzLDEuMzQ2LDMsM3MtMS4zNDYsMy0zLDNzLTMtMS4zNDYtMy0zUzMxLjM0NiwzMCwzMywzMCBNMzMsMjkgYy0yLjIwOSwwLTQsMS43OTEtNCw0czEuNzkxLDQsNCw0czQtMS43OTEsNC00UzM1LjIwOSwyOSwzMywyOUwzMywyOXoiLz48L2c+PC9zdmc+' alt='Humidity' class='review__svg'>" \
            "                        <div class='review__box'>" \
             "                           <p class='review__name'>Umidade do ar</p>" \
              "                      </div>" \
               "                     <div id='umiAr' class='review__estimate'>" + String(umidadeDoArLida) + "%</div>" \
                "                </figcaption>" \
                 "           </figure>" \
        
                  "          <figure class='review'>" \
                   "             <figcaption class='review__data'>" \
                    "                <img src='data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA0MCA0MCIgd2lkdGg9IjEyMHB4IiBoZWlnaHQ9IjEyMHB4Ij48cGF0aCBmaWxsPSIjZGZmMGZlIiBkPSJNMjAsMjIuMjE0Yy0yLjQ4MSwwLTQuNS0yLjAxOS00LjUtNC41VjZjMC0yLjQ4MSwyLjAxOS00LjUsNC41LTQuNXM0LjUsMi4wMTksNC41LDQuNXYxMS43MTQgQzI0LjUsMjAuMTk2LDIyLjQ4MSwyMi4yMTQsMjAsMjIuMjE0eiIvPjxwYXRoIGZpbGw9IiM0Nzg4YzciIGQ9Ik0yMCwyYzIuMjA2LDAsNCwxLjc5NCw0LDR2MTEuNzE0YzAsMi4yMDYtMS43OTQsNC00LDRzLTQtMS43OTQtNC00VjZDMTYsMy43OTQsMTcuNzk0LDIsMjAsMiBNMjAsMWMtMi43NSwwLTUsMi4yNS01LDV2MTEuNzE0YzAsMi43NSwyLjI1LDUsNSw1czUtMi4yNSw1LTVWNkMyNSwzLjI1LDIyLjc1LDEsMjAsMUwyMCwxeiIvPjxwYXRoIGZpbGw9IiM5OGNjZmQiIGQ9Ik0yMCwzOC41Yy00LjEzNiwwLTcuNS0zLjM2NC03LjUtNy41YzAtMi4yODMsMS4wMjUtNC40MTUsMi44MTMtNS44NUwxNS41LDI1di05LjVoOVYyNWwwLjE4NywwLjE1IEMyNi40NzUsMjYuNTg1LDI3LjUsMjguNzE3LDI3LjUsMzFDMjcuNSwzNS4xMzYsMjQuMTM2LDM4LjUsMjAsMzguNXoiLz48cGF0aCBmaWxsPSIjNDc4OGM3IiBkPSJNMjQsMTZ2OC43NnYwLjQ4bDAuMzc0LDAuM0MyNi4wNDMsMjYuODc5LDI3LDI4Ljg2OSwyNywzMWMwLDMuODYtMy4xNCw3LTcsN3MtNy0zLjE0LTctNyBjMC0yLjEzMSwwLjk1Ny00LjEyMSwyLjYyNi01LjQ2TDE2LDI1LjI0di0wLjQ4VjE2SDI0IE0yNSwxNUgxNXY5Ljc2Yy0xLjgyNywxLjQ2Ni0zLDMuNzE0LTMsNi4yNGMwLDQuNDE4LDMuNTgyLDgsOCw4IHM4LTMuNTgyLDgtOGMwLTIuNTI1LTEuMTczLTQuNzczLTMtNi4yNFYxNUwyNSwxNXoiLz48cGF0aCBmaWxsPSIjNDc4OGM3IiBkPSJNMjAuNSAxOUgyNXYxaC00LjVjLS4yNzYgMC0uNS0uMjI0LS41LS41djBDMjAgMTkuMjI0IDIwLjIyNCAxOSAyMC41IDE5ek0yMC41IDIzSDI1djFoLTQuNWMtLjI3NiAwLS41LS4yMjQtLjUtLjV2MEMyMCAyMy4yMjQgMjAuMjI0IDIzIDIwLjUgMjN6TTIwLjUgMTFIMjV2MWgtNC41Yy0uMjc2IDAtLjUtLjIyNC0uNS0uNXYwQzIwIDExLjIyNCAyMC4yMjQgMTEgMjAuNSAxMXpNMjAuNSA3SDI1djFoLTQuNUMyMC4yMjQgOCAyMCA3Ljc3NiAyMCA3LjV2MEMyMCA3LjIyNCAyMC4yMjQgNyAyMC41IDd6Ii8+PC9zdmc+' alt='Temperature' class='review__svg'>" \
                     "               <div class='review__box'>" \
                      "                  <p class='review__name'>Temperatura do ar</p>" \
                       "             </div>" \
                        "            <div  id='tempAr' class='review__estimate'>" + String(temperaturaDoArLida) + "ºC</div>" \
                         "       </figcaption>" \
                          "  </figure>" \
        
                           " <figure class='review'>" \
    "                            <figcaption class='review__data'>" \
     "                               <img src='data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB3aWR0aD0iMTI4cHQiIGhlaWdodD0iMTI4cHQiIHZpZXdCb3g9IjAgMCAxMjggMTI4IiB2ZXJzaW9uPSIxLjEiPgo8ZyBpZD0ic3VyZmFjZTQwMjg0NSI+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNjUuMDk4MDM5JSw0NC4zMTM3MjUlLDMwLjU4ODIzNSUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAxMjYuMTAxNTYyIDc2LjgwMDc4MSBDIDEyNS41IDc2LjE5OTIxOSAxMjQuODAwNzgxIDc1Ljg5ODQzOCAxMjQgNzUuODk4NDM4IEwgODkgNzUuODk4NDM4IEMgODUuMTk5MjE5IDc1Ljg5ODQzOCA4MS4zOTg0MzggNzQuNjAxNTYyIDc4LjUgNzIuMTAxNTYyIEwgNzguMzAwNzgxIDcyIEMgNzQuMzAwNzgxIDY4LjY5OTIxOSA2OS4xOTkyMTkgNjYuODk4NDM4IDY0IDY2Ljg5ODQzOCBDIDU4LjgwMDc4MSA2Ni44OTg0MzggNTMuNjk5MjE5IDY4LjY5OTIxOSA0OS42OTkyMTkgNzIgTCA0OS42MDE1NjIgNzIuMTAxNTYyIEMgNDYuNjAxNTYyIDc0LjUgNDIuODk4NDM4IDc1Ljg5ODQzOCAzOS4xMDE1NjIgNzUuODk4NDM4IEwgNC4xMDE1NjIgNzUuODk4NDM4IEMgMi4zOTg0MzggNzUuODk4NDM4IDEuMTAxNTYyIDc3LjE5OTIxOSAxLjEwMTU2MiA3OC44OTg0MzggTCAxLjEwMTU2MiAxMjQgQyAxLjEwMTU2MiAxMjUuNjk5MjE5IDIuMzk4NDM4IDEyNyA0LjEwMTU2MiAxMjcgTCAxMjQuMTAxNTYyIDEyNyBDIDEyNS44MDA3ODEgMTI3IDEyNy4xMDE1NjIgMTI1LjY5OTIxOSAxMjcuMTAxNTYyIDEyNCBMIDEyNy4xMDE1NjIgNzguODk4NDM4IEMgMTI3IDc4LjEwMTU2MiAxMjYuNjk5MjE5IDc3LjMwMDc4MSAxMjYuMTAxNTYyIDc2LjgwMDc4MSBaIE0gMTI2LjEwMTU2MiA3Ni44MDA3ODEgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoMTAwJSwxMDAlLDEwMCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA1OC4zOTg0MzggMTA4LjM5ODQzOCBDIDU3LjYwMTU2MiAxMDguMzk4NDM4IDU2Ljg5ODQzOCAxMDguMTAxNTYyIDU2LjMwMDc4MSAxMDcuNSBDIDU1LjEwMTU2MiAxMDYuMzAwNzgxIDU1LjEwMTU2MiAxMDQuMzk4NDM4IDU2LjMwMDc4MSAxMDMuMzAwNzgxIEwgNjAuMTk5MjE5IDk5LjM5ODQzOCBDIDU5LjY5OTIxOSA5Ni4zMDA3ODEgNTkuMTk5MjE5IDg5LjE5OTIxOSA2MC44OTg0MzggNzcuMzk4NDM4IEMgNjEuMTAxNTYyIDc1LjgwMDc4MSA2Mi42OTkyMTkgNzQuNjAxNTYyIDY0LjMwMDc4MSA3NC44OTg0MzggQyA2NS44OTg0MzggNzUuMTAxNTYyIDY3LjEwMTU2MiA3Ni42OTkyMTkgNjYuODAwNzgxIDc4LjMwMDc4MSBDIDY0LjY5OTIxOSA5Mi44OTg0MzggNjYuMzAwNzgxIDk5LjYwMTU2MiA2Ni4zMDA3ODEgOTkuNjAxNTYyIEMgNjYuNjAxNTYyIDEwMC42MDE1NjIgNjYuMzAwNzgxIDEwMS42OTkyMTkgNjUuNSAxMDIuNSBMIDYwLjUgMTA3LjUgQyA1OS44OTg0MzggMTA4LjE5OTIxOSA1OS4xMDE1NjIgMTA4LjM5ODQzOCA1OC4zOTg0MzggMTA4LjM5ODQzOCBaIE0gNTguMzk4NDM4IDEwOC4zOTg0MzggIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoMTAwJSwxMDAlLDEwMCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA3My4zOTg0MzggMTEzLjM5ODQzOCBDIDcyLjYwMTU2MiAxMTMuMzk4NDM4IDcxLjg5ODQzOCAxMTMuMTAxNTYyIDcxLjMwMDc4MSAxMTIuNSBMIDYxLjMwMDc4MSAxMDIuNSBDIDYwLjEwMTU2MiAxMDEuMzAwNzgxIDYwLjEwMTU2MiA5OS4zOTg0MzggNjEuMzAwNzgxIDk4LjMwMDc4MSBDIDYyLjUgOTcuMTAxNTYyIDY0LjM5ODQzOCA5Ny4xMDE1NjIgNjUuNSA5OC4zMDA3ODEgTCA3NS41IDEwOC4zMDA3ODEgQyA3Ni42OTkyMTkgMTA5LjUgNzYuNjk5MjE5IDExMS4zOTg0MzggNzUuNSAxMTIuNSBDIDc0Ljg5ODQzOCAxMTMuMTk5MjE5IDc0LjEwMTU2MiAxMTMuMzk4NDM4IDczLjM5ODQzOCAxMTMuMzk4NDM4IFogTSA3My4zOTg0MzggMTEzLjM5ODQzOCAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYig0NC4zMTM3MjUlLDc2LjA3ODQzMSUsMTAwJSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDY4LjE5OTIxOSA0Mi4zMDA3ODEgQyA2Ny42OTkyMTkgNDAuNjk5MjE5IDY2IDM5LjgwMDc4MSA2NC41IDQwLjMwMDc4MSBDIDYyLjg5ODQzOCA0MC44MDA3ODEgNjIgNDIuNSA2Mi41IDQ0IEMgNjMuMzk4NDM4IDQ2Ljg5ODQzOCA2My42MDE1NjIgNTEuODk4NDM4IDYzIDU3LjUgQyA2Mi44MDA3ODEgNTkuMTAxNTYyIDY0IDYwLjYwMTU2MiA2NS42OTkyMTkgNjAuODAwNzgxIEMgNjUuODAwNzgxIDYwLjgwMDc4MSA2NS44OTg0MzggNjAuODAwNzgxIDY2IDYwLjgwMDc4MSBDIDY3LjUgNjAuODAwNzgxIDY4LjgwMDc4MSA1OS42MDE1NjIgNjkgNTguMTAxNTYyIEMgNjkuMzk4NDM4IDU0LjM5ODQzOCA2OS44MDA3ODEgNDcuMzAwNzgxIDY4LjE5OTIxOSA0Mi4zMDA3ODEgWiBNIDY2IDM1LjM5ODQzOCBDIDY4LjUgMjguNjAxNTYyIDY4LjEwMTU2MiAyMS4zMDA3ODEgNjUuMTAxNTYyIDE0LjgwMDc4MSBDIDYyLjEwMTU2MiA4LjMwMDc4MSA1Ni42MDE1NjIgMy4zMDA3ODEgNDkuODk4NDM4IDAuODAwNzgxIEMgNDguMzAwNzgxIDAuMTk5MjE5IDQ2LjYwMTU2MiAxIDQ2IDIuNjAxNTYyIEMgNDMuNSA5LjM5ODQzOCA0My44OTg0MzggMTYuNjk5MjE5IDQ2Ljg5ODQzOCAyMy4xOTkyMTkgQyA0OS44OTg0MzggMjkuNjk5MjE5IDU1LjM5ODQzOCAzNC42OTkyMTkgNjIuMTAxNTYyIDM3LjE5OTIxOSBDIDYyLjM5ODQzOCAzNy4zMDA3ODEgNjIuODAwNzgxIDM3LjM5ODQzOCA2My4xMDE1NjIgMzcuMzk4NDM4IEMgNjQuMzk4NDM4IDM3LjM5ODQzOCA2NS41IDM2LjYwMTU2MiA2NiAzNS4zOTg0MzggWiBNIDQ0LjYwMTU2MiAzNi4xMDE1NjIgQyA0MC4xOTkyMTkgMzUuNjk5MjE5IDM1Ljg5ODQzOCAzNy4xMDE1NjIgMzIuNjAxNTYyIDM5Ljg5ODQzOCBDIDMxLjMwMDc4MSA0MSAzMS4xOTkyMTkgNDIuODk4NDM4IDMyLjE5OTIxOSA0NC4xMDE1NjIgQyAzNSA0Ny41IDM5IDQ5LjUgNDMuMzk4NDM4IDQ5Ljg5ODQzOCBDIDQzLjg5ODQzOCA0OS44OTg0MzggNDQuMzk4NDM4IDUwIDQ0Ljg5ODQzOCA1MCBDIDQ4LjgwMDc4MSA1MCA1Mi41IDQ4LjY5OTIxOSA1NS41IDQ2LjEwMTU2MiBDIDU2LjgwMDc4MSA0NSA1Ni44OTg0MzggNDMuMTAxNTYyIDU1Ljg5ODQzOCA0MS44OTg0MzggQyA1MyAzOC41IDQ5IDM2LjUgNDQuNjAxNTYyIDM2LjEwMTU2MiBaIE0gNDQuNjAxNTYyIDM2LjEwMTU2MiAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYigxMDAlLDEwMCUsMTAwJSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDc1Ljg5ODQzOCAzNC42OTkyMTkgQyA3NS4zOTg0MzggMzQuNjAxNTYyIDc1IDM0LjEwMTU2MiA3NS4xMDE1NjIgMzMuNjAxNTYyIEMgNzcuNSAyMCA5MC4zOTg0MzggMTEgMTA0IDEzLjM5ODQzOCBDIDEwNC41IDEzLjUgMTA0Ljg5ODQzOCAxNCAxMDQuODAwNzgxIDE0LjUgQyAxMDIuMzk4NDM4IDI4IDg5LjUgMzcuMTAxNTYyIDc1Ljg5ODQzOCAzNC42OTkyMTkgWiBNIDc1Ljg5ODQzOCAzNC42OTkyMTkgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNDQuMzEzNzI1JSw3Ni4wNzg0MzElLDEwMCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA4MC4zMDA3ODEgMzguMTAxNTYyIEMgNzguNjk5MjE5IDM4LjEwMTU2MiA3NyAzOCA3NS4zOTg0MzggMzcuNjk5MjE5IEMgNzMuMzAwNzgxIDM3LjMwMDc4MSA3MS44MDA3ODEgMzUuMzAwNzgxIDcyLjE5OTIxOSAzMy4xMDE1NjIgQyA3My41IDI1LjY5OTIxOSA3Ny42MDE1NjIgMTkuMzAwNzgxIDgzLjY5OTIxOSAxNS4xMDE1NjIgQyA4OS44MDA3ODEgMTAuODAwNzgxIDk3LjE5OTIxOSA5LjE5OTIxOSAxMDQuNjAxNTYyIDEwLjUgQyAxMDYuNjk5MjE5IDEwLjg5ODQzOCAxMDguMTk5MjE5IDEyLjg5ODQzOCAxMDcuODAwNzgxIDE1LjEwMTU2MiBDIDEwNi41IDIyLjUgMTAyLjM5ODQzOCAyOC44OTg0MzggOTYuMzAwNzgxIDMzLjEwMTU2MiBDIDkxLjUgMzYuMzAwNzgxIDg2IDM4LjEwMTU2MiA4MC4zMDA3ODEgMzguMTAxNTYyIFogTSA3OC42MDE1NjIgMzIgQyA4My42OTkyMTkgMzIuMzk4NDM4IDg4LjY5OTIxOSAzMSA5Mi44OTg0MzggMjguMTAxNTYyIEMgOTcuMTAxNTYyIDI1LjE5OTIxOSAxMDAuMTAxNTYyIDIwLjg5ODQzOCAxMDEuMzk4NDM4IDE2IEMgOTYuMzAwNzgxIDE1LjYwMTU2MiA5MS4zMDA3ODEgMTcgODcuMTAxNTYyIDE5Ljg5ODQzOCBDIDgyLjg5ODQzOCAyMi44OTg0MzggODAgMjcuMTAxNTYyIDc4LjYwMTU2MiAzMiBaIE0gNzguNjAxNTYyIDMyICIvPgo8L2c+Cjwvc3ZnPgo=' alt='Humidity soil' class='review__svg'>" \
      "                              <div class='review__box'>" \
       "                                 <p class='review__name'>Umidade do solo</p>" \
        "                            </div>" \
         "                           <div  id='umiSolo' class='review__estimate'>" + String(umidadeDoSoloLida) + "%</div>" \
          "                      </figcaption>" \
           "                 </figure>" \
        
        "                    <figure class='review'>" \
         "                       <figcaption class='review__data'>" \
          "                          <img src='data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCA4MCA4MCIgd2lkdGg9IjgwcHgiIGhlaWdodD0iODBweCI+PHBhdGggZmlsbD0iI2ZmZDdiZCIgZD0iTTkgMTRINWMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEMxMCAxMy41NSA5LjU1IDE0IDkgMTR6TTE5IDE0aC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzIwIDEzLjU1IDE5LjU1IDE0IDE5IDE0ek0yOSAxNGgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEMzMCAxMy41NSAyOS41NSAxNCAyOSAxNHpNMzkgMTRoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDNDAgMTMuNTUgMzkuNTUgMTQgMzkgMTR6TTQ5IDE0aC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzUwIDEzLjU1IDQ5LjU1IDE0IDQ5IDE0ek0xNSAyMGgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEMxNiAxOS41NSAxNS41NSAyMCAxNSAyMHpNMjUgMjBoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDMjYgMTkuNTUgMjUuNTUgMjAgMjUgMjB6TTM1IDIwaC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzM2IDE5LjU1IDM1LjU1IDIwIDM1IDIwek00NSAyMGgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEM0NiAxOS41NSA0NS41NSAyMCA0NSAyMHpNNTUgMjBoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDNTYgMTkuNTUgNTUuNTUgMjAgNTUgMjB6TTU5IDE0aC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzYwIDEzLjU1IDU5LjU1IDE0IDU5IDE0ek02OSAxNGgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEM3MCAxMy41NSA2OS41NSAxNCA2OSAxNHpNNjUgMjBoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDNjYgMTkuNTUgNjUuNTUgMjAgNjUgMjB6TTc1IDIwaC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzc2IDE5LjU1IDc1LjU1IDIwIDc1IDIweiIvPjxwYXRoIGZpbGw9IiNlMGFjODkiIGQ9Ik05IDI2SDVjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDMTAgMjUuNTUgOS41NSAyNiA5IDI2ek0xOSAyNmgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEMyMCAyNS41NSAxOS41NSAyNiAxOSAyNnpNMjkgMjZoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDMzAgMjUuNTUgMjkuNTUgMjYgMjkgMjZ6TTM5IDI2aC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzQwIDI1LjU1IDM5LjU1IDI2IDM5IDI2ek00OSAyNmgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEM1MCAyNS41NSA0OS41NSAyNiA0OSAyNnpNMTUgMzJoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDMTYgMzEuNTUgMTUuNTUgMzIgMTUgMzJ6TTI1IDMyaC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzI2IDMxLjU1IDI1LjU1IDMyIDI1IDMyek0zNSAzMmgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEMzNiAzMS41NSAzNS41NSAzMiAzNSAzMnpNNDUgMzJoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDNDYgMzEuNTUgNDUuNTUgMzIgNDUgMzJ6TTU1IDMyaC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzU2IDMxLjU1IDU1LjU1IDMyIDU1IDMyek01OSAyNmgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEM2MCAyNS41NSA1OS41NSAyNiA1OSAyNnpNNjkgMjZoLTRjLS41NSAwLTEtLjQ1LTEtMXYwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxdjBDNzAgMjUuNTUgNjkuNTUgMjYgNjkgMjZ6TTY1IDMyaC00Yy0uNTUgMC0xLS40NS0xLTF2MGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMXYwQzY2IDMxLjU1IDY1LjU1IDMyIDY1IDMyek03NSAzMmgtNGMtLjU1IDAtMS0uNDUtMS0xdjBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDF2MEM3NiAzMS41NSA3NS41NSAzMiA3NSAzMnoiLz48cGF0aCBmaWxsPSIjYjA4MTU5IiBkPSJNOSAzOEg1Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMxMCAzNy41NSA5LjU1IDM4IDkgMzh6TTE5IDM4aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMyMCAzNy41NSAxOS41NSAzOCAxOSAzOHpNMjkgMzhoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzMwIDM3LjU1IDI5LjU1IDM4IDI5IDM4ek0zOSAzOGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNDAgMzcuNTUgMzkuNTUgMzggMzkgMzh6TTQ5IDM4aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM1MCAzNy41NSA0OS41NSAzOCA0OSAzOHpNMTUgNDRoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzE2IDQzLjU1IDE1LjU1IDQ0IDE1IDQ0ek0yNSA0NGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDMjYgNDMuNTUgMjUuNTUgNDQgMjUgNDR6TTM1IDQ0aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMzNiA0My41NSAzNS41NSA0NCAzNSA0NHpNNDUgNDRoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzQ2IDQzLjU1IDQ1LjU1IDQ0IDQ1IDQ0ek01NSA0NGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNTYgNDMuNTUgNTUuNTUgNDQgNTUgNDR6TTU5IDM4aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM2MCAzNy41NSA1OS41NSAzOCA1OSAzOHpNNjkgMzhoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzcwIDM3LjU1IDY5LjU1IDM4IDY5IDM4ek02NSA0NGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNjYgNDMuNTUgNjUuNTUgNDQgNjUgNDR6TTc1IDQ0aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM3NiA0My41NSA3NS41NSA0NCA3NSA0NHpNOSA1MEg1Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMxMCA0OS41NSA5LjU1IDUwIDkgNTB6TTE5IDUwaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMyMCA0OS41NSAxOS41NSA1MCAxOSA1MHpNMjkgNTBoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzMwIDQ5LjU1IDI5LjU1IDUwIDI5IDUwek0zOSA1MGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNDAgNDkuNTUgMzkuNTUgNTAgMzkgNTB6TTQ5IDUwaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM1MCA0OS41NSA0OS41NSA1MCA0OSA1MHoiLz48cGF0aCBmaWxsPSIjNzA0ZTM0IiBkPSJNMTUgNTZoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzE2IDU1LjU1IDE1LjU1IDU2IDE1IDU2ek0yNSA1NmgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDMjYgNTUuNTUgMjUuNTUgNTYgMjUgNTZ6TTM1IDU2aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMzNiA1NS41NSAzNS41NSA1NiAzNSA1NnpNNDUgNTZoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzQ2IDU1LjU1IDQ1LjU1IDU2IDQ1IDU2ek01NSA1NmgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNTYgNTUuNTUgNTUuNTUgNTYgNTUgNTZ6Ii8+PHBhdGggZmlsbD0iI2IwODE1OSIgZD0iTTU5IDUwaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM2MCA0OS41NSA1OS41NSA1MCA1OSA1MHpNNjkgNTBoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzcwIDQ5LjU1IDY5LjU1IDUwIDY5IDUweiIvPjxwYXRoIGZpbGw9IiM3MDRlMzQiIGQ9Ik02NSA1NmgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNjYgNTUuNTUgNjUuNTUgNTYgNjUgNTZ6TTc1IDU2aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM3NiA1NS41NSA3NS41NSA1NiA3NSA1NnpNOSA2Mkg1Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMxMCA2MS41NSA5LjU1IDYyIDkgNjJ6TTE5IDYyaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMyMCA2MS41NSAxOS41NSA2MiAxOSA2MnpNMjkgNjJoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzMwIDYxLjU1IDI5LjU1IDYyIDI5IDYyek0zOSA2MmgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNDAgNjEuNTUgMzkuNTUgNjIgMzkgNjJ6TTQ5IDYyaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM1MCA2MS41NSA0OS41NSA2MiA0OSA2MnpNMTUgNjhoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzE2IDY3LjU1IDE1LjU1IDY4IDE1IDY4ek0yNSA2OGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDMjYgNjcuNTUgMjUuNTUgNjggMjUgNjh6TTM1IDY4aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEMzNiA2Ny41NSAzNS41NSA2OCAzNSA2OHpNNDUgNjhoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzQ2IDY3LjU1IDQ1LjU1IDY4IDQ1IDY4ek01NSA2OGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNTYgNjcuNTUgNTUuNTUgNjggNTUgNjh6TTU5IDYyaC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM2MCA2MS41NSA1OS41NSA2MiA1OSA2MnpNNjkgNjJoLTRjLS41NSAwLTEtLjQ1LTEtMWwwIDBjMC0uNTUuNDUtMSAxLTFoNGMuNTUgMCAxIC40NSAxIDFsMCAwQzcwIDYxLjU1IDY5LjU1IDYyIDY5IDYyek02NSA2OGgtNGMtLjU1IDAtMS0uNDUtMS0xbDAgMGMwLS41NS40NS0xIDEtMWg0Yy41NSAwIDEgLjQ1IDEgMWwwIDBDNjYgNjcuNTUgNjUuNTUgNjggNjUgNjh6TTc1IDY4aC00Yy0uNTUgMC0xLS40NS0xLTFsMCAwYzAtLjU1LjQ1LTEgMS0xaDRjLjU1IDAgMSAuNDUgMSAxbDAgMEM3NiA2Ny41NSA3NS41NSA2OCA3NSA2OHoiLz48L3N2Zz4=' alt='Temperature soil' class='review__svg'>" \
           "                         <div class='review__box'>" \
            "                            <p class='review__name'>Temperatura do solo</p>" \
             "                       </div>" \
              "                      <div  id='tempSolo' class='review__estimate'>" + String(temperaturaDoSoloLida) + " ºC</div>" \
               "                 </figcaption>" \
                "            </figure>" \
                 "       </div>" \
    
    "                    <div class='user-controls'>" \
     "                       <div class='control'>" \
      "                          <div class='control__user'>" \
       "                             <a id='lampada' href='/lampada' class='control__link'>" \
        "                                <img src='data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB3aWR0aD0iMTYwcHQiIGhlaWdodD0iMTYwcHQiIHZpZXdCb3g9IjAgMCAxNjAgMTYwIiB2ZXJzaW9uPSIxLjEiPgo8ZyBpZD0ic3VyZmFjZTM5MTU1NCI+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNzIuOTQxMTc2JSw2MC43ODQzMTQlLDI4LjIzNTI5NCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAxNDIuMzk4NDM4IDUzIEwgMTM3IDUzIEMgMTM2LjM5ODQzOCA1MyAxMzYgNTIuNjAxNTYyIDEzNiA1MiBDIDEzNiA1MS4zOTg0MzggMTM2LjM5ODQzOCA1MSAxMzcgNTEgTCAxNDIuMzk4NDM4IDUxIEMgMTQzIDUxIDE0My4zOTg0MzggNTEuMzk4NDM4IDE0My4zOTg0MzggNTIgQyAxNDMuMzk4NDM4IDUyLjYwMTU2MiAxNDMgNTMgMTQyLjM5ODQzOCA1MyBaIE0gMjMgNTMgTCAxNy42MDE1NjIgNTMgQyAxNyA1MyAxNi42MDE1NjIgNTIuNjAxNTYyIDE2LjYwMTU2MiA1MiBDIDE2LjYwMTU2MiA1MS4zOTg0MzggMTcgNTEgMTcuNjAxNTYyIDUxIEwgMjMgNTEgQyAyMy42MDE1NjIgNTEgMjQgNTEuMzk4NDM4IDI0IDUyIEMgMjQgNTIuNjAxNTYyIDIzLjYwMTU2MiA1MyAyMyA1MyBaIE0gMjMgNTMgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNzIuOTQxMTc2JSw2MC43ODQzMTQlLDI4LjIzNTI5NCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAxNDIuMzk4NDM4IDUzIEwgMTM3IDUzIEMgMTM2LjM5ODQzOCA1MyAxMzYgNTIuNjAxNTYyIDEzNiA1MiBDIDEzNiA1MS4zOTg0MzggMTM2LjM5ODQzOCA1MSAxMzcgNTEgTCAxNDIuMzk4NDM4IDUxIEMgMTQzIDUxIDE0My4zOTg0MzggNTEuMzk4NDM4IDE0My4zOTg0MzggNTIgQyAxNDMuMzk4NDM4IDUyLjYwMTU2MiAxNDMgNTMgMTQyLjM5ODQzOCA1MyBaIE0gMjMgNTMgTCAxNy42MDE1NjIgNTMgQyAxNyA1MyAxNi42MDE1NjIgNTIuNjAxNTYyIDE2LjYwMTU2MiA1MiBDIDE2LjYwMTU2MiA1MS4zOTg0MzggMTcgNTEgMTcuNjAxNTYyIDUxIEwgMjMgNTEgQyAyMy42MDE1NjIgNTEgMjQgNTEuMzk4NDM4IDI0IDUyIEMgMjQgNTIuNjAxNTYyIDIzLjYwMTU2MiA1MyAyMyA1MyBaIE0gMjMgNTMgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNzIuOTQxMTc2JSw2MC43ODQzMTQlLDI4LjIzNTI5NCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAxNDIuMzk4NDM4IDUzIEwgMTM3IDUzIEMgMTM2LjM5ODQzOCA1MyAxMzYgNTIuNjAxNTYyIDEzNiA1MiBDIDEzNiA1MS4zOTg0MzggMTM2LjM5ODQzOCA1MSAxMzcgNTEgTCAxNDIuMzk4NDM4IDUxIEMgMTQzIDUxIDE0My4zOTg0MzggNTEuMzk4NDM4IDE0My4zOTg0MzggNTIgQyAxNDMuMzk4NDM4IDUyLjYwMTU2MiAxNDMgNTMgMTQyLjM5ODQzOCA1MyBaIE0gMjMgNTMgTCAxNy42MDE1NjIgNTMgQyAxNyA1MyAxNi42MDE1NjIgNTIuNjAxNTYyIDE2LjYwMTU2MiA1MiBDIDE2LjYwMTU2MiA1MS4zOTg0MzggMTcgNTEgMTcuNjAxNTYyIDUxIEwgMjMgNTEgQyAyMy42MDE1NjIgNTEgMjQgNTEuMzk4NDM4IDI0IDUyIEMgMjQgNTIuNjAxNTYyIDIzLjYwMTU2MiA1MyAyMyA1MyBaIE0gMTI0LjE5OTIxOSA5Ny4xOTkyMTkgQyAxMjQgOTcuMTk5MjE5IDEyMy42MDE1NjIgOTcuMTk5MjE5IDEyMy4zOTg0MzggOTcgTCAxMTkuNjAxNTYyIDkzLjE5OTIxOSBDIDExOS4xOTkyMTkgOTIuODAwNzgxIDExOS4xOTkyMTkgOTIuMTk5MjE5IDExOS42MDE1NjIgOTEuODAwNzgxIEMgMTIwIDkxLjM5ODQzOCAxMjAuNjAxNTYyIDkxLjM5ODQzOCAxMjEgOTEuODAwNzgxIEwgMTI0LjgwMDc4MSA5NS42MDE1NjIgQyAxMjUuMTk5MjE5IDk2IDEyNS4xOTkyMTkgOTYuNjAxNTYyIDEyNC44MDA3ODEgOTcgQyAxMjQuNjAxNTYyIDk3IDEyNC4zOTg0MzggOTcuMTk5MjE5IDEyNC4xOTkyMTkgOTcuMTk5MjE5IFogTSAzOS42MDE1NjIgMTIuNjAxNTYyIEMgMzkuMzk4NDM4IDEyLjYwMTU2MiAzOSAxMi42MDE1NjIgMzguODAwNzgxIDEyLjM5ODQzOCBMIDM1IDguNjAxNTYyIEMgMzQuNjAxNTYyIDguMTk5MjE5IDM0LjYwMTU2MiA3LjYwMTU2MiAzNSA3LjE5OTIxOSBDIDM1LjM5ODQzOCA2LjgwMDc4MSAzNiA2LjgwMDc4MSAzNi4zOTg0MzggNy4xOTkyMTkgTCA0MC4xOTkyMTkgMTEgQyA0MC42MDE1NjIgMTEuMzk4NDM4IDQwLjYwMTU2MiAxMiA0MC4xOTkyMTkgMTIuMzk4NDM4IEMgNDAuMTk5MjE5IDEyLjYwMTU2MiA0MCAxMi42MDE1NjIgMzkuNjAxNTYyIDEyLjYwMTU2MiBaIE0gMTIwLjM5ODQzOCAxMi42MDE1NjIgQyAxMjAuMTk5MjE5IDEyLjYwMTU2MiAxMTkuODAwNzgxIDEyLjYwMTU2MiAxMTkuNjAxNTYyIDEyLjM5ODQzOCBDIDExOS4xOTkyMTkgMTIgMTE5LjE5OTIxOSAxMS4zOTg0MzggMTE5LjYwMTU2MiAxMSBMIDEyMy4zOTg0MzggNy4xOTkyMTkgQyAxMjMuODAwNzgxIDYuODAwNzgxIDEyNC4zOTg0MzggNi44MDA3ODEgMTI0LjgwMDc4MSA3LjE5OTIxOSBDIDEyNS4xOTkyMTkgNy42MDE1NjIgMTI1LjE5OTIxOSA4LjE5OTIxOSAxMjQuODAwNzgxIDguNjAxNTYyIEwgMTIxIDEyLjM5ODQzOCBDIDEyMC44MDA3ODEgMTIuNjAxNTYyIDEyMC42MDE1NjIgMTIuNjAxNTYyIDEyMC4zOTg0MzggMTIuNjAxNTYyIFogTSAzNS44MDA3ODEgOTcuMTk5MjE5IEMgMzUuNjAxNTYyIDk3LjE5OTIxOSAzNS4xOTkyMTkgOTcuMTk5MjE5IDM1IDk3IEMgMzQuNjAxNTYyIDk2LjYwMTU2MiAzNC42MDE1NjIgOTYgMzUgOTUuNjAxNTYyIEwgMzguODAwNzgxIDkxLjgwMDc4MSBDIDM5LjE5OTIxOSA5MS4zOTg0MzggMzkuODAwNzgxIDkxLjM5ODQzOCA0MC4xOTkyMTkgOTEuODAwNzgxIEMgNDAuNjAxNTYyIDkyLjE5OTIxOSA0MC42MDE1NjIgOTIuODAwNzgxIDQwLjE5OTIxOSA5My4xOTkyMTkgTCAzNi4zOTg0MzggOTcgQyAzNi4zOTg0MzggOTcgMzYuMTk5MjE5IDk3LjE5OTIxOSAzNS44MDA3ODEgOTcuMTk5MjE5IFogTSAxMzIuNjAxNTYyIDMxLjE5OTIxOSBDIDEzMi4xOTkyMTkgMzEuMTk5MjE5IDEzMS44MDA3ODEgMzEgMTMxLjYwMTU2MiAzMC42MDE1NjIgQyAxMzEuMzk4NDM4IDMwIDEzMS42MDE1NjIgMjkuNjAxNTYyIDEzMi4xOTkyMTkgMjkuMTk5MjE5IEwgMTM3LjE5OTIxOSAyNy4xOTkyMTkgQyAxMzcuODAwNzgxIDI3IDEzOC4xOTkyMTkgMjcuMTk5MjE5IDEzOC42MDE1NjIgMjcuODAwNzgxIEMgMTM4LjgwMDc4MSAyOC4zOTg0MzggMTM4LjYwMTU2MiAyOC44MDA3ODEgMTM4IDI5LjE5OTIxOSBMIDEzMyAzMS4xOTkyMTkgQyAxMzMgMzEuMTk5MjE5IDEzMi44MDA3ODEgMzEuMTk5MjE5IDEzMi42MDE1NjIgMzEuMTk5MjE5IFogTSAyMi4zOTg0MzggNzYuODAwNzgxIEMgMjIgNzYuODAwNzgxIDIxLjYwMTU2MiA3Ni42MDE1NjIgMjEuMzk4NDM4IDc2LjE5OTIxOSBDIDIxLjE5OTIxOSA3NS42MDE1NjIgMjEuMzk4NDM4IDc1LjE5OTIxOSAyMiA3NC44MDA3ODEgTCAyNyA3Mi44MDA3ODEgQyAyNy42MDE1NjIgNzIuNjAxNTYyIDI4IDcyLjgwMDc4MSAyOC4zOTg0MzggNzMuMzk4NDM4IEMgMjguNjAxNTYyIDc0IDI4LjM5ODQzOCA3NC4zOTg0MzggMjcuODAwNzgxIDc0LjgwMDc4MSBMIDIyLjgwMDc4MSA3Ni44MDA3ODEgQyAyMi42MDE1NjIgNzYuODAwNzgxIDIyLjM5ODQzOCA3Ni44MDA3ODEgMjIuMzk4NDM4IDc2LjgwMDc4MSBaIE0gMTM3LjYwMTU2MiA3Ni44MDA3ODEgQyAxMzcuMzk4NDM4IDc2LjgwMDc4MSAxMzcuMzk4NDM4IDc2LjgwMDc4MSAxMzcuMTk5MjE5IDc2LjgwMDc4MSBMIDEzMi4xOTkyMTkgNzQuODAwNzgxIEMgMTMxLjYwMTU2MiA3NC42MDE1NjIgMTMxLjM5ODQzOCA3NCAxMzEuNjAxNTYyIDczLjM5ODQzOCBDIDEzMS44MDA3ODEgNzIuODAwNzgxIDEzMi4zOTg0MzggNzIuNjAxNTYyIDEzMyA3Mi44MDA3ODEgTCAxMzggNzQuODAwNzgxIEMgMTM4LjYwMTU2MiA3NSAxMzguODAwNzgxIDc1LjYwMTU2MiAxMzguNjAxNTYyIDc2LjE5OTIxOSBDIDEzOC4zOTg0MzggNzYuNjAxNTYyIDEzOCA3Ni44MDA3ODEgMTM3LjYwMTU2MiA3Ni44MDA3ODEgWiBNIDI3LjM5ODQzOCAzMS4xOTkyMTkgQyAyNy4xOTkyMTkgMzEuMTk5MjE5IDI3LjE5OTIxOSAzMS4xOTkyMTkgMjcgMzEuMTk5MjE5IEwgMjIgMjkuMTk5MjE5IEMgMjEuMzk4NDM4IDI5IDIxLjE5OTIxOSAyOC4zOTg0MzggMjEuMzk4NDM4IDI3LjgwMDc4MSBDIDIxLjYwMTU2MiAyNy4xOTkyMTkgMjIuMTk5MjE5IDI3IDIyLjgwMDc4MSAyNy4xOTkyMTkgTCAyNy44MDA3ODEgMjkuMTk5MjE5IEMgMjguMzk4NDM4IDI5LjM5ODQzOCAyOC42MDE1NjIgMzAgMjguMzk4NDM4IDMwLjYwMTU2MiBDIDI4LjE5OTIxOSAzMSAyNy44MDA3ODEgMzEuMTk5MjE5IDI3LjM5ODQzOCAzMS4xOTkyMTkgWiBNIDI3LjM5ODQzOCAzMS4xOTkyMTkgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoMTAwJSw5My4zMzMzMzMlLDYzLjkyMTU2OSUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA1NyAxMjAuMzk4NDM4IEwgNTcgMTEyIEMgNTcgMTA0LjYwMTU2MiA1Mi4zOTg0MzggOTcuMzk4NDM4IDQ3IDg5IEMgNDAuMzk4NDM4IDc4LjYwMTU2MiAzMyA2Ni44MDA3ODEgMzMgNTIgQyAzMyAyNiA1NCA1IDgwIDUgQyAxMDYgNSAxMjcgMjYgMTI3IDUyIEMgMTI3IDY2LjgwMDc4MSAxMTkuNjAxNTYyIDc4LjYwMTU2MiAxMTMgODkgQyAxMDcuNjAxNTYyIDk3LjM5ODQzOCAxMDMgMTA0LjYwMTU2MiAxMDMgMTEyIEwgMTAzIDEyMC4zOTg0MzggWiBNIDU3IDEyMC4zOTg0MzggIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNzIuOTQxMTc2JSw2MC43ODQzMTQlLDI4LjIzNTI5NCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA4MCA2IEMgMTA1LjM5ODQzOCA2IDEyNiAyNi42MDE1NjIgMTI2IDUyIEMgMTI2IDY2LjM5ODQzOCAxMTguNjAxNTYyIDc4IDExMiA4OC4zOTg0MzggQyAxMDYuNjAxNTYyIDk3IDEwMiAxMDQuMzk4NDM4IDEwMiAxMTIgTCAxMDIgMTE5LjM5ODQzOCBMIDU4IDExOS4zOTg0MzggTCA1OCAxMTIgQyA1OCAxMDQuMzk4NDM4IDUzLjM5ODQzOCA5NyA0OCA4OC4zOTg0MzggQyA0MS4zOTg0MzggNzggMzQgNjYuMzk4NDM4IDM0IDUyIEMgMzQgMjYuNjAxNTYyIDU0LjYwMTU2MiA2IDgwIDYgTSA4MCA0IEMgNTMuMzk4NDM4IDQgMzIgMjUuMzk4NDM4IDMyIDUyIEMgMzIgNzguNjAxNTYyIDU2IDk1LjgwMDc4MSA1NiAxMTIgTCA1NiAxMjEuMzk4NDM4IEwgMTA0IDEyMS4zOTg0MzggTCAxMDQgMTEyIEMgMTA0IDk1LjgwMDc4MSAxMjggNzguNjAxNTYyIDEyOCA1MiBDIDEyOCAyNS4zOTg0MzggMTA2LjYwMTU2MiA0IDgwIDQgWiBNIDgwIDQgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoMTAwJSwxMDAlLDEwMCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSA5MC44MDA3ODEgNDUuMzk4NDM4IEMgOTIuODAwNzgxIDQ1LjM5ODQzOCA5NC42MDE1NjIgNDYuMTk5MjE5IDk2LjE5OTIxOSA0Ny42MDE1NjIgQyA5OS4xOTkyMTkgNTAuNjAxNTYyIDk5LjE5OTIxOSA1NS4xOTkyMTkgOTYuMTk5MjE5IDU4LjE5OTIxOSBMIDg4IDY2LjM5ODQzOCBMIDg4IDExNiBMIDcyIDExNiBMIDcyIDY2LjM5ODQzOCBMIDYzLjgwMDc4MSA1OC4xOTkyMTkgQyA2MC44MDA3ODEgNTUuMTk5MjE5IDYwLjgwMDc4MSA1MC42MDE1NjIgNjMuODAwNzgxIDQ3LjYwMTU2MiBDIDY1LjE5OTIxOSA0Ni4xOTkyMTkgNjcuMTk5MjE5IDQ1LjM5ODQzOCA2OS4xOTkyMTkgNDUuMzk4NDM4IEMgNzEuMTk5MjE5IDQ1LjM5ODQzOCA3MyA0Ni4xOTkyMTkgNzQuNjAxNTYyIDQ3LjYwMTU2MiBMIDgwLjE5OTIxOSA1My4xOTkyMTkgTCA4MyA1MC4zOTg0MzggTCA4NS44MDA3ODEgNDcuNjAxNTYyIEMgODcgNDYuMTk5MjE5IDg4LjgwMDc4MSA0NS4zOTg0MzggOTAuODAwNzgxIDQ1LjM5ODQzOCBNIDkwLjgwMDc4MSA0MS4zOTg0MzggQyA4Ny44MDA3ODEgNDEuMzk4NDM4IDg1IDQyLjYwMTU2MiA4Mi42MDE1NjIgNDQuODAwNzgxIEwgODAgNDcuMzk4NDM4IEwgNzcuMTk5MjE5IDQ0LjYwMTU2MiBDIDc1IDQyLjM5ODQzOCA3MiA0MS4xOTkyMTkgNjkgNDEuMTk5MjE5IEMgNjYgNDEuMTk5MjE5IDYzLjE5OTIxOSA0Mi4zOTg0MzggNjAuODAwNzgxIDQ0LjYwMTU2MiBDIDU2LjM5ODQzOCA0OSA1Ni4zOTg0MzggNTYuMzk4NDM4IDYwLjgwMDc4MSA2MC44MDA3ODEgTCA2OCA2OCBMIDY4IDEyMCBMIDkyIDEyMCBMIDkyIDY4IEwgOTkgNjEgQyAxMDMuMzk4NDM4IDU2LjYwMTU2MiAxMDMuMzk4NDM4IDQ5LjE5OTIxOSA5OSA0NC44MDA3ODEgQyA5Ni44MDA3ODEgNDIuMzk4NDM4IDkzLjgwMDc4MSA0MS4zOTg0MzggOTAuODAwNzgxIDQxLjM5ODQzOCBaIE0gOTAuODAwNzgxIDQxLjM5ODQzOCAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYigxNy42NDcwNTklLDIwLjc4NDMxNCUsMjUuMDk4MDM5JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDgwIDEyOCBDIDcyLjI2OTUzMSAxMjggNjYgMTM0LjI2OTUzMSA2NiAxNDIgQyA2NiAxNDkuNzMwNDY5IDcyLjI2OTUzMSAxNTYgODAgMTU2IEMgODcuNzMwNDY5IDE1NiA5NCAxNDkuNzMwNDY5IDk0IDE0MiBDIDk0IDEzNC4yNjk1MzEgODcuNzMwNDY5IDEyOCA4MCAxMjggWiBNIDgwIDEyOCAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYigxMDAlLDg5LjAxOTYwOCUsOTIuOTQxMTc2JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDY4IDE0NyBDIDYyIDE0NyA1NyAxNDIgNTcgMTM2IEwgNTcgMTEzIEwgMTAzIDExMyBMIDEwMyAxMzYgQyAxMDMgMTQyIDk4IDE0NyA5MiAxNDcgWiBNIDY4IDE0NyAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYig1NC41MDk4MDQlLDQ1Ljg4MjM1MyUsNjMuMTM3MjU1JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDEwMiAxMTQgTCAxMDIgMTM2IEMgMTAyIDE0MS42MDE1NjIgOTcuNjAxNTYyIDE0NiA5MiAxNDYgTCA2OCAxNDYgQyA2Mi4zOTg0MzggMTQ2IDU4IDE0MS42MDE1NjIgNTggMTM2IEwgNTggMTE0IEwgMTAyIDExNCBNIDEwNCAxMTIgTCA1NiAxMTIgTCA1NiAxMzYgQyA1NiAxNDIuNjAxNTYyIDYxLjM5ODQzOCAxNDggNjggMTQ4IEwgOTIgMTQ4IEMgOTguNjAxNTYyIDE0OCAxMDQgMTQyLjYwMTU2MiAxMDQgMTM2IFogTSAxMDQgMTEyICIvPgo8cGF0aCBzdHlsZT0iIHN0cm9rZTpub25lO2ZpbGwtcnVsZTpub256ZXJvO2ZpbGw6cmdiKDU0LjUwOTgwNCUsNDUuODgyMzUzJSw2My4xMzcyNTUlKTtmaWxsLW9wYWNpdHk6MTsiIGQ9Ik0gNzIgMTIwIEwgMTA0IDEyMCBMIDEwNCAxMjIgTCA3MiAxMjIgWiBNIDU2IDEyMCBMIDY4IDEyMCBMIDY4IDEyMiBMIDU2IDEyMiBaIE0gNzIgMTI2IEwgMTA0IDEyNiBMIDEwNCAxMjggTCA3MiAxMjggWiBNIDU2IDEyNiBMIDY4IDEyNiBMIDY4IDEyOCBMIDU2IDEyOCBaIE0gNzIgMTMyIEwgMTA0IDEzMiBMIDEwNCAxMzQgTCA3MiAxMzQgWiBNIDU2IDEzMiBMIDY4IDEzMiBMIDY4IDEzNCBMIDU2IDEzNCBaIE0gNzIgMTM4IEwgMTAyLjM5ODQzOCAxMzggTCAxMDIuMzk4NDM4IDE0MCBMIDcyIDE0MCBaIE0gNTcuNjAxNTYyIDEzOCBMIDY4IDEzOCBMIDY4IDE0MCBMIDU3LjYwMTU2MiAxNDAgWiBNIDU3LjYwMTU2MiAxMzggIi8+CjwvZz4KPC9zdmc+Cg==' alt='Lamp' class='control__svg'>" \
         "                           </a>              " \
          "                          <p class='control__name'>Lâmpada</p>     " \           
           "                         <div class='control__estimate'>ON/OFF</div>" \
            "                    </div>" \
             "               </div>" \
              "              <div class='control'>" \
               "                 <div class='control__user'>" \
                "                    <a id='bomba' href='/bomba' class='control__link'>" \
                 "                       <img src='data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHhtbG5zOnhsaW5rPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5L3hsaW5rIiB3aWR0aD0iODBwdCIgaGVpZ2h0PSI4MHB0IiB2aWV3Qm94PSIwIDAgODAgODAiIHZlcnNpb249IjEuMSI+CjxnIGlkPSJzdXJmYWNlNDE5NDQ4Ij4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYig1NC41MDk4MDQlLDcxLjc2NDcwNiUsOTQuMTE3NjQ3JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDcyLjM0Mzc1IDE3Ljc4NTE1NiBDIDY4Ljk0MTQwNiAxNy43ODUxNTYgNjcuNzg5MDYyIDE1LjQyOTY4OCA2Ni44NTkzNzUgMTMuNTMxMjUgTCA2Ni42NDg0MzggMTMuMTAxNTYyIEMgNjQuOTU3MDMxIDkuNzI2NTYyIDYyLjM3MTA5NCA2LjY2NDA2MiA2MS4wNjY0MDYgNS4yMjY1NjIgQyA2NS42OTUzMTIgNS4zMTI1IDcxLjUwMzkwNiA1Ljk0OTIxOSA3NS4xMTMyODEgOC4yODEyNSBDIDc2LjYxNzE4OCA5LjI1MzkwNiA3Ny41IDEwLjg3MTA5NCA3Ny41IDEyLjYzMjgxMiBDIDc3LjUgMTUuNDc2NTYyIDc1LjE4NzUgMTcuNzg1MTU2IDcyLjM0Mzc1IDE3Ljc4NTE1NiBaIE0gNzIuMzQzNzUgMTcuNzg1MTU2ICIvPgo8cGF0aCBzdHlsZT0iIHN0cm9rZTpub25lO2ZpbGwtcnVsZTpub256ZXJvO2ZpbGw6cmdiKDMwLjU4ODIzNSUsNDcuODQzMTM3JSw3MC45ODAzOTIlKTtmaWxsLW9wYWNpdHk6MTsiIGQ9Ik0gNjIuMjAzMTI1IDUuNzU3ODEyIEMgNjYuNTM5MDYyIDUuOTIxODc1IDcxLjYxNzE4OCA2LjYxNzE4OCA3NC44NjMyODEgOC43MTg3NSBDIDc2LjIwMzEyNSA5LjU3ODEyNSA3NyAxMS4wNDI5NjkgNzcgMTIuNjMyODEyIEMgNzcgMTUuMTk5MjE5IDc0LjkxMDE1NiAxNy4yODUxNTYgNzIuMzQzNzUgMTcuMjg1MTU2IEMgNjkuMjUzOTA2IDE3LjI4NTE1NiA2OC4yMjI2NTYgMTUuMTc1NzgxIDY3LjMwODU5NCAxMy4zMTI1IEwgNjcuMDk3NjU2IDEyLjg3ODkwNiBDIDY1LjY2Nzk2OSAxMC4wMjczNDQgNjMuNjEzMjgxIDcuNDAyMzQ0IDYyLjIwMzEyNSA1Ljc1NzgxMiBNIDU5LjkwMjM0NCA0LjcxNDg0NCBDIDU5LjkwMjM0NCA0LjcxNDg0NCA2My45MTc5NjkgOC43NzM0MzggNjYuMjAzMTI1IDEzLjMyNDIxOSBDIDY3LjE1NjI1IDE1LjIzMDQ2OSA2OC4zNDM3NSAxOC4yODUxNTYgNzIuMzQzNzUgMTguMjg1MTU2IEMgNzUuNDY4NzUgMTguMjg1MTU2IDc4IDE1Ljc1MzkwNiA3OCAxMi42MzI4MTIgQyA3OCAxMC42MzY3MTkgNzYuOTY4NzUgOC44ODI4MTIgNzUuNDA2MjUgNy44Nzg5MDYgQyA3NC41MjM0MzggNy4zMDg1OTQgNzAuNjE3MTg4IDQuNzE0ODQ0IDU5LjkwMjM0NCA0LjcxNDg0NCBaIE0gNTkuOTAyMzQ0IDQuNzE0ODQ0ICIvPgo8cGF0aCBzdHlsZT0iIHN0cm9rZTpub25lO2ZpbGwtcnVsZTpub256ZXJvO2ZpbGw6cmdiKDc2LjA3ODQzMSUsOTAuOTgwMzkyJSwxMDAlKTtmaWxsLW9wYWNpdHk6MTsiIGQ9Ik0gMi41IDc3LjUgTCAyLjUgNzAuNSBMIDY3IDcwLjUgQyA2OC45Mjk2ODggNzAuNSA3MC41IDY4LjkyOTY4OCA3MC41IDY3IEMgNzAuNSA2NS4wNzAzMTIgNjguOTI5Njg4IDYzLjUgNjcgNjMuNSBMIDEzIDYzLjUgQyA3LjIxMDkzOCA2My41IDIuNSA1OC43ODkwNjIgMi41IDUzIEMgMi41IDQ3LjIxMDkzOCA3LjIxMDkzOCA0Mi41IDEzIDQyLjUgTCA2NyA0Mi41IEMgNjguOTI5Njg4IDQyLjUgNzAuNSA0MC45Mjk2ODggNzAuNSAzOSBDIDcwLjUgMzcuMDcwMzEyIDY4LjkyOTY4OCAzNS41IDY3IDM1LjUgTCAxMi44MDg1OTQgMzUuNSBDIDcuMTI1IDM1LjUgMi41IDMwLjg3NSAyLjUgMjUuMTkxNDA2IEMgMi41IDIwLjU2MjUgNS42MTcxODggMTYuNDc2NTYyIDEwLjA4MjAzMSAxNS4yNTM5MDYgTCAzNC4xNDQ1MzEgOC42NDQ1MzEgTCAzNiAxNS4zOTg0MzggTCAxMS45MzM1OTQgMjIuMDAzOTA2IEMgMTAuNSAyMi4zOTQ1MzEgOS41IDIzLjcwNzAzMSA5LjUgMjUuMTkxNDA2IEMgOS41IDI3LjAxNTYyNSAxMC45ODQzNzUgMjguNSAxMi44MDg1OTQgMjguNSBMIDY3IDI4LjUgQyA3Mi43ODkwNjIgMjguNSA3Ny41IDMzLjIxMDkzOCA3Ny41IDM5IEMgNzcuNSA0NC43ODkwNjIgNzIuNzg5MDYyIDQ5LjUgNjcgNDkuNSBMIDEzIDQ5LjUgQyAxMS4wNzAzMTIgNDkuNSA5LjUgNTEuMDcwMzEyIDkuNSA1MyBDIDkuNSA1NC45Mjk2ODggMTEuMDcwMzEyIDU2LjUgMTMgNTYuNSBMIDY3IDU2LjUgQyA3Mi43ODkwNjIgNTYuNSA3Ny41IDYxLjIxMDkzOCA3Ny41IDY3IEMgNzcuNSA3Mi43ODkwNjIgNzIuNzg5MDYyIDc3LjUgNjcgNzcuNSBaIE0gMi41IDc3LjUgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNDUuNDkwMTk2JSw1OC44MjM1MjklLDc2Ljg2Mjc0NSUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAzMy43OTY4NzUgOS4yNjE3MTkgTCAzNS4zODI4MTIgMTUuMDQ2ODc1IEwgMTEuODAwNzgxIDIxLjUxOTUzMSBDIDEwLjE1MjM0NCAyMS45NzI2NTYgOSAyMy40ODA0NjkgOSAyNS4xOTE0MDYgQyA5IDI3LjI4OTA2MiAxMC43MTA5MzggMjkgMTIuODA4NTk0IDI5IEwgNjcgMjkgQyA3Mi41MTU2MjUgMjkgNzcgMzMuNDg0Mzc1IDc3IDM5IEMgNzcgNDQuNTE1NjI1IDcyLjUxNTYyNSA0OSA2NyA0OSBMIDEzIDQ5IEMgMTAuNzkyOTY5IDQ5IDkgNTAuNzkyOTY5IDkgNTMgQyA5IDU1LjIwNzAzMSAxMC43OTI5NjkgNTcgMTMgNTcgTCA2NyA1NyBDIDcyLjUxNTYyNSA1NyA3NyA2MS40ODQzNzUgNzcgNjcgQyA3NyA3Mi41MTU2MjUgNzIuNTE1NjI1IDc3IDY3IDc3IEwgMyA3NyBMIDMgNzEgTCA2NyA3MSBDIDY5LjIwNzAzMSA3MSA3MSA2OS4yMDcwMzEgNzEgNjcgQyA3MSA2NC43OTI5NjkgNjkuMjA3MDMxIDYzIDY3IDYzIEwgMTMgNjMgQyA3LjQ4NDM3NSA2MyAzIDU4LjUxNTYyNSAzIDUzIEMgMyA0Ny40ODQzNzUgNy40ODQzNzUgNDMgMTMgNDMgTCA2NyA0MyBDIDY5LjIwNzAzMSA0MyA3MSA0MS4yMDcwMzEgNzEgMzkgQyA3MSAzNi43OTI5NjkgNjkuMjA3MDMxIDM1IDY3IDM1IEwgMTIuODA4NTk0IDM1IEMgNy4zOTg0MzggMzUgMyAzMC42MDE1NjIgMyAyNS4xOTE0MDYgQyAzIDIwLjc4NTE1NiA1Ljk2NDg0NCAxNi44OTg0MzggMTAuMjE0ODQ0IDE1LjczNDM3NSBMIDMzLjc5Njg3NSA5LjI2MTcxOSBNIDM0LjQ5NjA5NCA4LjAzMTI1IEwgOS45NDkyMTkgMTQuNzY5NTMxIEMgNS4yNjk1MzEgMTYuMDU0Njg4IDIgMjAuMzM5ODQ0IDIgMjUuMTkxNDA2IEMgMiAzMS4xNDg0MzggNi44NTE1NjIgMzYgMTIuODA4NTk0IDM2IEwgNjcgMzYgQyA2OC42NTIzNDQgMzYgNzAgMzcuMzQ3NjU2IDcwIDM5IEMgNzAgNDAuNjUyMzQ0IDY4LjY1MjM0NCA0MiA2NyA0MiBMIDEzIDQyIEMgNi45Mzc1IDQyIDIgNDYuOTM3NSAyIDUzIEMgMiA1OS4wNjI1IDYuOTM3NSA2NCAxMyA2NCBMIDY3IDY0IEMgNjguNjUyMzQ0IDY0IDcwIDY1LjM0NzY1NiA3MCA2NyBDIDcwIDY4LjY1MjM0NCA2OC42NTIzNDQgNzAgNjcgNzAgTCAyIDcwIEwgMiA3OCBMIDY3IDc4IEMgNzMuMDYyNSA3OCA3OCA3My4wNjI1IDc4IDY3IEMgNzggNjAuOTM3NSA3My4wNjI1IDU2IDY3IDU2IEwgMTMgNTYgQyAxMS4zNDc2NTYgNTYgMTAgNTQuNjUyMzQ0IDEwIDUzIEMgMTAgNTEuMzQ3NjU2IDExLjM0NzY1NiA1MCAxMyA1MCBMIDY3IDUwIEMgNzMuMDYyNSA1MCA3OCA0NS4wNjI1IDc4IDM5IEMgNzggMzIuOTM3NSA3My4wNjI1IDI4IDY3IDI4IEwgMTIuODA4NTk0IDI4IEMgMTEuMjYxNzE5IDI4IDEwIDI2LjczODI4MSAxMCAyNS4xOTE0MDYgQyAxMCAyMy45Mjk2ODggMTAuODQ3NjU2IDIyLjgxNjQwNiAxMi4wNjI1IDIyLjQ4NDM3NSBMIDM2LjYxMzI4MSAxNS43NDYwOTQgWiBNIDM0LjQ5NjA5NCA4LjAzMTI1ICIvPgo8cGF0aCBzdHlsZT0iIHN0cm9rZTpub25lO2ZpbGwtcnVsZTpub256ZXJvO2ZpbGw6cmdiKDEwMCUsODkuMDE5NjA4JSw5Mi45NDExNzYlKTtmaWxsLW9wYWNpdHk6MTsiIGQ9Ik0gMzEuMDQ2ODc1IDcuMDQyOTY5IEwgNTEuNzQyMTg4IDUuNDQ1MzEyIEwgNTIuOTkyMTg4IDExLjIzNDM3NSBMIDMzLjY2MDE1NiAxOS4xNDg0MzggWiBNIDMxLjA0Njg3NSA3LjA0Mjk2OSAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYig1NC41MDk4MDQlLDQ1Ljg4MjM1MyUsNjMuMTM3MjU1JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDUxLjM0NzY1NiA1Ljk3NjU2MiBMIDUyLjQxNzk2OSAxMC45Mjk2ODggTCAzNC4wMjM0MzggMTguNDYwOTM4IEwgMzEuNjU2MjUgNy40OTYwOTQgTCA1MS4zNDc2NTYgNS45NzY1NjIgTSA1Mi4xNDA2MjUgNC45MTQwNjIgTCAzMC40Mzc1IDYuNTg5ODQ0IEwgMzMuMzAwNzgxIDE5LjgzNTkzOCBMIDUzLjU3MDMxMiAxMS41MzkwNjIgWiBNIDUyLjE0MDYyNSA0LjkxNDA2MiAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYigxMDAlLDg5LjAxOTYwOCUsOTIuOTQxMTc2JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDM2LjYyMTA5NCAyMS41NjY0MDYgQyAzNC44NDM3NSAyMS41NjY0MDYgMzMuMjc3MzQ0IDIwLjMwNDY4OCAzMi45MDIzNDQgMTguNTYyNSBMIDMwLjQyOTY4OCA3LjExMzI4MSBDIDMwLjIxNDg0NCA2LjExNzE4OCAzMC4zOTg0MzggNS4xMDE1NjIgMzAuOTUzMTI1IDQuMjQyMTg4IEMgMzEuNTAzOTA2IDMuMzkwNjI1IDMyLjM1NTQ2OSAyLjgwMDc4MSAzMy4zNDc2NTYgMi41ODU5MzggQyAzMy42MTMyODEgMi41MjczNDQgMzMuODg2NzE5IDIuNSAzNC4xNTYyNSAyLjUgQyAzNS45MzM1OTQgMi41IDM3LjUgMy43NjU2MjUgMzcuODc1IDUuNTAzOTA2IEwgNDAuMzQ3NjU2IDE2Ljk1MzEyNSBDIDQwLjU2MjUgMTcuOTQ5MjE5IDQwLjM3NSAxOC45Njg3NSAzOS44MjQyMTkgMTkuODI0MjE5IEMgMzkuMjczNDM4IDIwLjY3OTY4OCAzOC40MjE4NzUgMjEuMjY1NjI1IDM3LjQyOTY4OCAyMS40ODA0NjkgQyAzNy4xNjAxNTYgMjEuNTM5MDYyIDM2Ljg5MDYyNSAyMS41NjY0MDYgMzYuNjIxMDk0IDIxLjU2NjQwNiBaIE0gMzYuNjIxMDk0IDIxLjU2NjQwNiAiLz4KPHBhdGggc3R5bGU9IiBzdHJva2U6bm9uZTtmaWxsLXJ1bGU6bm9uemVybztmaWxsOnJnYig1NC41MDk4MDQlLDQ1Ljg4MjM1MyUsNjMuMTM3MjU1JSk7ZmlsbC1vcGFjaXR5OjE7IiBkPSJNIDM0LjE1NjI1IDMgQyAzNS42OTkyMTkgMyAzNy4wNTg1OTQgNC4wOTc2NTYgMzcuMzg2NzE5IDUuNjA5Mzc1IEwgMzkuODU5Mzc1IDE3LjA2MjUgQyA0MC4wNDY4NzUgMTcuOTI1NzgxIDM5Ljg4MjgxMiAxOC44MDg1OTQgMzkuNDA2MjUgMTkuNTU0Njg4IEMgMzguOTI1NzgxIDIwLjI5Njg3NSAzOC4xODc1IDIwLjgwODU5NCAzNy4zMjAzMTIgMjAuOTkyMTg4IEMgMzcuMDg5ODQ0IDIxLjA0Mjk2OSAzNi44NTU0NjkgMjEuMDcwMzEyIDM2LjYyMTA5NCAyMS4wNzAzMTIgQyAzNS4wNzgxMjUgMjEuMDcwMzEyIDMzLjcxODc1IDE5Ljk3MjY1NiAzMy4zOTA2MjUgMTguNDU3MDMxIEwgMzAuOTE3OTY5IDcuMDA3ODEyIEMgMzAuNzMwNDY5IDYuMTQ0NTMxIDMwLjg5NDUzMSA1LjI1NzgxMiAzMS4zNzEwOTQgNC41MTU2MjUgQyAzMS44NTE1NjIgMy43NzM0MzggMzIuNTkzNzUgMy4yNjE3MTkgMzMuNDU3MDMxIDMuMDc0MjE5IEMgMzMuNjg3NSAzLjAyMzQzOCAzMy45MjE4NzUgMyAzNC4xNTYyNSAzIE0gMzQuMTU2MjUgMiBDIDMzLjg1NTQ2OSAyIDMzLjU1MDc4MSAyLjAzMTI1IDMzLjI0MjE4OCAyLjA5NzY1NiBDIDMwLjkxNzk2OSAyLjU5NzY1NiAyOS40Mzc1IDQuODkwNjI1IDI5Ljk0MTQwNiA3LjIxODc1IEwgMzIuNDE0MDYyIDE4LjY2Nzk2OSBDIDMyLjg0NzY1NiAyMC42ODc1IDM0LjYzMjgxMiAyMi4wNzAzMTIgMzYuNjIxMDk0IDIyLjA3MDMxMiBDIDM2LjkyMTg3NSAyMi4wNzAzMTIgMzcuMjI2NTYyIDIyLjAzNTE1NiAzNy41MzEyNSAyMS45NzI2NTYgQyAzOS44NTkzNzUgMjEuNDY4NzUgNDEuMzM5ODQ0IDE5LjE3NTc4MSA0MC44MzU5MzggMTYuODUxNTYyIEwgMzguMzYzMjgxIDUuMzk4NDM4IEMgMzcuOTI1NzgxIDMuMzc4OTA2IDM2LjE0MDYyNSAyIDM0LjE1NjI1IDIgWiBNIDM0LjE1NjI1IDIgIi8+CjxwYXRoIHN0eWxlPSIgc3Ryb2tlOm5vbmU7ZmlsbC1ydWxlOm5vbnplcm87ZmlsbDpyZ2IoNzYuMDc4NDMxJSw5MC45ODAzOTIlLDEwMCUpO2ZpbGwtb3BhY2l0eToxOyIgZD0iTSAyIDcxIEwgNiA3MSBMIDYgNzcgTCAyIDc3IFogTSAyIDcxICIvPgo8L2c+Cjwvc3ZnPgo=' alt='Water pump' class='control__svg'>" \
                  "                  </a>              " \
                   "                 <p class='control__name'>Bomba de água</p>" \                
                    "                <div class='control__estimate'>ON/OFF</div>" \
                     "           </div>" \
                      "      </div>" \
                       " </div>" \
                     "</div>" \
                    
         "       </main>" \
         "   </div>" \
       " </div>" \
        "<div class='legal'>Feito com ♡ por Isabel Rubim</div>" \
    "</body>" \
    "</html>";

    client.print(html);
}
