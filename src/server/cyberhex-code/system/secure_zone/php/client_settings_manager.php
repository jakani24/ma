<?php
session_start();

// Check if the user is logged in
if (!isset($_SESSION['username']) or !isset($_SESSION["login"])) {
    // Redirect to the login page or handle unauthorized access
    header("Location: /login.php");
    exit();
}

$username = $_SESSION['username'];
$perms = $_SESSION["perms"];
$email = $_SESSION["email"];
if($perms[5]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
$setting_virus_ctrl_virus_found_action = "not configured yet";
$setting_server_server_url="not configured yet";
$setting_rtp_folder_scan_status=0;
include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
if(isset($_GET["update"])){
	safe_settings();
}
if(isset($_GET["delete"])){
	delete_item($_GET["db"],$_GET["delete"]);
}
if(isset($_GET["add"])){
	add_item($_GET["add"],$_GET["value"],$_GET["field"]);
}
load_settings();
function delete_item($db,$id){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$db=htmlspecialchars($db);
	$id=htmlspecialchars($id);
	$stmt = $conn->prepare("delete from $db where id=$id;");
	$stmt->execute();
	$stmt->close();
	$conn -> close();
}
function add_item($db,$value,$field){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$db=htmlspecialchars($db);
	$field=htmlspecialchars($field);
	$stmt = $conn->prepare("INSERT INTO $db ($field) VALUES(?);");
	$stmt->bind_param("s",$value);
	$stmt->execute();
	$stmt->close();
	$conn -> close();
}
function safe_settings(){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$value=htmlspecialchars($_GET["value"]);
	$name=htmlspecialchars($_GET["update"]);
	//update what should be done if a virus is found
	if($_GET["update"]=="setting_virus_ctrl_virus_found_action"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		//$stmt = $conn->prepare("UPDATE settings set value=? WHERE name='virus_ctrl:virus_found:action';");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}

	if($_GET["update"]=="setting_server_server_url"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="setting_rtp_folder_scan_status"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="rtp_included"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE rtp_included set path= ? WHERE id=?");
		$stmt->bind_param("si",$value,$id);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="rtp_excluded"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE rtp_excluded set path= ? WHERE id=?");
		$stmt->bind_param("si",$value,$id);
		$stmt->execute();
		$stmt->close();
	}
	$conn->close();
	
}
function load_settings(){
	global $setting_virus_ctrl_virus_found_action ;
	global $setting_server_server_url;
	global $setting_rtp_folder_scan_status;
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
		$setting_rtp_folder_scan_status=$row["value"];
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