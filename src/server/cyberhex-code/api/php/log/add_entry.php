<?php
//we need to auth the user => apikey
//put auth code here afterwards

//add the entry to the log db
//this page has no gui, it may return ok or error
if(!isset($_GET["loglevel"]) or !isset($_GET["logtext"]) or !isset($_GET["machine_id"]))
	echo("syn_err");
else{
	$loglevel=htmlspecialchars($_GET["loglevel"]);
	$logtext=htmlspecialchars($_GET["logtext"]);
	$machine_id=htmlspecialchars($_GET["machine_id"]);
	//include db pw
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("conn_err");
	}
	$sql = "INSERT INTO log (loglevel,machine_id,logtext) VALUES (?,?,?);";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("sss", $loglevel,$machine_id,$logtext);
	// Execute the statement
	if(!$stmt->execute())
		echo("wrt_err");
	else
		echo("wrt_ok");
	$stmt->close();
	$conn->close();
}
?>