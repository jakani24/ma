<?php
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
function add_item($db,$value1,$field1,$value2,$field2){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$db=htmlspecialchars($db);
	$field1=htmlspecialchars($field1);
	$field2=htmlspecialchars($field2);
	$stmt = $conn->prepare("INSERT INTO $db ($field1,$field2) VALUES(?,?);");
	$stmt->bind_param("ss",$value1,$value2);
	$stmt->execute();
	$stmt->close();
	$conn -> close();
}
function safe_settings(){//load settings 
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}

	//update excluded signature
	if($_GET["update"]=="sig_ex"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE sig_ex set signature= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	//update excluded signature description
	if($_GET["update"]=="sig_ex_desc"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE sig_ex set description= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	$conn->close();
	
}
?>