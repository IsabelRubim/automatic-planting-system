# automatic-planting-system

Painel para ESP32 controlado via web :seedling:

![App web](https://i.imgur.com/BiShv9s.png)

Construido com:
- CSS3
- HTML5
- C/C++
- [WebServer](https://www.espressif.com/en/products/hardware/esp32/overview)

**Instalação**
1. No [IDE do Arduino](https://www.arduino.cc/en/main/software), arquivo > preferências e na tela que se abriu
adiciona-se a URL no campo para gerenciadores de placas. Colando o seguinte link: https://dl.espressif.com/dl/package_esp32_index.json.
2. Após a instalação dos arquivos da ESP32, em ferramentas > placa > gerenciador de placas e digite no campo ESP32 e instala o arquivo
de instalação da placa.
3. Em seguida, seleciona a placa, ferramentas > placa e seleciona ESP32.

## Como enviar os dados corretos
São necessárias duas bibliotecas para o sensor [DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf) e uma para o DHT11
funcionar corretamente :blush:
- [OneWire](https://github.com/PaulStoffregen/OneWire)
- [Dallas Temperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)
- [DHT-Sensor](https://github.com/adafruit/DHT-sensor-library)

## Protótipo
![Protótipo](https://i.imgur.com/zj4wZUC.png)

## Futuro
- Suporte ao ESP32 com WebServer para o não carregamento da página após atualização dos dados, com isso, tornando a página mais dinâmica
