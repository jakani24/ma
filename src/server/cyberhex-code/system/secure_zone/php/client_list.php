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
if($perms[8]!=="1"){
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
	 <title>Change Password</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Client list</h4>
                </div>
                <div class="card-body">
					<!-- table with all clients => delete button -->
					<?php
						//include db pw
						include "../../../config.php";
						//delete user if requested
						if(isset($_GET["delete"])){
							$userid=htmlspecialchars($_GET["delete"]);
							$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
							if ($conn->connect_error) {
								die("Connection failed: " . $conn->connect_error);
							}
							$sql = "DELETE FROM machines WHERE id = ?";
							$stmt = $conn->prepare($sql);
							$stmt->bind_param("i", $userid);
							// Execute the statement
							$stmt->execute();
							$stmt->close();
							$conn->close();
						}
						
						
						//get count of users
						// Create a connection
						$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

						// Check the connection
						if ($conn->connect_error) {
							die("Connection failed: " . $conn->connect_error);
						}
						$sql = "SELECT count(*) AS machine_count FROM machines";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						$row = $result->fetch_assoc();
						$num_of_machines=$row["machine_count"];
						$stmt->close();
						$conn->close();
						
						//now list of all the users => userid, username, email, perms, delete
						// Create a connection
						$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

						// Check the connection
						if ($conn->connect_error) {
							die("Connection failed: " . $conn->connect_error);
						}
						$last_id=-1;
						//create the table header
						echo('<table class="table">');
						echo('<thead>');
						echo('<tr>');
						echo('<th>Machineid</th><th>Location</th><th>IP-Address</th><th>Delete machine</th>');
						echo('</tr>');
						echo('</thead>');
						echo('<tbody>');
						while($num_of_machines!=0){
							$sql = "SELECT * FROM machines WHERE id > $last_id";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							$last_id=$row["id"];
							$machine_id=$row["machine_name"];
							$machine_location=$row["machine_location"];
							$machine_ip=$row["machine_ip"];
							echo('<tr>');
								echo('<td>'.$machine_id.'</td>');
								echo('<td>'.$machine_location.'</td>');
								echo('<td>'.$machine_ip.'</td>');
								echo('<td><a href="client_list.php?delete='.$last_id.'">delete</a></td>');
							echo('</tr>');
							$stmt->close();
							$num_of_machines--;
						}
						echo('</tbody>');
						echo('</table>');
						$conn->close();
					?>
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
