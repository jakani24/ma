<?php
include "../accessctrl/check_apikey.php";
if(check_apikey()!==true){
	die("no_auth");
}

// Check if a file was uploaded
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_FILES["file"])) {
   //echo($_FILES["file"]["tmp_name"]);
   //include db pw
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("conn_err");
	}
   $fp=fopen($_FILES["file"]["tmp_name"],"r");
   while(!feof($fp)){
		$log_entry=fgets($fp);
		$log_entry=str_replace("\n","",$log_entry);
		$log=explode(";",$log_entry);
		$message=$log[2];
		$loglevel=$log[1];
		$machine_id=htmlspecialchars($_GET["machine_id"]);
		$time=$log[0];

		$sql = "INSERT INTO log (loglevel,machine_id,logtext,time) VALUES (?,?,?,?);";
		$stmt = $conn->prepare($sql);
		$stmt->bind_param("ssss", $loglevel,$machine_id,$message,$time);
		// Execute the statement
		if(!$stmt->execute())
			echo("wrt_err");
		else
			echo("wrt_ok");
		$stmt->close();
   }
   $conn->close();
}
?>
