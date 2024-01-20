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
if($perms[1]!=="1"){
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
                    <h4>User list</h4>
                </div>
                <div class="card-body">
					<!-- table with all users => delete button -->
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
							$sql = "DELETE FROM users WHERE id = ?";
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
						$sql = "SELECT count(*) AS user_count FROM users";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						$row = $result->fetch_assoc();
						$num_of_users=$row["user_count"];
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
						echo('<th>Userid</th><th>Username</th><th>Email</th><th>Permissions</th><th>Delete user</th>');
						echo('</tr>');
						echo('</thead>');
						echo('<tbody>');
						while($num_of_users!=0){
							$sql = "SELECT * FROM users where id > $last_id";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							$last_id=$row["id"];
							$username=$row["username"];
							$email=$row["email"];
							$perms=$row["perms"];
							echo('<tr>');
								echo('<td>'.$last_id.'</td>');
								echo('<td>'.$username.'</td>');
								echo('<td>'.$email.'</td>');
								echo('<td>'.$perms.'</td>');
								echo('<td><a href="user_list.php?delete='.$last_id.'">delete</a></td>');
							echo('</tr>');
							$stmt->close();
							$num_of_users--;
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
