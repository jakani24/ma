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

// Handle filter submission

$loglevel = htmlspecialchars(isset($_GET["loglevel"]) ? $_GET["loglevel"] : "");
$logtext = htmlspecialchars(isset($_GET["logtext"]) ? $_GET["logtext"] : "");
$machine_id = htmlspecialchars(isset($_GET["machine_id"]) ? $_GET["machine_id"] : "");
$time = htmlspecialchars(isset($_GET["time"]) ? $_GET["time"] : "");
$machine_location = htmlspecialchars(isset($_GET["machine_location"]) ? $_GET["machine_location"] : "");
$filter_query = "&loglevel=$loglevel&logtext=$logtext&machine_id=$machine_id&time=$time&machine_location=$machine_location";

include "../../../config.php";
//get data for pie chart
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}
$sql = "SELECT count(*) AS counter FROM log WHERE loglevel = 'VIRUS'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$result = $stmt->get_result();
$row = $result->fetch_assoc();
$virus=$row["counter"];
$sql = "SELECT count(*) AS counter FROM log WHERE loglevel = 'INFO'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$result = $stmt->get_result();
$row = $result->fetch_assoc();
$info=$row["counter"];
$sql = "SELECT count(*) AS counter FROM log WHERE loglevel = 'WARNING'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$result = $stmt->get_result();
$row = $result->fetch_assoc();
$warn=$row["counter"];
$sql = "SELECT count(*) AS counter FROM log WHERE loglevel = 'ERROR'";
$stmt = $conn->prepare($sql);
$stmt->execute();
$result = $stmt->get_result();
$row = $result->fetch_assoc();
$err=$row["counter"];

$stmt->close();
$conn->close();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
     <title>View log</title>
	 
	<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
    google.charts.load("current", {packages:["corechart"]});
	google.charts.setOnLoadCallback(drawCharts);

	function drawCharts() {
	  // First Chart
	  var data1 = google.visualization.arrayToDataTable([
		['Entrys', 'Loglevel'],
		['Virus',     <?php echo($virus);?>],
		['Error',      <?php echo($err);?>],
		['Info',  <?php echo($info);?>],
		['Warning', <?php echo($warn)?>],
	  ]);

	  var options1 = {
		title: 'Log Entrys',
		is3D: true,
	  };

	  var chart1 = new google.visualization.PieChart(document.getElementById('log_spread'));
	  chart1.draw(data1, options1);

	  // Second Chart
	  //we would be able to draw a second one if we want
	  var data2 = google.visualization.arrayToDataTable([
		['Category', 'Value'],
		['Category 1', 100],
		['Category 2', 200],
		['Category 3', 300],
	  ]);

	  var options2 = {
		title: 'Second Chart',
		is3D: true,
	  };

	  //var chart2 = new google.visualization.ColumnChart(document.getElementById('machine_spread'));
	  //chart2.draw(data2, options2);
	}
	  
    </script>

</head>
<body>
<br><br>
<div class="container-fluid">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>View log</h4>
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
                                $sql = "DELETE FROM log";
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
                        $sql = "SELECT count(*) AS log_count FROM machines,log WHERE machine_location LIKE ? AND loglevel LIKE ? AND logtext LIKE ? AND machine_id LIKE ? AND time LIKE ?";
                        $stmt = $conn->prepare($sql);
                        $loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
                        $machine_id = "%" . $machine_id . "%";
						$machine_location = "%" . $machine_location . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("sssss",$machine_location, $loglevel, $logtext, $machine_id, $time);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        $row = $result->fetch_assoc();
                        $total_entries = $row["log_count"];
                        
                        // Calculate total pages
                        $total_pages = ceil(($total_entries/2) / $page_size);
                        
                        // Query log entries for the current page with filters
                        $sql = "SELECT * FROM machines,log WHERE machine_location LIKE ? AND loglevel LIKE ? AND logtext LIKE ? AND machine_id LIKE ? AND time LIKE ? AND machine_name=machine_id ORDER BY log.id DESC LIMIT ?, ?";
                        $stmt = $conn->prepare($sql);
                        $loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
                        $machine_id = "%" . $machine_id . "%";
						$machine_location = "%" . $machine_location . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("sssssii", $machine_location, $loglevel, $logtext, $machine_id, $time, $offset, $page_size);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        if($current_page==1){
							echo("<div id='log_spread' style='width: 700px; height: 350px;'></div>");
							//echo("<div style='display:none' class='col' id='machine_spread' style='width: 30%; height: auto;'></div></div>");
						}
                        // Display log entries
                        echo '<table class="table" style="overflow-x:auto">';
                        echo '<thead>';
                        echo '<tr>'; 
                        echo '<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Machine id</th><th>Location</th><th>Time & date</th><th>Delete entry</th>';
                        echo '</tr>';
                        echo '</thead>';
                        echo '<tbody>';
                        
                        // Display filter options
                        echo '<tr>';
                        echo '<form action="view_log.php" method="get">';
                        echo '<input type="hidden" name="filter_submit" value="true">';
                        echo '<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>';
                        echo '<td><input type="text" class="form-control" name="loglevel" placeholder="' . str_replace("%","",$loglevel) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="logtext" placeholder="' . str_replace("%","",$logtext) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="machine_id" placeholder="' . str_replace("%","",$machine_id) . '"></td>';
						echo '<td><input type="text" class="form-control" name="machine_location" placeholder="' . str_replace("%","",$machine_location) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="time" placeholder="' . str_replace("%","",$time) . '"></td>';
                        echo '<td><button type="button" class="btn btn-danger" data-bs-toggle="modal" data-bs-target="#delete_all">Delete log</button></td>';
                        echo '</form>';
                        echo '</tr>';
                        
                        while($row = $result->fetch_assoc()) {
                            echo '<tr>';
                            echo '<td>' . $row["id"] . '</td>';
                            echo '<td>' . $row["loglevel"] . '</td>';
                            echo '<td>' . $row["logtext"] . '</td>';
                            echo '<td>' . $row["machine_id"] . '</td>';
							echo '<td>' . $row["machine_location"] . '</td>';
                            echo '<td>' . $row["time"] . '</td>';
                            echo '<td><a href="view_log.php?delete=' . $row["id"] . $filter_query . '&page=' . $current_page . '">delete</a></td>';
                            echo '</tr>';
                        }
                        
                        echo '</tbody>';
                        echo '</table>';
                        $conn->close();
                        
                        // Display pagination links with filter query
                        echo '<nav aria-label="Page navigation">';
                        echo '<ul class="pagination justify-content-center">';
                        for ($i = 1; $i <= $total_pages; $i++) {
                            echo '<li class="page-item ' . ($i == $current_page ? 'active' : '') . '"><a class="page-link" href="view_log.php?page=' . $i . $filter_query . '">' . $i . '</a></li>';
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
			<a style="width:25%" class="btn btn-danger" href="view_log.php?delete_all">Delete log</a>
		  </div>
		</div>
	  </div>
</div>
</div>
</body>
</html>
