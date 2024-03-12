<?php
//send message without waiting for it
//exec('run_baby_run > /dev/null &');

function send($message){
	$message=urlencode($message);
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	//get apikey of telegram bot
	$sql = "SELECT * FROM settings WHERE name = 'telegram_bot'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$telegram_bot="";
	$result = $stmt->get_result();
	if ($result->num_rows > 0) {
		$row = $result->fetch_assoc();
		$telegram_bot = $row['value'];
		$stmt->close();
		//now send a message to every user which has a telegram id
		$sql = "SELECT telegram_id FROM users";
		$stmt = $conn->prepare($sql);
		// Execute the statement
		$stmt->execute();
		// Get the result
		$telegram_id="";
		$result = $stmt->get_result();

		while($row = $result->fetch_assoc()) {
			$telegram_id=$row["telegram_id"];
			echo("curl \"https://api.telegram.org/$telegram_bot/sendMessage?chat_id=$telegram_id&text=$message\" > /dev/null &");
		}
		$stmt -> close();
	}
}
if(isset($_GET["send"]))
	send($_GET["send"]);
?>