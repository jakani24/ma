<?php
//send message without waiting for it
//exec('run_baby_run > /dev/null &');

function send($message){
	//get apikey of telegram bot
	$sql = "SELECT value FROM settings WHERE name = 'telegram_bot'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$telegram_bot="";
	$result = $stmt->get_result();
	if ($result->num_rows > 0) {
		// Fetch the first row as an associative array
		$row = $result->fetch_assoc();
		// Extract the value of the 'value' column
		$telegram_bot = $row['value'];
		// Do something with the value, for example:
		echo "Telegram Bot API Key: " . $telegram_bot;
	}
}
?>



	/*
	//if able to, send a telegram notification to user
	$sql = "SELECT telegram_id FROM users";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$telegram_id="";
	$result = $stmt->get_result();

	while($row = $result->fetch_assoc()) {
		$telegram_id=$row["telegram_id"];
		//exec("curl \"https://api.telegram.org/$api/sendMessage?chat_id=$telegram_id&text=$text\"");
		$url="https://api.telegram.org/$";
		$ch = curl_init($url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		$fileContents = curl_exec($ch);
	}
	$stmt -> close();
	*/