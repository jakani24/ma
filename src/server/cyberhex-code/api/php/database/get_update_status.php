<?php
	// Path to the folder
	$folderPath = '/var/www/html/database_srv';

	// Get a list of all files in the folder
	$files = scandir($folderPath);

	// Subtract 16*16 from the total number of files
	$totalFiles = count($files) - (16 * 16);

	// Calculate the percentage
	$percentage = ($totalFiles / (485+5)) * 100;

	// Ensure the percentage is within the range 0-100
	$percentage = max(0, min(100, $percentage));

	// Display the percentage
	echo round($percentage, 2);
?>
