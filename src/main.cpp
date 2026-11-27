#include <Arduino.h>
#include <ConectividadeESP32.h>

// --- Portal ---
#define NOME_PORTAL "Portal_Felipe"         // Nome da rede para configurar o WiFi
#define SENHA_PORTAL nullptr                // Ponteiro vazio sem senha
const uint8_t QNT_FALHAS_ABRIR_PORTAL = 3;  // Quantidade de tentativas de conexão para abrir o portal automaticamente
const uint32_t TEMPO_PORTAL_SEG = 120;      // Portal aberto pelo botão fecha após 2 min

void iniciarConexao();
void aoSalvarPortal(const String& ssid);
void aoConectarWiFi();
void aoDesconectarWiFi();
void verificarBotao();

// --- Broker MQTT ---
#define BROKER "test.mosquitto.org"
#define PORTA_BROKER 1883
#define CLIENT_ID "esp32-felipe-15325"

// --- Hardware ---
const int PINO_BOOT = 0;
const uint32_t TEMPO_BOTAO_ABRIR_PORTAL_MS = 3000; // QUando o botão for apertado por 3s abre o portal

void setup() 
{
  pinMode(PINO_BOOT, INPUT_PULLUP);
  iniciarConexao();

}

void loop() 
{
  conectividade.update();
}

void iniciarConexao()
{
  // Diz para a biblioteca quando abrir o portal sozinha e com que nome e senha
  // Conectividade.definirPortalAutomatico(ativo, nome, senha, falhas)
  // ativo -> Liga ou desliga a abertura automática por falhas (Obrigatório)
  // nome -> Nome da rede do portal
  // senha -> Senha do portal. Use nullptr para rede aberta
  // falhas -> Quantas tentativas seguidas sem sucesso antes de abrir o portal
  conectividade.definirPortalAutomatico(true, NOME_PORTAL, SENHA_PORTAL, QNT_FALHAS_ABRIR_PORTAL);

  // Registra a função que a biblioteca chama quando alguém clica em "Salvar e conectar" na página do portal
  conectividade.registrarCallbackPortalSalvo(aoSalvarPortal);

  // Registra a função chamada toda vez que o WiFi conecta
  conectividade.registrarCallbackWiFiConectado(aoConectarWiFi);

  // Registra a função chamada quando a conexão que existia cai
  conectividade.registrarCallbackWiFiDesconectado(aoDesconectarWiFi);

  // Função que dá a partida na conexão. Ela só liga o WiFi e precisa do parâmetro qual rede usar
  conectividade.beginSomenteWiFi(WIFI_SALVO);
}

void aoSalvarPortal(const String& ssid)
{
  Serial.printf("Rede selecionada: %s", ssid);
}

void aoConectarWiFi()
{
  Serial.printf("Conectado a %s - IP %s\n\r", conectividade.ssidAtual().c_str(), WiFi.localIP().toString().c_str());
}

void aoDesconectarWiFi()
{
  Serial.printf("WiFi desconectado - Tentando reconectador...");
}

void verificarBotao()
{
  static uint32_t ultimoTempo = 0;
  if(digitalRead(PINO_BOOT) == LOW)
  {
    if(ultimoTempo == 0) ultimoTempo = millis();
    if(!conectividade.portalAtivo()) 
    {
      ultimoTempo = 0;
      return;
    }

    if(millis() - ultimoTempo > TEMPO_BOTAO_ABRIR_PORTAL_MS)
    {
      conectividade.iniciarPortalConfiguracao(NOME_PORTAL, SENHA_PORTAL, TEMPO_PORTAL_SEG);
    }

    else
    {
      ultimoTempo = 0;
    }
  }
}