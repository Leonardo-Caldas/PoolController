#include <avr/pgmspace.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Relay.h"
#include <TickTwo.h>

const bool DEB_VERBOSE = true;

piscina::Relay relays[3] = {piscina::Relay(5, "filtro"), piscina::Relay(4, "aquecedor"), piscina::Relay(14, "luz")};

const char* SSID = "controle-piscina";
const char* PSK = "piscina-caldas";
AsyncWebServer server(80);
AsyncEventSource events("/events");

struct botaoRotina {
  /* Information related with timer status, whether it's (timer, flitro or luz) on or off.
    state = Timer State
    filtro = Filtro State
    luz = Luz State
  */
  bool state = true;
  bool filtro = true;
  bool luz = true;
} b_rotina;

/*
struct eventTri {
  char name[16];
  bool state = false;
};
eventTri eventsTri[4] = {{"desliga", false}, {"liga", false}, {"desliga", false}, {"liga", false}};
*/

void changeStateFiltro()
{
  if (!relays[0].get_state())
  {
    events.send("desliga", "filtro");
    events.send("desliga", "rotina");
  }
  else
  {
    events.send("liga", "filtro");
    events.send("liga", "rotina");
  }
}

void changeStateLuz()
{
  relays[2].set_state(1);
  events.send("desliga", "luz");
}

uint32_t TEMPO_FILTRO = 1000 * 3600 * 2; // 2h = 7.2E6ms
TickTwo local_timer_1(changeStateFiltro, TEMPO_FILTRO, 1);
// TickTwo local_timer_2(changeStateLuz, TEMPO_LUZ, 1);

String parseState(const int state)
{
  /*
  Convert bool to str.
  */
  if (state)
  {
    return "";
  }
  else
  {
    return "checked";
  }
}

const char* button_header[3] = {"Filtro", "Aquecedor", "Luz"};

/* 
String home_processors (const String& var)
{
        String text = "";
        if (var == "template") {
            // e.g. id="botao=rotina&state=0"
            text += R"raw(<label class="switch"><input id="botao=)raw";
            text += "rotina";
            text += R"raw(" type="checkbox" onchange="changeRelayState(this)")raw";
            text += parseState(1); // FIXME This results in incorrect data on the webpage, 
            // If you turn on the system, and reload the webpage the system will marked as off.
            if(DEB_VERBOSE)
            {
              Serial.println("b_rotina.state = " + b_rotina.state);
              Serial.println("b_rotina.state = " + b_rotina.state);
              Serial.println("b_rotina.state = " + b_rotina.state);
            }
            text += R"raw(><span class="slider"></span></label></div>)raw";
            for (int i = 0; i < 3; i++)
            {
              text += R"raw(<div class="container"><h3>)raw";
              text += button_header[i];
              text += R"raw(</h3><label class="switch"><input id="botao=)raw";
              text += String(relays[i].get_desciption());
              text += R"raw(" type="checkbox" onchange="changeRelayState(this)")raw";
              text += parseState(1);
              if(DEB_VERBOSE)
              {
                Serial.println("Relay(" + relays[i].get_desciption() + ") State = " + relays[i].get_state());
              }
              text += R"raw(><span class="slider"></span></label></div>)raw";
            }
            if(DEB_VERBOSE)
            {
              Serial.println("BEGIN HOME SCRIPT:\n" + text + "\n END HOME SCRIPT\n");
            }
            return text;
        }
        return String();
} 
*/

String processors (const String& var)
{
  /*
  */
        String text = "";
        if (var == "template") {
            // e.g. id="botao=rotina&state=0"
            text += R"raw(<label class="switch"><input id="botao=)raw";
            text += "rotina";
            text += R"raw(" type="checkbox" onchange="changeRelayState(this)")raw";
            text += parseState(b_rotina.state);
            if(DEB_VERBOSE)
            {
              Serial.println("b_rotina.state = " + b_rotina.state);
              Serial.println("b_rotina.filtro = " + b_rotina.filtro);
              Serial.println("b_rotina.luz = " + b_rotina.luz);
            }
            text += R"raw(><span class="slider"></span></label></div>)raw";
            for (int i = 0; i < 3; i++)
            {
              text += R"raw(<div class="container"><h3>)raw";
              text += button_header[i];
              text += R"raw(</h3><label class="switch"><input id="botao=)raw";
              text += String(relays[i].get_desciption());
              text += R"raw(" type="checkbox" onchange="changeRelayState(this)")raw";
              text += parseState(relays[i].get_state());
              if(DEB_VERBOSE)
              {
                Serial.println("Relay(" + relays[i].get_desciption() + ") State = " + relays[i].get_state());
              }
              text += R"raw(><span class="slider"></span></label></div>)raw";
            }
            if(DEB_VERBOSE)
            {
              Serial.println("BEGIN UPDATE SCRIPT:\n" + text + "\n END UPDATE SCRIPT\n");
            }
            return text;
        }
        return String();
}

void onUpdate(AsyncWebServerRequest *request)
{
  /*
  */
  String name_botao = "";
  String state_botao = "";
  if (request->hasParam("botao") & request->hasParam("state"))
  {
    name_botao += request->getParam("botao")->value();
    state_botao += request->getParam("state")->value();
    if (name_botao == "rotina")
    {
      if (state_botao == "0")
      {
        b_rotina.state = true;
        // relays[0].set_state(0);
        // relays[2].set_state(0);
        events.send("liga", "filtro");
        relays[0].set_state();
        local_timer_1.start();
        // local_timer_2.start();
        // eventsTri[1].state = true;
        // events.send("liga", "luz");
        // todo falta atualizar o site conforme os timer acabam
        
      }
      else
      {
        // desliga o filtro e as luzes e reseta os timers
        b_rotina.state = false;
        relays[0].set_state();
        // relays[2].set_state(1);
        local_timer_1.stop();
        // local_timer_2.stop();
        events.send("desliga", "filtro");
        // events.send("desliga", "luz");
      }
    }

    if (name_botao == "filtro")
    {
      if (state_botao == "0")
      {
        relays[0].set_state(0);
      }
      else
      {
        relays[0].set_state(1);
      }
      if(DEB_VERBOSE)
        {
          int i = 0;
          Serial.println("Relay(" + relays[i].get_desciption() + ") State = " + relays[i].get_state());
        }
    }
    if (name_botao == "luz")
    {
      if (state_botao == "0")
      {
        relays[2].set_state(0);
      }
      else
      {
        relays[2].set_state(1);
      }
      if(DEB_VERBOSE)
        {
          int i = 2;
          Serial.println("Relay(" + relays[i].get_desciption() + ") State = " + relays[i].get_state());
        }
    }
    if (name_botao == "aquecedor")
    {
      if (state_botao == "0")
      {
        relays[1].set_state(0);
      }
      else
      {
        relays[1].set_state(1);
      }
      if(DEB_VERBOSE)
        {
          int i = 1;
          Serial.println("Relay(" + relays[i].get_desciption() + ") State = " + relays[i].get_state());
        }
    }
  }
  request->send(200, "text/plain", "OK");
  if (DEB_VERBOSE)
  {
    Serial.println("name_botao="+name_botao+" state_botao="+state_botao);
  }
}

const char index_html[] PROGMEM = R"raw(
    <!DOCTYPE html>
    <html>
        <body>
            <meta name="viewport" content="width=device-width,initial-scale=1"/>
            <style>
                html {font-family: Verdana; background-color: white;}
                
                h1 {color: black;
                    text-align: center;}

                h3 {color: black;
                    text-align: center;}
                
                h6 {color: black;
                text-align: center;}

                .container {
                    width: 100%;
                    padding: 10px;
                    border: 10px;
                    margin: 10px;
                    border-radius: 15px;
                    background-color: rgba(211, 211, 211, 0.318);
                    text-align: center;
                }
                /* The switch - the box around the slider */
                .switch {
                position: relative;
                display: inline-block;
                width: 60px;
                height: 34px;
                }

                /* Hide default HTML checkbox */
                .switch input {
                opacity: 0;
                width: 0;
                height: 0;
                }

                /* The slider */
                .slider {
                position: absolute;
                cursor: pointer;
                top: 0;
                left: 0;
                right: 0;
                bottom: 0;
                background-color: rgba(245, 5, 5, 0.322);
                -webkit-transition: .4s;
                transition: .4s;
                border-radius: 34px;
                }

                .slider:before {
                position: absolute;
                content: "";
                height: 26px;
                width: 26px;
                left: 4px;
                bottom: 4px;
                background-color: white;
                -webkit-transition: .4s;
                transition: .4s;
                border-radius: 50%;
                }

                input:checked + .slider {
                background-color: #2196F3;
                }

                input:focus + .slider {
                box-shadow: 0 0 1px #2196F3;
                }

                input:checked + .slider:before {
                -webkit-transform: translateX(26px);
                -ms-transform: translateX(26px);
                transform: translateX(26px);
                }

                .timer {
                    text-align: right;
                }

            </style>
            <h1>Controle da Piscina</h1>
            <div class="container">
                <h3>Timer</h3>
                <h6>Liga o filtro por 2 horas.</h6>
                %template%
            <script>
                    function changeRelayState(element)
                    {
                        var xhr = new XMLHttpRequest();
                        if (!!element.checked)
                        {
                            xhr.open("GET", "/update?"+element.id+"&state=0");
                            xhr.send();
                        }
                        else
                        {
                            xhr.open("GET", "/update?"+element.id+"&state=1");
                            xhr.send();
                        }
                    }

                    function changeButtonState(element)
                    {
                        if (!!element.checked)
                        {
                            element.removeAttribute("checked");
                        }
                        else
                        {
                            element.setAttribute("checked", "true");
                        }

                    }

                    var evtSource = new EventSource('/events');

                    evtSource.addEventListener('filtro', function(e)
                    {
                        var elementId = "botao=" + e.type;
                        elem = document.getElementById(elementId);
                        if (e.data[0] == 'd')
                        {
                            if (elem.checked)
                            {
                                changeButtonState(elem);
                            }
                        }
                        else if (e.data[0] == 'l')
                        {
                            if (elem.checked == false)
                            {
                                changeButtonState(elem);
                            }
                        }
                    }, false);
                    evtSource.addEventListener('rotina', function(e)
                    {
                        var elementId = "botao=" + e.type;
                        elem = document.getElementById(elementId);
                        if (e.data[0] == 'd')
                        {
                            if (elem.checked)
                            {
                                changeButtonState(elem);
                            }
                        }
                        else if (e.data[0] == 'l')
                        {
                            if (elem.checked == false)
                            {
                                changeButtonState(elem);
                            }
                        }
                    }, false);
                    // evtSource.addEventListener('aquecedor', processEvents(e) , false);
                    evtSource.addEventListener('luz', function(e)
                    {
                        var elementId = "botao=" + e.type;
                        elem = document.getElementById(elementId);
                        if (e.data[0] == 'd')
                        {
                            if (elem.checked)
                            {
                                changeButtonState(elem);
                            }
                        }
                        else if (e.data[0] == 'l')
                        {
                            if (elem.checked == false)
                            {
                                changeButtonState(elem);
                            }
                        }
                    }, false);
            </script>
        </body>
    </html>
    )raw";

void setup()
{
  Serial.begin(115200);
  while(!WiFi.softAP(SSID,PSK)) {
    Serial.print(".");
    delay(100);
  }
  Serial.print("\ncontrole-piscina Online\n");
  Serial.print(WiFi.softAPIP());
  Serial.println("");
  WiFi.printDiag(Serial);
  Serial.println("");

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", index_html, home_processors);});
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){request->send_P(200, "text/html", index_html, processors);});
  server.on("/update", HTTP_GET, onUpdate);
  events.onConnect([](AsyncEventSourceClient *client){
    client->send("hello!",NULL,millis(),1000);
  });
  //server.on("/upload", HTTP_GET, onUpload);
  server.addHandler(&events);
  server.begin();
}

void loop()
{
  local_timer_1.update();
  // if (lacal_timer)
  // {
  //   todo progress bar
  // }
}