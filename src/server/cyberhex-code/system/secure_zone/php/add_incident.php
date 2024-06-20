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
if($perms[10]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
include "../../../api/php/log/add_server_entry.php"; //to log things
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Change Password</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>Add an incident</h4>
                </div>
                <div class="card-body">
					<form action="add_incident.php?add=true" method="post">
                        <div class="form-group">
                            <label for="keyword">Short description / keywords:</label>
                            <input type="text" class="form-control" id="keyword" name="keyword" required>
                        </div>
						<br>
                        <button type="submit" class="btn btn-primary btn-block">Create incident</button>
                    </form>
					<br>
					<!-- php code to add incidents-->
					<?php
						// Check if the form is submitted
						if ($_SERVER["REQUEST_METHOD"] == "POST" and $block==0) {
							//include db pw
							include "../../../config.php";
							// Retrieve user input
							$keyword=$_POST["keyword"];
							// Create a connection		
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
							if ($conn->connect_error) {
									$success=0;
									die("Connection failed: " . $conn->connect_error);
								}
							$stmt = $conn->prepare("INSERT INTO incidents (description, status) VALUES (?, 'open')");
							$stmt->bind_param("s", $keyword);

							$keyword=htmlspecialchars($_POST["keyword"]);
							
							$stmt->execute();
							$stmt->close();
							$conn->close();
							echo '<div class="alert alert-success" role="alert">
										Incident added successfully!
									  </div>';
							log_action("INCIDENT::ADD::SUCCESS","User ".$_SESSION["username"]." added an incident.",$_SESSION["id"]);	
						}
					?>
					
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
