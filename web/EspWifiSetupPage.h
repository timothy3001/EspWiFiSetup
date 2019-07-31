#include <Arduino.h>

const char pageWifiSetup[] PROGMEM = R"=====(

<!DOCTYPE html>

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1" />

  <title>WiFi Setup</title>

  <style>
    body {
      margin: 0;
      padding: 30px;
      background-color: white;
      color: #535353;
      font-family: "Trebuchet MS", "Lucida Sans Unicode", "Lucida Grande",
        "Lucida Sans", Arial, sans-serif;
    }

    body > h1 {
      margin-top: 10px;
    }

    .textInput {
      border-radius: 5px;
      border-style: solid;
      border-width: 1px;
      border-color: darkgray;
      height: 25px;
      max-width: 300px;
      min-width: 150px;
      width: 100%;
    }

    .inputLabel {
      width: 120px;
      font-size: 1.3em;
      display: block;
      margin-bottom: 10px;
    }

    .inputRow {
      width: 100%;
      padding-left: 10px;
      margin-top: 30px;
    }

    .submitButton {
      height: 40px;
      border-radius: 5px;
      border-width: 5px;
      border-style: hidden;
      width: 80px;
      font-size: 1.1em;
    }

    @media (min-width: 576px) {
      .inputLabel {
        display: inline-block;
      }

      .inputLabel {
        margin-bottom: 0;
      }
    }
  </style>

  <script>
    var submitButton;
    var ssidInput;
    var passwordInput;

    window.onload = () => {
      submitButton = document.getElementById("submitButton");
      ssidInput = document.getElementById("ssidInput");
      passwordInput = document.getElementById("passwordInput");

      submitButton.addEventListener("click", () => {
        ssid = ssidInput.value;
        password = passwordInput.value;

        if (!ssid || ssid.length == 0) {
          window.alert("You need to specify a valid SSID!");
        } else {
          var data =
            "ssid=" + encodeURI(ssid) + "&password=" + encodeURI(password);

          xhr = new XMLHttpRequest();

          xhr.open("POST", "/config", false);
          xhr.setRequestHeader(
            "Content-Type",
            "application/x-www-form-urlencoded"
          );

          xhr.onreadystatechange = function() {
            if (xhr.readyState == 4 && xhr.status == 200) {
              window.location = "/ok";
            } else {
              window.location = "/error";
            }
          };

          xhr.send(data);
        }
      });
    };
  </script>
</head>

<body>
  <h1 style="font-size: 3em">WiFi Setup</h1>
  <hr />
  <div class="inputRow">
    <span class="inputLabel">SSID </span>
    <input type="text" class="textInput" id="ssidInput" />
  </div>
  <div class="inputRow">
    <span class="inputLabel">Password </span>
    <input type="text" class="textInput" id="passwordInput" />
  </div>
  <div class="inputRow">
    <button class="submitButton" id="submitButton">Submit</button>
  </div>
</body>


)=====";