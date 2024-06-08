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
$setting_virus_ctrl_virus_process_found_kill = 0;
$setting_communication_unsafe_tls = "not configured yet";
$setting_server_server_url="not configured yet";
$setting_rtp_folder_scan_status=0;
$setting_rtp_process_scan_status=0;
include "../../../config.php";
include "../../../api/php/log/add_server_entry.php"; //to log things
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
if ($conn->connect_error) {
	$success=0;
	die("Connection failed: " . $conn->connect_error);
}
//we store all the functions like update,safe,load etc in this file, because else "client_settings.php" will get way to big
include "client_settings_functions.php";
if(isset($_GET["update"])){
	safe_settings();
	log_action("SERVER_SETTINGS::UPDATE::SUCCESS","User ".$_SESSION["username"]." updated some server settings.",$_SESSION["id"]);
}
if(isset($_GET["delete"])){
	delete_item($_GET["db"],$_GET["delete"]);
	log_action("SERVER_SETTINGS::DELETE::SUCCESS","User ".$_SESSION["username"]." deleted some server settings.",$_SESSION["id"]);
}
if(isset($_GET["add"])){
	add_item($_GET["add"],$_GET["value"],$_GET["field"]);
	log_action("SERVER_SETTINGS::ADD::SUCCESS","User ".$_SESSION["username"]." added some server settings.",$_SESSION["id"]);
}
load_settings();

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
function update_textfield(id,name){
	var element = document.getElementById(id);
	var value = element.value;
	fetch('server_settings.php?update='+name+'&value='+value);
}
<?php
	include "../../../config.php";
	$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
	if ($conn->connect_error) {
		$success=0;
		die("Connection failed: " . $conn->connect_error);
	}
	
	if(isset($_GET["update"])){
		$value=htmlspecialchars($_GET["value"]);
		$name=htmlspecialchars($_GET["update"]);
		if($_GET["update"]=="telegram_bot"){
			$stmt = $conn->prepare("INSERT INTO settings (name,value) VALUES (?,?) ON DUPLICATE KEY UPDATE value = ?;");
			$stmt->bind_param("sss",$name,$value,$value);
			$stmt->execute();
			$stmt->close();
			
		}
		
	}
	//load settings
	$sql = "SELECT * FROM settings WHERE name = 'telegram_bot'";
	$stmt = $conn->prepare($sql);
	// Execute the statement
	$stmt->execute();
	// Get the result
	$telegram_bot="";
	$result = $stmt->get_result();
	$row = $result->fetch_assoc();
	if($result->num_rows > 0){
		$telegram_bot=$row["value"];
	}
	$stmt -> close();
?>

	
//exec("curl \"https://api.telegram.org/$api/sendMessage?chat_id=$telegram_id&text=$text\"");
</script>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Server settings</h4>
                </div>
                <div class="card-body">
					<h7>Telegram Bot API-key</h7>
					<input type="text" id="telegram_bot" class="form-control" name="telegram_bot" value="<?php echo($telegram_bot); ?>" oninput="update_textfield('telegram_bot','telegram_bot');">
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>
