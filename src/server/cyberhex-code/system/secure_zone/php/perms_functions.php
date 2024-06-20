<?php

//get perms string from post request (like a uzser creation form)
function get_perm_str(){
	//ge tthe set permissions of the form
	$p1 = isset($_POST["add_user"]);
	$p2 = isset($_POST["delete_user"]);
	$p3 = isset($_POST["view_log"]);
	$p4 = isset($_POST["delete_log"]);
	$p5 = isset($_POST["server_settings"]);
	$p6 = isset($_POST["client_settings"]);
	$p7 = isset( $_POST["database_settings"]);
	$p8 = isset($_POST["add_clients"]);
	$p9 = isset($_POST["delete_clients"]);
	$p10 = isset($_POST["view_incidents"]);
	$p11 = isset($_POST["manage_incidents"]);
	//$p10 = "0";
	
	//init the permission string
	$perms_str="";
	//copy the perms into permission string)
	if($p1==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p2==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p3==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p4==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p5==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p6==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p7==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p8==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p9==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p10==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	if($p11==1)
		$perms_str.="1";
	else
		$perms_str.="0";
	return $perms_str;
}



?>