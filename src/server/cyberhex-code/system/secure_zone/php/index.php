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
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.9.2/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.1/dist/js/bootstrap.min.js" integrity="sha384-Atwg2Pkwv9vp0ygtn1JAojH0nYbwNJLPhwyoVbhoPwBhjQPR5VtM2+xf0Uwh9KtT" crossorigin="anonymous"></script>
    <script src="https://kit.fontawesome.com/8ab5a73cf2.js" crossorigin="anonymous"></script>
</head>
<body>
	<!-- Header -->
	<nav class="navbar navbar-expand-lg navbar-light bg-light">
	  <center><h2>Cyberhex <?php echo($page); ?></h2></center>
		<span class="navbar-text">
		  <a href="/logout.php">Logout</a>
		</span>
	</nav>
	<div class="container mt-5">
		<!-- Main content -->
		<h2 class="text-center">Welcome to Bootstrap</h2>
		<p class="text-center">This is a simple webpage using Bootstrap CSS.</p>
		<div class="alert alert-primary" role="alert">
			This is a primary alert—check it out!
		</div>
	</div>
</body>
</html>
