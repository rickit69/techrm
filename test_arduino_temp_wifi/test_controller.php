<?php

include("restclient.php");

if ($_POST) {
  $azione = $_POST["azione"];
  if ($azione == "Read temperature") {  
    $api = new RestClient();
    $result = $api->get("http://192.168.0.3/rt.html");
  } 

header('Location: /test_arduino_temp_wifi/test.php');

}


?>
