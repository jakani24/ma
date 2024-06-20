
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Change Password</title>
</head>
<body>
<?php
include "../api/php/log/add_server_entry.php"; //to log things
?>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>Add an admin user</h4>
                </div>
                <div class="card-body">
				<p>Please create an initial admin user. This user the can create new users etc.<br>Please use a strong password for this user!</p>
					<form action="create_admin.php?create=true" method="post">
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
						<br>
                        <button type="submit" class="btn btn-primary btn-block">Create user</button>
                    </form>
					<?php
						include "../config.php";
						if(isset($_GET["create"])){
							$email=htmlspecialchars($_POST["email"]);
							$username=htmlspecialchars($_POST["username"]);
							$password=htmlspecialchars($_POST["password"]);
							$permissions="11111111111";
							// Create connection
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

							// Check connection
							if ($conn->connect_error) {
								$success=0;
								die("Connection failed: " . $conn->connect_error);
							}
							
							//create the unauth user
							$stmt = $conn->prepare("INSERT INTO users (email, username, password,perms,allow_pw_login,send_login_message,use_2fa) VALUES ('unauth@cyberhex.local', 'not_authenticated_user', 'this_user_does_not_have_a_password', '000000000000000',0,0,0)");
							$stmt->execute();
							$stmt->close();
							
							
							$stmt = $conn->prepare("INSERT INTO users (email, username, password,perms,allow_pw_login,send_login_message,use_2fa) VALUES (?, ?, ?, ?,1,0,0)");
							$stmt->bind_param("ssss", $email, $username, $hash, $permissions);
							
							$email=htmlspecialchars($_POST["email"]);
							$username=htmlspecialchars($_POST["username"]);
							$password=$_POST["password"];
							$permissions="11111111111";
							$hash=password_hash($password, PASSWORD_BCRYPT);
							
							$stmt->execute();
							$stmt->close();
							$conn->close();
							echo '<br><div class="alert alert-success" role="alert">
								Admin user created successfully! <a href="add_passkey.php?username='.$username.'">Continue installation</a>
							</div>';
						}

					?>
				</div>
			</div>
		</div>
	</div>
</div>


