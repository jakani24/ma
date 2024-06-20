<?php
session_start();

// Check if the user is logged in
if (!isset($_SESSION['username']) or !isset($_SESSION["login"])) {
    // Redirect to the login page or handle unauthorized access
    header("Location: /login.php");
    exit();
}

$username = htmlspecialchars($_SESSION['username']);
$perms = $_SESSION["perms"];
if(isset($_GET["page"])){
	$page=htmlspecialchars($_GET["page"]);
}else{
	$page="welcome.php"; //this is actually the Dashboard
}

?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Cyberhex (<?php echo(str_replace("_"," ",explode(".",$page))[0]); ?>)</title>
</head>
<body>
	<!-- navbar -->
	<nav class="navbar navbar-light bg-light">
	  <div class="container-fluid">
		<span class="navbar-text">
		  Cyberhex (<?php echo(str_replace("_"," ",explode(".",$page))[0]); ?>)
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
			<?php 
				if(1)//every user can access this page
					echo('<li><a href="index.php?page=welcome.php">Dashboard</a></li>');
			?>
		  </ul>
		  <p>User</p>
		  <ul>
			<?php
			if(1)//every user can access this page
				echo('<li><a href="index.php?page=profile.php">Profile</a></li>');
			if(1)//every user can access this page
				echo('<li><a href="index.php?page=passwd.php">Change Password</a></li>');
		    if($perms[0]=="1")
				echo('<li><a href="index.php?page=add_user.php">Add User</a></li>');
			if($perms[1]=="1")
				echo('<li><a href="index.php?page=user_list.php">User List</a></li>');
		   ?>
		  </ul>
		  <?php
				if($perms[2]=="1")
					echo("<p>Log</p>");
		  ?>
		  <ul>
			<?php
		    if($perms[2]=="1")
				echo('<li><a href="index.php?page=view_log.php">View Log</a></li>');
			if($perms[2]=="1")
				echo('<li><a href="index.php?page=export_log.php">Export Log</a></li>');
			if($perms[2]=="1")
				echo('<li><a href="index.php?page=log_backups.php">Log Backups</a></li>');
			if($perms[2]=="1")
				echo('<li><a href="index.php?page=view_server_log.php">Server Log</a></li>');
			?>
		  </ul>
		  <?php
				if($perms[4]=="1" or $perms[5]=="1" or $perms[6]=="1")
					echo("<p>Cyberhex settings</p>");
		  ?>
		  <ul>
			<?php
			if($perms[4]=="1")
				echo('<li><a href="index.php?page=server_settings.php">Server Settings</a></li>');
			if($perms[5]=="1")
				echo('<li><a href="index.php?page=client_settings.php?show=general">Client Settings</a></li>');
			if($perms[6]=="1")
				echo('<li><a href="index.php?page=database_settings.php?show=update">Database Settings</a></li>');
			?>
		  </ul>
		  <?php
				if($perms[7]=="1" or $perms[8]=="1")
					echo("<p>Clients</p>");
		   ?>
		  <ul>
		  <?php
			if($perms[7]=="1")
				echo('<li><a href="index.php?page=add_client.php">Add Client</a></li>');
			if($perms[8]=="1")
				echo('<li><a href="index.php?page=client_list.php">Client List</a></li>');
		   ?>
		  </ul>
		  
		   <?php
				if($perms[9]=="1" or $perms[10]=="1")
					echo("<p>Incidents</p>");
		   ?>
		   <ul>
		  <?php
			if($perms[9]=="1" or $perms[10]=="1")
				echo('<li><a href="index.php?page=incident_list.php">View Incidents</a></li>');
			if($perms[10]=="1")
				echo('<li><a href="index.php?page=add_incident.php">Add Incident</a></li>');
		   ?>
		  </ul>

		</div>

		<!-- main part, with iframe -->
		<div class="col-10" >
		 <!-- iframe -->
		 
			<iframe src="<?php echo(str_replace(["://","http"],"",$page)); ?>" width="100%" height="1000px" frameborder="0" style="overflow:hidden"></iframe>
		</div>
	  </div>
	</div>
</body>
</html>
