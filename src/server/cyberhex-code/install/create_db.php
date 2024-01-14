
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cyberhex Installation</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            text-align: center;
            margin: 50px;
        }
        h1 {
            color: #333;
        }
    </style>
</head>
<body>
    <h1>We are creating the databases used in cyberhex, please stand by</h1>
</body>
</html>

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
    echo "Database created successfully<br>";
} else {
	$success=0;
    echo "Error creating database: " . $conn->error ."<br>";
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
	2fa VARCHAR(255)
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'users' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error ."<br>";
}

// Create log table
$sql = "CREATE TABLE IF NOT EXISTS log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    logtext VARCHAR(255) NOT NULL,
    loglevel VARCHAR(255) NOT NULL,
	machine_id VARCHAR(255)
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'log' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error  ."<br>";
}

// Create settings table
$sql = "CREATE TABLE IF NOT EXISTS settings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    value VARCHAR(255) NOT NULL
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'settings' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error  ."<br>";
}

// Create api table
$sql = "CREATE TABLE IF NOT EXISTS api (
    id INT AUTO_INCREMENT PRIMARY KEY,
    apikey VARCHAR(255) NOT NULL,
    machine_id VARCHAR(255) NOT NULL
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'api' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error  ."<br>";
}

// Create secrets table
$sql = "CREATE TABLE IF NOT EXISTS secrets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    cert VARCHAR(255) NOT NULL,
    machine_id VARCHAR(255) NOT NULL
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'secrets' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error  ."<br>";
}

// Create machine table
$sql = "CREATE TABLE IF NOT EXISTS machines (
    id INT AUTO_INCREMENT PRIMARY KEY,
    machine_name VARCHAR(255) NOT NULL,
    machine_location VARCHAR(255) NOT NULL,
	machine_ip VARCHAR(255) NOT NULL
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'machines' created successfully<br>";
} else {
	$success=0;
    echo "Error creating table: " . $conn->error  ."<br>";
}

if($success!==1){
	echo('<p style="font-size: 20px; color: red;">There was an error creating the databases. Please try again or contact support at: <a href="mailto:info.jakach@gmail.com">info.jakach@gmail.com</a></p>');
}else{
	echo('<p style="font-size: 20px; color: green;">Database created successfully! <a href="create_admin.php">Continue installation</a></p>');
}

$conn->close();
?>
