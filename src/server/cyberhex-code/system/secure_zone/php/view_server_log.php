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
include "create_log_backup.php";
// Handle filter submission

$loglevel = htmlspecialchars(isset($_GET["loglevel"]) ? $_GET["loglevel"] : "");
$logtext = htmlspecialchars(isset($_GET["logtext"]) ? $_GET["logtext"] : "");
$search_username = htmlspecialchars(isset($_GET["username"]) ? $_GET["username"] : "");
$time = htmlspecialchars(isset($_GET["time"]) ? $_GET["time"] : "");
$filter_query = "&loglevel=$loglevel&logtext=$logtext&username=$search_username&time=$time";

include "../../../config.php";


?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
     <title>View server log</title>

</head>
<body>
<br><br>
<div class="container-fluid">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>View server log</h4>
                </div>
                <div class="card-body" style="overflow-x:auto">
                    <!-- table with all log entrys => delete button -->
                    <?php
                        //delete entry if requested and if user has rights to do that
                        if(isset($_GET["delete"])){
                            if($perms[3]!=="1"){
                                echo '<div class="alert alert-danger" role="alert">
                                                You are not allowed to delete log entries. (insufficient permissions)
                                </div>';
                            }else{
                                $id=htmlspecialchars($_GET["delete"]);
                                $conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                                if ($conn->connect_error) {
                                    die("Connection failed: " . $conn->connect_error);
                                }
								create_server_log_backup($conn);
                                $sql = "DELETE FROM server_log WHERE id = ?";
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
						if(isset($_GET["delete_all"])){
                            if($perms[3]!=="1"){
                                echo '<div class="alert alert-danger" role="alert">
                                                You are not allowed to delete log entries. (insufficient permissions)
                                </div>';
                            }else{
                                $conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                                if ($conn->connect_error) {
                                    die("Connection failed: " . $conn->connect_error);
                                }
								create_server_log_backup($conn);
                                $sql = "DELETE FROM server_log";
                                $stmt = $conn->prepare($sql);
                                // Execute the statement
                                $stmt->execute();
                                $stmt->close();
                                $conn->close();
                                echo '<div class="alert alert-success" role="alert">
                                                Log deleted.
                                </div>';
                            }
                        }
                        
                        // Define page size and current page
                        $page_size = 50;
                        $current_page = htmlspecialchars(isset($_GET['page']) ? intval($_GET['page']) : 1);
                        $offset = ($current_page - 1) * $page_size;
                        
                        // Get total number of log entries based on filters
                        $conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                        if ($conn->connect_error) {
                            die("Connection failed: " . $conn->connect_error);
                        }
                        $sql = "SELECT count(server_log.id) AS log_count FROM users,server_log WHERE (userid=users.id OR userid=-1) AND username LIKE ? AND loglevel LIKE ? AND logtext LIKE ? AND time LIKE ?";
                        $stmt = $conn->prepare($sql);
                        $loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
						$search_username = "%" . $search_username . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("ssss",$search_username, $loglevel, $logtext, $time);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        $row = $result->fetch_assoc();
                        $total_entries = $row["log_count"];
                        
                        // Calculate total pages
                        $total_pages = ceil(($total_entries) / $page_size);
                        
                        // Query log entries for the current page with filters
                        $sql = "SELECT * FROM users,server_log WHERE (userid=users.id OR userid=-1) AND username LIKE ? AND loglevel LIKE ? AND logtext LIKE ? AND time LIKE ? ORDER BY server_log.id DESC LIMIT ?, ?";
                        $stmt = $conn->prepare($sql);
						$loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
						$search_username = "%" . $search_username . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("ssssii", $search_username, $loglevel, $logtext, $time, $offset, $page_size);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        // Display log entries
                        echo '<table class="table" style="overflow-x:auto">';
                        echo '<thead>';
                        echo '<tr>'; 
                        echo '<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Username</th><th>Time & date</th><th>Delete entry</th>';
                        echo '</tr>';
                        echo '</thead>';
                        echo '<tbody>';
                        
                        // Display filter options
                        echo '<tr>';
                        echo '<form action="view_server_log.php" method="get">';
                        echo '<input type="hidden" name="filter_submit" value="true">';
                        echo '<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>';
                        echo '<td><input type="text" class="form-control" name="loglevel" placeholder="' . str_replace("%","",$loglevel) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="logtext" placeholder="' . str_replace("%","",$logtext) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="username" placeholder="' . str_replace("%","",$search_username) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="time" placeholder="' . str_replace("%","",$time) . '"></td>';
                        echo '<td><button type="button" class="btn btn-danger" data-bs-toggle="modal" data-bs-target="#delete_all">Delete log</button></td>';
                        echo '</form>';
                        echo '</tr>';
                        
                        while($row = $result->fetch_assoc()) {
                            echo '<tr>';
                            echo '<td>' . $row["id"] . '</td>';
                            echo '<td>' . $row["loglevel"] . '</td>';
                            echo '<td>' . $row["logtext"] . '</td>';
                            echo '<td>' . $row["username"] . '</td>';
                            echo '<td>' . $row["time"] . '</td>';
                            echo '<td><a href="view_server_log.php?delete=' . $row["id"] . $filter_query . '&page=' . $current_page . '">delete</a></td>';
                            echo '</tr>';
                        }
                        
                        echo '</tbody>';
                        echo '</table>';
                        $conn->close();
                        
                        // Display pagination links with filter query
                        echo '<nav aria-label="Page navigation">';
                        echo '<ul class="pagination justify-content-center">';
                        for ($i = 1; $i <= $total_pages; $i++) {
                            echo '<li class="page-item ' . ($i == $current_page ? 'active' : '') . '"><a class="page-link" href="view_server_log.php?page=' . $i . $filter_query . '">' . $i . '</a></li>';
                        }
                        echo '</ul>';
                        echo '</nav>';
                    ?>
                </div>
            </div>
        </div>
    </div>
	<div class="modal fade" id="delete_all" tabindex="-1" aria-labelledby="delete_all_label" aria-hidden="true">
	  <div class="modal-dialog">
		<div class="modal-content">
		  <div class="modal-header">
			<h5 class="modal-title" id="delete_all_label">WARNING</h5>
			<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
		  </div>
		  <div class="modal-body">
			Warning! You are about to delete the entyre System0 log.<br>This cannot be undone.<br>We highly recommend exporting the entyre log, before deleting it.<br>
			You will not be able to undo this step. Important security evidence will be lost, if you press the "delete" button!
		  </div>
		  <div class="modal-footer">
			<button style="width:70%" type="button" class="btn btn-primary" data-bs-dismiss="modal">Cancel</button>
			<a style="width:25%" class="btn btn-danger" href="view_server_log.php?delete_all">Delete log</a>
		  </div>
		</div>
	  </div>
</div>
</div>
</body>
</html>
