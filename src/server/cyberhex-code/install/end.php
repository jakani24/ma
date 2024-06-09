<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	 <title>Cyberhex login page</title>
</head>
<body>

<div class="container mt-5">
    <div class="row justify-content-center">
        <div class="col-md-6">
            <div class="card">
                <div class="card-header">
                    <h4>You have installed cyberhex! Thank you for choosing us!</h4>
                </div>
                <div class="card-body">
					<a href="end.php?end=true">Finish installation.</a>
					<?php
						if(isset($_GET["end"])){
							$success=1;
						/*	if(!unlink("create_admin.php")){
								$success=0;
							}if(!unlink("welcome.php")){
								$success=0;
							}if(!unlink("create_db.php")){
								$success=0;
							}if(!unlink("add_passkey.php")){
								$success=0;
							}*/
							if($success!==1){
								echo '<br><div class="alert alert-danger" role="alert">
									There was an error finishing the installation. Please try again or contact support at: <a href="mailto:info.jakach@gmail.com">info.jakach@gmail.com</a>
								</div>';
							}else{
								echo '<br><div class="alert alert-success" role="alert">
									All done, you can now start using cyberhex! <a href="/login.php">Go to login page</a>
								</div>';
							}
						}
					?>
                </div>
            </div>
        </div>
    </div>
</div>
</body>
</html>