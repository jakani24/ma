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
                    <h4>Login to Cyberhex</h4>
                </div>
                <div class="card-body">
                    <form action="login.php" method="post">
                        <div class="form-group">
                            <label for="username">Username:</label>
                            <input type="text" class="form-control" id="username" name="username" required>
                        </div>
                        <div class="form-group">
                            <label for="password">Password:</label>
                            <input type="password" class="form-control" id="password" name="password" required>
                        </div>
                        <button type="submit" class="btn btn-primary btn-block">Login</button>
                    </form>
					<br>
					<!-- php code to verify password-->
					<?php
						// Check if the form is submitted
						if ($_SERVER["REQUEST_METHOD"] == "POST") {
							//include db pw
							include "../../../config.php";
							
							// Retrieve user input
							$username = htmlspecialchars($_POST["username"]);
							$password = $_POST["password"];


							// Create a connection
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

							// Check the connection
							if ($conn->connect_error) {
								die("Connection failed: " . $conn->connect_error);
							}
							$sql = "SELECT * FROM users WHERE username = ?";
							$stmt = $conn->prepare($sql);
							$stmt->bind_param("s", $username);
							
							// Execute the statement
							$stmt->execute();

							// Get the result
							$result = $stmt->get_result();
							
							// Check if the user exists and verify the password
							if ($result->num_rows > 0) {
								$row = $result->fetch_assoc();
								if (password_verify($password, $row['password'])) {
									echo '<div class="alert alert-success" role="alert">
											Login successful!
										  </div>';
								} else {
									echo '<div class="alert alert-danger" role="alert">
											Incorrect username or password.
										  </div>';
								}
							} else {
								echo '<div class="alert alert-danger" role="alert">
										Incorrect username or password.
									  </div>';
							}

							// Close the connection
							$stmt->close();
							$conn->close();
						}
					?>
					
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
