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
if ($perms[2] !== "1") {
    header("location:/system/insecure_zone/php/no_access.php");
    $block = 1;
    exit();
} else {
    $block = 0;
}

// Handle filter submission
$loglevel = isset($_GET["loglevel"]) ? $_GET["loglevel"] : "";
$logtext = isset($_GET["logtext"]) ? $_GET["logtext"] : "";
$machine_id = isset($_GET["machine_id"]) ? $_GET["machine_id"] : "";
$time = isset($_GET["time"]) ? $_GET["time"] : "";
$filter_query = "&loglevel=$loglevel&logtext=$logtext&machine_id=$machine_id&time=$time";

?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC"
        crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM"
        crossorigin="anonymous"></script>
    <title>View Log</title>
</head>

<body>

    <div class="container-fluid">
        <div class="row justify-content-center">
            <div class="col-md-12">
                <div class="card">
                    <div class="card-header">
                        <h4>View log</h4>
                    </div>
                    <div class="card-body" style="overflow-x:auto">
                        <!-- Export log -->
                        <a href="export_log.php?<?php echo $filter_query; ?>&export=true" class="btn btn-primary mb-3">Export</a>

                        <!-- Table with log entries -->
                        <?php
						//include db pw
                        include "../../../config.php";
						$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                        if ($conn->connect_error) {
                            die("Connection failed: " . $conn->connect_error);
                        }
						//create the export file
						if(isset($_GET["export"]))
						{
							$fp=fopen("/var/www/html/export/cyberhex_log_export.csv","w");
							//do all the logic here and write into file
							// Query log entries for the export file with filters
							$sql = "SELECT * FROM log WHERE loglevel LIKE ? AND logtext LIKE ? AND machine_id LIKE ? AND time LIKE ? ORDER BY id DESC LIMIT ?, ?";
							$stmt = $conn->prepare($sql);
							$loglevel = "%" . $loglevel . "%";
							$logtext = "%" . $logtext . "%";
							$machine_id = "%" . $machine_id . "%";
							$time = "%" . $time . "%";
							$stmt->bind_param("ssssii", $loglevel, $logtext, $machine_id, $time, $offset, $page_size);
							$stmt->execute();
							$result = $stmt->get_result();

							fwrite ($fp,"Entry id;Loglevel;Logtext;Machine id;Time & date\n");
							//now add entrys
							while ($row = $result->fetch_assoc()) {
								fwrite($fp,$row["id"] . ';');
								fwrite($fp,$row["loglevel"] . ';');
								fwrite($fp,$row["logtext"] . ';');
								fwrite($fp,$row["machine_id"] . ';');
								fwrite($fp,$row["time"] . ';\n');
							}
							fclose($fp);
							echo '<div class="alert alert-success" role="alert">
                                                Log export finished. <a href="/export/cyberhex_log_export.csv" download>Download export</a>
                            </div>';
							
						}

						//now display the normal page
                        // Define page size and current page
                        $page_size = 50;
                        $current_page = isset($_GET['page']) ? intval($_GET['page']) : 1;
                        $offset = ($current_page - 1) * $page_size;

                        // Get total number of log entries based on filters
                        $sql = "SELECT count(*) AS log_count FROM log WHERE loglevel LIKE ? AND logtext LIKE ? AND machine_id LIKE ? AND time LIKE ?";
                        $stmt = $conn->prepare($sql);
                        $loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
                        $machine_id = "%" . $machine_id . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("ssss", $loglevel, $logtext, $machine_id, $time);
                        $stmt->execute();
                        $result = $stmt->get_result();
                        $row = $result->fetch_assoc();
                        $total_entries = $row["log_count"];

                        // Calculate total pages
                        $total_pages = ceil($total_entries / $page_size);

                        // Query log entries for the current page with filters
                        $sql = "SELECT * FROM log WHERE loglevel LIKE ? AND logtext LIKE ? AND machine_id LIKE ? AND time LIKE ? ORDER BY id DESC LIMIT ?, ?";
                        $stmt = $conn->prepare($sql);
                        $loglevel = "%" . $loglevel . "%";
                        $logtext = "%" . $logtext . "%";
                        $machine_id = "%" . $machine_id . "%";
                        $time = "%" . $time . "%";
                        $stmt->bind_param("ssssii", $loglevel, $logtext, $machine_id, $time, $offset, $page_size);
                        $stmt->execute();
                        $result = $stmt->get_result();

                        // Display log entries
                        echo '<table class="table" style="overflow-x:auto">';
                        echo '<thead>';
                        echo '<tr>';
                        echo '<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Machine id</th><th>Time & date</th>';
                        echo '</tr>';
                        echo '</thead>';
                        echo '<tbody>';

                        // Display filter options
                        echo '<tr>';
                        echo '<form action="export_log.php" method="get">';
                        echo '<input type="hidden" name="filter_submit" value="true">';
                        echo '<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>';
                        echo '<td><input type="text" class="form-control" name="loglevel" placeholder="' . str_replace("%", "", $loglevel) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="logtext" placeholder="' . str_replace("%", "", $logtext) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="machine_id" placeholder="' . str_replace("%", "", $machine_id) . '"></td>';
                        echo '<td><input type="text" class="form-control" name="time" placeholder="' . str_replace("%", "", $time) . '"></td>';
                        echo '<td>---</td>';
                        echo '</form>';
                        echo '</tr>';

                        while ($row = $result->fetch_assoc()) {
                            echo '<tr>';
                            echo '<td>' . $row["id"] . '</td>';
                            echo '<td>' . $row["loglevel"] . '</td>';
                            echo '<td>' . $row["logtext"] . '</td>';
                            echo '<td>' . $row["machine_id"] . '</td>';
                            echo '<td>' . $row["time"] . '</td>';
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
    </div>
</body>

</html>
