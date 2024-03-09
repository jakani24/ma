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

<div class="container-fluid">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>View log</h4>
                </div>
                <div class="card-body" style="overflow-x:auto">
                    <!-- table with all users => delete button -->
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
                        
                        // Define page size and current page
                        $page_size = 100;
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
                        echo '<th>Entry id</th><th>Loglevel</th><th>Logtext</th><th>Machine id</th><th>Time & date</th><th>Delete entry</th>';
                        echo '</tr>';
                        echo '</thead>';
                        echo '<tbody>';
                        
                        while($row = $result->fetch_assoc()) {
                            echo '<tr>';
                            echo '<td>' . $row["id"] . '</td>';
                            echo '<td>' . $row["loglevel"] . '</td>';
                            echo '<td>' . $row["logtext"] . '</td>';
                            echo '<td>' . $row["machine_id"] . '</td>';
                            echo '<td>' . $row["time"] . '</td>';
                            echo '<td><a href="view_log.php?delete=' . $row["id"] . '">delete</a></td>';
                            echo '</tr>';
                        }
                        
                        echo '</tbody>';
                        echo '</table>';
                        $conn->close();
                        
                        // Display pagination links
                        echo '<nav aria-label="Page navigation">';
                        echo '<ul class="pagination justify-content-center">';
                        for ($i = 1; $i <= $total_pages; $i++) {
                            echo '<li class="page-item ' . ($i == $current_page ? 'active' : '') . '"><a class="page-link" href="view_log.php?page=' . $i . '">' . $i . '</a></li>';
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
