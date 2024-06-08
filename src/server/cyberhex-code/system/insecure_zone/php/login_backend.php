<?php
//this file handles the login via passkey
//with db:

require_once 'WebAuthn.php';
include "../../../config.php";
$conn = new mysqli($DB_SERVERNAME, $DB_USERNAME, $DB_PASSWORD,$DB_DATABASE);
if ($conn->connect_error) {
	$success=0;
	die("Connection failed: " . $conn->connect_error);
}
include "../../../api/php/notifications/sendmessage.php"; //to send user notification on login
include "../../../api/php/log/add_server_entry.php"; //to log things
try {
    session_start();

    // read get argument and post body
    $fn = filter_input(INPUT_GET, 'fn');
    $requireResidentKey = !!filter_input(INPUT_GET, 'requireResidentKey');
    $userVerification = filter_input(INPUT_GET, 'userVerification', FILTER_SANITIZE_SPECIAL_CHARS);

    $userId = filter_input(INPUT_GET, 'userId', FILTER_SANITIZE_SPECIAL_CHARS);
    $userName = filter_input(INPUT_GET, 'userName', FILTER_SANITIZE_SPECIAL_CHARS);
    $userDisplayName = filter_input(INPUT_GET, 'userDisplayName', FILTER_SANITIZE_SPECIAL_CHARS);

    $userId = preg_replace('/[^0-9a-f]/i', '', $userId);
    $userName = preg_replace('/[^0-9a-z]/i', '', $userName);
    $userDisplayName = preg_replace('/[^0-9a-z öüäéèàÖÜÄÉÈÀÂÊÎÔÛâêîôû]/i', '', $userDisplayName);

    $post = trim(file_get_contents('php://input'));
    if ($post) {
        $post = json_decode($post, null, 512, JSON_THROW_ON_ERROR);
    }

    if ($fn !== 'getStoredDataHtml') {

        // Formats
        $formats = [];
        //if (filter_input(INPUT_GET, 'fmt_android-key')) {
            $formats[] = 'android-key';
        //}
        ///if (filter_input(INPUT_GET, 'fmt_android-safetynet')) {
            $formats[] = 'android-safetynet';
        //}
        //if (filter_input(INPUT_GET, 'fmt_apple')) {
            $formats[] = 'apple';
        //}
        //if (filter_input(INPUT_GET, 'fmt_fido-u2f')) {
            $formats[] = 'fido-u2f';
        //}
        //if (filter_input(INPUT_GET, 'fmt_none')) {
            $formats[] = 'none';
        //}
        //if (filter_input(INPUT_GET, 'fmt_packed')) {
            $formats[] = 'packed';
        //}
        //if (filter_input(INPUT_GET, 'fmt_tpm')) {
            $formats[] = 'tpm';
        //}

		$rpId=$_SERVER['SERVER_NAME'];
		
		$typeUsb = true;
		$typeNfc = true;
		$typeBle = true;
		$typeInt = true;
		$typeHyb = true;

        // cross-platform: true, if type internal is not allowed
        //                 false, if only internal is allowed
        //                 null, if internal and cross-platform is allowed
        $crossPlatformAttachment = null;
        if (($typeUsb || $typeNfc || $typeBle || $typeHyb) && !$typeInt) {
            $crossPlatformAttachment = true;

        } else if (!$typeUsb && !$typeNfc && !$typeBle && !$typeHyb && $typeInt) {
            $crossPlatformAttachment = false;
        }


        // new Instance of the server library.
        // make sure that $rpId is the domain name.
        $WebAuthn = new lbuchs\WebAuthn\WebAuthn('WebAuthn Library', $rpId, $formats);

        // add root certificates to validate new registrations
        //if (filter_input(INPUT_GET, 'solo')) {
            $WebAuthn->addRootCertificates('rootCertificates/solo.pem');
        //}
        //if (filter_input(INPUT_GET, 'apple')) {
            $WebAuthn->addRootCertificates('rootCertificates/apple.pem');
        //}
        //if (filter_input(INPUT_GET, 'yubico')) {
            $WebAuthn->addRootCertificates('rootCertificates/yubico.pem');
        //}
        //if (filter_input(INPUT_GET, 'hypersecu')) {
            $WebAuthn->addRootCertificates('rootCertificates/hypersecu.pem');
        //}
        //if (filter_input(INPUT_GET, 'google')) {
            $WebAuthn->addRootCertificates('rootCertificates/globalSign.pem');
            $WebAuthn->addRootCertificates('rootCertificates/googleHardware.pem');
        //}
        //if (filter_input(INPUT_GET, 'microsoft')) {
            $WebAuthn->addRootCertificates('rootCertificates/microsoftTpmCollection.pem');
        //}
        //if (filter_input(INPUT_GET, 'mds')) {
            $WebAuthn->addRootCertificates('rootCertificates/mds');
        //}

    }

    // Handle different functions
    if ($fn === 'getCreateArgs') {
        // Get create arguments
        $createArgs = $WebAuthn->getCreateArgs(\hex2bin($userId), $userName, $userDisplayName, 60*4, $requireResidentKey, $userVerification);
        header('Content-Type: application/json');
        print(json_encode($createArgs));

        // Save challenge to session or somewhere else if needed
    } else if ($fn === 'getGetArgs') {
        $ids = [];

		//get registrations form user table
		//put credential id into session where userid = $userId
		
		$stmt = $conn->prepare("SELECT credential_id FROM users WHERE user_hex_id = ?");
		$stmt->bind_param("s", $userId);
        $stmt->execute();
		$registration = $stmt->get_result();
		$row = $registration->fetch_assoc();
		
        
        if ($registration->num_rows <= 0) {
            throw new Exception('User does not exist');
        }
		
		$_SESSION["registrations"]["credentialId"]=$row["credential_id"];
		
		$ids[]=$row["credential_id"];
        $_SESSION["registrations"]["userId"]=$userId;

        $getArgs = $WebAuthn->getGetArgs($ids, 60*4, $typeUsb, $typeNfc, $typeBle, $typeHyb, $typeInt, $userVerification);

        header('Content-Type: application/json');
        print(json_encode($getArgs));

        // save challange to session. you have to deliver it to processGet later.
        $_SESSION['challenge'] = $WebAuthn->getChallenge();

    }else if ($fn === 'processGet') {
        // Process get
        // Retrieve registration data from the database based on credential ID
        $id = base64_decode($post->id);
        $stmt = $conn->prepare("SELECT * FROM users WHERE credential_id = ?");
        $stmt->bind_param("s", $_SESSION["registrations"]["credentialId"]);
        $stmt->execute();
		$registration = $stmt->get_result();
		$row = $registration->fetch_assoc();
        
        if (!$registration) {
            throw new Exception('Public Key for credential ID not found!');
        }

        $clientDataJSON = base64_decode($post->clientDataJSON);
        $authenticatorData = base64_decode($post->authenticatorData);
        $signature = base64_decode($post->signature);
        $userHandle = base64_decode($post->userHandle);
        $challenge = $_SESSION['challenge'] ?? '';
        $credentialPublicKey = $row['public_key'];

        // Process the get request
        $WebAuthn->processGet($clientDataJSON, $authenticatorData, $signature, $credentialPublicKey, $challenge, null, $userVerification === 'required');

        // Authentication success
		//set sessionso user is authenticated
		$_SESSION["username"]=htmlspecialchars($row["username"]);
		$_SESSION["login"]=true;
		$_SESSION["perms"]=$row["perms"];
		$_SESSION["id"]=$row["id"];
		$_SESSION["email"]=$row["email"];
		$_SESSION["telegram_id"]=$row["telegram_id"];
		$_SESSION["allow_pw_login"]=$row["allow_pw_login"];
		$_SESSION["send_login_message"]=$row["send_login_message"];
		$_SESSION["use_2fa"]=$row["use_2fa"];
		
		$return = new stdClass();
		$return->success = true;
		
		if($_SESSION["use_2fa"]=="1"){
			unset($_SESSION["login"]); //set the login state to false
			$_SESSION["2fa_auth"]=true;
			$pin=mt_rand(100000, 999999);
			$_SESSION["pin"]=$pin;
			$ip = $_SERVER['REMOTE_ADDR'];
			$username=$row["username"];
			send_to_user("[2FA-Pin]\nHello $username\nHere is your pin to log into cyberhex: $pin. If you did not try to log in please take steps to secure your account!\nIP: $ip\n",$username);
			//send the user to 2fa auth page
			$return->msg="send_to_2fa";
		}else{
			log_action("LOGIN::SUCCESS","User ".$_SESSION["username"]." logged in with passkey.",$_SESSION["id"]);
			if($_SESSION["send_login_message"]=="1"){
				$ip = $_SERVER['REMOTE_ADDR'];
				$username=$row["username"];
				send_to_user("[LOGIN WARNING]\nHello $username\nSomebody has logged into Cyberhex with your account.\nIf this was you, you can ignore this message. Else please take steps to secure your account!\nIP: $ip\n",$username);
			}
		}
		
        header('Content-Type: application/json');
        print(json_encode($return));
    }

} catch (Throwable $ex) {
    $return = new stdClass();
    $return->success = false;
    $return->msg = $ex->getMessage();

    header('Content-Type: application/json');
    print(json_encode($return));
}

?>
