void get_homepage(int ch_id)
{
  n = n + 1; //used to count number of times accessed
  //HTML tags and stuff
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 300\r\n";
  String content = "";
  content += "<!DOCTYPE html>";
  content += "<html>";
  content += "<h1><font color=#dddddd> DanNet Web Server &nbsp;<a href=\"?about\"><button style=height:50px;width:100px;margin:10px><h>About</h></button></a></h1>";
  content += "<title>Teensy3.1-ESP8266 Wi-Fi Analog pin 0</title>";
  content += " <h1> <font color=#dddddd> TEENSY 3.1 </font> </h1>";
  content += " <p> <font color=#ff66ff> ESP8266 Wireless Webserver </font> </p>";
  //pull a value from global var
  content += " <p> <font color=#ffffff> Teensy server uptime ";
  content += String(millis());
  content += " milliseconds </p>";
  //pull a value from local var
  content += "<p> This page has been served ";
  content += String(n);
  content += " times since last reboot </font> </p>";
  content += "<p> State of the Toggle ";
  content += String(toggle);
  content += "<p> <font color=#ff66ff>  GPS Altitude is  ";
  content += String(falt);
  content += "<p> <font color=#ff66ff>  GPS Latitude is  ";
  content += String(flat);
  content += "<p> <font color=#ff66ff>  GPS Longitude is  ";
  content += String(flon);
  //pull a value from hardware
  content += "<p> <font color=#ff66ff>  Analog 9 pin 23 is currently ";
  content += String(BatteryLevel);
    //  content += String((int)BatteryLevel);   //could be recast as int to show whole number
  content += "  Volts </font> </p>";

  //HTML buttons
  content += "<h6>Toggle <a href=\"?toggle\"><button style=height:150px;width:200px;margin:50px><h1>PRESS</br>ME</h></button></a>&nbsp;Position <a href=\"?GPS\"><button style=height:150px;width:200px;margin:50px><h1>Get</br>GPS</h></button></a></h6>";
  content += "<h6>PIN_13 <a href=\"?pin=ON1\"><button style=height:100px;width:200px;margin:10px><h1>ON</button></a>&nbsp;<a href=\"?pin=OFF1\"><button style=height:100px;width:200px;margin:10px><h1>OFF</button></a></h6>";
  content += "<h6>GPIO2 <a href=\"?pin=ON2\"><button style=dsiplay:none;height:100px;width:200px;margin:10px><h1>ON</button></a>&nbsp;<a href=\"?pin=OFF2\"><button style=height:100px;width:200px;margin:10px><h1>OFF</button></a></h6>";
  content += "<button style=visibility:hidden;height:100px;width:200px;margin:50px><h1>NO</br>PRESS</br>ME</h></button>";     //hide a button
  //HTML body setup
  content += "<style>";
  content += "body {font:24px arialbold;background-color:#222222;}";
  content += ".unselectable {";
  content += "-moz-user-select: -moz-none;";
  content += "-khtml-user-select: none;";
  content += "-webkit-user-select: none;";
  content += "-ms-user-select: none;";
  content += "user-select: none;}";
  //HTML tag cleanup
  content += "</style>";
  content += "</head>";
  content += "</body>";
  content += "</html>";
  content += "<br />\n";
  content += "\r\n";

  header += "Content-Length:";
  header += (int)(content.length());
  header += "\r\n\r\n";
  //should be using sendCommand here for common code-->maybe someday
  //pull this out and crate new ftn for serve_content, then call this from various "pages"
  Serial1.print("AT+CIPSEND=");   //tell the ESP8266 to expect something from serial--> doc says 2048bytes MAX
  Serial1.print(ch_id);
  Serial1.print(",");
  Serial1.println(header.length() + content.length()); //header.length()+content.length() could be used for buffer control of what goes out

  if (wait_for_esp_response(2000))
  {
    Serial1.print(header);
    Serial1.print(content);
  }
  else
  {
    Serial1.print("AT+CIPCLOSE=");
    Serial1.println(ch_id);
  }
}//end of void get_homepage(int ch_id)

