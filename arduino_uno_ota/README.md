# ArduinoOTA
Arduino OTA programming</br></br>
<b>Steps</b></br>
<ul>
<li>Upload OTATest.ino firmware to ESP8266 module </br>(currently supports upload program wirelessly to Arduino Uno only)</li>
<li>Make circuit connections between Arduino Uno and ESP8266 module</li>
<li>Install Python 2.7 and set environmental variable</li>
<li>Paste files to Arduino Directory</li>
</ul>
<b>Upload firmware</b></br>
<ul>
<li>The sketch can be uploaded to ESP8266 using Arduino IDE.</li>
<li>Uncomment the line saveConfig and while(1) in void setup() to save your network ssid and password to ESP8266 internal flash. Upload and run the sketch until the action is completed. Comment the lines again, upload and run the sketch. Can implement own WiFi connection system instead of this method.</li>
<li>In this firmware, ESP8266 acts as UART bridge to upload program to Arduino</li>
</ul>

<b>Circuit Connection between Arduino and ESP8266</b></br>
<table>
<tr>
  <td><b>Arduino Uno</b></td>
  <td><b>ESP8266</b></td>
</tr>
<tr>
  <td>RX</td>
  <td>TX</td>
</tr>
<tr>
  <td>TX</td>
  <td>RX</td>
</tr>
<tr>
  <td>RESET</td>
  <td>Pin 4</td>
</tr>
<tr>
  <td>3V3</td>
  <td>VCC</td>
</tr>
<tr>
  <td>GND</td>
  <td>GND</td>
</tr>
</table>
<p><b>Install Python 2.7 and set environmental variable</b></br>
Install Python 2.7 from official website. After installation is complete, set python as environmental variable so PC will know where to find python.exe when executing python script.</p>
<p><b>Paste files to Arduino Directory</b></br>
Go to Arduino -> Hardware -> arduino -> avr, paste 3 files into this directory: ota_arduino.py, board.txt and platform.txt. Make a copy or backup first before doing so.Restart Arduino IDE. There should be an option to upload program using Serial or OTA. </br></p>


