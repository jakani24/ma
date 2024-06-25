<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Cyberhex login page</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>We are creating the databases used in cyberhex, please stand by</h4>
                </div>
                <div class="card-body">
					<p>If the creation fails, please wait a minute and try again. The database server might still be starting at the time.</p>
					<br>
                </div>
			<?php
			$success=1;
			include "../config.php";

			// Create connection
			$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD);

			// Check connection
			if ($conn->connect_error) {
				$success=0;
				die("Connection failed: " . $conn->connect_error);
			}

			// Create database
			$sql = "CREATE DATABASE IF NOT EXISTS $DB_DATABASE";
			if ($conn->query($sql) === TRUE) {
				echo '<br><div class="alert alert-success" role="alert">
					Database created successfully!
					</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating database: ' . $conn->error .'
				</div>';
			}

			$conn->close();

			// Connect to the new database
			$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

			// Check connection
			if ($conn->connect_error) {
				$success=0;
				die("Connection failed: " . $conn->connect_error);
			}

			// Create user table
			$sql = "CREATE TABLE IF NOT EXISTS users (
				id INT AUTO_INCREMENT PRIMARY KEY,
				username VARCHAR(255) NOT NULL,
				email VARCHAR(255) NOT NULL,
				perms VARCHAR(255),
				password VARCHAR(255),
				2fa VARCHAR(255),
				telegram_id VARCHAR(255),
				user_hex_id VARCHAR(255),
				credential_id VARBINARY(64),
				allow_pw_login INT,
				use_2fa INT,
				send_login_message INT,
				public_key TEXT,
				counter INT
			)";

			if ($conn->query($sql) === TRUE) {
				echo '<br><div class="alert alert-success" role="alert">
						Table users created successfully!
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating table users: ' . $conn->error .'
				</div>';
			}

			// Create log table
			$sql = "CREATE TABLE IF NOT EXISTS log (
				id INT AUTO_INCREMENT PRIMARY KEY,
				logtext VARCHAR(500) NOT NULL,
				loglevel VARCHAR(255) NOT NULL,
				machine_id VARCHAR(255),
				time VARCHAR(255)
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table log created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table log: ' . $conn->error .'
				</div>';
			}
			
			// Create incident table
			$sql = "CREATE TABLE IF NOT EXISTS incidents (
				id INT AUTO_INCREMENT PRIMARY KEY,
				status VARCHAR(50) NOT NULL,
				description VARCHAR(255) NOT NULL,
				opened VARCHAR(50),
				closed VARCHAR(50)
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table incidents created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table incidents: ' . $conn->error .'
				</div>';
			}
			
			// Create todo_items table
			$sql = "CREATE TABLE IF NOT EXISTS todo_items (
				id INT AUTO_INCREMENT PRIMARY KEY,
				done INT,
				done_by INT,
				belongs_to_list INT,
				text VARCHAR(500) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table todo_items created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table todo_items: ' . $conn->error .'
				</div>';
			}
			
			// Create todo lists table
			$sql = "CREATE TABLE IF NOT EXISTS todo_lists (
				id INT AUTO_INCREMENT PRIMARY KEY,
				belongs_to_incident INT,
				name VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table todo_items created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table todo_items: ' . $conn->error .'
				</div>';
			}
			
			// Create chat table
			$sql = "CREATE TABLE IF NOT EXISTS chats (
				id INT AUTO_INCREMENT PRIMARY KEY,
				belongs_to_incident INT,
				text TEXT NOT NULL,
				sent VARCHAR(50),
				from_userid INT
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table todo_items created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table todo_items: ' . $conn->error .'
				</div>';
			}
			
			
			// Create server log table
			$sql = "CREATE TABLE IF NOT EXISTS server_log (
				id INT AUTO_INCREMENT PRIMARY KEY,
				logtext VARCHAR(500) NOT NULL,
				loglevel VARCHAR(255) NOT NULL,
				userid INT,
				time VARCHAR(255)
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table log created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table log: ' . $conn->error .'
				</div>';
			}

			// Create settings table
			$sql = "CREATE TABLE IF NOT EXISTS settings (
				id INT AUTO_INCREMENT PRIMARY KEY,
				name VARCHAR(255) NOT NULL UNIQUE,
				value VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table settings created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table settings: ' . $conn->error .'
				</div>';
			}
			//user tasks table
			$sql = "CREATE TABLE IF NOT EXISTS user_tasks (
				id INT AUTO_INCREMENT PRIMARY KEY,
				task VARCHAR(255) NOT NULL UNIQUE
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table settings created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table settings: ' . $conn->error .'
				</div>';
			}
			//system tasks table
			$sql = "CREATE TABLE IF NOT EXISTS system_tasks (
				id INT AUTO_INCREMENT PRIMARY KEY,
				task VARCHAR(255) NOT NULL UNIQUE
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table settings created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table settings: ' . $conn->error .'
				</div>';
			}
			// Create rtp_included table
			$sql = "CREATE TABLE IF NOT EXISTS rtp_included (
				id INT AUTO_INCREMENT PRIMARY KEY,
				path VARCHAR(255) NOT NULL UNIQUE
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table rtp_included created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table rtp_included: ' . $conn->error .'
				</div>';
			}
			// Create rtp_excluded table
			$sql = "CREATE TABLE IF NOT EXISTS rtp_excluded (
				id INT AUTO_INCREMENT PRIMARY KEY,
				path VARCHAR(255) NOT NULL UNIQUE
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table rtp_excluded created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table rtp_excluded: ' . $conn->error .'
				</div>';
			}
			
			// Create dissalowed_start table
			$sql = "CREATE TABLE IF NOT EXISTS disallowed_start (
				id INT AUTO_INCREMENT PRIMARY KEY,
				path VARCHAR(255) NOT NULL UNIQUE
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table disalowed_start created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table disalowed_start: ' . $conn->error .'
				</div>';
			}

			// Create api table
			$sql = "CREATE TABLE IF NOT EXISTS api (
				id INT AUTO_INCREMENT PRIMARY KEY,
				apikey VARCHAR(500) NOT NULL,
				machine_id VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table api created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table api: ' . $conn->error .'
				</div>';
			}

			// Create secrets table
			$sql = "CREATE TABLE IF NOT EXISTS secrets (
				id INT AUTO_INCREMENT PRIMARY KEY,
				cert VARCHAR(500) NOT NULL,
				machine_id VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table secrets created successfully!
				</div>';
			} else {
				$success=0;
					echo '<br><div class="alert alert-danger" role="alert">
						Error creating table secrets: ' . $conn->error .'
				</div>';
			}

			// Create machine table
			$sql = "CREATE TABLE IF NOT EXISTS machines (
				id INT AUTO_INCREMENT PRIMARY KEY,
				machine_name VARCHAR(255) NOT NULL,
				machine_location VARCHAR(255) NOT NULL,
				machine_ip VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table machines created successfully!
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating table machines: ' . $conn->error .'
				</div>';
			}
			
			// Create vir_notify messages table
			$sql = "CREATE TABLE IF NOT EXISTS vir_notify (
				id INT AUTO_INCREMENT PRIMARY KEY,
				machine_id VARCHAR(255) NOT NULL,
				path VARCHAR(255) NOT NULL,
				hash VARCHAR(255) NOT NULL,
				action VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table machines created successfully!
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating table machines: ' . $conn->error .'
				</div>';
			}
			
			// Create sig_ex messages table
			$sql = "CREATE TABLE IF NOT EXISTS sig_ex (
				id INT AUTO_INCREMENT PRIMARY KEY,
				signature VARCHAR(255) NOT NULL,
				description VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table excluded signatures created successfully!
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating table excluded signatures: ' . $conn->error .'
				</div>';
			}
			
			// Create sig_in messages table
			$sql = "CREATE TABLE IF NOT EXISTS sig_in (
				id INT AUTO_INCREMENT PRIMARY KEY,
				signature VARCHAR(255) NOT NULL,
				description VARCHAR(255) NOT NULL
			)";

			if ($conn->query($sql) === TRUE) {
					echo '<br><div class="alert alert-success" role="alert">
						Table included signatures created successfully!
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating table included signatures: ' . $conn->error .'
				</div>';
			}
			
			// Attempt to create the directory where export files will be stored later on
			/*if (mkdir("/var/www/html/export", 0777, true)) {
				echo '<br><div class="alert alert-success" role="alert">
						Created export dir successfully.
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating export dir.
				</div>';
			}
			//Attempt to create the directory where import files will be stored later on
			if (mkdir("/var/www/html/import", 0777, true)) {
				echo '<br><div class="alert alert-success" role="alert">
						Created export dir successfully.
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating export dir.
				</div>';
			}
			
			//Attempt to create the directory where log backup files will be stored later on
			if (mkdir("/var/www/html/backup", 0777, true)) {
				echo '<br><div class="alert alert-success" role="alert">
						Created backup dir successfully.
				</div>';
			} else {
				$success=0;
				echo '<br><div class="alert alert-danger" role="alert">
						Error creating backup dir.
				</div>';
			}*/

			if($success!==1){
				echo '<br><div class="alert alert-danger" role="alert">
						There was an error creating the databases. Please try again or contact support at: <a href="mailto:info.jakach@gmail.com">info.jakach@gmail.com</a>
				</div>';
			}else{
				echo '<br><div class="alert alert-success" role="alert">
						Database created successfully! <a href="create_admin.php">Continue installation</a>
				</div>';
			}

			$conn->close();
			?>
			</div>
		</div>
    </div>
</div>
</body>
</html>
