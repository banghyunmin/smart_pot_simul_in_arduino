const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<title>IOT PROJECT</title>
</head>
<body>

<div id="demo">
<h1>SMART POT SETTING</h1>
  <input type="text" style="width: 150px" name="cds" id="CDSinput"/>
	<button type="button" style="width: 150px" onclick="sendCds()">Cds setting</button><BR>
  <input type="text" style="width: 150px" name="temp" id="TEMPinput"/>
	<button type="button" style="width: 150px" onclick="sendTemp()">Temperature setting</button><BR>
  <input type="text" style="width: 150px" name="hum" id="HUMinput"/>
	<button type="button" style="width: 150px" onclick="sendHum()">Humidity setting</button><BR>
</div>

<div>
	CDS  Value is : <span id="CDSValue">0</span>,
	CDS  limit is : <span id="CDSState">200</span><br>

	TEMP Value is : <span id="TEMPValue">0</span>,
	TEMP limit is : <span id="TEMPState">0</span><br>

	HUM  Value is : <span id="HUMValue">0</span>,
	HUM  limit is : <span id="HUMState">30</span><br>
</div>
<BR><BR>
<div>
  <button type="button" onclick="automode()">AUTO MODE</button><BR>
	[ <span id="autoState">OFF</span> ]
</div>
<script>
function sendCds() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CDSState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET",
  "setCDS?CDSinput="+document.getElementById("CDSinput").value, true);
  xhttp.send();
}
function sendTemp() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TEMPState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET",
  "setTEMP?TEMPinput="+document.getElementById("TEMPinput").value, true);
  xhttp.send();
}
function sendHum() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("HUMState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET",
  "setHUM?HUMinput="+document.getElementById("HUMinput").value, true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getCDS();
  getTEMP();
  getHUM();
  getmode();
}, 2000); //2000mSeconds update rate

function getCDS() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CDSValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readCDS", true);
  xhttp.send();
}
function getTEMP() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("TEMPValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readTEMP", true);
  xhttp.send();
}
function getHUM() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("HUMValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readHUM", true);
  xhttp.send();
}
function getmode() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("autoState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "getAUTO", true);
  xhttp.send();
}
function automode() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "setAUTO", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";