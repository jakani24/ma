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


$machine_id = htmlspecialchars(isset($_GET["machine_id"]) ? $_GET["machine_id"] : "");
$path = htmlspecialchars(isset($_GET["path"]) ? $_GET["path"] : "");
$hash = htmlspecialchars(isset($_GET["hash"]) ? $_GET["hash"] : "");
$action = htmlspecialchars(isset($_GET["action"]) ? $_GET["action"] : "");
$machine_location = htmlspecialchars(isset($_GET["machine_location"]) ? $_GET["machine_location"] : "");
$filter_query = "&hash=$hash&path=$path&machine_id=$machine_id&action=$action&machine_location=$machine_location";

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
<br><br>
<div class="container-fluid">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Dashboard</h4>
                </div>
                <div class="card-body" style="overflow-x:auto">
					<h4>Current Threads</h4>
					<!-- table with all log entrys => delete button -->
                    <?php
                        //include db pw
                        include "../../../config.php";
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
                                $sql = "DELETE FROM vir_notify WHERE id = ?";
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
                                $sql = "DELETE FROM vir_notify";
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
                        $sql = "SELECT count(*) AS log_count FROM machines,vir_notify WHERE machine_location LIKE ? AND path LIKE ? AND hash LIKE ? AND machine_id LIKE ? AND action LIKE ?";
                        $stmt = $conn->prepare($sql);
                        $path = "%" . $path . "%";
                        $hash = "%" . $hash . "%";
                        $machine_id = "%" . $machine_id . "%";
						$machine_location = "%" . $machine_location . "%";
                        $action = "%" . $action . "%";
                        $stmt->bind_param("sssss",$machine_location, $path, $hash, $machine_id, $action);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        $row = $result->fetch_assoc();
                        $total_entries = $row["log_count"];
                        
                        // Calculate total pages
                        $total_pages = ceil($total_entries / $page_size);
                        
                        // Query log entries for the current page with filters
                        $sql = "SELECT * FROM machines,vir_notify WHERE machine_location LIKE ? AND path LIKE ? AND hash LIKE ? AND machine_id LIKE ? AND action LIKE ? AND machine_name=machine_id ORDER BY vir_notify.id DESC LIMIT ?, ?";
                        $stmt = $conn->prepare($sql);
                        $path = "%" . $path . "%";
                        $hash = "%" . $hash . "%";
                        $machine_id = "%" . $machine_id . "%";
						$machine_location = "%" . $machine_location . "%";
                        $action = "%" . $action . "%";
                        $stmt->bind_param("sssssii",$machine_location, $path, $hash, $machine_id, $action, $offset, $page_size);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        
                        // Display log entries
                        echo '<table class="table" style="overflow-x:auto">';
                        echo '<thead>';
                        echo '<tr>'; 
                        echo '<th>Entry id</th><th>Machine id</th><th>Location</th><th>File</th><th>Hash</th><th>Action taken</th><th>Delete entry</th>';
                        echo '</tr>';
                        echo '</thead>';
                        echo '<tbody>';
                        
                        // Display filter options
                        echo '<tr>';
                        echo '<form action="welcome.php" method="get">';
                        echo '<input type="hidden" name="filter_submit" value="true">';
                        echo '<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>';
                        echo '<td><input type="text" class="form-control" name="machine_id" placeholder="' . str_replace("%","",$machine_id) . '"></td>';
						echo '<td><input type="text" class="form-control" name="machine_location" placeholder="' . str_replace("%","",$machine_location) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="path" placeholder="' . str_replace("%","",$path) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="hash" placeholder="' . str_replace("%","",$hash) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="action" placeholder="' . str_replace("%","",$action) . '"></td>';
                        echo '<td><button type="button" class="btn btn-danger" data-bs-toggle="modal" data-bs-target="#delete_all">Delete log</button></td>';
                        echo '</form>';
                        echo '</tr>';
                        
                        while($row = $result->fetch_assoc()) {
                            echo '<tr>';
                            echo '<td>' . $row["id"] . '</td>';
                            echo '<td>' . $row["machine_id"] . '</td>';
							echo '<td>' . $row["machine_location"] . '</td>';
                            echo '<td>' . $row["path"] . '</td>';
                            echo '<td>' . $row["hash"] . '</td>';
                            echo '<td>' . $row["action"] . '</td>';
                            echo '<td><a href="welcome.php?delete=' . $row["id"] . $filter_query . '&page=' . $current_page . '">delete</a></td>';
                            echo '</tr>';
                        }
                        
                        echo '</tbody>';
                        echo '</table>';
                        $conn->close();
                        
                        // Display pagination links with filter query
                        echo '<nav aria-label="Page navigation">';
                        echo '<ul class="pagination justify-content-center">';
                        for ($i = 1; $i <= $total_pages; $i++) {
                            echo '<li class="page-item ' . ($i == $current_page ? 'active' : '') . '"><a class="page-link" href="welcome.php?page=' . $i . $filter_query . '">' . $i . '</a></li>';
                        }
                        echo '</ul>';
                        echo '</nav>';
                    ?>
                </div>
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
			<a style="width:25%" class="btn btn-danger" href="welcome.php?delete_all">Delete log</a>
		  </div>
		</div>
	  </div>
</body>
</html>
