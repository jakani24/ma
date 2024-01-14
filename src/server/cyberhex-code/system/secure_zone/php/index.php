<?php
/*
session_start();

// Check if the user is logged in
if (!isset($_SESSION['username']) or !isset($_SESSION["login"])) {
    // Redirect to the login page or handle unauthorized access
    header("Location: /login.php");
    exit();
}

$username = $_SESSION['username'];
*/
$page="test.php";
$username="test";
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
</head>
<body style="height:100%">
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
		  <p>List A</p>
		  <ul>
		   <li>Item A</li>
		   <li>Item B</li>
		   <li>Item C</li>
		  </ul>
		  <p>List B</p>
		  <ul>
		   <li>Item A</li>
		   <li>Item B</li>
		   <li>Item C</li>
		  </ul>

		</div>

		<!-- main part, with iframe -->
		<div class="col-10" >
		 <!-- iframe -->
			<iframe src="https://www.example.com" width="100%" height="100%" frameborder="0" style="overflow:hidden"></iframe>
		</div>
	  </div>
	</div>
</body>
</html>
