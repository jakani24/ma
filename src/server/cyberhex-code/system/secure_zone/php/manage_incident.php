<?php
ini_set('post_max_size', '256M');
ini_set('upload_max_filesize', '256M');
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
$userid= $_SESSION["id"];
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
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("si", $name,$incident_id);
	$name=htmlspecialchars($_POST["name"]);
	$incident_id=htmlspecialchars($_GET["incident_id"]);
	$stmt->execute();
	$stmt->close();	
}

if(isset($_GET["add_todoitem"])){
	$todoitem=htmlspecialchars($_POST["todoitem"]);
	$list_id=htmlspecialchars($_GET["list_id"]);
	$sql="INSERT INTO todo_items (text,done_by,done,belongs_to_list) VALUES(?,0,0,?)";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("si", $todoitem,$list_id);
	$todoitem=htmlspecialchars($_POST["todoitem"]);
	$list_id=htmlspecialchars($_GET["list_id"]);
	$stmt->execute();
	$stmt->close();	
}

if(isset($_GET["update_box_id"])){
	$box_id=htmlspecialchars($_GET["update_box_id"]);
	$status=htmlspecialchars($_GET["checked"]);
	if($status=="true")
		$status=1;
	else
		$status=0;
	$sql="UPDATE todo_items SET done = ?, done_by = ? WHERE id = ?";
	$stmt = $conn->prepare($sql);
	$stmt->bind_param("iii", $status,$userid,$box_id);
	$stmt->execute();
	$stmt->close();	
}

if(isset($_GET["upload_evidence"])){
	$incident_id=htmlspecialchars($_GET["incident_id"]);
    $target_dir = "/var/www/html/incidents/$incident_id/evidence/";
    $original_filename = basename($_FILES["fileToUpload"]["name"]);
    $new_filename = $original_filename . ".evidence";
    $target_file = $target_dir . $new_filename;
	move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file);
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Change Password</title>
	 <script>
		function change_status(id,todo_id){
			box=document.getElementById(id);
			if(box!=null){
				var status=box.checked;
				fetch("manage_incident.php?update_box_id="+todo_id+"&checked="+status);
			}
		}
	 </script>
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
						<h4>Incident #<?php echo($_GET["incident_id"]); ?></h4>
						<?php
							//get some info about the incident from the db
							$sql="SELECT * FROM incidents WHERE id = ?";
							$stmt = $conn->prepare($sql);
							$incident_id=htmlspecialchars($_GET["incident_id"]);
							$stmt->bind_param("i", $incident_id);
							$stmt->execute();
							$results = $stmt->get_result();
							$data = $results->fetch_assoc();
							echo("<p>Status: ".$data["status"]."</p>");
							echo("<p>Description: ".$data["description"]."</p>");
							echo("<p>Opened: ".$data["opened"]."</p>");
							echo("<p>Closed: ".$data["closed"]."</p>");
						?>
					</div>
					<div id="evidence" style="display:none">
						<p>Files uploaded here can not be deleted. You can upload important files that might help authoritys here.</p>
						<form action="manage_incident.php?show=evidence&upload_evidence=true&incident_id=<?php echo($_GET["incident_id"]); ?>" method="post" enctype="multipart/form-data">
							<div class="form-group">
								<label for="fileToUpload">Select file to upload:</label>
								<input type="file" class="form-control-file" name="fileToUpload" id="fileToUpload" required>
							</div>
							<button type="submit" class="btn btn-primary">Upload file</button>
						</form>
						<table class="table">
						<tr><th>File</th><th>Download</th></tr>
						<?php
							$incident_id=htmlspecialchars($_GET["incident_id"]);
							$directory = "/var/www/html/incidents/$incident_id/evidence/";
							$csvFiles = glob($directory . '*.evidence');
							$fileCreationTimes = [];
							//sort the files
							 foreach ($csvFiles as $file) {
								$fileCreationTimes[$file] = filectime($file);
							}
							arsort($fileCreationTimes);
							// Loop through each file and print its name
							foreach ($fileCreationTimes as $file=> $creationTime) {
								echo("<tr><td>".str_replace(".evidence","",basename($file))."</td><td><a href='/backup/".basename($file)."' download>Download</a></td></tr>");
							}
 
						?>
					
					</table>
					</div>
					<div id="chat" style="display:none">
						
					</div>
					<div id="todo" style="display:none">
						<br>
						<a data-bs-target="#add_todo" data-bs-toggle="modal" href="#add_todo" class="btn btn-primary">Add a todo list</a>
						<br><br>
						<?php
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

									echo("<table class='table'>");
									echo("<tr><th>Done</th><th>Entry</th><th>Done By</th></tr>");
									//display form to add a todo entry
									echo("<tr><td></td>");
										echo("<td><form action='manage_incident.php?list_id=$list_id&show=todo&add_todoitem=true&incident_id=".$_GET["incident_id"]."' method='POST'><input type='text' class='form-control' id='todoitem' name='todoitem' required></td><td> <button type='submit' class='btn btn-primary btn-block'>Add item</button></form></td>");
									echo("</tr>");
									if ($result_entries->num_rows > 0) {
										// Output data of each entry
										while($entry = $result_entries->fetch_assoc()) {
											echo("<tr>");
												//display status
												if($entry["done"]==1)
													echo("<td><input class='form-check-input' type='checkbox' id='todoitem".$entry["id"]."' onclick='change_status(\"todoitem".$entry["id"]."\",".$entry["id"].")' checked></td>");
												else
													echo("<td><input class='form-check-input' type='checkbox' id='todoitem".$entry["id"]."' onclick='change_status(\"todoitem".$entry["id"]."\",".$entry["id"].")'></td>");
												//display text
												echo("<td>".$entry["text"]."</td>");
												//display user who checked it
												$sql="SELECT username FROM users WHERE id=?";
												$stmt2 = $conn->prepare($sql);
												$stmt2->bind_param("i", $entry["done_by"]);
												$stmt2->execute();
												$result_lists2 = $stmt2->get_result();
												$user = $result_lists2->fetch_assoc();
												if($entry["done"]==1)
													echo("<td>".$user["username"]."</td>");
												else
													echo("<td></td>");
												$stmt2->close();
											echo("</tr>");
										}
									} 
									echo("</table>");
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
					<form action="manage_incident.php?show=todo&add_todolist=true&incident_id=<?php echo($_GET["incident_id"]); ?>" method="post">
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
