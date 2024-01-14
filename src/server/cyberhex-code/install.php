<?php

$servername = "cyberhex-db";
$username = "root";
$password = "1234";
$database = "cyberhex_db";

// Create connection
$conn = new mysqli($servername, $username, $password);

// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Create database
$sql = "CREATE DATABASE IF NOT EXISTS $database";
if ($conn->query($sql) === TRUE) {
    echo "Database created successfully\n";
} else {
    echo "Error creating database: " . $conn->error;
}

$conn->close();

// Connect to the new database
$conn = new mysqli($servername, $username, $password, $database);

// Check connection
if ($conn->connect_error) {
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
    echo "Table 'users' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

// Create log table
$sql = "CREATE TABLE IF NOT EXISTS log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    logtext VARCHAR(255) NOT NULL,
    loglevel VARCHAR(255) NOT NULL,
	machine_id VARCHAR(255),
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'log' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

// Create settings table
$sql = "CREATE TABLE IF NOT EXISTS settings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    value VARCHAR(255) NOT NULL,
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'settings' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

// Create api table
$sql = "CREATE TABLE IF NOT EXISTS api (
    id INT AUTO_INCREMENT PRIMARY KEY,
    apikey VARCHAR(255) NOT NULL,
    machine_id VARCHAR(255) NOT NULL,
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'api' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

// Create secrets table
$sql = "CREATE TABLE IF NOT EXISTS secrets (
    id INT AUTO_INCREMENT PRIMARY KEY,
    cert VARCHAR(255) NOT NULL,
    machine_id VARCHAR(255) NOT NULL,
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'secrets' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

// Create machine table
$sql = "CREATE TABLE IF NOT EXISTS machines (
    id INT AUTO_INCREMENT PRIMARY KEY,
    machine_name VARCHAR(255) NOT NULL,
    machine_location VARCHAR(255) NOT NULL,
	machine_ip VARCHAR(255) NOT NULL,
)";

if ($conn->query($sql) === TRUE) {
    echo "Table 'machines' created successfully\n";
} else {
    echo "Error creating table: " . $conn->error;
}

$conn->close();
?>
