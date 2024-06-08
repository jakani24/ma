<?php

//this file does only house functions and does not have a ui

function create_log_backup($conn){ //the $conn is the linker variable to the db server
	//create a filename
	$filename="log_export_".date('Y-m-d H:i:s').".csv";
	
	$fp=fopen("/var/www/html/backup/$filename","w");
	//do all the logic here and write into file
	// Query log entries for the export file
	$sql = "SELECT * FROM machines,log WHERE machine_name=machine_id ORDER BY log.id DESC";
	$stmt = $conn->prepare($sql);

	$stmt->execute();
	$result = $stmt->get_result();

	fwrite ($fp,"Entry id;Loglevel;Logtext;Machine id;Machine Location;Time & date\n");
	//now add entrys
	while ($row = $result->fetch_assoc()) {
		fwrite($fp,$row["id"] . ';');
		fwrite($fp,$row["loglevel"] . ';');
		fwrite($fp,$row["logtext"] . ';');
		fwrite($fp,$row["machine_id"] . ';');
		fwrite($fp,$row["machine_location"] . ';');
		fwrite($fp,$row["time"] . ";\n");
	}
	fclose($fp);
	$stmt->close();
	$conn->close();
	
}

function create_dashboard_backup($conn){
	//create a filename
	$filename="dashboard_export_".date('Y-m-d H:i:s').".csv";
	
	$fp=fopen("/var/www/html/backup/$filename","w");
	//do all the logic here and write into file
	// Query log entries for the export file
	$sql = "SELECT * FROM machines,vir_notify WHERE machine_name=machine_id ORDER BY vir_notify.id DESC";
	$stmt = $conn->prepare($sql);

	$stmt->execute();
	$result = $stmt->get_result();

	fwrite ($fp,"Entry id;Machine id;Machine Location;Path;Hash;Action taken\n");
	//now add entrys
	while ($row = $result->fetch_assoc()) {
		fwrite($fp,$row["id"] . ';');
		fwrite($fp,$row["machine_id"] . ';');
		fwrite($fp,$row["machine_location"] . ';');
		fwrite($fp,$row["path"] . ';');
		fwrite($fp,$row["hash"] . ';');
		fwrite($fp,$row["action"] . ";\n");
	}
	
	fclose($fp);
	$stmt->close();
	$conn->close();	
	
}

?>