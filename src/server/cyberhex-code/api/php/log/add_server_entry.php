<?php

	function log_action($loglevel,$logtext,$userid){
		include "../../../config.php";
		$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
		if ($conn->connect_error) {
			die("conn_err");
		}
		
		$time=date("Y-m-d H:i:s");
		
		$sql="INSERT INTO server_log (loglevel,logtext,userid,time) VALUES (?,?,?,?);";
		$stmt = $conn->prepare($sql);
		$stmt->bind_param("ssss", $loglevel,$logtext,$userid,$time);
		// Execute the statement
		$stmt->execute();
		$stmt->close();
		$conn->close();
		
	}

?>