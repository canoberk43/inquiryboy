int PWMA=11;  // SAĞ MOTOR
int A1N2=8;
int A1N1=7;

int STBY=6;   // 5V ARDUİNO

int B1N1=5;   // SOL MOTOR
int B1N2=4;
int PWMB=3;

int Sensor8=A0;
int Sensor7=A1;
int Sensor6=A2;
int Sensor5=A3; //ÇİZGİ İZLEMEK İÇİN SENSÖR PİNLERİ
int Sensor4=A4;
int Sensor3=A5;
int Sensor2=A6;
int Sensor1=A7;

int pozisyon;
int sensor[8];
int turev;
int hata;
int son_hata;  // PİD İÇİN TANIMLAMALAR
int pid;
int hedef=4565;
int hiz=180;
int i;

float KP=0.1;
float KD=0.01;   //PİD İÇİN DÜZELTME DEĞERLERİ

bool sonsensor;  //ÇİZGİYİ EN SON GÖREN SENSÖR İÇİN TANIMLAMA


int MZ80;
int engel;  //MESAFE SENSÖRÜ İÇİN TANIMLAMA
int gorev=0;

unsigned long a;
unsigned long b;   //AĞIRLIKLI ORTALAMA İÇİN TANIMLAMA


int leftmotor,rightmotor; //MOTORLARIN HIZI İÇİN TANIMLAMA

unsigned long simdikizaman=0;
unsigned long oncekizaman=0;         //ZAMANI KULLANMAK İÇİN GEREKLİ TANIMLAMALAR
int aralik=1000;
                                 



void setup() {
  pinMode(PWMA,OUTPUT);
  pinMode(A1N2,OUTPUT);
  pinMode(A1N1,OUTPUT);
 
  pinMode(B1N1,OUTPUT);   //MOTOR SÜRÜCÜNÜN PİNLERİ ÇIKIŞ OLARAK AYARLANDI
  pinMode(B1N2,OUTPUT);
  pinMode(PWMB,OUTPUT);
  digitalWrite(STBY,HIGH);

  pinMode(Sensor8,INPUT);
  pinMode(Sensor7,INPUT);
  pinMode(Sensor6,INPUT); //ÇİZGİ İZLEYEN SENSÖR PİNLERİ GİRİŞ OLARAK AYARLANDI
  pinMode(Sensor5,INPUT);
  pinMode(Sensor4,INPUT);
  pinMode(Sensor3,INPUT);
  pinMode(Sensor2,INPUT);
  pinMode(Sensor1,INPUT);
  pinMode(MZ80,INPUT);
  
  Serial.begin(9600); // HABERLEŞME BAŞLATILDI


}

void loop() {

  

  sensor[0]=analogRead(Sensor1);
  sensor[1]=analogRead(Sensor2);
  sensor[2]=analogRead(Sensor3);
  sensor[3]=analogRead(Sensor4);     //ÇİZGİ İZLEYEN SENSÖRLERDEN OKUNAN DEĞERLER BİR DİZİYE AKTARILDI
  sensor[4]=analogRead(Sensor5);
  sensor[5]=analogRead(Sensor6);
  sensor[6]=analogRead(Sensor7);
  sensor[7]=analogRead(Sensor8);


  a=0;
  b=0;
  for(i=0; i<8 ; i=i+1 ) {
    a += (unsigned long)sensor[i]*(unsigned long)(i+1)*(unsigned long)(1000);  //OLUŞTURULAN DİZİ İLE AĞIRLIKLI ORTALAMA HESAPLANDI
    b += (unsigned long)sensor[i];
  }

  pozisyon=a/b;
  
  
  hata = hedef-pozisyon;    //AĞIRLIKLI ORTALAMA VE HEDEF,HATA DEĞERLERİ İLE PİD HESAPLANDI
  turev=hata-son_hata;
  
  pid=(hata*KP)+(turev*KD);
  
  son_hata=hata;

  
  leftmotor = hiz+pid;
  rightmotor = hiz - pid;
  if(leftmotor>255){leftmotor=255;}
  if(rightmotor>255){rightmotor=255;} //PİD DEĞERİ MOTOR DEĞİŞKENLERİNE ATANDI,SINIRLARI BELİRLENDİ
  if(leftmotor<0){leftmotor=0;}
  if(rightmotor<0){rightmotor=0;}
  
  digitalWrite(A1N1,HIGH);
  digitalWrite(A1N2,LOW);
  analogWrite(PWMA,rightmotor); 
                                        //PİD DEĞERİ MOTORLARA AKTARILDI
  digitalWrite(B1N1,HIGH);
  digitalWrite(B1N2,LOW); 
  analogWrite(PWMB,leftmotor);



if(analogRead(Sensor1)<300){
  sonsensor = 1;
}                            //ÇİZGİYİ EN SON GÖREN EN UÇTAKİ SENSÖRLER İÇİN SONSENSOR DEĞİŞKENİNE ATAMA YAPILDI
if(analogRead(Sensor8)<300){
  sonsensor = 0;
}


//ÇİZGİYİ EN SON GÖREN SENSÖRE GÖRE ROBOTUN YÖNÜ BELİRLENDİ
if(analogRead(Sensor1)>800 && analogRead(Sensor2)>800 && analogRead(Sensor3)>800 && analogRead(Sensor4)>800 && analogRead(Sensor5)>800 && analogRead(Sensor6)>800 && analogRead(Sensor7)>800 && analogRead(Sensor8)>800) {
  while(analogRead(Sensor4)>300 ||  analogRead(Sensor5)>300 ){
    if( sonsensor == 1 ) {
        digitalWrite(A1N1,HIGH);
        digitalWrite(A1N2,LOW);
        analogWrite(PWMA,180);   
        
        digitalWrite(B1N1,LOW);
        digitalWrite(B1N2,LOW); 
        analogWrite(PWMB,0); 
      
    }
    
    
    if( sonsensor == 0) {
        digitalWrite(A1N1,LOW);
        digitalWrite(A1N2,LOW);
        analogWrite(PWMA,0);   
        
        digitalWrite(B1N1,HIGH);
        digitalWrite(B1N2,LOW);
        analogWrite(PWMB,180); 
    }
  }
}


//ROBOTUN ÇİZGİ TAKİP KISMI BİTTİ,GÖREV KISIMLARI BAŞLADI
//MESAFE SENSÖRÜNDEN ALINAN DEĞER ENGEL DEĞİŞKENİNE ATANDI,ROBOT HER ENGEL GÖRDÜĞÜNDE GOREV DEĞİŞKENİ 1 ARTTIRILDI
engel=digitalRead(MZ80); 
if(engel==0){
gorev++;
}

  
  
  //ROBOT 1 SANİYE SAĞA DÖNÜP BEYAZ ÇİZGİYİ GÖRENE KADAR İLERLEDİ,BEYAZ ÇİZGİ BİTENE KADAR DÜZ İLERLEDİ,BEYAZ ÇİZGİ BTİNCE 1SN SOLA DÖNÜP ÇİZGİYİ GÖRENE KADAR ÜZ DEVAM ETTİ 
  if(gorev == 1){
  simdikizaman=millis();
  while(simdikizaman - oncekizaman <= aralik){
    digitalWrite(A1N1,LOW);
    digitalWrite(A1N2,LOW);
    analogWrite(PWMA,0);   
    
    digitalWrite(B1N1,HIGH);
    digitalWrite(B1N2,LOW); 
    analogWrite(PWMB,150);
   
  simdikizaman = oncekizaman ;
  }

  while(analogRead(Sensor4)>300 ||  analogRead(Sensor5)>300){

      digitalWrite(A1N1,HIGH);
      digitalWrite(A1N2,LOW);
      analogWrite(PWMA,150);   
      
      digitalWrite(B1N1,HIGH);
      digitalWrite(B1N2,LOW);
      analogWrite(PWMB,150); 
    }

      if(analogRead(Sensor1)>800 && analogRead(Sensor2)>800 && analogRead(Sensor3)>800 && analogRead(Sensor4)>800 && analogRead(Sensor5)>800 && analogRead(Sensor6)>800 && analogRead(Sensor7)>800 && analogRead(Sensor8)>800){
          
          oncekizaman=0;
          simdikizaman=millis();
          while(simdikizaman - oncekizaman <= aralik){
          digitalWrite(A1N1,HIGH);
          digitalWrite(A1N2,LOW);
          analogWrite(PWMA,150);   
          
          digitalWrite(B1N1,LOW);
          digitalWrite(B1N2,LOW); 
          analogWrite(PWMB,0);
         
         simdikizaman = oncekizaman ;
          }
       }

      while(analogRead(Sensor4)>300 ||  analogRead(Sensor5)>300){

      digitalWrite(A1N1,HIGH);
      digitalWrite(A1N2,LOW);
      analogWrite(PWMA,150);   
      
      digitalWrite(B1N1,HIGH);
      digitalWrite(B1N2,LOW);
      analogWrite(PWMB,150); 
    }




}








//ROBOT ENGELİ GÖRÜNCE DURDU ENGEL KALDIRILINCA İLERLEMEYE DEVAM ETTİ
if(gorev == 2){
  while(digitalRead(MZ80) == 0){
   digitalWrite(A1N1,LOW);
   digitalWrite(A1N2,LOW);
   analogWrite(PWMA,0);
    
   digitalWrite(B1N1,LOW);
   digitalWrite(B1N2,LOW); 
   analogWrite(PWMB,0);  
  }
  
  }

  
 //ROBOT ENGELİ KESİKLİ ÇİZGİYE KADAR TAKİP ETTİ KESİKLİ ÇİZGİYE ULAŞINCA ENGELİ SOLLADI 
  if(gorev == 3){
   while(analogRead(Sensor1)<300 || analogRead(Sensor2)<300 || analogRead(Sensor3)<300 || analogRead(Sensor4)<300 || analogRead(Sensor5)<300 || analogRead(Sensor6)<300 || analogRead(Sensor7)<300 || analogRead(Sensor8)<300)
{
    if(digitalRead(MZ80) == 0 ){
      
   digitalWrite(A1N1,LOW);
   digitalWrite(A1N2,LOW);
   analogWrite(PWMA,0);
    
   digitalWrite(B1N1,LOW);
   digitalWrite(B1N2,LOW); 
   analogWrite(PWMB,0);
 } 
    
    else {
  digitalWrite(A1N1,HIGH);
  digitalWrite(A1N2,LOW);
  analogWrite(PWMA,rightmotor); 
  
  digitalWrite(B1N1,HIGH);
  digitalWrite(B1N2,LOW); 
  analogWrite(PWMB,leftmotor);
  }
}

oncekizaman=0;
simdikizaman=millis();
  while(simdikizaman - oncekizaman <= aralik){
  digitalWrite(A1N1,LOW);
  digitalWrite(A1N2,LOW);
  analogWrite(PWMA,0);   
  
  digitalWrite(B1N1,HIGH);
  digitalWrite(B1N2,LOW); 
  analogWrite(PWMB,150);
   
  simdikizaman = oncekizaman ;
}
  while(analogRead(Sensor4)>300 ||  analogRead(Sensor5)>300){

      digitalWrite(A1N1,HIGH);
      digitalWrite(A1N2,LOW);
      analogWrite(PWMA,150);   
      
      digitalWrite(B1N1,HIGH);
      digitalWrite(B1N2,LOW);
      analogWrite(PWMB,150); 
    }

  }
 
 
 
 
 //ROBOT BEYAZ DİKDÖRTGENİN İÇİNE PARK ETTİ
 if(analogRead(Sensor1)<300 && analogRead(Sensor2)<300 && analogRead(Sensor3)<300 && analogRead(Sensor4)<300 && analogRead(Sensor5)<300&& analogRead(Sensor6)<300 && analogRead(Sensor7)<300 && analogRead(Sensor8)<300 )
 {
  delay(1000);
  if(analogRead(Sensor1)>800 && analogRead(Sensor2)>800 && analogRead(Sensor3)>800 && analogRead(Sensor4)>800 && analogRead(Sensor5)>800 && analogRead(Sensor6)>800 && analogRead(Sensor7)>800 && analogRead(Sensor8)>800){
        oncekizaman=0;
        simdikizaman=millis();
        while(simdikizaman - oncekizaman <= aralik){
        digitalWrite(A1N1,HIGH);
        digitalWrite(A1N2,LOW);
        analogWrite(PWMA,rightmotor);   
        
        digitalWrite(B1N1,HIGH);
        digitalWrite(B1N2,LOW); 
        analogWrite(PWMB,leftmotor);
       
      simdikizaman = oncekizaman ;
   }
}
 }
 
 
 



   

  
  }
  
