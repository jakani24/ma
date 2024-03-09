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
//for the get_perms_str() function
include "perms_functions.php";
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
<?php
	//load users attributes
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

	// Check the connection
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	$sql="SELECT * FROM users WHERE id=?";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("i", htmlspecialchars($_GET["userid"]));
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$stmt->close();
	$m_username=$row["username"];
	$m_email=$row["email"];
	$m_permissions=$row["perms"];
?>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>Add a user</h4>
                </div>
                <div class="card-body">
					<form action="manage_user.php?update=true" method="post">
                        <div class="form-group">
							<label for="username">Username:</label>
							<input type="text" class="form-control" id="username" name="username" value="<?php echo($m_username); ?>" required>
						</div>
						<div class="form-group">
							<label for="email">Email:</label>
							<input type="email" class="form-control" id="email" name="email" value="<?php echo($m_email); ?>" required>
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
								<?php
								if($m_permissions[0]=="1")
									echo('<td><input type="checkbox" name="add_user" checked></td>');
								else
									echo('<td><input type="checkbox" name="add_user"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">2</th>
							  <td>Delete/list/manage user <a data-bs-target="#warning" data-bs-toggle="modal" href="#warning2">(Warning!)</a></td>
								<?php
								if($m_permissions[1]=="1")
									echo('<td><input type="checkbox" name="delete_user" checked></td>');
								else
									echo('<td><input type="checkbox" name="delete_user"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">3</th>
							  <td>View log</td>
								<?php
								if($m_permissions[2]=="1")
									echo('<td><input type="checkbox" name="view_log" checked></td>');
								else
									echo('<td><input type="checkbox" name="view_log"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">4</th>
							  <td>Delete log</td>
							  <?php
								if($m_permissions[3]=="1")
									echo('<td><input type="checkbox" name="delete_log" checked></td>');
								else
									echo('<td><input type="checkbox" name="delete_log"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">5</th>
							  <td>Server Settings</td>
							  <?php
								if($m_permissions[4]=="1")
									echo('<td><input type="checkbox" name="server_settings" checked></td>');
								else
									echo('<td><input type="checkbox" name="server_settings"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">6</th>
							  <td>Client settings</td>
							  <?php
								if($m_permissions[5]=="1")
									echo('<td><input type="checkbox" name="client_settings" checked></td>');
								else
									echo('<td><input type="checkbox" name="client_settings"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">7</th>
							  <td>Database settings</td>
							  <?php
								if($m_permissions[6]=="1")
									echo('<td><input type="checkbox" name="database_settings" checked></td>');
								else
									echo('<td><input type="checkbox" name="database_settings"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">8</th>
							  <td>Add clients</td>
							  <?php
								if($m_permissions[7]=="1")
									echo('<td><input type="checkbox" name="add_clients" checked></td>');
								else
									echo('<td><input type="checkbox" name="add_clients"></td>');
								?>
							</tr>
							<tr>
							  <th scope="row">9</th>
							  <td>Delete/list clients</td>
							  <?php
								if($m_permissions[8]=="1")
									echo('<td><input type="checkbox" name="delete_clients" checked></td>');
								else
									echo('<td><input type="checkbox" name="delete_clients"></td>');
								?>
							</tr>
						  </tbody>
						</table>

                        <button type="submit" class="btn btn-primary btn-block">Update user</button>
                    </form>
					<br>
					<!-- php code to add user-->
					<?php
						// Check if the form is submitted
						if ($_SERVER["REQUEST_METHOD"] == "POST" and $block==0) {
							//include db pw
							include "../../../config.php";
							// Retrieve user input
							$m_email=$_POST["email"];
							$m_username=$_POST["username"];
							// Create a connection
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

							// Check the connection
							if ($conn->connect_error) {
								die("Connection failed: " . $conn->connect_error);
							}
							
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
							if ($conn->connect_error) {
									$success=0;
									die("Connection failed: " . $conn->connect_error);
								}
							$stmt = $conn->prepare("UPDATE users set email=?, username=?,perms=?");
							$stmt->bind_param("ssss", $email, $username, $permissions);

							$email=htmlspecialchars($_POST["email"]);
							$username=htmlspecialchars($_POST["username"]);
							$permissions=get_perm_str();
							
							$stmt->execute();
							$stmt->close();
							$conn->close();
							echo '<div class="alert alert-success" role="alert">
										User updates successfully!
									  </div>';
							
						}elseif($block==1){
							echo '<div class="alert alert-danger" role="alert">
												You do not have permission to update a user!
											  </div>';
						}
					?>
					
                </div>
            </div>
        </div>
		<div class="modal fade" id="warning" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="false">
		  <div class="modal-dialog">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="exampleModalLabel">User add permission warning</h5>
				<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
			  </div>
			  <div class="modal-body">
				A user with the permission "add_user" can add new users with all permissions.<br>
				Including permissions which the user, who creates the new user does not have.<br>
				This can be used for privilege escalation!<br>
				Please only allow a few trusted users this permission!
			  </div>
			</div>
		  </div>
		</div>
		<div class="modal fade" id="warning2" tabindex="-1" aria-labelledby="warning2_label" aria-hidden="false">
		  <div class="modal-dialog">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="warning2_label">User manage permission warning</h5>
				<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
			  </div>
			  <div class="modal-body">
				A user with the permission "manage_user" can change all permissions of all users.<br>
				Including permissions which the user, who has this permission does not have.<br>
				This can be used for privilege escalation!<br>
				Please only allow a few trusted users this permission!
			  </div>
			</div>
		  </div>
		</div>
    </div>
</div>
</body>
</html>
