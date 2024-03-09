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
if($perms[6]!=="1"){
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
	 <title>Database Settings</title>
</head>
<body>
<script>
async function update_db(){
	var loader=document.getElementById("loader");
	var finish=document.getElementById("finish");
	var button=document.getElementById("update_btn");
	loader.style.display="block";
	finish.style.display="none";
	button.style.display="none";
	await fetch('/api/php/database/compile_database.php');
	loader.style.display="none";
	button.style.display="block";
	finish.style.display="block";
}
async function update_percentage() {
    var percentageElement = document.getElementById("percentage");
    
    // Display loading indicator
    percentageElement.innerHTML = "Loading...";
    
    // Make an AJAX request to the PHP script
    await fetch('/api/php/database/get_update_status.php')
        .then(response => response.json())
        .then(data => {
            // Update the percentage on the webpage
            percentageElement.innerHTML = "Status: " + data.toFixed(2) + "%";
        })
        .catch(error => {
            // Display error message if request fails
            percentageElement.innerHTML = "Error: " + error.message;
        });
}

// Call update_percentage() every 5 seconds
setInterval(update_percentage, 5000);
</script>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Database Settings</h4>
                </div>
                <div class="card-body">
					<button id="update_btn" class="btn btn-dark" onclick="update_db();">Update / regenerate Database (!this can take very long ~1h)</button>
					<div style="display:none" id="loader">
						<br>
						<div class="alert alert-success" role="alert">
							Database update is running, please do not close this tab and do not navigate away!
							<div id="percentage"></div>
						</div>
						<div class="spinner-border" role="status">
						  <span class="visually-hidden">Loading...</span>
						</div>
					</div>
					<div class="alert alert-success" role="alert" style="display:none" id="finish">
							Database update finished!
					</div>
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>