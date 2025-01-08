/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: script.js is part of WeTimer / WeDT                           */
/*                                                                          */
/* WeTimer / WeDT is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* WeTimer / WeDT is distributed in the hope that it will be useful, but    */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                          */
/****************************************************************************/

// pour debug du developpement, adresse IP de la Wemos connectée au wifi :
//var netIP     = '192.168.1.189'; // DomoPassaduy proto 1
//var netIP     = '192.168.1.137'; // DomoPassaduy proto 
//var netIP     = '10.10.10.10';
var netIP     = 'wetimer.local';
var netDevURL = 'http://' + netIP;

var refreshTimeoutID = 0;
var tblDelai = new Array(10);
var pServo   = new Array(3);
pServo[0]    = new Array(11);
pServo[1]    = new Array(11);
pServo[2]    = new Array(11);
var cServo   = new Array(3);
cServo[0]    = new Array(4);
cServo[1]    = new Array(4);
cServo[2]    = new Array(4);
var cServoOK = false;
var testModeEnCours = 0;

var celluleEnCours = null;
var delaiEnCours   = null;
var changeTimeoutID = null;
var isTouchDevice = 'ontouchstart' in document.documentElement;

var ws          = null; // webSocket
var wsm_max_len = 4096; /* bigger length causes uart0 buffer overflow with low speed smart device */
var switch_on   = 0;
var switch_off  = 1
var switch_0    = switch_off;
var switch_1    = switch_off;
var status      = null;

var ssid = "";
var pwd  = "";
var apConfigChange = false;
var getVersionOK = false;

var pageEnCours = 1;
var description = "";

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
function index_beforeunload() {
  localStorage.setItem("unloadTime", JSON.stringify(new Date()));
}
async function index_onload() {
  // Ajuste la position du pied de page en fonction de la hauteur
  index_resize();
  // Récupère les données de la minuterie
  XMLHttpRequest_get("/getversion");
  XMLHttpRequest_get("/getstatus");
  XMLHttpRequest_get("/gettempvol");
  // Connexion au WebSocket pour info dynamiques
  ws_connect();
  // Retrouve la page active en cas de reload
  if (isReloadOrOnload() == "reload") {
    var storedPage = localStorage.getItem("pageEnCours");
    if (storedPage !== null) {
      pageEnCours = storedPage;
      if (pageEnCours == 2) {
        showSettings();
      } 
    }
  }
}
function isReloadOrOnload() {
  // Détermine si c'est un refresh ou un nouveau chargement
  let loadTime = new Date();
  let unloadTime = localStorage.getItem("unloadTime");
  if (unloadTime === null) {
    return "onload";
  } else {
    unloadTime = new Date(JSON.parse(unloadTime));
    let refreshTime = loadTime.getTime() - unloadTime.getTime();
    if(refreshTime < 3000) { // moins de 3 secondes milliseconds
      return "reload";
    } else {
      return "onload";
    }
  }
}
function reloadPage1() {
  pageEnCours = 1;
  localStorage.setItem("pageEnCours", pageEnCours);
  document.location.reload(true);
}
function index_onclick() {
  // Masque le menu s'il est visible
  if (!menu.classList.contains("noshow")) {
    menu.classList.add("noshow");
  }
}
function index_ondblclick() {
  height = window.innerHeight;
  width  = window.innerWidth;

  var fontSize = parseFloat(window.getComputedStyle(document.body).getPropertyValue('font-size'));
  var fontName = window.getComputedStyle(document.body).getPropertyValue('font-family');
  var fSize2   = window.getComputedStyle(document.getElementById("description_page1", null)).getPropertyValue("font-size") 
  var fSize3   = window.getComputedStyle(document.getElementById("tsetup", null)).getPropertyValue("font-size") 
  var message = "";
  message  = "Hauteur   = " + String(height);
  message += "\nLargeur   = " + String(width);
  message += "\nFont name = " + String(fontName);
  message += "\nFont size = " + String(fontSize) + "px";
  message += "\nfSize2 = " + fSize2;
  message += "\nfSize3 = " + fSize3;
  alert(message);
}
function index_resize() {
  height = window.innerHeight;
  width  = window.innerWidth;
  if (height < 730) {
    pied_page = document.getElementById("pied_page");
    pied_page.style.bottom = "";
    pied_page.style.top = (730 - pied_page.offsetHeight) + "px";
  } else {
    pied_page.style.top = "";
    pied_page.style.bottom = "0";
  }
}
function XMLHttpRequest_get(requette) {
  // Requette XML HTTP GET
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (xhttp.readyState == 4) {
      if ((xhttp.status == 200) || (xhttp.status == 0)) {
        XMLHttpResult(requette, xhttp.responseXML, xhttp.responseText);
      } else {
        alert("XMLHttpRequest_get(" + requette + ") : Error " + xhttp.status + "\n" + xhttp.responseText);
      }
    }
  };
  if (location.protocol == 'file:') {
    uri = netDevURL + requette;
  } else {
    uri = requette;
  }
  xhttp.open("GET", uri, true);
  xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
  xhttp.send();
}
async function XMLHttpRequest_post(requette) {
  // Requette XML HTTP POST
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (xhttp.readyState == 4) {
      if ((xhttp.status == 200) || (xhttp.status == 0)) {
        XMLHttpResult(requette, xhttp.responseXML, xhttp.responseText);
      } else {
        alert("XMLHttpRequest_post() : Error " + xhttp.status + "\n" + xhttp.responseText);
      }
    }
  };
  if (location.protocol == 'file:') {
    postURI = netDevURL + requette;
  } else {
    postURI = requette;
  }
  // Données à envoyer
  var toSend = "";

  if (requette == "/settempvol") {
    //------------------------------------------------------------------
    var tempVol = document.getElementById("tempVol").value;
    toSend = "tempvol=" + tempVol;
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);

  } else if (requette == "/setconfig") {
    //------------------------------------------------------------------
    // Transforme les tableaux de délais et positions de servo en JSON
    toSend += "json={"
    toSend += "\"delai\":\"" + JSON.stringify(tblDelai) + "\",";
    toSend += "\"pservo\":\"" + JSON.stringify(pServo) + "\"}";
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);

  } else if (requette == "/setservoconfig") {
    //------------------------------------------------------------------
    // Transforme le tableaux de configuration de servo en JSON
    toSend += "json={"
    toSend += "\"cservo\":\"" + JSON.stringify(cServo) + "\",";
    toSend += "\"pservo\":\"" + JSON.stringify(pServo) + "\"}";
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);

  } else if ((requette == "/servotest") || (requette == "/servopos")){
    //------------------------------------------------------------------
    if (requette == "/servotest") {
      // Retrouve le mode de test (val ou pwm)
      var mode = getTestMode();
      // Données POST : mode, servonum, limit1, limit2, pwm1, pwm2, val
      var sNum   = document.getElementById("servoNum").value;
      var limit1 = document.getElementById("course1").value;
      var limit2 = document.getElementById("course2").value;
      var pwm1   = document.getElementById("pwm1").value;
      var pwm2   = document.getElementById("pwm2").value;
      var val    = document.getElementById("confRange").value;
    } else { // requette == "/servopos"
      // Force le mode de test à val
      var mode = 0;
      // Données POST : mode, servonum, limit1, limit2, pwm1, pwm2, val
      var sNum   = document.getElementById("sNum").innerText;
      var limit1 = cServo[sNum][0];
      var limit2 = cServo[sNum][1];
      var pwm1   = cServo[sNum][2];
      var pwm2   = cServo[sNum][3];
      var val    = document.getElementById("posAdjust").value
    }
    toSend  = "mode="      + mode;
    toSend += "&servonum=" + sNum;
    toSend += "&limit1="   + limit1;
    toSend += "&limit2="   + limit2;
    toSend += "&pwm1="     + pwm1;
    toSend += "&pwm2="     + pwm2;
    toSend += "&val="      + val;
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);

  } else if (requette == "/uploadconfig") {
    //------------------------------------------------------------------
    // Envoi d'un fichier de configuration complet
    var file = document.getElementById("fileSelect").files[0];  // file from input
    var toSend = await readFile(file)
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "text/plain");
    xhttp.send(toSend);

  } else if (requette == "/setdescription") {
    //------------------------------------------------------------------
    toSend += "{\"description\":\"" + escapeXML(description) + "\"}";
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);

  } else if ((requette == "/testflasher") || (requette == "/setflashdata")) {
    //------------------------------------------------------------------
    var test_vol    = document.getElementById("btn_test_vol").classList.contains("on");
    var test_verrou = document.getElementById("btn_test_verrou").classList.contains("on");
    if (test_verrou) {
      toSend = "mode=2";
    } else if (test_vol) {
      toSend = "mode=3";
    } else {
      toSend = "mode=0";
    }
    if ((test_verrou) || (test_vol) || (requette == "/setflashdata")) {
      toSend += "&cycle=" + document.getElementById("cycleFlash").value;
      toSend += "&allume=" + document.getElementById("allumeFlash").value;
      toSend += "&eteind=" + document.getElementById("eteintFlash").value;
      toSend += "&nombre=" + document.getElementById("nbFlash").value;
    }
      // Ajoute les paramètres on / off
    if (requette == "/setflashdata") {
      var flash_verrou = document.getElementById("btn_flash_verrou").classList.contains("on");
      if (flash_verrou) {
        toSend += "&flash_verrou=1"
      } else {
        toSend += "&flash_verrou=0"
      }
      var flash_vol_on = document.getElementById("btn_flash_vol").classList.contains("on");
      if (flash_vol_on) {
        toSend += "&flash_vol_on=1"
      } else {
        toSend += "&flash_vol_on=0"
      }
    }
    // Poste la requette
    xhttp.open("POST", postURI, true);
    xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhttp.send(toSend);
  }

}
async function readFile(file) {
    let result = await new Promise((resolve) => {
        let fileReader = new FileReader();
        fileReader.onload = (e) => resolve(fileReader.result);
        fileReader.readAsArrayBuffer(file);
    });
    return result;
}
function XMLHttpResult(requette, xml, text) {
  // Traitement de la réponse XML HTTP GET si existe...
  if (xml != null) {
    switch (requette) {
      case "/getversion":
        updateVersion(xml);
        break;
      case "/getstatus":
        updateStatus(xml);
        break;
      case "/gettempvol":
        // No break, continue next lines...
      case "/settempvol":
        updateTempVol(xml);
        break;
      case "/getconfig":
        updateConfig(xml);
        break;
      case "/getservoconfig":
        updateServoConfig(xml);
        break;
      case "/getjson":
        ;
        break;
      case "/resetflashdata":
      case "/getflashdata":
        updateFlashData(xml);
        break;
      case "/uploadconfig":
        result = unescapeXML(xml.getElementsByTagName("uploadconfig")[0].innerHTML);
        if (result == "OK") {
          alert("Fichier de configuration rechargé dans la minuterie avec succès.");
        } else {
          alert("Erreur lors de l'envoi du fichier de configuration à la minuterie :\n" + result);
        }
        // Recharge la nouvelle config de la minuterir
        XMLHttpRequest_get("/getversion");
        XMLHttpRequest_get("/getconfig");
        XMLHttpRequest_get("/getservoconfig");
        XMLHttpRequest_get("/gettempvol");
        break;
      case "/setdescription":
        result = unescapeXML(xml.getElementsByTagName("setdescription")[0].innerHTML);
        if (result != "OK") {
          alert("Erreur lors de l'envoi de la description à la minuterie :\n" + result);
        }
        XMLHttpRequest_get("/getversion");
    }
  }
}
function updateVersion(xml) {
  var pversion1 = document.getElementById("pversion1");
  var pversion2 = document.getElementById("pversion2");
  var version = xml.getElementsByTagName("version")[0].childNodes[0].nodeValue;
  pversion1.innerText = version.split(".", 2).join('.');
  pversion2.innerText = version;
  var timer_ssid = document.getElementById("timer_ssid");
  ssid = xml.getElementsByTagName("ssid")[0].innerHTML;
  pwd = xml.getElementsByTagName("pwd")[0].innerHTML;
  timer_ssid.innerText = ssid;
  description = unescapeXML(xml.getElementsByTagName("description")[0].innerHTML);
  document.getElementById("description_page1").innerText = description;
  document.getElementById("description_page2").innerText = description;
  getVersionOK = true;
}
function updateStatus(xml) {
  
  var timer_status = document.getElementById("timer_status");
  status = xml.getElementsByTagName("status")[0].childNodes[0].nodeValue;
  timer_status.innerText = status;

  if (status != "PARC") {
    if (switch_0 == switch_off) {
      document.getElementById("led0").style.backgroundImage="url(images/ledrouge.svg)";
    } else {
      document.getElementById("led0").style.backgroundImage="url(images/ledverte.svg)";
    }
    if (switch_1 == switch_off) {
      document.getElementById("led1").style.backgroundImage="url(images/ledrouge.svg)";
    } else {
      document.getElementById("led1").style.backgroundImage="url(images/ledverte.svg)";
    }
  } else {
    document.getElementById("led0").style.backgroundImage="url(images/ledeteinte.svg)";
    document.getElementById("led1").style.backgroundImage="url(images/ledeteinte.svg)";
  }

  var duree_vol = document.getElementById("duree_vol");
  var dureevol = xml.getElementsByTagName("dureevol")[0].childNodes[0].nodeValue;
  duree_vol.innerText = dureevol;

  var batterie = document.getElementById("batterie");
  var tension = xml.getElementsByTagName("tension")[0].childNodes[0].nodeValue;
  tension_float = parseFloat(tension);
  if (tension_float > 2.5) {
    batterie.innerHTML = "<p>" + tension + "v</p>";
  } else {
    batterie.innerHTML = "";
  }
  if (tension_float <= 2.5) {
    batterie.style.backgroundImage="url(images/batterie0.svg)";
  } else if (tension_float < 3.55) {
    batterie.style.backgroundImage="url(images/batterie4.svg)";
  } else if (tension_float < 3.88) {
    batterie.style.backgroundImage="url(images/batterie3.svg)";
  } else if (tension_float < 3.95) {
    batterie.style.backgroundImage="url(images/batterie2.svg)";
  } else {
    batterie.style.backgroundImage="url(images/batterie1.svg)";
  }

  // Appel recursif pour boucler au lieu d'utiliser  setInterval()
  // Cela assure que te traitement à été terminé avant de relancer
  // la requette vers le serveur web.
  autoRefreshStatus();

}
function autoRefreshStatus() {
  // Appel recursif pour boucler au lieu d'utiliser  setInterval()
  // Cela assure que te traitement à été terminé avant de relancer
  // la requette vers le serveur web.
  var interval = 1000; // Refresh toute les secondes
  //if (refreshTimeoutID > 0) {
    clearTimeout(refreshTimeoutID)
  //}
  refreshTimeoutID = setTimeout(function() { XMLHttpRequest_get("/getstatus") }, interval);
}
function updateTempVol(xml) {

  var tempVol = document.getElementById("tempVol");
  var tempvol = xml.getElementsByTagName("tempvol")[0].childNodes[0].nodeValue;
  tempVol.value = parseInt(tempvol/100);
  calculTemps('tempVol', 'tempsMinutes');
}
function updateConfig(xml) {
  var table = document.getElementById("setup_table");
  var delais = xml.getElementsByTagName("delai");
  if (delais.length != 10) {
    document.location.reload(true);
  }
  for (const d of delais) {
    num = Number(d.getElementsByTagName("num")[0].innerHTML);
    val = Number(d.getElementsByTagName("val")[0].innerHTML);
    tblDelai[num] = val;
  }
  var pservo = xml.getElementsByTagName("pservo");
  if (pservo.length != 33) {
    document.location.reload(true);
  }
  for (const p of pservo) {
    num = Number(p.getElementsByTagName("num")[0].innerHTML);
    pos = Number(p.getElementsByTagName("pos")[0].innerHTML);
    val = Number(p.getElementsByTagName("microsecondes")[0].innerHTML);
    pServo[num][pos] = val;
  }
  var buff  = "";
  var cumul = 0;
  /*
  // Status :                      // Déclencheur :                                      // Servos :
  #define  STATUS_PARC           0 // Position au repos (DT +5 secondes) et à l'allumage // Position[0]
  #define  STATUS_ARMEMENT       1 // STATUS_PARC         + délai N°0 (délai armement)    // <No change>
  #define  STATUS_TREUIL_MONTEE  2 // STATUS_ARMEMENT     + SWITCH_1 = ON                 // Position[1]
  #define  STATUS_TREUIL_VIRAGE  3 // STATUS_ARMEMENT     + SWITCH_1 = OFF                // Position[2]
  #define  STATUS_DEVERROUILLE   4 // STATUS_ARMEMENT     + SWITCH_1 = ON + SWITCH_2 = ON // Position[3]
  #define  STATUS_LARGUE         5 // STATUS_DEVERROUILLE + SWITCH_1 = OFF                // <No change>
  #define  STATUS_PITCHUP        6 // STATUS_LARGUE       + délai N°1                     // Position[4]
  #define  STATUS_MONTEE_1       7 // STATUS_PITCHUP      + délai N°2                     // Position[5]
  #define  STATUS_MONTEE_2       8 // STATUS_MONTEE_1     + délai N°3                     // Position[6]
  #define  STATUS_BUNT           9 // STATUS_MONTEE_2     + délai N°4                     // Position[7]
  #define  STATUS_PLANE_1       10 // STATUS_BUNT         + délai N°5                     // Position[8]
  #define  STATUS_PLANE_2       11 // STATUS_PLANE_1      + délai N°6                     // Position[9]
  #define  STATUS_DT            12 // STATUS_PLANE_2      + délai N°7                     // Position[10]
  //retour STATUS_PARC           0 // STATUS_DT             + délai N°8 (= 5 secondes)      // Position[0]
  // Délai N°9 = futur délai de refermeture pour crochet piloté par servo
  */
  //       <tr><td>Phase</td><td>Délai</td><td>Cumul</td><td>Servo1</td><td>Servo2</td><td>Servo3</td></tr>
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_0'>Parc</td>"
  buff += "  <td>&nbsp;</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds00.svg' /></td>"
  buff += "  <td class='cellServo' id='servo_0_0' onclick='adjustServo(this)'>" + pServo[0][0]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_0' onclick='adjustServo(this)'>" + pServo[1][0]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_0' onclick='adjustServo(this)'>" + pServo[2][0]  + "</td>"
  buff += "</tr>\n";
  buff += "<tr>"
  buff += "  <td class='tdCol1'>Armement</td>"
  buff += "  <td>&nbsp;</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds21.svg' /></td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "</tr>\n";
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_1'>Treuil montée</td>"
  buff += "  <td class='cellDelai' id='delai_0' onclick='inputDelai(this)'>" + (tblDelai[0] /100).toFixed(2) + "</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds21.svg' /></td>"
  buff += "  <td class='cellServo' id='servo_0_1' onclick='adjustServo(this)'>" + pServo[0][1]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_1' onclick='adjustServo(this)'>" + pServo[1][1]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_1' onclick='adjustServo(this)'>" + pServo[2][1]  + "</td>"
  buff += "</tr>\n";
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_2'>Treuil virage</td>"
  buff += "  <td>&nbsp;</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds11.svg' /></td>"
  buff += "  <td class='cellServo' id='servo_0_2' onclick='adjustServo(this)'>" + pServo[0][2]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_2' onclick='adjustServo(this)'>" + pServo[1][2]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_2' onclick='adjustServo(this)'>" + pServo[2][2]  + "</td>"
  buff += "</tr>\n";
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_3'>Dévérouillé</td>"
  buff += "  <td>&nbsp;</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds22.svg' /></td>"
  buff += "  <td class='cellServo' id='servo_0_3' onclick='adjustServo(this)'>" + pServo[0][3]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_3' onclick='adjustServo(this)'>" + pServo[1][3]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_3' onclick='adjustServo(this)'>" + pServo[2][3]  + "</td>"
  buff += "</tr>\n";
  buff += "<tr>"
  buff += "  <td class='tdCol1'>Larguage</td>"
  buff += "  <td class='center'><image class='tbl_leds' src='images/leds10.svg' /></td>"
  buff += "  <td class='cellValue'>0</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[1] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_4'>Pitchup</td>"
  buff += "  <td class='cellDelai' id='delai_1' onclick='inputDelai(this)'>" + (tblDelai[1] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_4' onclick='adjustServo(this)'>" + pServo[0][4]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_4' onclick='adjustServo(this)'>" + pServo[1][4]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_4' onclick='adjustServo(this)'>" + pServo[2][4]  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[2] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_5'>Montée 1</td>"
  buff += "  <td class='cellDelai' id='delai_2' onclick='inputDelai(this)'>" + (tblDelai[2] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_5' onclick='adjustServo(this)'>" + pServo[0][5]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_5' onclick='adjustServo(this)'>" + pServo[1][5]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_5' onclick='adjustServo(this)'>" + pServo[2][5]  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[3] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_6'>Montée 2</td>"
  buff += "  <td class='cellDelai' id='delai_3' onclick='inputDelai(this)'>" + (tblDelai[3] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_6' onclick='adjustServo(this)'>" + pServo[0][6]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_6' onclick='adjustServo(this)'>" + pServo[1][6]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_6' onclick='adjustServo(this)'>" + pServo[2][6]  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[4] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_7'>Bunt</td>"
  buff += "  <td class='cellDelai' id='delai_4' onclick='inputDelai(this)'>" + (tblDelai[4] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_7' onclick='adjustServo(this)'>" + pServo[0][7]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_7' onclick='adjustServo(this)'>" + pServo[1][7]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_7' onclick='adjustServo(this)'>" + pServo[2][7]  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[5] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_8'>Plané 1</td>"
  buff += "  <td class='cellDelai' id='delai_5' onclick='inputDelai(this)'>" + (tblDelai[5] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_8' onclick='adjustServo(this)'>" + pServo[0][8]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_8' onclick='adjustServo(this)'>" + pServo[1][8]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_8' onclick='adjustServo(this)'>" + pServo[2][8]  + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[6] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_9'>Plané 2</td>"
  buff += "  <td class='cellDelai' id='delai_6' onclick='inputDelai(this)'>" + (tblDelai[6] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_9' onclick='adjustServo(this)'>" + pServo[0][9]  + "</td>"
  buff += "  <td class='cellServo' id='servo_1_9' onclick='adjustServo(this)'>" + pServo[1][9]  + "</td>"
  buff += "  <td class='cellServo' id='servo_2_9' onclick='adjustServo(this)'>" + pServo[2][9]  + "</td>"
  buff += "</tr>\n";
  cumul = tblDelai[7] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1' id='posName_10'>DT</td>"
  buff += "  <td class='cellValue noInput' id='delai_7'>" + (tblDelai[7] /100).toFixed(0) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(0) + "</td>"
  buff += "  <td class='cellServo' id='servo_0_10' onclick='adjustServo(this)'>" + pServo[0][10] + "</td>"
  buff += "  <td class='cellServo' id='servo_1_10' onclick='adjustServo(this)'>" + pServo[1][10] + "</td>"
  buff += "  <td class='cellServo' id='servo_2_10' onclick='adjustServo(this)'>" + pServo[2][10] + "</td>"
  buff += "</tr>\n";
  cumul += tblDelai[8] /100;
  buff += "<tr>"
  buff += "  <td class='tdCol1'>Retour parc</td>"
  buff += "  <td class='cellDelai' id='delai_8' onclick='inputDelai(this)'>" + (tblDelai[8] /100).toFixed(2) + "</td>"
  buff += "  <td class='cellValue'>" + cumul.toFixed(2) + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;" + "</td>"
  buff += "</tr>\n";
  // 1 ligne de plus prévue pour le délai d'ouverture de crochet
  // (option future
  buff += "<tr>"
  buff += "  <td class='tdCol1 disabled'>Refermeture crochet</td>"
  buff += "  <td class='cellDelai disabled' id='delai_9' onclick=';'>" + (tblDelai[9] /100).toFixed(2) + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;"  + "</td>"
  buff += "  <td>" + "&nbsp;" + "</td>"
  buff += "</tr>\n";

  table.innerHTML = buff;

}
function updateServoConfig(xml) {
  
  var cservo = xml.getElementsByTagName("cservo")
  if (cservo.length != 12) {
    document.location.reload(true);
  }
  for (const c of cservo) {
    num  = Number(c.getElementsByTagName("num")[0].innerHTML)
    conf = Number(c.getElementsByTagName("conf")[0].innerHTML)
    val  = Number(c.getElementsByTagName("val")[0].innerHTML)
    cServo[num][conf] = val;
  }
  cServoOK = true;

}
function changeVal(inputID, increment, entier=true) {

  var inp = document.getElementById(inputID);
  var newVal = Number(inp.value);
  newVal += increment;
  forceInterval(inp, newVal, Number(inp.min), Number(inp.max));

}
function calculTemps(entree, resultat, entier=true) {
  var inp = document.getElementsByName(entree);
  if (inp.length > 0) {
    if (entier) {
      var secondes = parseInt(inp[0].value, 10);
    } else {
      var secondes = parseFloat(inp[0].value);
    }
    var minutes = Math.floor(secondes / 60);
    var sec = parseFloat(secondes - minutes * 60).toFixed(2);
    var result = document.getElementById(resultat);
    if (result != null) {
      result.textContent = minutes + " minute(s) " + sec + " seconde(s)";
    }
  }
}
function showSettings() {

  // Swap la visibilité des 2 pages
  page_1 = document.getElementById("page_1");
  page_2 = document.getElementById("page_2");
  if (page_1.classList.contains("noshow")) {
    page_1.classList.remove("noshow");
    page_2.classList.add("noshow");
    pageEnCours = 1;
  } else {
    page_1.classList.add("noshow");
    page_2.classList.remove("noshow");
    XMLHttpRequest_get("/getconfig");
    pageEnCours = 2;
  }
  // Sauvegarde de la page en cours pour y revenir au reload
  localStorage.setItem("pageEnCours", pageEnCours);

}
async function adjustServo(td) {

  // Memorise la cellule de tableau en cours d'édition
  celluleEnCours = td;

  var arg = td.id.split("_");

  // Titre de la boite de dialogue
  var sNum        = Number(arg[1]);
  var numPosition = Number(arg[2]);
  var nomPosition = document.getElementById("posName_" + String(numPosition)).innerText
  var sNumAffiche = sNum + 1;
  document.getElementById("input_position_title").innerHTML = "servo<span id='sNum'>" + sNumAffiche + "</span><br /><span class='nobold'>Position[<span id='pNum'>" + numPosition + "</span>] " + nomPosition + "</span>";
  // Retrouve la plage de réglage du servo
  cServoOK = false
  XMLHttpRequest_get("/getservoconfig");
  while (!cServoOK) {
    await sleep(100);
  }
  cServoOK = false
  // On rempli le champ de saisie avec la valeur de la cellule de tableau
  var input = document.getElementById("posAdjust");
  input.min = cServo[sNum][0];
  input.max = cServo[sNum][1];
  input.value = parseInt(td.innerText);
  var slider = document.getElementById("posRange");
  slider.min = cServo[sNum][0];
  slider.max = cServo[sNum][1];
  slider.value = parseInt(td.innerText);
  // Affichage limites de réglage à coté du slider
  document.getElementById("posLimit1").innerText = cServo[sNum][0];
  document.getElementById("posLimit2").innerText = cServo[sNum][1];
  
  // Positionne le servo à la valeur en cours
  XMLHttpRequest_post("/servopos");
  
  // Affichage de la boite de dialogue
  dialog_mask = document.getElementById("dialog_mask");
  input_position = document.getElementById("input_position");
  dialog_mask.classList.remove("noshow");
  input_position.classList.remove("noshow");

}
function positionOK() {
  // Récupère la valeur entrée
  var input = document.getElementById("posAdjust");
  var nouvelleValeur = parseInt(input.value);
  // Mise à jour de l'affichage
  celluleEnCours.innerText = nouvelleValeur;
  // Mise à jour du tableau de données
  var sNum   = Number(document.getElementById("sNum").innerText);
  var pNum   = Number(document.getElementById("pNum").innerText);
  pServo[sNum][pNum] = nouvelleValeur;
  // Envoi des tableau de données à la minuterie
  XMLHttpRequest_post("/setconfig");
  // Fermeture de la boite de dialogue
  dialogHide();  
}
function showPosRangeVal(valeur) {
  // Mise à jour de l'input de position du servo en fonction du slider
  document.getElementById("posAdjust").value = valeur;
}
function setPosRangeVal(valeur) {
  // Mise à jour du slider en fonction de l'input de position du servo
  document.getElementById("posRange").value = valeur;
  document.getElementById("posRange").text  = valeur;
  XMLHttpRequest_post("/servopos");
}
async function configServo(sNum) {

  // Titre de la boite de dialogue
  document.getElementById("input_servo_title").innerText = "Amplitude servo" + sNum;
  // Valeurs initiales
  cServoOK = false
  XMLHttpRequest_get("/getservoconfig");
  while (!cServoOK) {
    await sleep(100);
  }
  cServoOK = false
  sNum = sNum - 1;
  // Mémorise les anciennes valeurs dans des inpute masquées
  document.getElementById("servoNum").value = sNum;
  document.getElementById("oldCourse1").value = Number(cServo[sNum][0]);
  document.getElementById("oldCourse2").value = Number(cServo[sNum][1]);
  document.getElementById("oldPwm1").value = Number(cServo[sNum][2]);
  document.getElementById("oldPwm2").value = Number(cServo[sNum][3]);
  // Met à jour les valeurs visibles
  document.getElementById("course1").value = Number(cServo[sNum][0]);
  changePlageServo("course1")
  document.getElementById("course2").value = Number(cServo[sNum][1]);
  changePlageServo("course2")
  document.getElementById("pwm1").value = Number(cServo[sNum][2]);
  document.getElementById("pwm2").value = Number(cServo[sNum][3]);
  // Force déplacement de test en mode val
  document.getElementById("sliderVal").checked = true;
  changeTestMode();
  var moyenne = Math.round((Number(cServo[sNum][0]) + Number(cServo[sNum][1])) / 2);
  showRangeVal(moyenne);
  document.getElementById("confRange").value = moyenne;
  // Affichage de la boite de dialogue
  dialog_mask = document.getElementById("dialog_mask");
  input_servo = document.getElementById("input_servo");
  dialog_mask.classList.remove("noshow");
  input_servo.classList.remove("noshow");
  // Positionne le servo :
  XMLHttpRequest_post("/servotest")
}
function inputDelai(td) {

  // Memorise la cellule de tableau en cours d'édition
  celluleEnCours = td;
  // On rempli le champ de saisie avec la valeur de la cellule de tableau
  document.getElementById("delaiSec").value = parseFloat(td.innerText);
  // On met à jour le calcul minutes/secondes
  calculTemps('delaiSec', 'delaiMinutes', false);
  // Retrouve le délai à éditer
  args = td.id.split("_");
  delaiEnCours = Number(args[1]);
  switch (delaiEnCours) {
    case 0:
      document.getElementById("input_delai_title").innerText = "Délai d'armement";
      break;
    case 1:
      document.getElementById("input_delai_title").innerText = "Délai pitchup";
      break;
    case 2:
      document.getElementById("input_delai_title").innerText = "Délai montée 1";
      break;
    case 3:
      document.getElementById("input_delai_title").innerText = "Délai montée 2";
      break;
    case 4:
      document.getElementById("input_delai_title").innerText = "Délai bunt";
      break;
    case 5:
      document.getElementById("input_delai_title").innerText = "Délai plané 1";
      break;
    case 6:
      document.getElementById("input_delai_title").innerText = "Délai plané 2";
      break;
    case 7:
      document.getElementById("input_delai_title").innerText = "Durée totale du vol";
      break;
    case 8:
      document.getElementById("input_delai_title").innerText = "Délai retour parc";
      break;
  }
  // Affichage de la boite de dialogue
  dialog_mask = document.getElementById("dialog_mask");
  input_delai = document.getElementById("input_delai");
  dialog_mask.classList.remove("noshow");
  input_delai.classList.remove("noshow");

}
function delaiOK() {
  // Récupère la valeur entrée
  var nouvelleValeur = parseFloat(document.getElementById("delaiSec").value).toFixed(2);
  // Mise à jour de l'affichage
  celluleEnCours.innerText = nouvelleValeur;
  // Mise à jour du tableau de données
  tblDelai[delaiEnCours] = Number(nouvelleValeur) * 100;
  // Envoi des tableau de données à la minuterie
  XMLHttpRequest_post("/setconfig");
  // Fermeture de la boite de dialogue
  dialogHide();
}
function dialogHide() {
  document.getElementById("input_delai").classList.add("noshow");
  document.getElementById("input_servo").classList.add("noshow");
  document.getElementById("input_position").classList.add("noshow");
  document.getElementById("input_wifi").classList.add("noshow");
  document.getElementById("input_flasher").classList.add("noshow");
  document.getElementById("dialog_mask").classList.add("noshow");
}
function swapPWM() {
  var pwm1   = document.getElementById("pwm1");
  var pwm2   = document.getElementById("pwm2");
  var tmpVal = pwm1.value;
  pwm1.value = pwm2.value;
  pwm2.value = tmpVal;
  changePlageServo("pwm1")
  changePlageServo("pwm2")
}
function changePlageServo(inputID) {
  var inp = document.getElementById(inputID);
  var course1 = Number(document.getElementById("course1").value);
  var course2 = Number(document.getElementById("course2").value);
  var pwm1    = Number(document.getElementById("pwm1").value);
  var pwm2    = Number(document.getElementById("pwm2").value);
  var range  = document.getElementById("confRange");
  var oldRangeValue = Number(range.Value);
  if (getTestMode() == 0) { // Mode valeur
    if (inp.id == "course1") {
      if (course1 < course2) {
        range.min = inp.value;
      } else {
        range.max = inp.value;
      }
    } else if (inp.id == "course2") {
      if (course1 < course2) {
        range.max = inp.value;
      } else {
        range.min = inp.value;
      }
    }
  } else { // Mode impulsion
    if (inp.id == "pwm1") {
      if (pwm1 < pwm2) {
        range.min = inp.value;
        range.classList.remove("flip");
      } else {
        range.max = inp.value;
        range.classList.add("flip");
      }
    } else if (inp.id == "pwm2") {
      if (pwm1 < pwm2) {
        range.max = inp.value;
        range.classList.remove("flip");
      } else {
        range.min = inp.value;
        range.classList.add("flip");
      }
    }
  }
  showRangeVal(range.value);
}
function showRangeVal(valeur) {
  document.getElementById("rangeValue").innerText = valeur;
}
function changeTestMode() {
  newMode = getTestMode();
  if (newMode != testModeEnCours) {
    var range      = document.getElementById("confRange");
    var rangeValue = document.getElementById("rangeValue");
    var course1    = Number(document.getElementById("course1").value);
    var course2    = Number(document.getElementById("course2").value);
    var pwm1       = Number(document.getElementById("pwm1").value);
    var pwm2       = Number(document.getElementById("pwm2").value);
    if (newMode == 0) { // Mode valeur
      var newRangeValue = remap(range.value, pwm1, pwm2, course1, course2);
      if (course1 < course2) {
        range.min = course1;
        range.max = course2;
      } else {
        range.max = course1;
        range.min = course2;
      }
      range.value = newRangeValue;
      showRangeVal(newRangeValue)
    } else { // radio.value == 1 , Mode impulsion
      var newRangeValue = remap(range.value, course1, course2, pwm1, pwm2);
      if (pwm1 < pwm2) {
        range.min = pwm1;
        range.max = pwm2;
        range.classList.remove("flip");
      } else {
        range.max = pwm1;
        range.min = pwm2;
        range.classList.add("flip");
      }
      range.value = newRangeValue;
      showRangeVal(newRangeValue)
    }
    testModeEnCours = newMode;
  }
}
function getTestMode() {
  // Retrouve le mode de test (val ou pwm)
  var radio = document.getElementsByName('slider_option');
  for (i = 0; i < radio.length; i++) {
    if (radio[i].checked) {
      return radio[i].value;
      break;
    }
  }
}
function beginTouch(event, inputID, increment) {
  event.stopPropagation();
  event.preventDefault();
  beginChange(inputID, increment);
}
function beginChange(inputID, increment) {
  var inp = document.getElementById(inputID);
  var newVal = Number(inp.value);
  newVal += increment;
  forceInterval(inp, newVal, Number(inp.min), Number(inp.max));
  if (inputID == "confRange") showRangeVal(inp.value);
  if (inputID == "posAdjust") setPosRangeVal(inp.value);
  if (changeTimeoutID) {
    clearTimeout(changeTimeoutID);
    changeTimeoutID = null;
  }
  changeTimeoutID = setTimeout(function() {
    change(inputID, increment); 
  }, 500);
}
function change(inputID, increment) {
  var inp = document.getElementById(inputID);
  var newVal = Number(inp.value);
  newVal += increment;
  forceInterval(inp, newVal, Number(inp.min), Number(inp.max));
  if (inputID == "confRange") showRangeVal(inp.value);
  if (inputID == "posAdjust") setPosRangeVal(inp.value);
  if (changeTimeoutID) {
    clearTimeout(changeTimeoutID);
    changeTimeoutID = null;
  }
  changeTimeoutID = setTimeout(function() {
    change(inputID, increment);
  }, 100);
}
function stopTouch(event, inputID) {
  event.stopPropagation();
  event.preventDefault();
  stopChange(inputID);
}
function stopChange(inputID) {
  clearTimeout(changeTimeoutID);
  changeTimeoutID = null;
  if (inputID == "confRange") XMLHttpRequest_post("/servotest");
  if (inputID == "posAdjust") XMLHttpRequest_post("/servopos");
}
function forceInterval(inp, valeur, mini, maxi) {
  if (valeur < mini) {
    valeur = mini;
  }
  if (valeur > maxi) {
    valeur = maxi;
  }
  if (inp.name == "delaiSec") {
    inp.value = valeur.toFixed(2);
  } else {
    inp.value = valeur;
  }
}
function servoOK() {
  // Valide les réglages des plages de servo
  // Numéro du servo en cours d'édition
  var sNum = Number(document.getElementById("servoNum").value);
  // Valeurs avant modification
  var oldCourse1 = Number(document.getElementById("oldCourse1").value);
  var oldCourse2 = Number(document.getElementById("oldCourse2").value);
  var oldPwm1    = Number(document.getElementById("oldPwm1").value);
  var oldPwm2    = Number(document.getElementById("oldPwm2").value);
  // Valeurs modifiées
  cServo[sNum][0] = Number(document.getElementById("course1").value);
  cServo[sNum][1] = Number(document.getElementById("course2").value);
  cServo[sNum][2] = Number(document.getElementById("pwm1").value);
  cServo[sNum][3] = Number(document.getElementById("pwm2").value);
  // Recalcul toutes les positions de servo en fonction des nouvelles plage
  for (var i=0; i<pServo[sNum].length; i++) {
    pServo[sNum][i] = remap(pServo[sNum][i], oldCourse1, oldCourse2, cServo[sNum][0], cServo[sNum][1]);
    // Met à jour les valeurs dans le tableau
    cellId = "servo_" + String(sNum) + "_" + String(i);
    cellule = document.getElementById(cellId);
    cellule.innerText = pServo[sNum][i];
  }
  // Envoi la mise à jour à la minuterie
  XMLHttpRequest_post("/setservoconfig");
  // Masque la boite de dialogue
  dialogHide();

}
function remap(in_val, in_min, in_max, out_min, out_max) {
  // Remappe un nombre d'une plage à une autre.
  // Autrement dit, une valeur de in_min serait mappée à out_min,
  // une valeur de in_max à out_max, des valeurs intermédiaires
  // à des valeurs intermédiaires.
  return Math.round((Number(in_val) - Number(in_min)) * (Number(out_max) - Number(out_min)) / (Number(in_max) - Number(in_min)) + Number(out_min));
}
function ws_connect() {
  if(ws == null) {
    if (location.protocol == 'file:') {
      ws = new WebSocket("ws://" + netIP + ":81");
    } else {
      ws = new WebSocket("ws://" + window.location.host + ":81");
    }
    document.getElementById("ws_state").innerHTML = "CONNECTING";
    ws.onopen = ws_onopen;
    ws.onclose = ws_onclose;
    ws.onmessage = ws_onmessage;
  } else
    ws.close();
}
function ws_onopen() {
  document.getElementById("ws_state").innerHTML = "<span style='color:blue'>CONNECTED</span>";
}
function ws_onclose() {
  document.getElementById("ws_state").innerHTML = "<span style='color:gray'>CLOSED</span>";
  ws.onopen = null;
  ws.onclose = null;
  ws.onmessage = null;
  ws = null;
}
function ws_onmessage(e_msg) {
  e_msg = e_msg || window.event; // MessageEvent
  const obj = JSON.parse(e_msg.data);
  
  switch (obj.key) {
    case "STATUS":
      status = obj.value;
      timer_status = document.getElementById("timer_status").innerText = status;
      if (status != "PARC") {
        if (switch_0 == switch_off) {
          document.getElementById("led0").style.backgroundImage="url(images/ledrouge.svg)";
        } else {
          document.getElementById("led0").style.backgroundImage="url(images/ledverte.svg)";
        }
        if (switch_1 == switch_off) {
          document.getElementById("led1").style.backgroundImage="url(images/ledrouge.svg)";
        } else {
          document.getElementById("led1").style.backgroundImage="url(images/ledverte.svg)";
        }
      } else {
        document.getElementById("led0").style.backgroundImage="url(images/ledeteinte.svg)";
        document.getElementById("led1").style.backgroundImage="url(images/ledeteinte.svg)";
      }
      break;
    case "SWITCH_0":
      switch_0   = Number(obj.value);
      if (switch_0 == switch_on) {
        document.getElementById("led0").style.backgroundImage="url(images/ledverte.svg)";
      } else {
        document.getElementById("led0").style.backgroundImage="url(images/ledrouge.svg)";
      }
      break;
    case "SWITCH_1":
      switch_1   = Number(obj.value);
      if (switch_1 == switch_on) {
        document.getElementById("led1").style.backgroundImage="url(images/ledverte.svg)";
      } else {
        document.getElementById("led1").style.backgroundImage="url(images/ledrouge.svg)";
      }
      break;
  }
  
}
function showMenu(event) {
  // Bascule la visibilité du menu
  var menu = document.getElementById("menu");
  if (menu.classList.contains("noshow")) {
    menu.classList.remove("noshow");
  } else {
    menu.classList.add("noshow");
  }
  event.stopPropagation();
}
function saveFile() {
  document.getElementById('fileSelect').click();
}
function openFile() {
  document.getElementById("saveLink").click();
}
function fileSelected(file_input) {
  if (file_input.files.length == 1) {
    if (confirm("Recharger la configuration :\n" + file_input.files[0].name + "\ndans la minuterie ?")) {;
      XMLHttpRequest_post("/uploadconfig");
    }
  }
}
async function inputWifi() {
  apConfigChange = false;
  var apSSID      = document.getElementById('apSSID');
  var apPwd1      = document.getElementById('apPwd1');
  var apPwd2      = document.getElementById('apPwd2');
  // Récupère les données de la minuterie
  getVersionOK = false;
  XMLHttpRequest_get("/getversion");
  while (!getVersionOK) {
    await sleep(100);
  }
  getVersionOK = false;
  // Mise à jour des champs d'entrée
  apSSID.value = ssid;
  apPwd1.value = pwd;
  // Affichage de la boite de dialogue
  dialog_mask = document.getElementById("dialog_mask");
  input_delai = document.getElementById("input_wifi");
  dialog_mask.classList.remove("noshow");
  input_delai.classList.remove("noshow");
}
function updateAPconfig() {
  // SSID
  // Un SSID vide rétablie la valeur par défaut : WeThermic_XX:YY:ZZ
  // avec XX:YY:ZZ correspondant à la fin de l'adresse MAC de l'ESP
  var apSSID      = document.getElementById('apSSID').value;
  if ((apSSID.length != 0) && (apSSID.length < 2)) {
    alert("SSID trop court !");
    return;
  }
  if ((apSSID.length != 0) && (apSSID.length > 32)) {
    alert("SSID trop long !");
    return;
  }
  // Mot de passe
  var apPwd1      = document.getElementById('apPwd1').value;
  var apPwd2      = document.getElementById('apPwd2').value;
  // Vérification du mot de passe
  if ((apPwd1.length != 0) || (apPwd1.length != 0)) {
    if (apPwd1 !== apPwd2) {
      alert("Les mot de passe entrés ne son pas identiques !");
      return;
    }
    if (apPwd1.length < 8) {
      alert("Mot de passe trop court !");
      return;
    }
    if (apPwd1.length > 63) {
      alert("Mot de passe trop long !");
      return;
    }
  }
  // Le mot de passe entré semble bon, on envoie la requette XMLHttp
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (xhttp.readyState == 4) {
      if ((xhttp.status == 200) || (xhttp.status == 0)) {
        // Recupération du résultat
        result = xhttp.responseXML.getElementsByTagName("result")[0].innerHTML;
        if (result == "OK") {
          alert("Mise à jour de la configuration WiFi OK.");
        } else {
          alert("Erreur lors de la mise à jour de la configuration WiFi :\n" + result);
        }
      } else {
        alert("updateAPconfig() : Error " + xhttp.status + "\n" + xhttp.responseText);
      }
    }
  };
  var ssid_encode = encodeURIComponent(apSSID);
  var pwd_encode  = encodeURIComponent(apPwd1);
  if (location.protocol == 'file:') {
    xhttp.open("POST", netDevURL + "/setapconfig", true);
  } else {
    xhttp.open("POST", "/setapconfig", true);
  }
  xhttp.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
  xhttp.send("ssid=" + ssid_encode + "&pwd=" + pwd_encode);
  // Fermeture de la boite de dialogue
  dialogHide();
}
function toogleShowPasswd() {
  const bouton = document.getElementById('btnShowPasswd');
  var apPwd1 = document.getElementById('apPwd1');
  var apPwd2 = document.getElementById('apPwd2');
  if (apPwd1.type === "password") {
    apPwd1.type = "text";
    apPwd2.type = "text";
    bouton.src="images/oeuil-barre.svg"
    bouton.title="Hide password"
  } else {
    apPwd1.type = "password";
    apPwd2.type = "password";
    bouton.src="images/oeuil.svg"
    bouton.title="Show password"
  }
}
function inputDescription() {
  // Saisie de la description de la minuterie
  var newDescription = prompt("Description de cette\nconfiguration de minuterie :", description);
  if (newDescription !== null) {
    if (newDescription.length > 63) {
      alert("Description trop longue !\n(longueur maxi : 63 caractères)");
      return;
    }
    description = newDescription;
    XMLHttpRequest_post("/setdescription");
  }
}
function escapeXML(buffer) {
  // Caractère ne devant pas apparaître dans un XML :
  // "   &quot;
  // '   &apos;
  // <   &lt;
  // >   &gt;
  // &   &amp;
  var escape = buffer;
  escape = escape.replaceAll("&", "&amp;");
  escape = escape.replaceAll("\"", "&quot;");
  escape = escape.replaceAll("'", "&apos;");
  escape = escape.replaceAll("<", "&lt;");
  escape = escape.replaceAll(">", "&gt;");
  return escape;
}
function unescapeXML(buffer) {
  var unescape = buffer;
  unescape = buffer.replaceAll("&amp;", "&");
  unescape = unescape.replaceAll("&quot;", "\"", );
  unescape = unescape.replaceAll("&apos;", "'", );
  unescape = unescape.replaceAll("&lt;", "<", );
  unescape = unescape.replaceAll("&gt;", ">", );
  return unescape;
}
function rd_ton_off(event) {
  // Bascule la visibilité du menu
  var btn_rdt    = document.getElementById("btn_rdt");
  var btn_on_off = document.getElementById("rdtonoff");
  if (btn_rdt.classList.contains("btn_rdt_disabled")) {
    btn_rdt.classList.remove("btn_rdt_disabled");
    btn_on_off.src = "images/rdton.svg";
  } else {
    btn_rdt.classList.add("btn_rdt_disabled");
    btn_on_off.src = "images/rdtoff.svg";
  }
  event.stopPropagation();
}
function sendRDT() {
  var btn_rdt    = document.getElementById("btn_rdt");
  if (!btn_rdt.classList.contains("btn_rdt_disabled")) {
    XMLHttpRequest_get('/rdt');
  }
}
function doNothing() {
  return;
}
function ConfigFlasher() {
  // Récupère les données du flasher
  XMLHttpRequest_get("/getflashdata");
  
  // Affichage de la boite de dialogue
  document.getElementById("input_flasher").classList.remove("noshow");
  document.getElementById("dialog_mask").classList.remove("noshow");
}
function updateFlashData(xml) {
  // Mise à jour des éléments de config flash
  // en résultat de la requette XMLHttpRequest_get("/getflashdata");
  var flash_verrou = xml.getElementsByTagName("flash_verrou")[0].innerHTML;
  var btn_on_off = document.getElementById("btn_flash_verrou");
  if (flash_verrou == 0) { // off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    document.getElementById("flash_status_verrou").innerText = "Flash déverrouillage inactif";
  } else {
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    document.getElementById("flash_status_verrou").innerText = "Flash déverrouillage actif";
  }
  var flash_vol_on = xml.getElementsByTagName("flash_vol_on")[0].innerHTML;
  var btn_on_off = document.getElementById("btn_flash_vol");
  if (flash_vol_on == 0) { // off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    document.getElementById("flash_status_vol").innerText = "Flash vol inactif";
  } else {
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    document.getElementById("flash_status_vol").innerText = "Flash vol actif";
  }
  var tOn = xml.getElementsByTagName("tOn")[0].innerHTML;
  document.getElementById("allumeFlash").value = Number(tOn);
  document.getElementById('val_allume_flash').innerHTML = tOn + "&nbsp;ms";
  var tOff = xml.getElementsByTagName("tOff")[0].innerHTML;
  document.getElementById("eteintFlash").value = Number(tOff);
  document.getElementById('val_eteint_flash').innerHTML = tOff + "&nbsp;ms";
  var tCycle = xml.getElementsByTagName("tCycle")[0].innerHTML;
  document.getElementById("cycleFlash").value  =  Number(tCycle);
  document.getElementById('val_cycle_flash').innerHTML = tCycle + "&nbsp;s";
  var nFlash = xml.getElementsByTagName("nFlash")[0].innerHTML;
  document.getElementById("nbFlash").value     =  Number(nFlash);
  document.getElementById('val_nb_flash').innerHTML = nFlash;
}
function toogleFlashVol() {
  var btn_on_off = document.getElementById("btn_flash_vol");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    document.getElementById("flash_status_vol").innerText = "Flash vol inactif";
  } else {
    // Passage de off a on
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    document.getElementById("flash_status_vol").innerText = "Flash vol actif";
  }
}
function toogleFlashVerrou() {
  var btn_on_off = document.getElementById("btn_flash_verrou");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    document.getElementById("flash_status_verrou").innerText = "Flash déverrouillage inactif";
  } else {
    // Passage de off a on
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    document.getElementById("flash_status_verrou").innerText = "Flash déverrouillage actif";
  }
}
function updateFlasher() {
  // Arrête si test en cours
  stopFlashTest();
  // Envoie les nouvelles données à la minuterie
  XMLHttpRequest_post("/setflashdata");
  // Masque la boite de dialogue
  dialogHide();
}
function cancelFlasher() {
  // Arrête si test en cours
  stopFlashTest();
  // Réinitialise les données depuis la minuterie
  XMLHttpRequest_get("/resetflashdata");
  // Masque la boite de dialogue
  dialogHide();
}
function flashDefault() {
  // Le flash vol est actif
  var btn_on_off = document.getElementById("btn_flash_vol");
  btn_on_off.classList.remove("off");
  btn_on_off.classList.add("on");
  btn_on_off.src = "images/btnon.svg";
  document.getElementById("flash_status_vol").innerText = "Flash vol actif";
  // Le flash dévérouillage est actif
  btn_on_off = document.getElementById("btn_flash_verrou");
  btn_on_off.classList.remove("off");
  btn_on_off.classList.add("on");
  btn_on_off.src = "images/btnon.svg";
  document.getElementById("flash_status_verrou").innerText = "Flash déverrouillage actif";
  // Valeurs des inputs
  document.getElementById("cycleFlash").value  =  2;
  document.getElementById('val_cycle_flash').innerHTML = "2&nbsp;s";
  document.getElementById("allumeFlash").value = 35;
  document.getElementById('val_allume_flash').innerHTML = "35&nbsp;ms";
  document.getElementById("eteintFlash").value = 25;
  document.getElementById('val_eteint_flash').innerHTML = "25&nbsp;ms";
  document.getElementById("nbFlash").value     =  3;
  document.getElementById('val_nb_flash').innerHTML = 3;
  // Force test off & arret test si actif
  btn_on_off = document.getElementById("btn_test_vol");
  if (btn_on_off.classList.contains("on")) {
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    XMLHttpRequest_post("/testflasher");
  }
  btn_on_off = document.getElementById("btn_test_verrou");
  if (btn_on_off.classList.contains("on")) {
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
    XMLHttpRequest_post("/testflasher");
  }
}
function toggleTestVol() {
  var btn_on_off = document.getElementById("btn_test_vol");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
  } else {
    // Passage de off a on
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    // Si le test verrou est actif, on le désactive
    btn_on_off = document.getElementById("btn_test_verrou");
    if (btn_on_off.classList.contains("on")) {
      // Passage de on a off
      btn_on_off.classList.remove("on");
      btn_on_off.classList.add("off");
      btn_on_off.src = "images/btnoff.svg";
    }
  }
  XMLHttpRequest_post("/testflasher");
}
function toggleTestVerrou() {
  var btn_on_off = document.getElementById("btn_test_verrou");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";

  } else {
    // Passage de off a on
    btn_on_off.classList.remove("off");
    btn_on_off.classList.add("on");
    btn_on_off.src = "images/btnon.svg";
    // Si le test vol est actif, on le désactive
    btn_on_off = document.getElementById("btn_test_vol");
    if (btn_on_off.classList.contains("on")) {
      // Passage de on a off
      btn_on_off.classList.remove("on");
      btn_on_off.classList.add("off");
      btn_on_off.src = "images/btnoff.svg";
    }
  }
  XMLHttpRequest_post("/testflasher");
}
function stopFlashTest() {
  // Si le test vol est actif, on le désactive
  btn_on_off = document.getElementById("btn_test_vol");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
  }
  // Si le test verrou est actif, on le désactive
  btn_on_off = document.getElementById("btn_test_verrou");
  if (btn_on_off.classList.contains("on")) {
    // Passage de on a off
    btn_on_off.classList.remove("on");
    btn_on_off.classList.add("off");
    btn_on_off.src = "images/btnoff.svg";
  }
  XMLHttpRequest_post("/testflasher");
}
function updateFlashValue(slider) {
  // Update la valeur en fonction du slider concerné
  if (slider.id == "cycleFlash") {
    document.getElementById('val_cycle_flash').innerHTML = String(slider.value) + '&nbsp;s';
  } else if (slider.id == "allumeFlash") {
    document.getElementById('val_allume_flash').innerHTML = String(slider.value) + '&nbsp;ms';
  } else if (slider.id == "eteintFlash") {
    document.getElementById('val_eteint_flash').innerHTML = String(slider.value) + '&nbsp;ms';
  } else if (slider.id == "nbFlash") {
    document.getElementById('val_nb_flash').innerHTML = String(slider.value);
  }
  // Si un test est actif, on l'arrete et on le relance pour passer les nouvelles valeurs
  // et voir le résultat en temps réel.
  if (document.getElementById("btn_test_vol").classList.contains("on")) {
    document.getElementById("btn_test_vol").classList.remove("on")
    XMLHttpRequest_post("/testflasher");
    document.getElementById("btn_test_vol").classList.add("on")
    XMLHttpRequest_post("/testflasher");
  }
  if (document.getElementById("btn_test_verrou").classList.contains("on")) {
    document.getElementById("btn_test_verrou").classList.remove("on")
    XMLHttpRequest_post("/testflasher");
    document.getElementById("btn_test_verrou").classList.add("on")
    XMLHttpRequest_post("/testflasher");
  }
}







