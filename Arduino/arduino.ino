#include "WiFiEsp.h" //WiFi library
#ifndef HAVE_HWSERIAL1
#endif
#include "SoftwareSerial.h" //Serial library

SoftwareSerial Serial1(3, 2); // TX, RX

char ssid[] = "whypi"; //와이파이 이름
char pass[] = "skxodid0305"; //와이파이 비밀번호
int status = WL_IDLE_STATUS; //와이파이 연결 상태

//아두이노 핀 설정
int trigPin = 12;
int echoPin = 11;
int piezo =10 ;
int led =13;

bool isOpen = false; //현재 열렸는지
int stat = 4; //현재 상태
char server[] = "52.253.107.159"; //서버 주소

//인터넷 클라이언트 설정
WiFiEspClient client;

//상자가 열리면 요청하는 함수
//응답 형식 : int
//응답 상세
// -1: 에러, 0: 잠금 없음, 1: 알림만 보내기(open 함수 호출), 2: 알림 + LED, 3: 알림 + 소리, 4: 알림 + LED + 소리
int open()
{
  //client.start();
  Serial.println("Starting connection to server...");
  
  client.setTimeout(5000);
  if (client.connect(server, 8000)) {
    Serial.println("Connected to server");
    //HTTP 요청 설정
    client.println("GET /open HTTP/1.1"); //GET 요청 (/open)
    client.println("HOST:52.253.107.159");
    client.println("Connection: close"); //연결 종료
    client.println();
  }
  
  //응답 받기
  while (client.connected())
  {
    if (client.available())
    {
      String c = client.readStringUntil('\n');
      //char c = client.read();
      Serial.println("d"+c);
      if(c[0]=='0'||c[0]=='1'||c[0]=='2'||c[0]=='3'||c[0]=='4') return c[0]-'0'; //응답이 0~4일 경우 int로 변환해서 return
    }
  }
  return stat;
}

void setup()
{
  Serial.begin(9600); //시리얼 출력 설정
  Serial1.begin(9600); //ESP 시리얼 출력 설정
  //ESP 모듈 설정
  WiFi.init(&Serial1);

  //와이파이 모듈 연결상태 확인
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    //연결될 때 까지
    while (true);
  }

  //연결된 경우
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    //네트워크에 연결
    status = WiFi.begin(ssid, pass);
  }

  //연결되었을 시 출력
  Serial.println("You're connected to the network");
  printWifiStatus();
  Serial.println("WiFi Setup Complete!");
  //아두이노 핀 설정
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(piezo, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
}

void loop()
{
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  Serial.println(stat);
  Serial.println(getDistance());
  //Serial.println(open());
  
  if(getDistance() <= 10)
  {
    digitalWrite(led, HIGH);
    if(isOpen == false) stat = open();
    isOpen = true;
  }
  else
  {
    isOpen = false;
    digitalWrite(led, LOW);
  }

  if(isOpen == true) aggro(stat);
  else delay(1000);
  
}

void aggro(int stat)
{
  if(stat==2||stat==4) digitalWrite(led, HIGH);
  delay(100);
  digitalWrite(led, LOW);
  if(stat==3||stat==4) tone(piezo, 262);;
  delay(100);
  noTone(piezo);
}

float getDistance()
{
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  float duration = pulseIn(echoPin, HIGH);

  float distance = ((float)(340*duration)/10000)/2;
  return distance;
}

//WiFi 상태 출력
void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
