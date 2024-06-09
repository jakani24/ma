<?php
session_start();
if(isset($_SESSION["login"])){
	header("LOCATION:/system/secure_zone/php/index.php");
}
include "../../../api/php/notifications/sendmessage.php"; //to send user notification on login
include "../../../api/php/log/add_server_entry.php"; //to log things
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/js/bootstrap.bundle.min.js" integrity="sha384-MrcW6ZMFYlzcLA8Nl+NtUVF0sA7MsXsP1UyJoMp4YLEuNSfAP+JcXn/tWtIaxVXM" crossorigin="anonymous"></script>
	<title>Cyberhex login page</title>
	<style>
    .bg-image {
      background-image: url('/logo.png');
      background-size: contain;
      background-repeat: no-repeat;
      background-position: center;
      height: 100vh; /* Set height to viewport height */
    }
  </style>
</head>
<body>
<script>
//js that handelsour passkey login

async function checkRegistration() {
            try {

                if (!window.fetch || !navigator.credentials || !navigator.credentials.create) {
                    throw new Error('Browser not supported.');
                }

                // get check args
                let rep = await window.fetch('login_backend.php?fn=getGetArgs' + getGetParams(), {method:'GET',cache:'no-cache'});
                const getArgs = await rep.json();

                // error handling
                if (getArgs.success === false) {
                    throw new Error(getArgs.msg);
                }

                // replace binary base64 data with ArrayBuffer. a other way to do this
                // is the reviver function of JSON.parse()
                recursiveBase64StrToArrayBuffer(getArgs);

                // check credentials with hardware
                const cred = await navigator.credentials.get(getArgs);

                // create object for transmission to server
                const authenticatorAttestationResponse = {
                    id: cred.rawId ? arrayBufferToBase64(cred.rawId) : null,
                    clientDataJSON: cred.response.clientDataJSON  ? arrayBufferToBase64(cred.response.clientDataJSON) : null,
                    authenticatorData: cred.response.authenticatorData ? arrayBufferToBase64(cred.response.authenticatorData) : null,
                    signature: cred.response.signature ? arrayBufferToBase64(cred.response.signature) : null,
                    userHandle: cred.response.userHandle ? arrayBufferToBase64(cred.response.userHandle) : null
                };

                // send to server
                rep = await window.fetch('login_backend.php?fn=processGet' + getGetParams(), {
                    method:'POST',
                    body: JSON.stringify(authenticatorAttestationResponse),
                    cache:'no-cache'
                });
                const authenticatorAttestationServerResponse = await rep.json();

                // check server response
                if (authenticatorAttestationServerResponse.success) {
                    reloadServerPreview();
                    //window.alert(authenticatorAttestationServerResponse.msg || 'login success');
					//auth success, send to index
					if(authenticatorAttestationServerResponse.msg=="send_to_2fa"){
						window.location.href = "/system/insecure_zone/php/2fa.php";
					}else{
						window.location.href = "/system/secure_zone/php/index.php";
					}
                } else {
                    throw new Error(authenticatorAttestationServerResponse.msg);
                }

            } catch (err) {
                reloadServerPreview();
				if(err.message=="User does not exist"){
					//we will display a warning here later on
					//alert("User does not exist!!! check line 71 of login.html to set warning");
					var alert_message=document.getElementById("no_passkey");
					alert_message.style.display="block";
				}else{
					window.alert(err.message || 'unknown error occured');
				}
            }
        }

        function queryFidoMetaDataService() {
            window.fetch('login_backend.php?fn=queryFidoMetaDataService' + getGetParams(), {method:'GET',cache:'no-cache'}).then(function(response) {
                return response.json();

            }).then(function(json) {
               if (json.success) {
                   window.alert(json.msg);
               } else {
                   throw new Error(json.msg);
               }
            }).catch(function(err) {
                window.alert(err.message || 'unknown error occured');
            });
        }

        /**
         * convert RFC 1342-like base64 strings to array buffer
         * @param {mixed} obj
         * @returns {undefined}
         */
        function recursiveBase64StrToArrayBuffer(obj) {
            let prefix = '=?BINARY?B?';
            let suffix = '?=';
            if (typeof obj === 'object') {
                for (let key in obj) {
                    if (typeof obj[key] === 'string') {
                        let str = obj[key];
                        if (str.substring(0, prefix.length) === prefix && str.substring(str.length - suffix.length) === suffix) {
                            str = str.substring(prefix.length, str.length - suffix.length);

                            let binary_string = window.atob(str);
                            let len = binary_string.length;
                            let bytes = new Uint8Array(len);
                            for (let i = 0; i < len; i++)        {
                                bytes[i] = binary_string.charCodeAt(i);
                            }
                            obj[key] = bytes.buffer;
                        }
                    } else {
                        recursiveBase64StrToArrayBuffer(obj[key]);
                    }
                }
            }
        }

        /**
         * Convert a ArrayBuffer to Base64
         * @param {ArrayBuffer} buffer
         * @returns {String}
         */
        function arrayBufferToBase64(buffer) {
            let binary = '';
            let bytes = new Uint8Array(buffer);
            let len = bytes.byteLength;
            for (let i = 0; i < len; i++) {
                binary += String.fromCharCode( bytes[ i ] );
            }
            return window.btoa(binary);
        }
		
		function ascii_to_hex(str) {
			let hex = '';
			for (let i = 0; i < str.length; i++) {
				let ascii = str.charCodeAt(i).toString(16);
				hex += ('00' + ascii).slice(-2); // Ensure each hex value is 2 characters long
			}
			return hex;
		}
        /**
         * Get URL parameter
         * @returns {String}
         */
        function getGetParams() {
            let url = '';

            url += '&apple=1';
            url += '&yubico=1';
            url += '&solo=1'
            url += '&hypersecu=1';
            url += '&google=1';
            url += '&microsoft=1';
            url += '&mds=1';

            url += '&requireResidentKey=0';

            url += '&type_usb=1';
            url += '&type_nfc=1';
            url += '&type_ble=1';
            url += '&type_int=1';
            url += '&type_hybrid=1';

            url += '&fmt_android-key=1';
            url += '&fmt_android-safetynet=1';
            url += '&fmt_apple=1';
            url += '&fmt_fido-u2f=1';
            url += '&fmt_none=1';
            url += '&fmt_packed=1';
            url += '&fmt_tpm=1';

            url += '&rpId=auth.jakach.com';

            url += '&userId=' + encodeURIComponent(ascii_to_hex(document.getElementById('username').value));
            url += '&userName=' + encodeURIComponent(document.getElementById('username').value);
            url += '&userDisplayName=' + encodeURIComponent(document.getElementById('username').value);

            url += '&userVerification=discouraged';

            return url;
        }

        function reloadServerPreview() {
        }

        function setAttestation(attestation) {
            let inputEls = document.getElementsByTagName('input');
            for (const inputEl of inputEls) {
                if (inputEl.id && inputEl.id.match(/^(fmt|cert)\_/)) {
                    inputEl.disabled = !attestation;
                }
                if (inputEl.id && inputEl.id.match(/^fmt\_/)) {
                    inputEl.checked = attestation ? inputEl.id !== 'fmt_none' : inputEl.id === 'fmt_none';
                }
                if (inputEl.id && inputEl.id.match(/^cert\_/)) {
                    inputEl.checked = attestation ? inputEl.id === 'cert_mds' : false;
                }
            }
        }

        /**
         * force https on load
         * @returns {undefined}
         */
        window.onload = function() {
            if (location.protocol !== 'https:' && location.host !== 'localhost') {
                location.href = location.href.replace('http', 'https');
            }
        }

</script>
<div class="container-fluid bg-image">
	<div class="container mt-5">
		<div class="row justify-content-center">
			<div class="col-md-6">
				<div class="card">
					<div class="card-header">
						<h4>Login to Cyberhex</h4>
					</div>
					<div class="card-body">
						<form action="login.php" method="post">
							<div class="form-group">
								<label for="username">Username:</label>
								<input type="text" class="form-control" id="username" name="username" required>
							</div>
							<div class="form-group">
								<label for="password">Password:</label>
								<input type="password" class="form-control" id="password" name="password" required>
							</div>
							<br>
							<div class="row">
								<div class="col">
									<button style="align:left" type="submit" class="btn btn-primary btn-block">Login with password</button>
								</div>
								<div class="col">
									<center>Or</center>
								</div>
								<div class="col">
									<button style="align:right" type="button" class="btn btn-primary btn-block" onclick="checkRegistration()">Login with a passkey</button>
								</div>
							</div>
						</form>
						<br><br>
						<div class="alert alert-danger" role="alert" style="display:none" id="no_passkey">
							You do not have a passkey associatet with your account.
						</div>
						<!-- php code to verify password-->
						<?php
							// Check if the form is submitted
							if ($_SERVER["REQUEST_METHOD"] == "POST") {
								//include db pw
								include "../../../config.php";
								
								// Retrieve user input
								$username = htmlspecialchars($_POST["username"]);
								$password = $_POST["password"];


								// Create a connection
								$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD, $DB_DATABASE);

								// Check the connection
								if ($conn->connect_error) {
									die("Connection failed: " . $conn->connect_error);
								}
								$sql = "SELECT * FROM users WHERE username = ?";
								$stmt = $conn->prepare($sql);
								$stmt->bind_param("s", $username);
								
								// Execute the statement
								$stmt->execute();

								// Get the result
								$result = $stmt->get_result();
								
								// Check if the user exists and verify the password
								if ($result->num_rows > 0) {
									$row = $result->fetch_assoc();
									if($row["allow_pw_login"]==1){
										if (password_verify($password, $row['password'])) {
											$_SESSION["username"]=htmlspecialchars($username);
											$_SESSION["login"]=true;
											$_SESSION["perms"]=$row["perms"];
											$_SESSION["email"]=$row["email"];
											$_SESSION["id"]=$row["id"];
											$_SESSION["telegram_id"]=$row["telegram_id"];
											$_SESSION["allow_pw_login"]=$row["allow_pw_login"];
											$_SESSION["send_login_message"]=$row["send_login_message"];
											$_SESSION["use_2fa"]=$row["use_2fa"];
											if($_SESSION["use_2fa"]=="1"){
												unset($_SESSION["login"]); //set the login state to false
												$_SESSION["2fa_auth"]=true;
												$pin=mt_rand(100000, 999999);
												$_SESSION["pin"]=$pin;
												$ip = $_SERVER['REMOTE_ADDR'];
												send_to_user("[2FA-Pin]\nHello $username\nHere is your pin to log into cyberhex: $pin. If you did not try to log in please take steps to secure your account!\nIP: $ip\n",$username);
												//send the user to 2fa auth page
												echo '<script>window.location.href = "/system/insecure_zone/php/2fa.php";</script>';
											}else{
												log_action("LOGIN::SUCCESS","User ".$_SESSION["username"]." logged in with password.",$_SESSION["id"]);
												if($_SESSION["send_login_message"]=="1"){
													$ip = $_SERVER['REMOTE_ADDR'];
													$username=$row["username"];
													send_to_user("[LOGIN WARNING]\nHello $username\nSomebody has logged into Cyberhex with your account.\nIf this was you, you can ignore this message. Else please take steps to secure your account!\nIP: $ip\n",$username);
												}
												echo '<script>window.location.href = "/system/secure_zone/php/index.php";</script>';
											}
											exit();
										} else {
											log_action("LOGIN::FAILURE","User ".$username." entered wrong password.",1);
											echo '<div class="alert alert-danger" role="alert">
													Incorrect username or password.
												  </div>';
										}
									}
									else{
										echo '<div class="alert alert-danger" role="alert">
											Password login is disabled on your account. Please use your passkey
										  </div>';
									}
								} else {
									log_action("LOGIN::FAILURE","User ".$username." entered unknown username.",1);
									echo '<div class="alert alert-danger" role="alert">
											Incorrect username or password.
										  </div>';
								}
								

								// Close the connection
								$stmt->close();
								$conn->close();
							}
						?>
						
					</div>
				</div>
			</div>
		</div>
	</div>
</div>
</body>
</html>
