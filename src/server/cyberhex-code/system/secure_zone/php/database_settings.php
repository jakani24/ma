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
if($perms[6]!=="1"){
	header("location:/system/insecure_zone/php/no_access.php");
	$block=1;
	exit();
}else{
	$block=0;
}
//for the get_perms_str() function
include "perms_functions.php";
include "../../../config.php";
//create connection to db
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

// Check the connection
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Database Settings</title>
</head>
<body>
<script>
async function update_db(){
	var loader=document.getElementById("loader");
	var finish=document.getElementById("finish");
	var button=document.getElementById("update_btn");
	loader.style.display="block";
	finish.style.display="none";
	button.style.display="none";
	await fetch('/api/php/database/compile_database.php');
	loader.style.display="none";
	button.style.display="block";
	finish.style.display="block";
}
async function update_percentage() {
    var percentageElement = document.getElementById("percentage");
    
    // Display loading indicator
    percentageElement.innerHTML = "Loading...";
    
    // Make an AJAX request to the PHP script
    await fetch('/api/php/database/get_update_status.php')
        .then(response => response.json())
        .then(data => {
            // Update the percentage on the webpage
            percentageElement.innerHTML = "Status: " + data.toFixed(2) + "%";
        })
        .catch(error => {
            // Display error message if request fails
            percentageElement.innerHTML = "Error: " + error.message;
        });
}

// Call update_percentage every 5 seconds
setInterval(update_percentage, 5000);

//update an entry
function update_textfield(db,id,name,itemid){
	var element = document.getElementById(id);
	var value = element.value;
	fetch('database_settings.php?update='+name+'&value='+value+'&id='+itemid+'&db='+db);
}
//delete an entry
async function delete_item(db,id){
	await fetch('database_settings.php?delete='+id+'&db='+db);
	location.reload();
}
//add an entry
async function add_item(db,element_id1,field1,element_id2,field2){ //we have two valus, two dbs and so on, becuase we have the signature and the description
	var element1 = document.getElementById(element_id1);
	var value1 = element1.value;
	var element2 = document.getElementById(element_id2);
	var value2 = element2.value;
	await fetch('database_settings.php?add='+db+'&value1='+value1+'&field1='+field1+'&value2='+value2+'&field2='+field2);
	location.reload();
}
</script>
<?php
	//we store all the functions like update,safe,load etc in this file, because else "database_settings.php" will get way to big
	include "database_settings_functions.php";
	if(isset($_GET["update"])){
		safe_settings($_GET["db"]);
	}
	if(isset($_GET["delete"])){
		delete_item($_GET["db"],$_GET["delete"]);
	}
	if(isset($_GET["add"])){
		add_item($_GET["add"],$_GET["value1"],$_GET["field1"],$_GET["value2"],$_GET["field2"]);
	}
?>
<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-12">
            <div class="card">
                <div class="card-header">
                    <h4>Database Settings</h4>
                </div>
                <div class="card-body">
				<ul class="nav nav-tabs">
					  <li class="nav-item">
						<a class="nav-link" href="database_settings.php?show=update" id="update_tab">Database update</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="database_settings.php?show=excluded" id="excluded_tab">Excluded signatures</a>
					  </li>
					  <li class="nav-item">
						<a class="nav-link" href="database_settings.php?show=excluded" id="included_tab">Included Signatures</a>
					  </li>

				</ul>
					<div id="update" style="display:none">
						<h4>Database Update</h4>
						<button id="update_btn" class="btn btn-dark" onclick="update_db();">Update / regenerate Database (!this can take very long ~1h)</button>
						<div style="display:none" id="loader">
							<br>
							<div class="alert alert-success" role="alert">
								Database update is running, please do not close this tab and do not navigate away!
								<div id="percentage"></div>
							</div>
							<div class="spinner-border" role="status">
							  <span class="visually-hidden">Loading...</span>
							</div>
						</div>
						<div class="alert alert-success" role="alert" style="display:none" id="finish">
								Database update finished!
						</div>
					</div>
					<div id="excluded" style="display:none">
						<h4>Excluded signatures</h4>
						<!-- Table with user defined, excluded hashes -->
						<?php
							//load entrys from excluded db
							$sql = "SELECT * FROM sig_ex";
							$stmt = $conn->prepare($sql);
							$stmt->execute();
							$result = $stmt->get_result();
							
							// Display log entries
							echo '<table class="table" style="overflow-x:auto">';
							echo '<thead>';
							echo '<tr>'; 
							echo '<th>#</th><th>Signature</th><th>Description</th><th>Add / Delete</th>';
							echo '</tr>';
							echo '</thead>';
							echo '<tbody>';
							echo('<tr>');
								echo('<th scope="row">000</th>');
								echo('<td><input type="text" id="sig_ex" class="form-control" name="name"></td>');
								echo('<td><input type="text" id="sig_ex_desc" class="form-control" name="name"></td>');
								echo('<td><button type="button" class="btn btn-primary" onclick="add_item(\'sig_ex\',\'sig_ex\',\'signature\',\'sig_ex_desc\',\'description\');">Add</button></td>');
							echo('</tr>');
							while($row = $result->fetch_assoc()) {
								echo '<tr>';
								echo("<th scope=\"row\">".$row["id"]."</th>");
								echo("<td><input type=\"text\" id=\"sig_ex".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["signature"]."\" oninput=\"update_textfield('sig_ex','sig_ex".$row["id"]."','sig','".$row["id"]."');\"></td>");
								echo("<td><input type=\"text\" id=\"sig_ex_desc".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["description"]."\" oninput=\"update_textfield('sig_ex','sig_ex_desc".$row["id"]."','sig_desc','".$row["id"]."');\"></td>");
								echo("<td><button type=\"button\" class=\"btn btn-danger\" onclick=\"delete_item('sig_ex',".$row["id"].");\">Delete</button></td>");
								echo '</tr>';
							}
							
							echo '</tbody>';
							echo '</table>';
							$conn->close();
						?>
					
					</div>
					<div id="included" style="display:none">
						<h4>Included signatures</h4>
						<!-- Table with user defined, included hashes -->
						<?php
							//load entrys from excluded db
							$sql = "SELECT * FROM sig_in";
							$stmt = $conn->prepare($sql);
							$stmt->execute();
							$result = $stmt->get_result();
							
							// Display log entries
							echo '<table class="table" style="overflow-x:auto">';
							echo '<thead>';
							echo '<tr>'; 
							echo '<th>#</th><th>Signature</th><th>Description</th><th>Add / Delete</th>';
							echo '</tr>';
							echo '</thead>';
							echo '<tbody>';
							echo('<tr>');
								echo('<th scope="row">000</th>');
								echo('<td><input type="text" id="sig_in" class="form-control" name="name"></td>');
								echo('<td><input type="text" id="sig_in_desc" class="form-control" name="name"></td>');
								echo('<td><button type="button" class="btn btn-primary" onclick="add_item(\'sig_in\',\'sig_in\',\'signature\',\'sig_in_desc\',\'description\');">Add</button></td>');
							echo('</tr>');
							while($row = $result->fetch_assoc()) {
								echo '<tr>';
								echo("<th scope=\"row\">".$row["id"]."</th>");
								echo("<td><input type=\"text\" id=\"sig_in".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["signature"]."\" oninput=\"update_textfield('sig_in','sig_in".$row["id"]."','sig','".$row["id"]."');\"></td>");
								echo("<td><input type=\"text\" id=\"sig_in_desc".$row["id"]."\" class=\"form-control\" name=\"name\" value=\"".$row["description"]."\" oninput=\"update_textfield('sig_in','sig_in_desc".$row["id"]."','sig_desc','".$row["id"]."');\"></td>");
								echo("<td><button type=\"button\" class=\"btn btn-danger\" onclick=\"delete_item('sig_in',".$row["id"].");\">Delete</button></td>");
								echo '</tr>';
							}
							
							echo '</tbody>';
							echo '</table>';
							$conn->close();
						?>
					
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
