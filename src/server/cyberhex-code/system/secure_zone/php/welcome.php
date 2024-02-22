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
                    <h4>Dashboard</h4>
                </div>
                <div class="card-body">
					<h4>Current Threads</h4>
					<?php
						include "../../../config.php";
						// Create a connection
						$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

						// Check the connection
						if ($conn->connect_error) {
							die("Connection failed: " . $conn->connect_error);
						}
						$sql = "SELECT count(*) AS vir_count FROM vir_notify";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						$row = $result->fetch_assoc();
						$num_of_entrys=$row["vir_count"];
						$stmt->close();
						$conn->close();
						
						//now list of all the entrys => machineid, file, hash, action taken
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
						echo('<th>Entryid</th><th>Machineid</th><th>File</th><th>Hash</th><th>Action Taken</th>');
						echo('</tr>');
						echo('</thead>');
						echo('<tbody>');
						while($num_of_entrys!=0){
							$sql = "SELECT * FROM vir_notify where id > $last_id";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							$last_id=$row["id"];
							$machineid=$row["machine_id"];
							$hash=$row["hash"];
							$file=$row["path"];
							$action=$row["action"];
							echo('<tr>');
								echo('<td>'.$last_id.'</td>');
								echo('<td>'.$machineid.'</td>');
								echo('<td>'.$file.'</td>');
								echo('<td>'.$hash.'</td>');
								echo('<td>'.$action.'</td>');
							echo('</tr>');
							$stmt->close();
							$num_of_entrys--;
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
