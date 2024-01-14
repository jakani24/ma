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
		  <p>This container takes up 20% of the width.</p>
		</div>

		<!-- main part, with iframe -->
		<div class="col-10" >
		 <!-- iframe -->
		 <p>This container takes up 80% of the width.</p>
		</div>
	  </div>
	</div>
</body>
</html>
