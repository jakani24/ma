<?php
session_start();

// Check if the user is logged in
if (!isset($_SESSION['username']) or !isset($_SESSION["login"])) {
    // Redirect to the login page or handle unauthorized access
    header("Location: /login.php");
    exit();
}

$username = $_SESSION['username'];
$perms = $_SESSION["perms"];
$email = $_SESSION["email"];
if($perms[0]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
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
                    <h4>Add a user</h4>
                </div>
                <div class="card-body">
					<form action="add_user.php?add=true" method="post">
                        <div class="form-group">
                            <label for="username">Username:</label>
                            <input type="text" class="form-control" id="username" name="username" required>
                        </div>
						<div class="form-group">
                            <label for="email">Email:</label>
                            <input type="email" class="form-control" id="email" name="email" required>
                        </div>
						<div class="form-group">
                            <label for="password">Password:</label>
                            <input type="password" class="form-control" id="password" name="password" required>
                        </div>
						<label for="perms_table">User permissions:</label>
						<table class="table" id="perms_table" name="perms_table">
						  <thead>
							<tr>
							  <th>#</th>
							  <th>Description</th>
							  <th>Allow/Deny</th>
							</tr>
						  </thead>
						  <tbody>
							<tr>
							  <th scope="row">1</th>
							  <td>Add user <a data-bs-target="#warning" data-bs-toggle="modal" href="#warning">(Warning!)</a></td>
							  <td><input type="checkbox" name="add_user"></td>
							</tr>
							<tr>
							  <th scope="row">2</th>
							  <td>Delete/list user</td>
							  <td><input type="checkbox" name="delete_user"></td>
							</tr>
							<tr>
							  <th scope="row">3</th>
							  <td>View log</td>
							  <td><input type="checkbox" name="view_log"></td>
							</tr>
							<tr>
							  <th scope="row">4</th>
							  <td>Delete log</td>
							  <td><input type="checkbox" name="delete_log"></td>
							</tr>
							<tr>
							  <th scope="row">5</th>
							  <td>Server Settings</td>
							  <td><input type="checkbox" name="server_settings"></td>
							</tr>
							<tr>
							  <th scope="row">6</th>
							  <td>Client settings</td>
							  <td><input type="checkbox" name="client_settings"></td>
							</tr>
							<tr>
							  <th scope="row">7</th>
							  <td>Database settings</td>
							  <td><input type="checkbox" name="database_settings"></td>
							</tr>
							<tr>
							  <th scope="row">8</th>
							  <td>Add clients</td>
							  <td><input type="checkbox" name="add_clients"></td>
							</tr>
							<tr>
							  <th scope="row">8</th>
							  <td>Delete/list clients</td>
							  <td><input type="checkbox" name="delete_clients"></td>
							</tr>
						  </tbody>
						</table>

                        <button type="submit" class="btn btn-primary btn-block">Add user</button>
                    </form>
					<br>
					<!-- php code to verify password-->
					<?php
						// Check if the form is submitted
						if ($_SERVER["REQUEST_METHOD"] == "POST" and $block=0) {
							//include db pw
							include "../../../config.php";
							
							// Retrieve user input
							$password = $_POST["password"];
							$new_password1=$_POST["new_password1"];
							$new_password2=$_POST["new_password2"];
							$hash=password_hash($new_password1, PASSWORD_BCRYPT);
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
										$stmt->bind_param("ss", $hash, $username);
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
						}elseif($block==1){
							echo '<div class="alert alert-danger" role="alert">
												You do not have permission to add a user!
											  </div>';
						}
					?>
					
                </div>
            </div>
        </div>
		<div class="modal fade" id="perms_help" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="false">
		  <div class="modal-dialog">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="exampleModalLabel">Permission Explanation</h5>
				<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
			  </div>
			  <div class="modal-body">
				1 Add User<br>
				1 User List (delete...)<br>
				1 View Log<br>
				1 Delete Log<br>
				1 Server Settings<br>
				1 Client Settings<br>
				1 Database Settings<br>
				1 Add Cleints (+ add client)<br>
				1 Client List (manage)<br>
				1 Reserved for later use
			  </div>
			</div>
		  </div>
		</div>
    </div>
</div>
</body>
</html>
