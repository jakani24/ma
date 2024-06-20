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
if($perms[9]!=="1" && $perms[10]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}

include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
if ($conn->connect_error) {
	$success=0;
	die("Connection failed: " . $conn->connect_error);
}

include "../../../api/php/log/add_server_entry.php"; //to log things

if(isset($_GET["add_todolist"])){
	$name=htmlspecialchars($_POST["name"]);
	$incident_id=htmlspecialchars($_GET["incident_id"]);
	$sql="INSERT INTO todo_lists (name,belongs_to_incident) VALUES(?,?)";
	$stmt->bind_param("si", $name,$incident_id);
	$name=htmlspecialchars($_POST["name"]);
	$incident_id=htmlspecialchars($_GET["incident_id"]);
	$stmt->execute();
	$stmt->close();	
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
                    <h4>Incident</h4>
                </div>
                <div class="card-body">
					<ul class="nav nav-tabs">
					  <li class="nav-item">
						<a class="nav-link" href="manage_incident.php?show=overview&incident_id=<?php echo($_GET["incident_id"]); ?>" id="overview_tab">Overview</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="manage_incident.php?show=evidence&incident_id=<?php echo($_GET["incident_id"]); ?>" id="evidence_tab">Files / Evidence</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="manage_incident.php?show=chat&incident_id=<?php echo($_GET["incident_id"]); ?>" id="chat_tab">Chat</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="manage_incident.php?show=todo&incident_id=<?php echo($_GET["incident_id"]); ?>" id="todo_tab">Todo</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="manage_incident.php?show=settings&incident_id=<?php echo($_GET["incident_id"]); ?>" id="settings_tab">Incident Settings</a>
					  </li>
					  

					</ul>
					<div id="overview" style="display:none">
						
					</div>
					<div id="evidence" style="display:none">
						
					</div>
					<div id="chat" style="display:none">
						
					</div>
					<div id="todo" style="display:none">
						<a data-bs-target="#add_todo" data-bs-toggle="modal" href="#add_todo" class="btn btn-primary">Add a todo list</a>
						<?php
							//list todos from all lists
							
							//list todos -> list each entry of each todo
							
							
							$sql_lists = "SELECT id, name FROM todo_lists WHERE belongs_to_incident = ?";
							$stmt = $conn->prepare($sql_lists);
							$incident_id=htmlspecialchars($_GET["incident_id"]);
							$stmt->bind_param("i", $incident_id);
							$incident_id=htmlspecialchars($_GET["incident_id"]);
							$stmt->execute();
							$result_lists = $stmt->get_result();

							if ($result_lists->num_rows > 0) {
								// Output data of each to-do list
								while($list = $result_lists->fetch_assoc()) {
									echo "<h2>" . $list["name"] . "</h2>";

									$list_id = $list["id"];
									$sql_entries = "SELECT * FROM todo_items WHERE belongs_to_list = $list_id";
									$result_entries = $conn->query($sql_entries);

									if ($result_entries->num_rows > 0) {
										// Output data of each entry
										echo "<ul>";
										while($entry = $result_entries->fetch_assoc()) {
											echo "<li>" . $entry["text"] . " (Done: " . $entry["done"] . ", Done By: " . $entry["done_by"] . ")</li>";
										}
										echo "</ul>";
									} else {
										echo "No entries for this list.";
									}
								}
							} else {
								echo "No to-do lists found.";
							}
						
						?>
					</div>
					<div id="settings" style="display:none">
						
					</div>
                </div>
            </div>
			<div class="modal fade" id="add_todo" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="false">
			  <div class="modal-dialog">
				<div class="modal-content">
				  <div class="modal-header">
					<h5 class="modal-title" id="exampleModalLabel">Permission Explanation</h5>
					<button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
				  </div>
				  <div class="modal-body">
					<form action="manage_incident.php?add_todolist=true&incident_id=<?php echo($_GET["incident_id"]); ?>" method="post">
                        <div class="form-group">
                            <label for="name">Name:</label>
                            <input type="text" class="form-control" id="name" name="name" required>
                        </div>
						<br>
                        <button type="submit" class="btn btn-primary btn-block">Create todo list</button>
                    </form>
				  </div>
				</div>
			  </div>
			</div>
        </div>
    </div>
</div>
<script>
	//decide which div should be shown:
    // Get the URL parameters
    const queryString = window.location.search;
    const urlParams = new URLSearchParams(queryString);

    // Get the value of the "show" parameter
    const show_div = document.getElementById(urlParams.get('show'));
	const nav_tab = document.getElementById(urlParams.get('show')+"_tab");
	show_div.style.display="block";
	nav_tab.setAttribute('class', 'nav-link active');
</script>
</body>
</html>
