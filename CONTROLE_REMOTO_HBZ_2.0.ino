#include <EEPROM.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(4, 5, 2);
RtcDS1302<ThreeWire> Rtc(myWire);

int posicaoPeriodoSerial = 0;
int posicaoAnoSerial = 4;
int periodo = 6;
byte mesInicial;
byte anoInicial;
int dataLimite;

// Este codigo limpa os dados da memoria interna do arduino
void erase(void) {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (!Rtc.IsDateTimeValid()) {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();

  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  // CONFIGURA MES LIMITE NA MEMORIA
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
  Serial.println();

  mesInicial = EEPROM.read(posicaoPeriodoSerial);
  byte mesAtual = compiled.Month();

  // caso nao existir data limite na memoria, insere.
  if (!mesInicial) {
    Serial.println("Inicializa mes atual...");

    EEPROM.write(posicaoPeriodoSerial, mesAtual);
    mesInicial = EEPROM.read(posicaoPeriodoSerial);

    Serial.print("Iniciou o mes atual como: ");
    Serial.println(mesAtual);
  }

  Serial.print("mostra mes inicial: ");
  Serial.println(mesInicial);

  int anoAtual = compiled.Year() - 2000;
  anoInicial = EEPROM.read(posicaoAnoSerial);

  if (!anoInicial) {
    Serial.println("Inicializa ano atual...");

    EEPROM.write(posicaoAnoSerial, anoAtual);
    anoInicial = EEPROM.read(posicaoPeriodoSerial);

    Serial.print("Iniciou o ano atual como: ");
    Serial.println(anoInicial);
  }

  // data limite considera os 6 meses
  dataLimite = mesInicial;

  // acrescenta o periodo aqui
  for (int i = 1; i < periodo + 1; i++) {
    dataLimite += 1;

    if (dataLimite > 12) {
      dataLimite = 1;
      anoInicial += 1;
    }
  }

  Serial.print("mostra mes limite: ");
  Serial.println(dataLimite);
  Serial.print("mostra ano limite: ");
  Serial.println(anoInicial);


  if (anoInicial <= anoAtual) {
    if (dataLimite <= mesAtual) {
      Serial.println("tem que acender o led do alerta");
    } else {
      Serial.println("o mes eh menor que o periodo de 6 meses");
    }
  } else {
    Serial.println("o ano eh menor que o periodo de 6 meses");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}
