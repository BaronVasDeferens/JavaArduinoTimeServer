/*
TinyTime
by Skot West

Queries a little time server and receives a String formatted
(hr,min,sec,month,day,yr)
compatible with Time.h setTime command.
*/

#include <SPI.h>         
#include <EthernetV2_0.h>
#include <Time.h>
#include <SD.h>


//For main loop
boolean initialized = false;

//Local Ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};  
EthernetClient client;

//Remote server
byte server[] = {192,168,0,117};
int port = 123;

//Input from server
int BUFFER_SIZE = 30;
String timeString = "";

//Data collection pins
int photoPin = A5;

//SD card
int SDCardPin = 10;
const int chipSelect = 4;

int dataValue;

void setup() 
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  //Initialize SD card
  pinMode(SDCardPin, OUTPUT);
  Serial.print("Init SD card...");
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("FAILED.");
    // don't do anything more:
    return;
  }
  else
    Serial.println("SUCCESS!");
  
  //Initialize photCell data pin
  pinMode(photoPin, INPUT);
  
  //Start Ethernet
  Ethernet.begin(mac);
  
  //Take a breath...
  delay(1000);
  
  //Report IP
  
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
  
  
  Serial.print("Connecting to time server...");
  
  if(client.connect(server,port))
  {
    Serial.println("CONNECTED!");
    client.println();
  }
  else
    Serial.println("FAILED.");
}

void loop()
{
  //Syncronize with time server
  if (initialized == false)
  {
        if (client.available())
         {  
           timeString = client.readString();
           
           //Close stream
           client.stop();  
           
           //Test output
           //Serial.print("I got ");
           //Serial.println(timeString);
           //Serial.println("I think this is " + (String)(timeString.length()) + " chars long.");
           
           //Break it apart
           String myHour, myMinute, mySecond, myDay, myMonth, myYear, myDayOfWeek, myTimeZone;
           
           myHour = timeString.substring(0,2);
           myMinute = timeString.substring(3,5);
           mySecond = timeString.substring(6,8);
           
           myDay = timeString.substring(12,14);
           myMonth = timeString.substring(9,11);
           myYear = timeString.substring(15,17);
          
           //Test for error
           if (myMonth.toInt() > 12)
             Serial.println("Time Sync Error: month out of bounds"); 
      
      
           //Test input
           //Serial.println("TEST: HOUR:MIN:SEC: " + myHour + ":" +myMinute + ":" + mySecond);
           //Serial.println("TEST: MON/DAY/YEAR: " + myMonth + "/" + myDay + "/" + myYear);
      
           //setTime(hr,min,sec,day,month,yr)
           setTime(myHour.toInt(), myMinute.toInt(), mySecond.toInt(), myDay.toInt(), myMonth.toInt(), (myYear.toInt()+2000));
      
           //Display adjusted time
           //time = now();
           
           //Display synchronized time
           Serial.println();
           Serial.print(hour());
           Serial.print(":");
           Serial.print(minute());
           Serial.print(":");
           Serial.print(second());
           Serial.print(" ");
           Serial.print(month());
           Serial.print("/");
           Serial.print(day());
           Serial.print("/");
           Serial.print(year());
          
          initialized = true;
          
       }//cleinet.available()
    
    }//initialized
    
    //Measure light level
    else
    {
        dataValue = analogRead(photoPin);
     
        String dataString = "";
        File dataFile = SD.open("datalog.txt", FILE_WRITE);
        
        dataString += (String)hour() + ":" + (String)minute() + ":" + (String)second() + " LIGHT = " + dataValue;
        time_t time = now();
        Serial.println(time); 
        
        if(dataFile)
        {
           dataFile.println(dataString);
           dataFile.close();
           Serial.println(dataString);
           delay(30000);
        }
        
        else
          Serial.println("File error");
    }
    
    
    
}//loop  

