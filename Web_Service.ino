void setupWiFi()
{
  Serial.println ( "Setting up WiFi..." );
  Serial1.begin (115200);

  delay (500);
  sendCommand ( "AT" );             //proof of life
  delay (500);
  sendCommand ( "AT+RST" );         //restart
  delay (5000);
  sendCommand ("AT+CWMODE?");      //Request connection mode 1=Station, 2=softAP, 3=Station and softAP
  delay (500);
  sendCommand ("AT+CWMODE=2");      //set as softAP, don't try mode 3 w/o connecting to real AP
  delay (500);
  //  sendCommand ( "AT+CWLAP" );     //list Access Points
  // delay (5000);
  //  sendCommand ( "AT+CWJAP=\"SSID\",\"\"PASSWORD" );  //connect to Access Point-->insert correct SSID and PASSWORD
  // delay(5000);
  sendCommand ( "AT+CIFSR" );       //get local IP address (softAP or station)
  delay (500);
  sendCommand ( "AT+CIPMUX=1" );    //multiple connection allowed
  delay (500);
  sendCommand ( "AT+CIPSERVER=1,80" );//create server and set to port 80
  delay (1000);

  Serial.println ( "" );
  Serial.println ( "WiFi Initialized" );
  delay (2000);   //give it a couple of seconds before announcing that we are ready...

  Serial.println("---------------*****##### READY TO SERVE #####*****---------------");
}//end of void setupWiFi()

bool read_till_eol()
{
  static int i = 0;
  if (Serial1.available())
  {
    buffer[i++] = Serial1.read();
    if (i == BUFFER_SIZE)  i = 0;
    if (i > 1 && buffer[i - 2] == 13 && buffer[i - 1] == 10)
    {
      buffer[i] = 0;
      i = 0;
      Serial.print(buffer);
      return true;
    }
  }
  return false;
}//end of bool read_till_eol()

void sendCommand ( char * cmd )
{
  Serial1.println ( cmd );    //send out to the ESP8266
  getResponse();
}

void getResponse ()
{
  unsigned long startTime;
  char ch;
  char lastCh;

  startTime = millis();
  lastCh = ' ';
  while (millis() < (startTime + 3000))
    if (Serial1.available())
    {
      ch = Serial1.read();
      Serial.print (ch);
      if ((lastCh == 'O') && (ch == 'K'))
        break;
      lastCh = ch;
    }
}// end of void getResponse ()

void ParseWeb()
{
  int ch_id, packet_len;
  char *pb;
  
  if (read_till_eol())
  {
    if (strncmp(buffer, "+IPD,", 5) == 0)
    {
      // request: +IPD,ch,len:data
      sscanf(buffer + 5, "%d,%d", &ch_id, &packet_len);
      if (packet_len > 0)
      {
        // read serial until packet_len character received
        // start from :
        pb = buffer + 5;
        while (*pb != ':') pb++;
        pb++;

        //switch case would work better here knowing there will be several different cases

        if (strncmp(pb, "GET /?GPS", 9) == 0) //compare the read buffer to "GET /?pin=ON1" when adding new msg don't forget to change #or chars.
        { //normally would not want multiple ftn controlling same pin
          wait_for_esp_response(1000);              //toggle state

          GetGPS();                               //get GPS info
          get_homepage(ch_id);                    //update homepage
        }

        if (strncmp(pb, "GET /?toggle", 12) == 0)   //compare the read buffer to "GET /?toggle"
        { //this is not a good way to do this, a refresh can toggle the state of the pin
          wait_for_esp_response(1000);              //toggle state

          //toggle != toggle;                       //toggle state-->system didn't like this way

          if (toggle == false)
            toggle = true;                          //example of how to toggle a value
          else
            toggle = false;

          get_homepage(ch_id);                    //update homepage or next page ftn call -->need to seperate current homepage from the serve page commands
        }

        if (strncmp(pb, "GET /?pin=ON1", 12) == 0)  //compare the read buffer to "GET /?pin=ON1"
        { //normally would not want multiple ftn controlling same pin
          wait_for_esp_response(1000);              //toggle state
          digitalWrite(ledPin, HIGH);               //set output to state-->might want some feedback to check that this actually happens
          get_homepage(ch_id);                    //update homepage
        }

        if (strncmp(pb, "GET /?pin=OFF1", 13) == 0) //compare the read buffer to "GET /?pin=ON1"
        { //normally would not want multiple ftn controlling same pin
          wait_for_esp_response(1000);              //toggle state
          digitalWrite(ledPin, LOW);                //set output to state
          get_homepage(ch_id);                    //update homepage
        }

        if (strncmp(pb, "GET /", 5) == 0)
        {
          wait_for_esp_response(1000);
          Serial.println("-> serving homepage");
          get_homepage(ch_id);
        }
      }//end of if (packet_len > 0)
    }//end of if (strncmp(buffer, "+IPD,", 5) == 0)
  }//end of if (read_till_eol())
}//end of void parseWeb() 
