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
?>
<!DOCTYPE html>
<html lang="en">

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet"
        integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous">
    </script>
    <title>Export Log</title>
</head>

<body>

    <div class="container mt-5">
        <div class="row justify-content-center">
            <div class="col-md-12">
                <div class="card">
                    <div class="card-header">
                        <h4>Export Log</h4>
                    </div>
                    <div class="card-body" style="overflow-x:auto">
                        <p>You can use filters before you export the log. The filter preview is below.</p>
                        <a href="export_log.php?export=true">Export log</a>
                        <!-- table with all users => delete button -->
                        <?php
                        //include db pw
                        include "../../../config.php";
                        //delete entry if requested and if user has rights to do that
                        if (isset($_GET["export"])) {
                            $conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                            if ($conn->connect_error) {
                                die("Connection failed: " . $conn->connect_error);
                            }

                            $export_file_path = "/var/www/html/export/cyberhex_log_export.csv";

                            $filter_query = "";
                            // Apply filters if present
                            if (isset($_GET["loglevel"])) {
                                $filter_query .= "&loglevel=" . urlencode($_GET["loglevel"]);
                            }
                            if (isset($_GET["logtext"])) {
                                $filter_query .= "&logtext=" . urlencode($_GET["logtext"]);
                            }
                            if (isset($_GET["machine_id"])) {
                                $filter_query .= "&machine_id=" . urlencode($_GET["machine_id"]);
                            }
                            if (isset($_GET["time"])) {
                                $filter_query .= "&time=" . urlencode($_GET["time"]);
                            }

                            $export_file = fopen($export_file_path, 'w');
                            if ($export_file === false) {
                                echo '<br><div class="alert alert-danger" role="alert">
                                        Error creating export file.
                                    </div>';
                            } else {
                                fwrite($export_file, "id;loglevel;logtext;machine_id;time\r\n");

                                $sql = "SELECT * FROM log";

                                // Apply filters if present
                                if (!empty($filter_query)) {
                                    $sql .= " WHERE 1=1";
                                    parse_str(substr($filter_query, 1), $filter_array);
                                    foreach ($filter_array as $key => $value) {
                                        $sql .= " AND $key LIKE '%" . $conn->real_escape_string($value) . "%'";
                                    }
                                }

                                $result = $conn->query($sql);

                                if ($result->num_rows > 0) {
                                    while ($row = $result->fetch_assoc()) {
                                        fwrite($export_file, "{$row['id']};{$row['loglevel']};{$row['logtext']};{$row['machine_id']};{$row['time']}\r\n");
                                    }
                                }

                                fclose($export_file);
                                echo '<br><div class="alert alert-success" role="alert">
                                        Export file created <a href="' . $export_file_path . '" download>Download</a>
                                    </div>';
                            }

                            $conn->close();
                        }
                        ?>
                        <!-- Display log entries with filters -->
                        <?php
                        include "../../../config.php";

                        // Define page size and current page
                        $page_size = 50;
                        $current_page = isset($_GET['page']) ? intval($_GET['page']) : 1;
                        $offset = ($current_page - 1) * $page_size;

                        // Get total number of log entries
                        $conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
                        if ($conn->connect_error) {
                            die("Connection failed: " . $conn->connect_error);
                        }

                        $sql = "SELECT count(*) AS log_count FROM log";
                        $result = $conn->query($sql);
                        $row = $result->fetch_assoc();
                        $total_entries = $row["log_count"];

                        // Calculate total pages
                        $total_pages = ceil($total_entries / $page_size);

                        // Query log entries for the current page
                        $sql = "SELECT * FROM log ORDER BY id DESC LIMIT ?, ?";
                        $stmt = $conn->prepare($sql);
                        $stmt->bind_param("ii", $offset, $page_size);
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
                        $loglevel_ss = isset($_GET["loglevel"]) ? $_GET["loglevel"] : "Loglevel";
                        $logtext_ss = isset($_GET["logtext"]) ? $_GET["logtext"] : "Logtext";
                        $machine_id_ss = isset($_GET["machine_id"]) ? $_GET["machine_id"] : "Machine id";
                        $time_ss = isset($_GET["time"]) ? $_GET["time"] : "Date & time";
                        echo '<tr>';
                        echo '<form action="export_log.php" method="get">';
                        echo '<td><button type="submit" class="btn btn-primary btn-block">Filter</button></td>';
                        echo '<td><input type="text" class="form-control" name="loglevel" placeholder="' . $loglevel_ss . '"></td>';
                        echo '<td><input type="text" class="form-control" name="logtext" placeholder="' . $logtext_ss . '"></td>';
                        echo '<td><input type="text" class="form-control" name="machine_id" placeholder="' . $machine_id_ss . '"></td>';
                        echo '<td><input type="text" class="form-control" name="time" placeholder="' . $time_ss . '"></td>';
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

                        // Display pagination links
                        echo '<nav aria-label="Page navigation">';
                        echo '<ul class="pagination justify-content-center">';
                        for ($i = 1; $i <= $total_pages; $i++) {
                            echo '<li class="page-item ' . ($i == $current_page ? 'active' : '') . '"><a class="page-link" href="?page=' . $i . '">' . $i . '</a></li>';
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
