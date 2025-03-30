#pragma once
#include <Arduino.h>

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" href="data:,">
    <style>
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        h1 {
            background-color: #338;
            color: white;
            padding: 20px 0px;
        }
        .button {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 2px 20px;
            text-decoration: none;
            font-size: 24px;
            margin: 2px;
            cursor: pointer;
        }
        .button-label {
            font-size: 32px;
            font-weight: bold;
        }
        .button2 {
            background-color: #555555;
        }
        .state {
            color: #338;
            font-weight: bold;
            font-size: 48px;
        }
        .substate {
            color: #55B;
            font-weight: normal;
            font-size: 24px;
        }
    </style>
</head>

<body>
    <h1>Hottub Monitor</h1>
    <p>Current Temperature:</p>
    <p id="temp" class="state">%TEMP%<br /></p>
</body>
<script>
    function refresh() {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function () {
            if (xhr.readyState == XMLHttpRequest.DONE) {
                const temp = xhr.responseText;
                document.getElementById("temp").innerHTML = temp;
            }
        }
        xhr.open("GET", "/temp", true);
        xhr.send(null);
    }
    setInterval(() => refresh(), 1000);

</script>

</html>
)=====";
