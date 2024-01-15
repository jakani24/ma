<?php
session_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Change Password</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>Change Password (<?php echo($username); ?>)</h4>
                </div>
                <div class="card-body">
                        <div class="form-group">
                            <label for="password">Password:</label>
                            <input type="password" class="form-control" id="password" name="password" required>
                        </div>
						<div class="form-group">
                            <label for="password">New Password:</label>
                            <input type="password" class="form-control" id="password1" name="password1" required>
                        </div>
						<div class="form-group">
                            <label for="password">Repeat New Password:</label>
                            <input type="password" class="form-control" id="password2" name="password2" required>
                        </div>
						<br>
                        <button type="submit" class="btn btn-primary btn-block">Update</button>
                    </form>
					<br>
					<!-- php code to verify password-->
					<?php
						// Check if the form is submitted
						if ($_SERVER["REQUEST_METHOD"] == "POST") {
							//include db pw
							include "../../../config.php";
							
							// Retrieve user input
							$password = $_POST["password"];
							$new_password1=$_POST["new_password1"];
							$new_password2=$_POST["new_password2"];

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
							$stmt->close();
							$conn->close();
							
							
							// Check if the user exists and verify the password
							if($new_password1===$new_password2){
								if ($result->num_rows > 0) {
									$row = $result->fetch_assoc();
									if (password_verify($password, $row['password'])) {
										//password correct update
										// Create connection
										$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

										// Check connection
										if ($conn->connect_error) {
											$success=0;
											die("Connection failed: " . $conn->connect_error);
										}
										$stmt = $conn->prepare("UPDATE users set password = ? where username = ?");
										$stmt->bind_param("ss", $new_password1, $username);
										$stmt->execute();
										$stmt->close();
										$conn->close();
										echo '<br><div class="alert alert-success" role="alert">
											Information updated successfully!
										  </div>';
										
									} else {
										echo '<div class="alert alert-danger" role="alert">
												Incorrect password.
											  </div>';
									}
								} else {
									echo '<div class="alert alert-danger" role="alert">
											Incorrect password.
										  </div>';
								}
							}else{
								echo '<div class="alert alert-danger" role="alert">
											New password does not match.
										  </div>';
							}

							// Close the connection
						}
					?>
					
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
