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
if($perms[2]!=="1"){
    header("location:/system/insecure_zone/php/no_access.php");
    exit();
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
     <title>View log</title>
	 
	<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>

</head>
<body>
<br><br>
<div class="container-fluid">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Log backups</h4>
                </div>
                <div class="card-body" style="overflow-x:auto">
                    <!-- list the log file backups -->
					<h5>In order to ensure no attacker can delete evidence, you cannot delete these log backups!</h5>
					<table class="table">
						<tr><th>Log backup</th><th>Download</th></tr>
						<?php
							// Define the directory to search for .csv (our log backup) files
							$directory = '/var/www/html/backups';

							// Use glob to find all .csv files in the specified directory
							$csvFiles = glob($directory . '/*.csv');

							// Loop through each file and print its name
							foreach ($csvFiles as $file) {
								echo("<tr><td>".basename($file)."</td><td><a href='/backup/".basename($file)."' download>Download</a></td></tr>");
							}
						?>
					
					</table>
                </div>
            </div>
        </div>
    </div>
</div>
</div>
</body>
</html>
