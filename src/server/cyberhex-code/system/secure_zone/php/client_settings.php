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
//include db connection
include "../../../config.php";
	
//db: id,name,value
$setting_virus_ctrl_virus_found_action = "not configured yet";
$setting_server_server_url="not configured yet";
$setting_rtp_folder_scan_status=0;
function load_settings(){
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);
	if ($conn->connect_error) {
		die("Connection failed: " . $conn->connect_error);
	}
	//get setting: setting_virus_ctrl_virus_found_action
	$sql = "SELECT value FROM settings WHERE name = 'virus_ctrl:virus_found:action'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($row!==null){
		$setting_virus_ctrl_virus_found_action=$row["value"];
	}
	$stmt -> close();
	
	//get setting: setting_rtp_folder_scan_status
	$sql = "SELECT value FROM settings WHERE name = 'rtp_folder_scan:status'";
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
	
		
	//get setting: setting_server_server_url
	$sql = "SELECT value FROM settings WHERE name = 'server:server_url'";
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
</script>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Client settings</h4>
                </div>
                <div class="card-body">
					<!-- Dropdown for virus controll action -->
					<h5>What should be done, if the scanner finds a virus?</h5>
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
				
					<h5>What is the URL of this server? (url or ip address where the clients connect to)</h5>
					<form method="get" action="client_settings.php">
						<input type="text" class="form-control form-control-sm" style="width:20%" name="name" value="<?php echo($setting_server_server_url); ?>">
						<input type="text" class="form-control" style="display:none" name="update" value="setting_server_server_url">
						<button type="submit" class="btn btn-primary btn-block">Update URL</button>
					</form>
				
				
					<div class="form-check form-switch">
						<input class="form-check-input" type="checkbox" role="switch" id="flexSwitchCheckDefault">
						<label class="form-check-label" for="flexSwitchCheckDefault">Default switch checkbox input</label>
					</div>

                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
