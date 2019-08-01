#include <Arduino.h>

const char pageWifiSetupOk[] PROGMEM = R"=====(

<!DOCTYPE html>

<html>
  <head>
    <title>WiFi Setup</title>

    <style>
      body {
        margin: 0;
        padding: 30px;
        background-color: white;
        color: #505050;
        font-family: "Trebuchet MS", "Lucida Sans Unicode", "Lucida Grande",
          "Lucida Sans", Arial, sans-serif;
      }

      body > h1 {
        margin-top: 10px;
      }

      .infoSpan {
        font-size: 1.5em;
        margin-top: 20px;
        display: inline-block;
        margin: 20px;
      }

      .infoSpanContainer {
        background-color: #e5ffdd;
        display: block;
        margin-top: 10px;
        min-height: 50px;
        border-radius: 15px;
      }
    </style>
  </head>

  <body>
    <h1 style="font-size: 3em">WiFi Setup</h1>
    <hr />
    <div class="infoSpanContainer">
      <span class="infoSpan">Settings sent successful, restarting ESP.</span>
    </div>
  </body>
</html>


)=====";