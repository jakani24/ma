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
				2fa VARCHAR(255)
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
				logtext VARCHAR(255) NOT NULL,
				loglevel VARCHAR(255) NOT NULL,
				machine_id VARCHAR(255)
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
				name VARCHAR(255) NOT NULL,
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

			// Create api table
			$sql = "CREATE TABLE IF NOT EXISTS api (
				id INT AUTO_INCREMENT PRIMARY KEY,
				apikey VARCHAR(255) NOT NULL,
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
				cert VARCHAR(255) NOT NULL,
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
