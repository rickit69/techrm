<?php
echo 'Arduino REST client<br>';

include("restclient.php");

$api = new RestClient();

$result = $api->get("http://192.168.0.3/stato.html");


$code = $result->info->http_code;

if($code == 200) {
       $xml = new SimpleXMLElement($result->response);
       echo "Loaded XML...";
   } else {
       echo "GET failed, error code: ".$code;
   }

?>



<html>
  <title> Remote control ARDUINO </title>
  <body>
    <center>
      <form action="test_controller.php" method="POST">
      <table border="0" width="25%">
      <tr>
	<th align="left">  Status Arduino  </th>
	<th>  <?php $code == 200 ? print "<font color=green> ONLINE </font>" : print "<font color=red> OFFLINE </font>" ?>  </th>
      </tr>

      <tr>
	<td> TEMPERATURE: </td>
	<?php if ($code == 200) { ?>
		<td align="center"> <?php echo $xml ?> Celsius </td>
	<?php } else { ?>
		<td align="center">  Not available </td>
	<?php } ?>
      </tr>
      <tr>
	<td align="center" colspan="2"> <input type="submit" value="Read temperature" name="azione"> </td>
      </tr>


      </table>
      </form>
    </center>



  </body>
</html>
