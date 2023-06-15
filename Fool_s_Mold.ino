#include <ESP8266WiFi.h> //Biblioteca para funcionamento do WiFi do ESP
#include <ESP8266WebServer.h> //Biblioteca para o ESP funcionar como servidor
#include <DHT.h> //Biblioteca para funcionamento do sensor de temperatura e umidade DHT11

const char* ssid = "Damaa"; // Rede WiFi
const char* password = "baleiagorda"; //Senha da Rede WiFi

ESP8266WebServer server(80); //server na porta 80

#define DHTPIN D2

#define DHTTYPE DHT11 //Tipo do sensor DHT11

#define LED_PIN D3 // Pino do LED

DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

float temperatura; //variável para armazenar a temperatura
float umidade;    //Variável para armazenar a umidade
String condicao;  //Variável de condição

void setup()
{
  Serial.begin(9600); //Inicializa a comunicação serial
  delay(50);          // ?Intervalo para aguardar a estabilização do sistema
  dht.begin();        //Inicializa o sensor DHT11
  pinMode(LED_PIN, OUTPUT); // Define o pino do LED como saída

  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid);                  //Imprime na serial o nome da Rede Wi-Fi

  WiFi.begin(ssid, password); //Inicialização da comunicação Wi-Fi

  //Verificação da conexão
  while (WiFi.status() != WL_CONNECTED)
  {                 //Enquanto estiver aguardando status da conexão
    delay(1000);
    Serial.print("."); //Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Função para exibir o IP da ESP

  server.on("/", handle_OnConnect);    //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
  server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound

  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");
}

void loop()
{
  server.handleClient(); //Chama o método handleClient()
}

void handle_OnConnect()
{
  temperatura = dht.readTemperature(); //Realiza a leitura da temperatura
  umidade = dht.readHumidity();       //Realiza a leitura da umidade

  Serial.print("Temperatura: ");
  Serial.print(temperatura); //Imprime no monitor serial o valor da temperatura lida
  Serial.println(" ºC");
  Serial.print("Umidade: ");
  Serial.print(umidade); //Imprime no monitor serial o valor da umidade lida
  Serial.println(" %");
  if (temperatura >= 20 && temperatura <= 30 && umidade > 10)
  {
    condicao = "Atenção! Risco alto de Mofo";
    Serial.println("Atenção! Risco alto de Mofo");
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    Serial.println("Condições ok's");
    condicao = "Condições ok's";
    digitalWrite(LED_PIN, LOW);
  }
  server.send(200, "text/html", EnvioHTML(temperatura, umidade, condicao)); //Envia as informações usando o código 200, especifica o conteúdo como "text/html" e chama a função EnvioHTML
}

void handle_NotFound()
{ //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/pain" e envia a mensagem "Não encontrado"
}
String EnvioHTML(float Temperaturastat, float Umidadestat, String condicao)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<meta http-equiv='refresh' content='2'>";
  ptr += "<meta charset='UTF-8'>";
  ptr += "<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr += "<style>body {font-family: 'Open Sans', sans-serif; background-color: #f2f2f2;}\n";
  ptr += ".container {max-width: 500px; margin: 0 auto; padding: 20px; background-color: #ffffff; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); border-radius: 10px;}\n";
  ptr += "h1 {color: #333333; text-align: center;}\n"; // Adiciona alinhamento centralizado ao título
  ptr += "p {color: #555555; text-align: center;}\n"; // Adiciona alinhamento centralizado ao texto
  ptr += ".status {color: #555555; font-size: 20px; padding: 10px; border-radius: 5px;}\n"; // Estilos para o detalhe da condição
  ptr += ".status.red {color: red;}\n"; // Adiciona a cor vermelha à classe .red
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div class=\"container\">\n";
  ptr += "<h1>Fool's Mold</h1>\n";
  ptr += "<p><b>Detector 1 (armário cozinha)</b></p>\n";
  ptr += "<p><b>Temperatura:</b> ";
  ptr += (float)Temperaturastat;
  ptr += " ºC</p>\n";
  ptr += "<p><b>Umidade:</b> ";
  ptr += (float)Umidadestat;
  ptr += " %</p>\n";
  ptr += "<p class=\"status";
  
  // Adiciona a classe .red quando a condição do sensor é atingida
  if (condicao == "Atenção! Risco alto de Mofo") {
    ptr += " red";
  }
  
  ptr += "\"><b>Condição:</b> ";
  ptr += (String)condicao;
  ptr += "</p>\n";
  ptr += "<div class=\"sidebar\">\n"; // Adiciona detalhes nas laterais da página
  ptr += "</div>\n";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;}
