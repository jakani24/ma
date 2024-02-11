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
if($perms[5]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
$setting_virus_ctrl_virus_found_action = "not configured yet";
$setting_server_server_url="not configured yet";
$setting_rtp_folder_scan_status=0;
$setting_rtp_process_scan_status=0;
include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
if(isset($_GET["update"])){
	safe_settings();
}
if(isset($_GET["delete"])){
	delete_item($_GET["db"],$_GET["delete"]);
}
if(isset($_GET["add"])){
	add_item($_GET["add"],$_GET["value"],$_GET["field"]);
}
load_settings();
function delete_item($db,$id){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$db=htmlspecialchars($db);
	$id=htmlspecialchars($id);
	$stmt = $conn->prepare("delete from $db where id=$id;");
	$stmt->execute();
	$stmt->close();
	$conn -> close();
}
function add_item($db,$value,$field){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$db=htmlspecialchars($db);
	$field=htmlspecialchars($field);
	$stmt = $conn->prepare("INSERT INTO $db ($field) VALUES(?);");
	$stmt->bind_param("s",$value);
	$stmt->execute();
	$stmt->close();
	$conn -> close();
}
function safe_settings(){
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	$value=htmlspecialchars($_GET["value"]);
	$name=htmlspecialchars($_GET["update"]);
	//update what should be done if a virus is found
	if($_GET["update"]=="setting_virus_ctrl_virus_found_action"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		//$stmt = $conn->prepare("UPDATE settings set value=? WHERE name='virus_ctrl:virus_found:action';");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}

	if($_GET["update"]=="setting_server_server_url"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="setting_rtp_folder_scan_status"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="setting_rtp_process_scan_status"){		
		$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
		$stmt->bind_param("sss",$name,$value,$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="rtp_included"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE rtp_included set path= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="rtp_excluded"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE rtp_excluded set path= ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	if($_GET["update"]=="user_tasks"){	
		$id=htmlspecialchars($_GET["id"]);
		$stmt = $conn->prepare("UPDATE user_tasks set task = ? WHERE id=$id");
		$stmt->bind_param("s",$value);
		$stmt->execute();
		$stmt->close();
	}
	$conn->close();
	
}
function load_settings(){
	global $setting_virus_ctrl_virus_found_action ;
	global $setting_server_server_url;
	global $setting_rtp_folder_scan_status;
	global $setting_rtp_process_scan_status;
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	//get setting: setting_virus_ctrl_virus_found_action
	$sql = "SELECT * FROM settings WHERE name = 'setting_virus_ctrl_virus_found_action'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($result->num_rows > 0){
		$setting_virus_ctrl_virus_found_action=$row["value"];
	}
	$stmt -> close();
	
	//get setting: setting_rtp_folder_scan_status
	$sql = "SELECT * FROM settings WHERE name = 'setting_rtp_folder_scan_status'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		$setting_rtp_folder_scan_status=$row["value"];
	}
	$stmt -> close();
	
	//get setting: setting_rtp_process_scan_status
	$sql = "SELECT * FROM settings WHERE name = 'setting_rtp_process_scan_status'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		$setting_rtp_process_scan_status=$row["value"];
	}
	$stmt -> close();
	
		
	//get setting: setting_server_server_url
	$sql = "SELECT * FROM settings WHERE name = 'setting_server_server_url'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		$setting_server_server_url=$row["value"];
	}
	$stmt -> close();
	$conn -> close();
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
<script>
	function set_name(id, name) {
		var element = document.getElementById(id);
		if (element) {
			element.textContent = name;
		}
    }

	function update_setting(id, name,value){
		fetch('client_settings.php?update='+name+'&value='+value).then(response => {
		// Check if the response status is ok (status code 200-299)
			if (!response.ok) {
				set_name(id,'ERR: can not update setting');
			}else{
				set_name(id,value);
			}
		});
	}
	function update_switch(id,name){
		var element = document.getElementById(id);
		var value = element.checked;
		fetch('client_settings.php?update='+name+'&value='+value);
	}
	function update_textfield(id,name,itemid){
		var element = document.getElementById(id);
		var value = element.value;
		fetch('client_settings.php?update='+name+'&value='+value+'&id='+itemid);
	}
	async function delete_item(db,id){
		await fetch('client_settings.php?delete='+id+'&db='+db);
		location.reload();
	}
	async function add_item(db,element_id,field){
		var element = document.getElementById(element_id);
		var value = element.value;
		await fetch('client_settings.php?add='+db+'&value='+value+'&field='+field);
		location.reload();
	}
	async function add_task(db,field,task_time,task_action,task_argument,task_name){
		var element = document.getElementById(task_time);
		var time = element.value;
		var element = document.getElementById(task_action);
		var action = element.value;
		var element = document.getElementById(task_argument);
		var argument = element.value;
		var element = document.getElementById(task_name);
		var name = element.value;
		var task=time+";"+action+";"+argument+";"+name;
		await fetch('client_settings.php?add='+db+'&value='+task+'&field='+field);
		location.reload();
	}
	async function update_task(db,id,task_time,task_action,task_argument,task_name){
		var element = document.getElementById(task_time);
		var time = element.value;
		var element = document.getElementById(task_action);
		var action = element.value;
		var element = document.getElementById(task_argument);
		var argument = element.value;
		var element = document.getElementById(task_name);
		var name = element.value;
		var task=time+';'+action+';'+argument+';'+name;
		await fetch('client_settings.php?update='+db+'&value='+task+'&id='+id);
	}
</script>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Client settings</h4>
                </div>
                <div class="card-body">
					<h4>General</h4>
					<!-- Dropdown for virus controll action -->
					<h7>What should be done, if the scanner finds a virus?</h7>
					<div class="dropdown">
					  <button class="btn btn-secondary dropdown-toggle" type="button" id="dropdownMenuButton1" data-bs-toggle="dropdown" aria-expanded="false">
						<?php echo($setting_virus_ctrl_virus_found_action) ?>
					  </button>
					  <ul class="dropdown-menu" aria-labelledby="dropdownMenuButton1">
						<li><a class="dropdown-item" href="#" onclick="update_setting('dropdownMenuButton1','setting_virus_ctrl_virus_found_action','remove')">remove</a></li>
						<li><a class="dropdown-item" href="#" onclick="update_setting('dropdownMenuButton1','setting_virus_ctrl_virus_found_action','quarantine')">quarantine</a></li>
						<li><a class="dropdown-item" href="#" onclick="update_setting('dropdownMenuButton1','setting_virus_ctrl_virus_found_action','ignore')">ignore</a></li>
						<li><a class="dropdown-item" href="#" onclick="update_setting('dropdownMenuButton1','setting_virus_ctrl_virus_found_action','call_srv')">call_srv</a></li>
					  </ul>
					</div>
					<br>
					<h7>What is the URL of this server? (url or ip address where the clients connect to)</h7>
						<input type="text" id="server_url_input" class="form-control" name="name" value="<?php echo($setting_server_server_url); ?>" oninput="update_textfield('server_url_input','setting_server_server_url','0')">
					<br>
					<h4>RTP</h4>
					<h7>RTP: folderscanner on/off</h7>
					<div class="form-check form-switch">
						<?php if($setting_rtp_folder_scan_status=="true")
							echo ("<input class=\"form-check-input\" type=\"checkbox\" role=\"switch\" id=\"flexSwitchCheckDefault\" onclick=\"update_switch('flexSwitchCheckDefault','setting_rtp_folder_scan_status')\" checked>");
						else
							echo ("<input class=\"form-check-input\" type=\"checkbox\" role=\"switch\" id=\"flexSwitchCheckDefault\" onclick=\"update_switch('flexSwitchCheckDefault','setting_rtp_folder_scan_status')\">");
						?>
						<label class="form-check-label" for="flexSwitchCheckDefault">Check file modifications</label>
					</div>
					<div class="form-check form-switch">
						<?php if($setting_rtp_process_scan_status=="true")
							echo ("<input class=\"form-check-input\" type=\"checkbox\" role=\"switch\" id=\"flexSwitchCheckDefault1\" onclick=\"update_switch('flexSwitchCheckDefault1','setting_rtp_process_scan_status')\" checked>");
						else
							echo ("<input class=\"form-check-input\" type=\"checkbox\" role=\"switch\" id=\"flexSwitchCheckDefault1\" onclick=\"update_switch('flexSwitchCheckDefault1','setting_rtp_process_scan_status')\">");
						?>
						<label class="form-check-label" for="flexSwitchCheckDefault1">Check Processes</label>
					</div>
					<br>
					<h7>Included folders for RTP folderscanner</h7>
					<table class="table">
					<thead>
					<tr>
					  <th scope="col">#</th>
					  <th scope="col">Path</th>
					  <th scope="col">Add / Delete</th>
					</tr>
				  </thead>
				  <tbody>
						<tr>
							<th scope="row">000</th>
							<td><input type="text" id="rtp_included" class="form-control" name="name"></td>
							<td><button type="button" class="btn btn-primary" onclick="add_item('rtp_included','rtp_included','path');">Add</button></td>
						</tr>
					<?php
						//load all the entrys from a db table
						$sql = "SELECT path,id FROM rtp_included ORDER BY id";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						while ($row = $result->fetch_assoc()){
							//print out the items
							echo("<tr>");
								echo("<th scope=\"row\">".$row["id"]."</th>");
								echo("<td><input type=\"text\" id=\"rtp_included".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["path"]."\" oninput=\"update_textfield('rtp_included".$row["id"]."','rtp_included','".$row["id"]."');\"></td>");
								echo("<td><button type=\"button\" class=\"btn btn-danger\" onclick=\"delete_item('rtp_included',".$row["id"].");\">Delete</button></td>");
							echo("</tr>");
						}
						
						$stmt -> close();
					?>
					</tbody>
					</table>
					<h7>Excluded folders for RTP folderscanner</h7>
					<table class="table">
					<thead>
					<tr>
					  <th scope="col">#</th>
					  <th scope="col">Path</th>
					  <th scope="col">Add / Delete</th>
					</tr>
				  </thead>
				  <tbody>
						<tr>
							<th scope="row">000</th>
							<td><input type="text" id="rtp_excluded" class="form-control" name="name"></td>
							<td><button type="button" class="btn btn-primary" onclick="add_item('rtp_excluded','rtp_excluded','path');">Add</button></td>
						</tr>
					<?php
						//load all the entrys from a db table
						$sql = "SELECT path,id FROM rtp_excluded ORDER BY id";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						while ($row = $result->fetch_assoc()){
							//print out the items
							echo("<tr>");
								echo("<th scope=\"row\">".$row["id"]."</th>");
								echo("<td><input type=\"text\" id=\"rtp_excluded".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["path"]."\" oninput=\"update_textfield('rtp_excluded".$row["id"]."','rtp_excluded','".$row["id"]."');\"></td>");
								echo("<td><button type=\"button\" class=\"btn btn-danger\" onclick=\"delete_item('rtp_excluded',".$row["id"].");\">Delete</button></td>");
							echo("</tr>");
						}
						
						$stmt -> close();
					?>
					</tbody>
					</table>
					<br>
					<h4>Tasks</h4>
					<table class="table">
					<thead>
					<tr>
					  <th scope="col">#</th>
					  <th scope="col">Time</th>
					  <th scope="col">Action</th>
					  <th scope="col">Argument</th>
					  <th scope="col">Name</th>
					  <th scope="col">Add / Delete</th>
					</tr>
				  </thead>
				  <tbody>
						<tr>
							<th scope="row">000</th>
							<td><input type="text" id="task_time" class="form-control" name="task_time"></td>
							<td>
								<select class="form-select" data-live-search="true" id="task_action">
								  <option value="choose_action">Choose an action</option>
								  <option value="scanfile">scanfile</option>
								  <option value="scanfolder">scanfolder</option>
								</select>
							</td>
							<td><input type="text" id="task_argument" class="form-control" name="task_argument"></td>
							<td><input type="text" id="task_name" class="form-control" name="task_name"></td>
							<td><button type="button" class="btn btn-primary" onclick="add_task('user_tasks','task','task_time','task_action','task_argument','task_name');">Add</button></td>
						</tr>
					<?php
						//load all the entrys from a db table
						$sql = "SELECT task,id FROM user_tasks ORDER BY id";
						$stmt = $conn->prepare($sql);
						// Execute the statement
						$stmt->execute();
						// Get the result
						$result = $stmt->get_result();
						while ($row = $result->fetch_assoc()){
							$buf=explode(";",$row["task"]);
							$time=$buf[0];
							$action=$buf[1];
							$argument=$buf[2];
							$name=$buf[3];
							//print out the items
							echo("<tr>");
								echo("<th scope=\"row\">".$row["id"]."</th>");
								echo("<td><input type=\"text\" id=\"task_time".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$time."\" oninput=\"update_task('user_tasks',".$row["id"].",'task_time".$row["id"]."','task_action".$row["id"]."','task_argument".$row["id"]."','task_name".$row["id"]."');\"></td>");
								echo("<td><input type=\"text\" id=\"task_action".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$action."\" oninput=\"update_task('user_tasks',".$row["id"].",'task_time".$row["id"]."','task_action".$row["id"]."','task_argument".$row["id"]."','task_name".$row["id"]."');\"></td>");
								echo("<td>");
								echo('<select class="form-select" data-live-search="true" id="task_action">');
								  echo('<option value="choose_action">Choose an action</option>');
								  if($action=="scanfile")
									echo('<option value="scanfile" selected >scanfile</option>');
								else
									echo('<option value="scanfile" >scanfile</option>');
								 if($action=="scanfolder")
								  echo('<option value="scanfolder" selected>scanfolder</option>');
								else
									echo('<option value="scanfolder">scanfolder</option>');
								echo'(</select>');
								echo('</td>');
								
								echo("<td><input type=\"text\" id=\"task_argument".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$argument."\" oninput=\"update_task('user_tasks',".$row["id"].",'task_time".$row["id"]."','task_action".$row["id"]."','task_argument".$row["id"]."','task_name".$row["id"]."');\"></td>");
								echo("<td><input type=\"text\" id=\"task_name".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$name."\" oninput=\"update_task('user_tasks',".$row["id"].",'task_time".$row["id"]."','task_action".$row["id"]."','task_argument".$row["id"]."','task_name".$row["id"]."');\"></td>");
								echo("<td><button type=\"button\" class=\"btn btn-danger\" onclick=\"delete_item('user_tasks',".$row["id"].");\">Delete</button></td>");
							echo("</tr>");
						}
						
						$stmt -> close();
					?>
					</tbody>
					</table>
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
