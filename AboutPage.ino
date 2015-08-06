void get_aboutpage(int ch_id)
{
    delay(100);
 

  //HTML tags and stuff
  String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\nRefresh: 300\r\n";
  String content = "";
  content += "<!DOCTYPE html>";
  content += "<html>";
  content += "<head>";
  content += "<h1><font color=#dddddd> DanNet Web Server</h1>";
  content += "<title>About Rocket Project v-2</title>";
  //GPS SW Ver.
  content += "<p> TinyGPS library v. ";
  content += String(TinyGPS::library_version());
   //HTML buttons
  content += "<a href=\"?home\"><button style=height:150px;width:200px;margin:50px><h1>Home</br>Page</h></button></a></h6>";

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
}//end of void get_aboutpage(int ch_id)

