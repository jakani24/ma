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
	$block=1;
	exit();
}else{
	$block=0;
}
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
                    <h4>Log view</h4>
                </div>
                <div class="card-body">
					<!-- table with all users => delete button -->
					<?php
						//include db pw
						include "../../../config.php";						
						//get count of log entrys
						// Create a connection
						$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

						// Check the connection
						if ($conn->connect_error) {
							die("Connection failed: " . $conn->connect_error);
						}
						$sql = "SELECT count(*) AS log_count FROM log";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						$row = $result->fetch_assoc();
						$num_of_log_entrys=$row["log_count"];
						$stmt->close();
						$conn->close();
						
						//list out log => id, loglevel, logtext, machine_id
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
						echo('<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Machine id</th>');
						echo('</tr>');
						echo('</thead>');
						echo('<tbody>');
						
						//put filter options here
						echo('<tr>');
						echo('<td>---</td>');
						echo('<form action="view_log.php" method="get">');
						echo('<td><input type="text" class="form-control" name="loglevel" placeholder="Loglevel"></td>');
						
						while($num_of_log_entrys!=0){
							$sql = "SELECT * FROM log where id > $last_id";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							$last_id=$row["id"];
							$loglevel=$row["username"];
							$logtext=$row["email"];
							$machine_id=$row["perms"];
							echo('<tr>');
								echo('<td>'.$last_id.'</td>');
								echo('<td>'.$loglevel.'</td>');
								echo('<td>'.$logtext.'</td>');
								echo('<td>'.$machine_id.'</td>');
								//echo('<td><a href="user_list.php?delete='.$last_id.'">delete</a></td>');
							echo('</tr>');
							$stmt->close();
							$num_of_log_entrys--;
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
