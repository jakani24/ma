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
if(isset($_GET["page"])){
	$page=$_GET["page"];
}else{
	$page="welcome.php";
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Cyberhex</title>
</head>
<body>
	<!-- navbar -->
	<nav class="navbar navbar-light bg-light">
	  <div class="container-fluid">
		<span class="navbar-text">
		  Cyberhex (<?php echo($page); ?>)
		</span>
		<span class="navbar-text text-center">
		  <?php echo($username); ?>
		</span>
		<span class="navbar-text text-right">
			<a href="/logout.php">Logout</a>
		</span>
	  </div>
	</nav>

	<div class="container-fluid">
	  <div class="row">
	  
		<!-- sidebar -->
		<div class="col-2">
		  <p>Home</p>
		  <ul>
			<li><a href="index.php?page=welcome.php">Welcome</a></li>
		  </ul>
		  <p>User</p>
		  <ul>
		   <li><a href="index.php?page=profile.php">Profile</a></li>
		   <li><a href="index.php?page=passwd.php">Change Password</a></li>
		   <li><a href="index.php?page=add_user.php">Add User</a></li>
		   <li><a href="index.php?page=user_list.php">User List</a></li>
		  </ul>
		  <p>Log</p>
		  <ul>
		   <li><a href="index.php?page=view_log.php">View Log</a></li>
		   <li><a href="index.php?page=export_log.php">Export Log</a></li>
		  </ul>
		  <p>Cyberhex settings</p>
		  <ul>
		   <li><a href="index.php?page=server_settings.php">Server Settings</a></li>
			<li><a href="index.php?page=client_settings.php">General Client Settings</a></li>
			<li><a href="index.php?page=rtp_client_settings.php">RTP Client Settings</a></li>
			<li><a href="index.php?page=database_settings.php">Database Settings</a></li>
		  </ul>
		  <p>Clients</p>
		  <ul>
		   <li><a href="index.php?page=mass_add_clients.php">Mass Add CLients</a></li>
		   <li><a href="index.php?page=add_client.php">Add Client</a></li>
		   <li><a href="index.php?page=client_list.php">Client List</a></li>
		  </ul>

		</div>

		<!-- main part, with iframe -->
		<div class="col-10" >
		 <!-- iframe -->
		 
			<iframe src="<?php echo($page); ?>" width="100%" height="1000px" frameborder="0" style="overflow:hidden"></iframe>
		</div>
	  </div>
	</div>
</body>
</html>
