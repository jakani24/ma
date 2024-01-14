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
        form {
            display: inline-block;
            text-align: left;
        }
        label {
            display: block;
            margin-bottom: 10px;
        }
        input {
            width: 200px;
            padding: 8px;
            margin-bottom: 15px;
        }
        button {
            padding: 10px 20px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
		footer {
            text-align: center;
            position: fixed;
            bottom: 0;
            width: 100%;
        }
    </style>
</head>
<body>
    <h1>Please create an admin user:</h1>
	<p>The admin user is later used to create new users, add machines and do all administrative tasks in cyberhex.</p>
	<p>Please choose a strong password, because the admin user is one of the main attack vectors of cyberhex.</p>
    <form action="create_admin.php?create=true" method="post">
        <label for="username">Username:</label>
        <input type="text" id="username" name="username" required>

        <label for="email">email:</label>
        <input type="text" id="email" name="email" required>

        <label for="password">Password:</label>
        <input type="password" id="password" name="password" required>
		<br>
        <button type="submit">Create admin</button>
    </form>
<?php
	include "../config.php";
	if(isset($_GET["create"])){
		$email=htmlspecialchars($_POST["email"]);
		$username=htmlspecialchars($_POST["username"]);
		$password=htmlspecialchars($_POST["password"]);
		$permissions="1111111111";
		// Create connection
		$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

		// Check connection
		if ($conn->connect_error) {
			$success=0;
			die("Connection failed: " . $conn->connect_error);
		}
		$stmt = $conn->prepare("INSERT INTO users (email, username, password,perms) VALUES (?, ?, ?, ?)");
		$stmt->bind_param("ssss", $email, $username, $hash, $permissions);
		
		$email=htmlspecialchars($_POST["email"]);
		$username=htmlspecialchars($_POST["username"]);
		$password=htmlspecialchars($_POST["password"]);
		$permissions="1111111111";
		$hash=password_hash('user_password_here', PASSWORD_BCRYPT);
		
		$stmt->execute();
		$stmt->close();
		$conn->close();
		echo('<p style="font-size: 20px; color: green;">Admin created successfully! <a href="end.php">Continue installation</a></p>');
	}

?>
    <footer>
        <p>&copy; 2024 Cyberhex Antivirus by Jakach Software <a href="mailto:info.jakach@gmail.com">info.jakach@gmail.com</a></p>
    </footer>
</body>
</html>
