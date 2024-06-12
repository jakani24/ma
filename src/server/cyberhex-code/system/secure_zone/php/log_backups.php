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

//to delete old log files
function delete_old_files($directory, $days) {
    // Ensure the directory path ends with a slash
    if (substr($directory, -1) !== DIRECTORY_SEPARATOR) {
        $directory .= DIRECTORY_SEPARATOR;
    }

    // Get the current time
    $now = time();
	$deleted_files=0;
    // Open the directory
    if ($handle = opendir($directory)) {
        // Loop through each file in the directory
        while (false !== ($file = readdir($handle))) {
            // Skip . and .. special entries
            if ($file !== '.' && $file !== '..') {
                // Get the full path of the file
                $filePath = $directory . $file;

                // Check if it's a file (skip directories)
                if (is_file($filePath)) {
                    // Get the file's modification time
                    $fileModTime = filemtime($filePath);

                    // Calculate the file age in seconds
                    $fileAge = $now - $fileModTime;

                    // Convert the days to seconds and check if the file is older than the specified number of days
                    if ($fileAge > ($days * 24 * 60 * 60)) {
                        // Delete the file
                        unlink($filePath);
						$deleted_files++;
                    }
                }
            }
        }
        // Close the directory handle
        closedir($handle);
    }
	return $deleted_files;
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
     <title>View log</title>
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
					<h5>In order to ensure no attacker can delete evidence, you cannot delete these log backups unless they are older then 90 days!</h5>
					<a class="btn btn-danger" href="log_backups.php?delete_old">Delete old log files</a>
					<?php
					if(isset($_GET["delete_old"])){
							if($perms[3]!=="1"){
								echo '<br><br><div class="alert alert-danger" role="alert">
												You are not allowed to delete log entries. (insufficient permissions)
								</div>';
							}else{
								$deleted_files=delete_old_files("/var/www/html/backup/",90);
								echo '<br><br><div class="alert alert-success" role="alert">
												Deleted '.$deleted_files.' files.
								</div>';
							}
						}
					?>
					<table class="table">
						<tr><th>Log backup</th><th>Download</th></tr>
						<?php
							// Define the directory to search for .csv (our log backup) files
							$directory = '/var/www/html/backup/';

							// Use glob to find all .csv files in the specified directory
							$csvFiles = glob($directory . '*.csv');
							$fileCreationTimes = [];
							//sort the files
							 foreach ($csvFiles as $file) {
								$fileCreationTimes[$file] = filectime($file);
							}
							arsort($fileCreationTimes);
							// Loop through each file and print its name
							foreach ($fileCreationTimes as $file=> $creationTime) {
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
