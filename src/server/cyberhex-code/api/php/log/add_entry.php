<?php
//we need to auth the user => apikey
//put auth code here afterwards
include "../accessctrl/check_apikey.php";
if(check_apikey()!==true){
	die("no_auth");
}

//add the entry to the log db
//this page has no gui, it may return ok or error
if(!isset($_GET["loglevel"]) or !isset($_GET["logtext"]) or !isset($_GET["machine_id"]) or !isset($_GET["time"]))
	echo("syn_err");
else{
	$loglevel=htmlspecialchars($_GET["loglevel"]);
	$logtext=htmlspecialchars($_GET["logtext"]);
	$machine_id=htmlspecialchars($_GET["machine_id"]);
	$time=htmlspecialchars($_GET["time"]);
	//include db pw
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("conn_err");
	}
	$sql = "INSERT INTO log (loglevel,machine_id,logtext,time) VALUES (?,?,?,?);";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("ssss", $loglevel,$machine_id,$logtext,$time);
	// Execute the statement
	if(!$stmt->execute())
		echo("wrt_err");
	else
		echo("wrt_ok");
	$stmt->close();
	$conn->close();
}
?>