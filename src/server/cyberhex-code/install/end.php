<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cyberhex Installation</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            text-align: center;
            margin: 50px;
        }
        h1 {
            color: #333;
        }
    </style>
</head>
<body>
    <h1>You have installed cyberhex! Thank you for choosing us!</h1>
	<a href="end.php?end=true">Finish installation.</a>
</body>
<?php
if(isset($_GET["end"])){
	$success=1;
	if(!unlink("create_admin.php")){
		$success=0;
	}if(!unlink("welcome.php")){
		$success=0;
	}if(!unlink("create_db.php")){
		$success=0;
	}
	if($success!==1){
	echo('<p style="font-size: 20px; color: red;">There was an error finishing the installation. Please try again or contact support at: <a href="mailto:info.jakach@gmail.com">info.jakach@gmail.com</a></p>');
	}else{
		echo('<p style="font-size: 20px; color: green;">All done, you can now start using cyberhex! <a href="/login.php">Go to login page</a></p>');
	}
	
	
}
?>
</html>
