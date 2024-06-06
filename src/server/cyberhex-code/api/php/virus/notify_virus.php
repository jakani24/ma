<?php
//we need to auth the user => apikey
include "../accessctrl/check_apikey.php";
if(check_apikey()!==true){
	die("no_auth");
}
include "../notifications/sendmessage.php";
//add the entry to the log db
//this page has no gui, it may return ok or error
if(!isset($_GET["machine_id"]) or !isset($_GET["hash"]) or !isset($_GET["file"]) or !isset($_GET["action"]))
	echo("syn_err");
else{
	$hash=htmlspecialchars($_GET["hash"]);
	$path=htmlspecialchars($_GET["file"]);
	$machine_id=htmlspecialchars($_GET["machine_id"]);
	$action=htmlspecialchars($_GET["action"]);
	//include db pw
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("conn_err");
	}
	$sql = "INSERT INTO vir_notify (hash,machine_id,path,action) VALUES (?,?,?,?);";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("ssss", $hash,$machine_id,$path,$action);
	// Execute the statement
	if(!$stmt->execute())
		echo("wrt_err");
	else
		echo("wrt_ok");
	
	$sql="SELECT machine_location FROM machines WHERE machine_name = ?";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("s", $machine_id);
	$stmt->execute();
    $result = $stmt->get_result();
    $row = $result->fetch_assoc();
    $location = $row["machine_location"];
	
	$stmt->close();
	$conn->close();
	//send the message to every user that has set his telegram key in the settings
	send("[VIRUS WARNING!]\nhash: $hash\npath: $path\nmachine_id: $machine_id\nmachine_location: $location\naction: $action\n");
}
?>