<?php

include("restclient.php");

if ($_POST) {
  $azione = $_POST["azione"];
  if ($azione == "Turn ON") {  
    $api = new RestClient();
    $result = $api->get("http://192.168.0.3/on.html");
  } else if ($azione == "Turn OFF") {
    $api = new RestClient();
    $result = $api->get("http://192.168.0.3/off.html");
  }

header('Location: /test_arduino/test.php');

}


?>
