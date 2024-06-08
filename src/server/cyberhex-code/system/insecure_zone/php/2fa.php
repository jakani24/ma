<?php
session_start();
if(isset($_SESSION["login"])){
	header("LOCATION:/system/secure_zone/php/index.php");
}
if(!isset($_SESSION["2fa_auth"])){ //so only someone who has allready confirmed his passwd can get here
	header("LOCATION:/system/insecure_zone/php/login.php");
	exit();
}
include "../../../api/php/notifications/sendmessage.php"; //to send user notification on login
include "../../../api/php/log/add_server_entry.php"; //to log things
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	<title>Cyberhex login page</title>
	<style>
    .bg-image {
      background-image: url('/logo.png');
      background-size: contain;
      background-repeat: no-repeat;
      background-position: center;
      height: 100vh; /* Set height to viewport height */
    }
  </style>
</head>
<body>
<div class="container-fluid bg-image">
	<div class="container mt-5">
		<div class="row justify-content-center">
			<div class="col-md-6">
				<div class="card">
					<div class="card-header">
						<h4>Login to Cyberhex using second factor</h4>
					</div>
					<div class="card-body">
						<form action="2fa.php" method="post">
							<div class="form-group">
								<label for="pin">2FA-Pin (sent to you via Telegram):</label>
								<input type="text" class="form-control" id="pin" name="pin" required>
							</div>
							<br>
							<div class="col">
									<button style="align:left" type="submit" class="btn btn-primary btn-block">Check pin</button>
							</div>
							<br>
						</form>
						<!-- php code to verify 2fa pin-->
						<?php
							// Check if the form is submitted
							if ($_SERVER["REQUEST_METHOD"] == "POST") {
								//include db pw
								include "../../../config.php";
								
								// Retrieve user input
								$pin = htmlspecialchars($_POST["pin"]);
								if($pin==$_SESSION["pin"])	
								{
									$_SESSION["login"]=true;
									log_action("LOGIN::2FA::SUCCESS","User ".$_SESSION["username"]." logged in with second factor.",$_SESSION["id"]);
									if($_SESSION["send_login_message"]=="1"){
										$ip = $_SERVER['REMOTE_ADDR'];
										$username=$_SESSION["username"];
										send_to_user("[LOGIN WARNING]\nHello $username\nSomebody has logged into Cyberhex with your account.\nIf this was you, you can ignore this message. Else please take steps to secure your account!\nIP: $ip\n",$username);
									}
									echo '<script>window.location.href = "/system/secure_zone/php/index.php";</script>';
								}else {
									$pin=mt_rand(100000, 999999);
									$_SESSION["pin"]=$pin;
									$ip = $_SERVER['REMOTE_ADDR'];
									$username=$_SESSION["username"];
									send_to_user("[2FA-Pin]\nHello $username\nHere is your pin to log into cyberhex: $pin. If you did not try to log in please take steps to secure your account!\nIP: $ip\n",$username);
									echo '<div class="alert alert-danger" role="alert">
													Incorrect pin. A new pin has been sent to you via Telegram. The old pin is no longer valid!
												  </div>';
								}
							}
						?>
						
					</div>
				</div>
			</div>
		</div>
	</div>
</div>
</body>
</html>
