<?php
echo 'Arduino REST client<br>';

include("restclient.php");

$api = new RestClient();

$result = $api->get("http://192.168.0.3/stato.html");


$code = $result->info->http_code;

if($code == 200) {
       $xml = new SimpleXMLElement($result->response);
       echo "Loaded XML, root element: " . $xml;
   } else {
       echo "GET failed, error code: ".$code;
   }

?>



<html>
  <title> Remote control ARDUINO </title>
  <body>
    <center>
      <form action="test_controller.php" method="POST">
      <table border="1" width="50%">
      <tr>
	<th align="left">  Status Arduino  </th>
	<th>  <?php $code == 200 ? print "<font color=green> ONLINE </font>" : print "<font color=red> OFFLINE </font>" ?>  </th>
      </tr>

      <tr>
	<td> STATUS </td>
	<td align="center"> <?php $xml == 1 ? print "<font color=green> ON </font>" : print "<font color=red> OFF </font>" ?> </td>
      </tr>
      <tr>
	<td align="center"> <input type="submit" value="Turn ON" name="azione"> </td>
	<td align="center"> <input type="submit" value="Turn OFF" name="azione"> </td>
      </tr>


      </table>
      </form>
    </center>



  </body>
</html>
