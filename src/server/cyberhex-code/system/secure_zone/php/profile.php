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
?>
<?php
//update the info, if provided.
if ($_SERVER["REQUEST_METHOD"] == "POST") {
	//include db pw
	include "../../../config.php";
	$email=htmlspecialchars($_POST["email"]);
	$username_new=htmlspecialchars($_POST["username"]);
	// Create connection
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

	// Check connection
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$stmt = $conn->prepare("UPDATE users set email = ?, username = ? where username = ?");
	$stmt->bind_param("sss", $email, $username_new, $username);
	
	$email=htmlspecialchars($_POST["email"]);
	$username_new=htmlspecialchars($_POST["username"]);
	$stmt->execute();
	$stmt->close();
	$conn->close();
	$username=$username_new;
	$_SESSION["username"]=$username;
	$_SESSION["email"]=$email;
}

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Profile</title>
</head>
<body>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
				<div class="card-header">
					<h4>Your Profile (<?php echo($username); ?>)</h4>
				</div>
				<div class="card-body">
					<form action="profile.php?update=true" method="post">
						<div class="form-group">
							<label for="username">Username:</label>
							<input type="text" class="form-control" id="username" name="username" value="<?php echo($username); ?>" required>
						</div>
						<div class="form-group">
							<label for="email">Email:</label>
							<input type="email" class="form-control" id="email" name="email" value="<?php echo($email); ?>" required>
						</div>
						<div class="form-group">
							<label for="perms">Permissions: 
								<button type="button" class="btn btn-primary" data-bs-toggle="modal" data-bs-target="#exampleModal">
								  Launch demo modal
								</button>
							</label>
							<input type="text" class="form-control" id="perms" name="perms" value="<?php echo($perms); ?>" required readonly>
						</div>
						<br>
						<button type="submit" class="btn btn-primary btn-block">Update</button>
					</form>
					<?php
						if(isset($_GET["update"])){
							echo '<br><div class="alert alert-success" role="alert">
										Information updated successfully!
									  </div>';
						}
					?>
				</div>
			</div>
		</div>
		<div class="modal fade" id="exampleModal" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="true">
		  <div class="modal-dialog">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="exampleModalLabel">Modal title</h5>
				<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
			  </div>
			  <div class="modal-body">
				...
			  </div>
			  <div class="modal-footer">
				<button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
				<button type="button" class="btn btn-primary">Save changes</button>
			  </div>
			</div>
		  </div>
		</div>
	</div>
</div>

</body>
</html>
