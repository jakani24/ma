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
$telegram_id=$_SESSION["telegram_id"];
?>
<?php
//update the info, if provided.
if ($_SERVER["REQUEST_METHOD"] == "POST") {
	//include db pw
	include "../../../config.php";
	$email=htmlspecialchars($_POST["email"]);
	$username_new=htmlspecialchars($_POST["username"]);
	$telegram_id=htmlspecialchars($_POST["telegram_id"]);
	$pw_login=isset($_POST["pw_login"]);
	$send_login_message=isset($_POST["send_login_message"]);
	// Create connection
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);

	// Check connection
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$user_hex_id=bin2hex($username_new);
	$stmt = $conn->prepare("UPDATE users set email = ?, username = ?, telegram_id = ?, allow_pw_login = ?, user_hex_id = ?, send_login_message = ? where username = ?");
	$stmt->bind_param("sssissi", $email, $username_new,$telegram_id, $pw_login,$user_hex_id, $send_login_message , $username);
	
	$email=htmlspecialchars($_POST["email"]);
	$username_new=htmlspecialchars($_POST["username"]);
	$telegram_id=htmlspecialchars($_POST["telegram_id"]);
	$stmt->execute();
	$stmt->close();
	$conn->close();
	$username=$username_new;
	$_SESSION["username"]=$username;
	$_SESSION["email"]=$email;
	$_SESSION["telegram_id"]=$telegram_id;
	$_SESSION["allow_pw_login"]=$pw_login;
	$_SESSION["send_login_message"]=$send_login_message;
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
							<label for="telegram_id">Telegram ID:</label>
							<input type="text" class="form-control" id="telegram_id" name="telegram_id" value="<?php echo($telegram_id); ?>">
						</div>
						<div class="form-group">
							<label for="perms">Permissions: 
								<a data-bs-target="#perms_help" data-bs-toggle="modal" href="#perms_help">?</a>
							</label>
							<input type="text" class="form-control" id="perms" name="perms" value="<?php echo($perms); ?>" readonly>
						</div>
						<div class="form-group">
							<?php
								if($_SESSION["allow_pw_login"]==1){
									echo("<input type='checkbox' id='pw_login' name='pw_login' checked>");
								}else{
									echo("<input type='checkbox' id='pw_login' name='pw_login'>");
								}
							?>
							<label for="pw_login">Allow password logins. (Please make shure you have a passkey, if you disable this!)</label>
						</div>
						<br>
						<div class="form-group">
							<?php
								if($_SESSION["send_login_message"]==1){
									echo("<input type='checkbox' id='send_login_message' name='send_login_message' checked>");
								}else{
									echo("<input type='checkbox' id='send_login_message' name='send_login_message'>");
								}
							?>
							<label for="pw_login">Send you a message when somebody logs in with your account (You need to set your Telegram id for this to work)</label>
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
		<div class="modal fade" id="perms_help" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="false">
		  <div class="modal-dialog">
			<div class="modal-content">
			  <div class="modal-header">
				<h5 class="modal-title" id="exampleModalLabel">Permission Explanation</h5>
				<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
			  </div>
			  <div class="modal-body">
				1 Add User<br>
				1 Delete/list/manage users<br>
				1 View Log<br>
				1 Delete Log<br>
				1 Server Settings<br>
				1 Client Settings<br>
				1 Database Settings<br>
				1 Add Clients<br>
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
