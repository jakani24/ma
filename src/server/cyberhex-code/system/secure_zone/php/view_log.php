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
						//delete entry if requested and if user has rights to do that
						if(isset($_GET["delete"])){
							if($perms[3]!=="1"){
								echo '<div class="alert alert-danger" role="alert">
												You are not allowed to delete log entrys. (insufficient permissions)
								</div>';
							}else{
								$id=htmlspecialchars($_GET["delete"]);
								$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
								if ($conn->connect_error) {
									die("Connection failed: " . $conn->connect_error);
								}
								$sql = "DELETE FROM log WHERE id = ?";
								$stmt = $conn->prepare($sql);
								$stmt->bind_param("i", $id);
								// Execute the statement
								$stmt->execute();
								$stmt->close();
								$conn->close();
								echo '<div class="alert alert-success" role="alert">
												Log entry deleted.
								</div>';
							}
						}
						
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
						echo('<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Machine id</th><th>Delete entry</th>');
						echo('</tr>');
						echo('</thead>');
						echo('<tbody>');
						
						//put filter options here
						if(isset($_GET["loglevel"]))
							$loglevel_ss=$_GET["loglevel"]; //put the loglevel search string to that and afterwards show it in the filter optionss. so a user sees what he has filtered for
						else
							$loglevel_ss="Loglevel";
						
						if(isset($_GET["logtext"]))
							$logtext_ss=$_GET["logtext"];
						else
							$logtext_ss="Logtext";
						
						if(isset($_GET["machine_id"]))
							$machine_id_ss=$_GET["machine_id"];
						else
							$machine_id_ss="Machine id";
						echo('<tr>');
						echo('<form action="view_log.php" method="get">');
						echo('<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>');
						echo('<td><input type="text" class="form-control" name="loglevel" placeholder="'.$loglevel_ss.'"></td>');
						echo('<td><input type="text" class="form-control" name="logtext" placeholder="'.$logtext_ss.'"></td>');
						echo('<td><input type="text" class="form-control" name="machine_id" placeholder="'.$machine_id_ss.'"></td>');
						echo('<td>---</td>');
						echo('</form>');
						echo('</tr>');
						
						while($num_of_log_entrys!=0){
							$sql = "SELECT * FROM log where id > $last_id";
							$stmt = $conn->prepare($sql);
							// Execute the statement
							$stmt->execute();
							// Get the result
							$result = $stmt->get_result();
							$row = $result->fetch_assoc();
							$last_id=$row["id"];
							$loglevel=$row["loglevel"];
							$logtext=$row["logtext"];
							$machine_id=$row["machine_id"];
							$show=true;
							//evaluate filter, decide if entry should be shown or not
							if(isset($_GET["loglevel"]) && $_GET["loglevel"]!==""){
								if(stripos($loglevel,$_GET["loglevel"])===false){
									$show=false;
								}
							}if(isset($_GET["logtext"]) && $_GET["logtext"]!==""){
								if(stripos($logtext,$_GET["logtext"])===false){
									$show=false;
								}
							}if(isset($_GET["machine_id"]) && $_GET["machine_id"]!==""){
								if(stripos($machine_id,$_GET["machine_id"])===false){
									$show=false;
								}
							}
							if($show==true){
								echo('<tr>');
									echo('<td>'.$last_id.'</td>');
									echo('<td>'.$loglevel.'</td>');
									echo('<td>'.$logtext.'</td>');
									echo('<td>'.$machine_id.'</td>');
									echo('<td><a href="view_log.php?delete='.$last_id.'">delete</a></td>');
								echo('</tr>');
							}
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
