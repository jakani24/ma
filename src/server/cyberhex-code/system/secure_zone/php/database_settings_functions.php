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
function safe_settings($db){//load settings 
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$value=htmlspecialchars($_GET["value"]);
	$name=htmlspecialchars($_GET["update"]);
	$db=htmlspecialchars($db);
	//update signature
	if($_GET["update"]=="sig"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE $db set signature= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	//update signature description
	if($_GET["update"]=="sig_desc"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE $db set description= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	$conn->close();
	
}

function list_yara_files($dir) {
    $yar_files = [];

    // Open the directory
    if ($handle = opendir($dir)) {
        // Iterate over each entry in the directory
        while (false !== ($entry = readdir($handle))) {
            // Exclude current directory (.) and parent directory (..)
            if ($entry != "." && $entry != "..") {
                $path = $dir . '/' . $entry;
                // If the entry is a directory, call the function recursively
                if (is_dir($path)) {
                    $yar_files = array_merge($yara_files, list_yar_files($path));
                }
                // If the entry is a file and ends with .yar extension, add it to the array
                elseif (is_file($path) && pathinfo($path, PATHINFO_EXTENSION) === 'yar') {
                    $yar_files[] = $path;
                }
            }
        }
        // Close the directory handle
        closedir($handle);
    }

    return $yar_files;
}
?>