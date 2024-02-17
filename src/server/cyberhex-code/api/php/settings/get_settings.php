<?php
//we need to auth the user => apikey
//put auth code here afterwards
include "../accessctrl/check_apikey.php";
if(check_apikey()!==true){
	die("no_auth");
}

$setting_virus_ctrl_virus_found_action = "not configured yet";
$setting_server_server_url="not configured yet";
$setting_rtp_folder_scan_status=0;
$setting_rtp_process_scan_status=0;
include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
if(isset($_GET["settings"])){
	/*
		example settings:
		virus_ctrl:virus_found:action ignore
		server:server_url https://jakach.duckdns.org
		rtp_folder_scan:status on
	*/
	load_settings();
	echo(load_secret());
	echo("virus_ctrl:virus_found:action ".$setting_virus_ctrl_virus_found_action."\n");
	echo("server:server_url ".$setting_server_server_url."\n");
	echo("rtp_folder_scan:status ".$setting_rtp_folder_scan_status."\n");
	echo("rtp_process_scan:status ".$setting_rtp_process_scan_status."\n");
}
if(isset($_GET["rtp_included"])){
	echo(load_secret());
	//load all the entrys from a db table
	$sql = "SELECT path,id FROM rtp_included ORDER BY id";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	while ($row = $result->fetch_assoc()){
			echo("\"".$row["path"]."\"\n");
	}
	$stmt -> close();	
	
}
if(isset($_GET["rtp_excluded"])){
	echo(load_secret());
	//load all the entrys from a db table
	$sql = "SELECT path,id FROM rtp_excluded ORDER BY id";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	while ($row = $result->fetch_assoc()){
			echo("\"".$row["path"]."\"\n");
	}
	$stmt -> close();
	
}
if(isset($_GET["sched"])){
	echo(load_secret());
	//load all the entrys from a db table
	$sql = "SELECT task,id FROM user_tasks ORDER BY id";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	while ($row = $result->fetch_assoc()){
		$buf=explode(";",$row["task"]);
		$time=$buf[0];
		$action=$buf[1];
		$argument=$buf[2];
		$name=$buf[3];
		$name=str_replace(" ","_",$name);
		//e.g.: "43 * * * *" scanfolder "c:\" tsk1
		echo("\"$time\" $action \"$argument\" $name\n");
	}
	$stmt -> close();
	$sql = "SELECT task,id FROM system_tasks ORDER BY id";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	while ($row = $result->fetch_assoc()){
		$buf=explode(";",$row["task"]);
		$time=$buf[0];
		$action=$buf[1];
		$argument=$buf[2];
		$name=$buf[3];
		$name=str_replace(" ","_",$name);
		//e.g.: "43 * * * *" scanfolder "c:\" tsk1
		echo("\"$time\" $action \"$argument\" $name\n");
	}
	$stmt -> close();
	
}
function load_settings(){
	global $setting_virus_ctrl_virus_found_action ;
	global $setting_server_server_url;
	global $setting_rtp_folder_scan_status;
	global $setting_rtp_process_scan_status;
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	//get setting: setting_virus_ctrl_virus_found_action
	$sql = "SELECT * FROM settings WHERE name = 'setting_virus_ctrl_virus_found_action'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($result->num_rows > 0){
		$setting_virus_ctrl_virus_found_action=$row["value"];
	}
	$stmt -> close();
	
	//get setting: setting_rtp_folder_scan_status
	$sql = "SELECT * FROM settings WHERE name = 'setting_rtp_folder_scan_status'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		if($row["value"]=="true")
			$setting_rtp_folder_scan_status="on";
		else
			$setting_rtp_folder_scan_status="off";
	}
	$stmt -> close();
	
	//get setting: setting_rtp_process_scan_status
	$sql = "SELECT * FROM settings WHERE name = 'setting_rtp_process_scan_status'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		if($row["value"]=="true")
			$setting_rtp_process_scan_status="on";
		else
			$setting_rtp_process_scan_status="off";
	}
	$stmt -> close();
	
		
	//get setting: setting_server_server_url
	$sql = "SELECT * FROM settings WHERE name = 'setting_server_server_url'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		$setting_server_server_url=$row["value"];
	}
	$stmt -> close();
	$conn -> close();
}
?>