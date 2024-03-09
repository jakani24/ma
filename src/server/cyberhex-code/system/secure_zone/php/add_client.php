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
if($perms[7]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
		if ($conn->connect_error) {
			$success=0;
			die("Connection failed: " . $conn->connect_error);
		}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Add Machine</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>Add a machine</h4>
                </div>
                <div class="card-body">
					<form action="add_client.php?add=true" method="post">
						<div class="form-group">
                            <label for="location">Location:</label>
                            <input type="text" class="form-control" id="location" name="location" placeholder="Office1 - Computer of Lisa" required>
                        </div>
						<div class="form-group">
                            <label for="ip">IP: (can be left blank)</label>
                            <input type="text" class="form-control" id="ip" name="ip">
                        </div>
						<br>
                        <button type="submit" class="btn btn-primary btn-block">Add Machine</button>
                    </form>
                </div>
				<?php
					if(isset($_GET["add"])){
						//php code to add the new machine
						//we need to create the following files:
						/*
						* secrets => machineid,cert,apikey
						* setup_script => server location, to download settings, databases etc
						* add installer.exe to download list => downloads the app and all the files 
						*/
						$location=htmlspecialchars($_POST["location"]);
						//create machineid:
						$random_bytes = random_bytes(6);

						// Convert the random bytes to hexadecimal
						$machineid = bin2hex($random_bytes);
						if($_POST["ip"]=="")
							$ip="nan";
						
						//check if there isn't a machine with the same name allready
						$sql = "SELECT * FROM machines WHERE machine_location = ?";
						$stmt = $conn->prepare($sql);
						$stmt->bind_param("s", $location);
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						$stmt->close();
						if($result->num_rows > 0){
							//theres allready a machine with this location
							echo '<div class="alert alert-danger" role="alert">
                                               There is allready a machine with the exact same location<br>Please change the location.
                                </div>';
						}
						else{
						
							$stmt = $conn->prepare("INSERT INTO machines (machine_name, machine_location,machine_ip) VALUES (?, ?, ?)");
							$stmt->bind_param("sss", $machineid, $location, $ip);
							$stmt->execute();
							$stmt->close();
							
							//create secrets
							$random_bytes = random_bytes(248);

							// Convert the random bytes to hexadecimal
							$cert = bin2hex($random_bytes);
							//create secrets
							$random_bytes = random_bytes(248);

							// Convert the random bytes to hexadecimal
							$apikey = bin2hex($random_bytes);
							$stmt = $conn->prepare("INSERT INTO secrets (machine_id, cert) VALUES (?, ?)");
							$stmt->bind_param("ss", $machineid, $cert);
							$stmt->execute();
							$stmt->close();
							$stmt = $conn->prepare("INSERT INTO api (machine_id, apikey) VALUES (?, ?)");
							$stmt->bind_param("ss", $machineid, $apikey);
							$stmt->execute();
							$stmt->close();
							
							//get the address of this server
							$sql = "SELECT * FROM settings WHERE name = 'setting_server_server_url'";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							if($row!==null){
								$this_server=$row["value"];
							}else{
								$this_server="http://localhost";
							}
							$stmt -> close();
							//create the files and download them
							$fp=fopen("/var/www/html/export/setup.txt","w");
							fwrite($fp,"server $this_server");
							fclose($fp);
							$fp=fopen("/var/www/html/export/secrets.txt","w");
							fwrite($fp,"machineid $machineid\n");
							fwrite($fp,"cert $cert\n");
							fwrite($fp,"apikey $apikey\n");
							fclose($fp);
							echo("<a href='/export/secrets.txt' download>Download config file1</a>");
							echo("<a href='/export/setup.txt' download>Download config file2</a>");
							echo("<a href='/install.bat' download>Download installer</a>");
						}
					}
				?>
            </div>
        </div>
    </div>
</div>
</body>
</html>
