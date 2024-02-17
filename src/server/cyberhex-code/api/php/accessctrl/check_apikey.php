<?php
function check_apikey(){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
		if ($conn->connect_error) {
			$success=0;
			die("Connection failed: " . $conn->connect_error);
		}
	if(!isset($_GET["apikey"]) or !isset($_GET["machine_id"])){
		return false;
	}
	else{
		$apikey=$_GET["apikey"];
		$machineid=$_GET["machine_id"];
		$sql = "SELECT * FROM api WHERE apikey = ? and machine_id = ?";
		$stmt = $conn->prepare($sql);
		$stmt->bind_param("ss", $apikey,$machineid);
		
		// Execute the statement
		$stmt->execute();

		// Get the result
		$result = $stmt->get_result();
		
		// Check if the user exists and verify the password
		if ($result->num_rows > 0) {
			return true;
			//apikey authenticated
		}
	}
	return false;
}
function load_secret(){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
		if ($conn->connect_error) {
			$success=0;
			die("Connection failed: " . $conn->connect_error);
		}
	if(!isset($_GET["apikey"]) or !isset($_GET["machine_id"])){
		return "nan";
	}
	else{
		$apikey=$_GET["apikey"];
		$machineid=$_GET["machine_id"];
		$sql = "SELECT * FROM secrets machine_id = ?";
		$stmt = $conn->prepare($sql);
		$stmt->bind_param("s",$machineid);
		
		// Execute the statement
		$stmt->execute();

		// Get the result
		$result = $stmt->get_result();
		
		$row = $result->fetch_assoc();
		if($row!==null){
			return $row["cert"];
		}
	}
	return "nan";
}
?>